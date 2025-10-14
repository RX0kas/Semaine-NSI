import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(__file__))) # dossier src

import glfw
from src.camera import Camera
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
        self.__camera = Camera()

        glClearColor(0.5, 0.5, 0.5, 1)

    def run(self):
        self.__fenetre.show()

        lastFrame = 0.0

        while not self.__fenetre.devrait_fermer():
            currentFrame = self.__fenetre.getTime()
            deltaTime = currentFrame - lastFrame
            lastFrame = currentFrame

            self.__imguiRenderer.newFrame()
            glClear(GL_COLOR_BUFFER_BIT)

            # Sert a envoyer au shader les donnés de la camera
            # On aurait pu le calculer sur python mais il est trop lent
            self.__turtleRenderer.shader.setVec2f("center",self.__camera.x,self.__camera.y)
            self.__turtleRenderer.shader.setFloat("zoom",self.__camera.zoom)

            self.__turtleRenderer.render()
            self.__imguiRenderer.show_debug_window(deltaTime,self.__camera)

            self.__imguiRenderer.render()
            self.__fenetre.swapbuffer()

        self.__turtleRenderer.cleanup()
        self.__fenetre.supprimer()

    def stop(self):
        glfw.set_window_should_close(self.__fenetre.getWindow(), True)