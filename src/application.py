import glfw
from src.camera import Camera
from src.interface import Interface
from src.window import Window
from src.imguiRenderer import ImGuiRenderer

import src.cpp_backend as backend
from OpenGL.GL import *
from numpy import frombuffer,flip,uint8,reshape

class Application:
    def initialise_fractales(self):
        #backend.ajouter_fractale("arbre","Arbre","prototype.png")
        #backend.ajouter_fractale("arbre3","Arbre à 3 branches","prototype.png")
        #backend.ajouter_fractale("courbe_de_koch_quadratique","Courbe de Koch Quadratique","prototype.png")
        #backend.ajouter_fractale("courbe_de_koch_quadratique_inv","Courbe de Koch Quadratique Inversé","prototype.png")
        #backend.ajouter_fractale("flocon_koch","Flocon de Koch","prototype.png")
        #backend.ajouter_fractale("ligne_koch","Ligne de Koch","prototype.png")
        pass


    def __init__(self):
        self.__fenetre = Window(800, 600, "Test fenetre")
        win = self.__fenetre.getWindow()
        glfw.make_context_current(win)
        backend.TurtleRenderer.initialize_glad()
        glfw.make_context_current(win)
        self.initialise_fractales()

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
        self.__camera = Camera(win)
        self.__turtleRenderer.set_camera(self.__camera.x,self.__camera.y,self.__camera.zoom)

        backend.init_fbo(self.__fenetre.width, self.__fenetre.height)

        from OpenGL.GL import glClearColor
        glClearColor(0.5, 0.5, 0.5, 1)

        self.__interface = Interface()
        

    def run(self):
        self.__fenetre.show()
        
        should_screen = False
        lastFrame = 0.0
        while not self.__fenetre.devrait_fermer():
            glfw.poll_events()
            currentFrame = self.__fenetre.getTime()
            deltaTime = currentFrame - lastFrame
            lastFrame = currentFrame

            # Update functions
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
            should_screen = False
            backend.begin("test")
            if backend.button("Save Screenshot"):
                should_screen = True
            backend.end()
            self.__imguiRenderer.render()
            
            glfw.swap_buffers(self.__fenetre.getWindow())
            if should_screen:
                self.save_screenshot("screenshot.png")


        self.__imguiRenderer.shutdown()
        self.__turtleRenderer.cleanup()
        self.__fenetre.supprimer()
        print("Stop")

    def stop(self):
        glfw.set_window_should_close(self.__fenetre.getWindow(), True)
        
    def save_screenshot(self, filename):
        # obtenir la taille du framebuffer
        w,h = backend.get_frame_size()
        # lire les pixels
        glPixelStorei(GL_PACK_ALIGNMENT, 1)
        pixels = glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE)
        from PIL import Image
        arr = frombuffer(pixels,dtype=uint8,count=-1,offset=0).reshape(h, w, 4)
        arr = flip(arr) # renverser parce que OpenGL lit du bas vers le haut
        img = Image.fromarray(arr, 'RGBA')
        img.save(filename)
        print(f"Screenshoot saved to {filename}")