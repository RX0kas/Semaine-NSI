from OpenGL.GL import *
from shader import Shader

import math
import numpy as np


class TurtleRenderer:
    """
    Classe responsable du rendu OpenGL d'une "tortue" (à la manière de Logo/Turtle graphics).

    Elle gère :
    - La compilation et l'utilisation des shaders
    - La configuration du VAO/VBO pour stocker les sommets
    - L'affichage du chemin tracé par la tortue
    - L'affichage de la tortue elle-même (représentée par un triangle)
    """

    def __init__(self):
        """
        Initialise le pipeline de rendu :
        - Compile et active les shaders
        - Crée et configure un VAO/VBO pour stocker les sommets
        - Prépare l'attribut de position des sommets
        """
        # Compiler et lier les shaders
        shader = Shader("turtle")
        shader.use()

        # Générer et binder un VAO (stocke la configuration des attributs)
        self.vao = glGenVertexArrays(1)
        self.vbo = glGenBuffers(1)

        glBindVertexArray(self.vao)
        glBindBuffer(GL_ARRAY_BUFFER, self.vbo)

        # Associer l'attribut "a_position" du shader aux données du VBO
        position = glGetAttribLocation(shader.getProgram(), "a_position")
        glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 0, None)
        glEnableVertexAttribArray(position)

    def render(self, turtle):
        """
        Rendu de la scène :
        - Efface l'écran
        - Dessine le chemin parcouru par la tortue
        - Dessine la tortue à sa position actuelle

        :param turtle: objet contenant la position, l'angle et les sommets du chemin
        """
        vertices = turtle.get_vertices()
        if len(vertices) > 0:
            glBindVertexArray(self.vao)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo)
            # Charger les sommets du chemin dans le buffer GPU
            glBufferData(GL_ARRAY_BUFFER, vertices.nbytes, vertices, GL_STATIC_DRAW)
            # Dessiner des lignes (chaque 2 sommets = un segment)
            glDrawArrays(GL_LINES, 0, len(vertices) // 2)


        self.__draw_turtle(turtle)

    def render(self, turtle):
        """
        Rendu de la scène :
        - Efface l'écran
        - Dessine le chemin parcouru par la tortue
        - Dessine la tortue à sa position actuelle

        :param turtle: objet contenant la position, l'angle et les sommets du chemin
        """
        # --- Affichage du chemin (lignes) ---
        vertices = turtle.get_vertices()
        if len(vertices) > 0:
            glBindVertexArray(self.vao)
            glBindBuffer(GL_ARRAY_BUFFER, self.vbo)
            glBufferData(GL_ARRAY_BUFFER, vertices.nbytes, vertices, GL_STATIC_DRAW)

            # Draw all completed paths
            completed_vertices = np.array(turtle.vertices, dtype=np.float32)
            if len(completed_vertices) > 0:
                glBufferData(GL_ARRAY_BUFFER, completed_vertices.nbytes, completed_vertices, GL_STATIC_DRAW)
                # Draw as line strips for each continuous path
                # We need to split by paths - for simplicity, we'll draw all as individual lines
                glDrawArrays(GL_LINES, 0, len(completed_vertices) // 2)

            # Draw current path if it exists
            if turtle.current_path and len(turtle.current_path) >= 4:  # Need at least 2 points
                current_vertices = np.array(turtle.current_path, dtype=np.float32)
                glBufferData(GL_ARRAY_BUFFER, current_vertices.nbytes, current_vertices, GL_STATIC_DRAW)
                glDrawArrays(GL_LINE_STRIP, 0, len(current_vertices) // 2)

        # --- Affichage de la tortue (triangle) ---
        self.__draw_turtle(turtle)

    def __draw_turtle(self, turtle):
        """
        Dessine la tortue sous forme de petit triangle orienté.

        :param turtle: objet avec attributs (x, y, angle) représentant la tortue
        """
        size = 0.05  # taille du triangle
        angle_rad = math.radians(turtle.angle)  # orientation de la tortue en radians

        # Triangle en coordonnées locales (avant rotation et translation)
        points = [
            [size, 0.0],  # pointe avant
            [-size / 2, size / 2],  # arrière gauche
            [-size / 2, -size / 2]  # arrière droit
        ]

        # Appliquer rotation + translation
        rotated_points = []
        for point in points:
            x = point[0] * math.cos(angle_rad) - point[1] * math.sin(angle_rad)
            y = point[0] * math.sin(angle_rad) + point[1] * math.cos(angle_rad)
            rotated_points.extend([x + turtle.x, y + turtle.y])

        # Convertir en tableau numpy compatible avec OpenGL
        vertices = np.array(rotated_points, dtype=np.float32)

        # Charger et dessiner le triangle
        glBindVertexArray(self.vao)
        glBindBuffer(GL_ARRAY_BUFFER, self.vbo)
        glBufferData(GL_ARRAY_BUFFER, vertices.nbytes, vertices, GL_STATIC_DRAW)
        glDrawArrays(GL_TRIANGLES, 0, 3)

    def cleanup(self):
        """
        Libère les ressources GPU utilisées par le VAO et le VBO.
        """
        glDeleteVertexArrays(1, [self.vao])
        glDeleteBuffers(1, [self.vbo])
