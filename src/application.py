import OpenGL.GL
import glfw
from src.camera import Camera
from src.interface import Interface
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
        self.__turtleRenderer = backend.TurtleRenderer("D:\\NSI\\SemaineNSI\\shaders\\turtle.frag.glsl","D:\\NSI\\SemaineNSI\\shaders\\turtle.vert.glsl")
        self.__imguiRenderer = ImGuiRenderer(win)
        self.__turtle = backend.Turtle()
        self.__turtle.show_turtle = True
        self.__turtle.turtle_size = 0.05
        self.__camera = Camera(win)
        self.__turtleRenderer.set_camera(self.__camera.x,self.__camera.y,self.__camera.zoom)

        backend.init_fbo(self.__fenetre.width, self.__fenetre.height)

        from OpenGL.GL import glClearColor
        glClearColor(0.5, 0.5, 0.5, 1)

        self.__interface = Interface()

    def run(self):
        self.__fenetre.show()

        # TODO: réallouer la texture FBO quand la taille change

        lastFrame = 0.0
        while not self.__fenetre.devrait_fermer():
            glfw.poll_events()
            currentFrame = self.__fenetre.getTime()
            deltaTime = currentFrame - lastFrame
            lastFrame = currentFrame

            # Update functions
            backend.update_color_edit_timer(deltaTime)
            self.__camera.update()
            self.__interface.update()

            self.__turtleRenderer.set_camera(
                self.__camera.x,
                self.__camera.y,
                self.__camera.zoom
            )

            self.__imguiRenderer.newFrame()

            backend.render_dock_space()
            # start DrawShaderWindow
            backend.begin_shader_preview()
            backend.render_menu_bar()

            backend.begin_render_shader_to_fbo()
            self.__turtleRenderer.render()
            backend.end_render_shader_to_fbo()

            backend.end_shader_preview() # end DrawShaderWindow

            self.__interface.render()
            self.__imguiRenderer.show_debug_window(deltaTime, self.__camera)
            backend.render_preview_screenshot()

            OpenGL.GL.glBindFramebuffer(OpenGL.GL.GL_FRAMEBUFFER, 0)
            self.__imguiRenderer.render()

            glfw.swap_buffers(self.__fenetre.getWindow())


        self.__imguiRenderer.shutdown()
        self.__turtleRenderer.cleanup()
        self.__fenetre.supprimer()
        print("Stop")

    def stop(self):
        glfw.set_window_should_close(self.__fenetre.getWindow(), True)