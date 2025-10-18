import glfw
from src.returtle import Turtle
import src.imgui_py_backend as imgui

class ImGuiRenderer:
    def __init__(self, window):
        imgui.init_imgui()
        
        self.__window = window
        self.__showDebug = True
        self.__lastPressed = 0.0
        self.__timer = 0.1  # anti-spam clavier

    def newFrame(self):
        imgui.new_frame()

        if glfw.get_time()-self.__lastPressed>self.__timer and glfw.get_key(self.__window,glfw.KEY_F3)==glfw.PRESS:
            self.__showDebug = not self.__showDebug
            self.__lastPressed = glfw.get_time()

    def render(self):
        imgui.render()

    def shutdown(self):
        imgui.shutdown()

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
            if imgui.collapsing_header("ImGui"):
                imgui.draw_imgui_info_widget()
            if imgui.collapsing_header("Camera"):
                pos = [camera.x,camera.y]
                _, newPos = imgui.slider_float2("Position",pos,-5,5)
                camera.x = newPos[0]
                camera.y = newPos[1]
                _, camera.zoom = imgui.slider_float("Zoom",camera.zoom,0.5,10)
            imgui.end()
