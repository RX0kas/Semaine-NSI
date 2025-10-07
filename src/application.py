import glfw

from src.window import Window
from src.turtleRenderer import TurtleRenderer
from src.returtle import Turtle
from src.imguiRenderer import ImGuiRenderer
from OpenGL.GL import *

class Application:
    def __init__(self):
        self.__fenetre = Window(800, 600, "Test fenetre")
        self.__turtleRenderer = TurtleRenderer()
        self.__imguiRenderer = ImGuiRenderer(self.__fenetre.getWindow())
        self.__turtle = Turtle()

        glClearColor(0.5, 0.5, 0.5, 1)

    def run(self):
        self.__fenetre.show()

        while not self.__fenetre.devrait_fermer():
            self.__imguiRenderer.newFrame()
            glClear(GL_COLOR_BUFFER_BIT)

            self.__turtleRenderer.render()
            self.__imguiRenderer.show_debug_window()

            self.__imguiRenderer.endFrame()
            self.__fenetre.swapbuffer()

        self.__turtleRenderer.cleanup()
        self.__fenetre.supprimer()

    def stop(self):
        glfw.set_window_should_close(self.__fenetre.getWindow(), True)
