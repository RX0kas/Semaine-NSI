import glfw
from OpenGL.GL import *


def on_resize(window, width, height):
    """
    Callback appelée automatiquement lorsque la fenêtre est redimensionnée.

    Elle ajuste le viewport OpenGL et la matrice de projection afin
    que l'affichage conserve les proportions correctes.

    :param window: la fenêtre GLFW qui a été redimensionnée
    :param width: nouvelle largeur de la fenêtre
    :param height: nouvelle hauteur de la fenêtre
    """
    # Définir la zone de rendu (viewport) sur toute la surface de la fenêtre
    glViewport(0, 0, width, height)

    # Passer en mode projection (choix de la caméra/coordonnées)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()

    # Calculer le ratio largeur/hauteur
    aspect = width / height

    # Définir une projection orthographique qui conserve les proportions
    if aspect >= 1:
        # Fenêtre plus large que haute → élargir les limites en X
        glOrtho(-aspect, aspect, -1, 1, -1, 1)
    else:
        # Fenêtre plus haute que large → élargir les limites en Y
        glOrtho(-1, 1, -1 / aspect, 1 / aspect, -1, 1)

    # Revenir en mode "modèle-vue" (où l'on place les objets dans la scène)
    glMatrixMode(GL_MODELVIEW)


class Window:
    """
    Classe utilitaire pour créer et gérer une fenêtre GLFW avec contexte OpenGL.
    """

    def __init__(self, width: int, height: int, title: str):
        """
        Crée une nouvelle fenêtre OpenGL.

        :param width: largeur de la fenêtre en pixels
        :param height: hauteur de la fenêtre en pixels
        :param title: titre de la fenêtre
        """
        # Initialiser la bibliothèque GLFW
        if not glfw.init():
            raise Exception("GLFW n'a pas pu être initialisé")

        # Créer la fenêtre
        self.window = glfw.create_window(width, height, title, None, None)

        if not self.window:
            glfw.terminate()
            raise Exception("La fenêtre GLFW n'a pas pu être créée")

        # Rendre le contexte OpenGL actuel (lié à cette fenêtre)
        glfw.make_context_current(self.window)

        # Associer la fonction de callback au redimensionnement
        glfw.set_window_size_callback(self.window, on_resize)

        # Configurer la vue initiale
        on_resize(self.window, width, height)

    def swapbuffer(self):
        """
        Met à jour l'affichage de la fenêtre :
        - traite les événements (clavier, souris, etc.)
        - échange les buffers pour afficher la scène rendue
        """
        glfw.poll_events()
        glfw.swap_buffers(self.window)

    def supprimer(self):
        """
        Libère les ressources GLFW et ferme la fenêtre.
        """
        glfw.terminate()

    def devrait_fermer(self):
        """
        Vérifie si l'utilisateur a demandé la fermeture de la fenêtre.
        :return: True si la fenêtre doit être fermée, False sinon
        """
        return glfw.window_should_close(self.window)
