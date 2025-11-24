import time

from glfw import *
from OpenGL.GL import *
from src.camera import Camera
from src.interface import Interface
import src.cpp_backend as backend

dragging = False
last_mouse = None
last_time = time.time()
first_time = True

def on_resize(window, width, height):
    global first_time
    # Définir la zone de rendu (viewport) sur toute la surface de la fenêtre
    #glViewport(0, 0, width, height)

    if not first_time:
        backend.init_fbo(width, height)
    first_time = False

def scroll_callback(window, xoffset, yoffset):
    mx, my = get_cursor_pos(window)
    w, h = backend.get_fbo_screen_size()
    Camera.instance().on_scroll(xoffset, yoffset, (mx, my), (w, h))

def mouse_button_callback(window, button, action, mods):
    global dragging, last_mouse
    if button == MOUSE_BUTTON_MIDDLE:
        dragging = (action == PRESS)
        if dragging:
            last_mouse = get_cursor_pos(window)

def cursor_pos_callback(window, xpos, ypos):
    global last_mouse
    now = time.time()
    dt = now - Camera.instance()._last_time  # delta temps réel
    if dragging and last_mouse is not None:
        dx = xpos - last_mouse[0]
        dy = ypos - last_mouse[1]
        w, h = backend.get_fbo_screen_size()
        Camera.instance().on_drag(dx, dy, (w, h), dt)
        last_mouse = (xpos, ypos)


class Window:
    """
    Classe utilitaire pour créer et gérer une fenêtre GLFW avec contexte OpenGL.
    """
    
    __instance = None

    def __init__(self, width: int, height: int, title: str):
        """
        Crée une nouvelle fenêtre OpenGL.

        :param width: largeur de la fenêtre en pixels
        :param height: hauteur de la fenêtre en pixels
        :param title: titre de la fenêtre
        """
        # Initialiser la bibliothèque GLFW
        if not init():
            raise Exception("GLFW n'a pas pu être initialisé")
        
        window_hint(CONTEXT_VERSION_MAJOR, 3)
        window_hint(CONTEXT_VERSION_MINOR, 3)
        window_hint(OPENGL_PROFILE, OPENGL_CORE_PROFILE)
        window_hint(VISIBLE,GL_FALSE)
        window_hint(OPENGL_FORWARD_COMPAT, GL_TRUE)

        # Créer la fenêtre
        self.__window = create_window(width, height, title, None, None)
        self.__height = height
        self.__width = width

        if not self.__window:
            terminate()
            raise Exception("La fenêtre GLFW n'a pas pu être créée")

        # Rendre le contexte OpenGL actuel (lié à cette fenêtre)
        make_context_current(self.__window)

        swap_interval(0) # Desactiver la limite de FPS

        # Associer la fonction de callback au redimensionnement
        set_window_size_callback(self.__window, on_resize)
        set_scroll_callback(self.__window,scroll_callback)
        set_mouse_button_callback(self.__window, mouse_button_callback)
        set_cursor_pos_callback(self.__window, cursor_pos_callback)

        # Configurer la vue initiale
        on_resize(self.__window, width, height)
        
        if Window.__instance is not None:
            raise PermissionError("Window ne peux pas avoir plus d'une instance")
        Window.__instance = self

    def show(self):
        show_window(self.__window)

    def supprimer(self):
        """
        Libère les ressources GLFW et ferme la fenêtre.
        """
        terminate()

    def devrait_fermer(self):
        """
        Vérifie si l'utilisateur a demandé la fermeture de la fenêtre.
        :return: True si la fenêtre doit être fermée, False sinon
        """
        return window_should_close(self.__window)

    def getWindow(self):
        return self.__window

    def getTime(self) -> float:
        return get_time()
    
    def getHeight(self) -> float:
        return self.__height
    
    def getWidth(self) -> float:
        return self.__width
    
    height = property(getHeight)
    width = property(getWidth)
    
    @staticmethod
    def instance() -> "Window":
        return Window.__instance # type: ignore