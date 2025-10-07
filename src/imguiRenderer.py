import glfw
from imgui_bundle import imgui, imgui_ctx
from imgui_bundle.python_backends import glfw_backend
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

    def endFrame(self):
        imgui.render()
        self.__GLFWimpl.render(imgui.get_draw_data())        
        backup_current_context = glfw.get_current_context()
        imgui.update_platform_windows()
        imgui.render_platform_windows_default()
        glfw.make_context_current(backup_current_context)
        

    def show_debug_window(self,t:Turtle):
        if self.__showDebug:
            imgui.begin("Debug")
            if imgui.collapsing_header("Turtle"):
                imgui.separator_text("ReTurtle")
                imgui.text(f"Angle: {t.angle}")
                imgui.text(f"Positon: ({round(t.x*100,2)},{round(t.y*100,2)}")
                _, t.pen_down = imgui.checkbox("Dessine: ",t.pen_down)
                imgui.separator_text("Renderer")
                imgui.text(f"Vertex num: {len(t.get_vertices())}")
                _, t.show_turtle = imgui.checkbox("Dessine tortue: ",t.show_turtle)
            imgui.end()
