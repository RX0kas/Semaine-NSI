import glfw
from src.camera import Camera
from src.window import Window
from src.imguiRenderer import ImGuiRenderer

import src.cpp_backend as backend

class Application:
    def __init__(self):
        self.__fenetre = Window(800, 600, "Test fenetre")
        win = self.__fenetre.getWindow()
        glfw.make_context_current(win)
        backend.TurtleRenderer.initialize_glad()
        glfw.make_context_current(win)

        from OpenGL.GL import glGetString, GL_VERSION
        try:
            print("PyOpenGL GL_VERSION:", glGetString(GL_VERSION))
        except Exception as e:
            print("PyOpenGL can't query GL_VERSION yet:", e)
        self.__turtleRenderer = backend.TurtleRenderer()
        self.__imguiRenderer = ImGuiRenderer(win)
        self.__turtle = backend.Turtle()
        self.__turtle.show_turtle = True
        self.__turtle.turtle_size = 0.05
        self.__camera = Camera()
        self.__turtleRenderer.set_camera(self.__camera.x,self.__camera.y,self.__camera.zoom)
        from OpenGL.GL import glClearColor
        glClearColor(0.5, 0.5, 0.5, 1)

    def run(self):
        self.__fenetre.show()

        lastFrame = 0.0
        from OpenGL.GL import glClear,GL_COLOR_BUFFER_BIT
        while not self.__fenetre.devrait_fermer():
            currentFrame = self.__fenetre.getTime()
            deltaTime = currentFrame - lastFrame
            lastFrame = currentFrame

            # Update functions
            # TODO: faire un system d'event
            self.__camera.update()

            self.__imguiRenderer.newFrame()

            glClear(GL_COLOR_BUFFER_BIT)

            # Sert a envoyer au shader les donnés de la camera
            self.__turtleRenderer.set_camera(
                self.__camera.x,
                self.__camera.y,
                self.__camera.zoom
            )
            self.__turtleRenderer.render()

            self.__imguiRenderer.show_debug_window(deltaTime,self.__camera)

            self.__imguiRenderer.render()
            self.__fenetre.swapbuffer()

        self.__imguiRenderer.shutdown()
        self.__turtleRenderer.cleanup()
        self.__fenetre.supprimer()

    def stop(self):
        glfw.set_window_should_close(self.__fenetre.getWindow(), True)