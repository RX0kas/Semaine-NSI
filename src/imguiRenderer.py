import glfw
from imgui_bundle import imgui, imgui_ctx
from src.backend import glfw_backend
from src.returtle import Turtle

class ImGuiRenderer:
    def __init__(self, window):
        imgui.create_context()
        
        self.__GLFWimpl = glfw_backend.GlfwRenderer(window)
        io = imgui.get_io()
        
        io.config_flags |= imgui.ConfigFlags_.docking_enable
        io.config_flags |= imgui.ConfigFlags_.viewports_enable
        
        self.__window = window
        self.__showDebug = True
        self.__lastPressed = 0.0
        self.__timer = 0.1  # anti-spam clavier

    def newFrame(self):
        imgui.new_frame()

        if glfw.get_time()-self.__lastPressed>self.__timer and glfw.get_key(self.__window,glfw.KEY_F3)==glfw.PRESS:
            self.__showDebug = not self.__showDebug
            self.__lastPressed = glfw.get_time()

    def endFrame(self):
        imgui.render()
        self.__GLFWimpl.render(imgui.get_draw_data())        
        backup_current_context = glfw.get_current_context()
        imgui.update_platform_windows()
        imgui.render_platform_windows_default()
        glfw.make_context_current(backup_current_context)
        

    def show_debug_window(self,deltaTime,camera):
        if self.__showDebug:
            t = Turtle.get_turtle()
            imgui.begin("Debug")
            if imgui.collapsing_header("Turtle"):
                imgui.separator_text("ReTurtle")
                imgui.text(f"Angle: {t.angle}")
                imgui.text(f"Positon: ({round(t.x*100,2)},{round(t.y*100,2)})")
                imgui.separator_text("Renderer")
                imgui.text(f"Vertex num: {len(t.get_vertices())}")
                _, t.show_turtle = imgui.checkbox("Dessine tortue",t.show_turtle)
                _, t.turtle_size = imgui.slider_float("Taille", t.turtle_size, 0.001, 1)
            if imgui.collapsing_header("Application"):
                imgui.text("DeltaTime: {}".format(round(deltaTime,3)))
                imgui.text("FPS: {}".format(round(1/deltaTime,3)))
            if imgui.collapsing_header("Camera"):
                pos = [camera.x,camera.y]
                _, newPos = imgui.slider_float2("Position",pos,-5,5)
                camera.x = newPos[0]
                camera.y = newPos[1]
                _, camera.zoom = imgui.slider_float("Zoom",camera.zoom,0.5,10)
            imgui.end()
