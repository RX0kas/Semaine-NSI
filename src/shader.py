import os.path

from OpenGL.GL import *
from src.exception import ShaderType, ShaderError
from ctypes import c_uint
import numpy as np

class Shader:
    """
    Classe utilitaire pour gérer un programme de shaders OpenGL.

    Elle permet de :
    - Charger et compiler un vertex shader et un fragment shader à partir de fichiers.
    - Lier les shaders dans un programme OpenGL.
    - Activer l'utilisation de ce programme.
    - Mettre à jour les uniformes (bool, int, float, vecteurs).
    """

    def __init__(self, shaderName: str):
        """
        Crée et compile un programme de shaders à partir des fichiers donnés.

        :param shaderName: nom des fichier source du shader
        :raises ShaderError: si la lecture, compilation ou liaison échoue
        """
        # Créer un programme OpenGL vide
        self.__program = glCreateProgram()

        # Lire le code source des fichiers
        contentV = self.__readShaderFile(shaderName + ".vert")
        contentF = self.__readShaderFile(shaderName + ".frag")

        if contentV is None:
            raise ShaderError(ShaderType.VERTEX, "Erreur dans la lecture du fichier vertex shader")
        if contentF is None:
            raise ShaderError(ShaderType.FRAGMENT, "Erreur dans la lecture du fichier fragment shader")

        # --- Compilation du vertex shader ---
        vertexShader = glCreateShader(GL_VERTEX_SHADER)
        glShaderSource(vertexShader, contentV)
        glCompileShader(vertexShader)
        if glGetShaderiv(vertexShader, GL_COMPILE_STATUS) != GL_TRUE:
            raise ShaderError(ShaderType.VERTEX, f"{glGetShaderInfoLog(vertexShader)}")

        # --- Compilation du fragment shader ---
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER)
        glShaderSource(fragmentShader, contentF)
        glCompileShader(fragmentShader)
        if glGetShaderiv(fragmentShader, GL_COMPILE_STATUS) != GL_TRUE:
            raise ShaderError(ShaderType.FRAGMENT, f"{glGetShaderInfoLog(fragmentShader)}")

        # --- Liaison des shaders au programme ---
        glAttachShader(self.__program, vertexShader)
        glAttachShader(self.__program, fragmentShader)
        glLinkProgram(self.__program)

        # Vérifier le lien
        if glGetProgramiv(self.__program, GL_LINK_STATUS) != GL_TRUE:
            raise ShaderError(ShaderType.LINKING, f"{glGetProgramInfoLog(self.__program)}")

        # Nettoyage des objets shaders (plus nécessaires une fois liés au programme)
        glDeleteShader(vertexShader)
        glDeleteShader(fragmentShader)

    def __preprocesseur(self, data: str) -> str:
        """
        Préprocesseur minimal pour le code GLSL.
        (Peut être enrichi plus tard pour gérer des #include, macros, etc.)
        """
        return data

    def __readShaderFile(self, name: str):
        """
        Lit un fichier shader et applique le préprocesseur.

        :param name: nom du fichier shader
        :return: contenu du fichier (chaîne de caractères) ou None en cas d'erreur
        """
        chemin = os.path.join(os.path.dirname(__file__),"shaders",f"{name}.glsl")
        try:
            with open(chemin, "r") as f:
                content = f.read()
            return self.__preprocesseur(content)
        except Exception:
            return None

    def use(self):
        """
        Active ce programme de shaders dans OpenGL.
        """
        glUseProgram(self.__program)

    def getProgram(self) -> int:
        """
        Retourne l'identifiant OpenGL du programme de shaders.
        """
        return self.__program # type: ignore

    # --- Méthodes utilitaires pour définir des uniformes GLSL ---

    def setBool(self, name: str, value: bool):
        glUniform1i(glGetUniformLocation(self.__program, name), value)

    def setInt(self, name: str, value: int):
        glUniform1i(glGetUniformLocation(self.__program, name), value)

    def setUInt(self, name: str, value: c_uint):
        """
        Définit un uniforme entier non signé (rarement utilisé).
        Non signé veut dire qu'il n'y a pas de nombre negatif (jsp si tu le savais donc au cas ou)
        """
        glUniform1ui(glGetUniformLocation(self.__program, name), value)

    def setFloat(self, name: str, value: float):
        glUniform1f(glGetUniformLocation(self.__program, name), value)

    def setVec2f(self, name: str, v0: float, v1: float):
        glUniform2f(glGetUniformLocation(self.__program, name), v0, v1)

    def setVec3f(self, name: str, v0: float, v1: float, v2: float):
        glUniform3f(glGetUniformLocation(self.__program, name), v0, v1, v2)

    def setMat3f(self,name:str,matrix):
        glUniformMatrix3fv(glGetUniformLocation(self.__program, name),1,GL_FALSE,matrix)