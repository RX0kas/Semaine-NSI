import glfw
import src.cpp_backend as imgui
from src.window import Window

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
            t = imgui.Turtle.get_turtle()
            w = Window.instance()
            imgui.begin("Debug")
            if imgui.collapsing_header("Turtle"):
                imgui.separator_text("Turtle CPP")
                imgui.text(f"Angle: {t.angle}")
                imgui.text(f"Positon: ({round(t.x*100,2)},{round(t.y*100,2)})")
                imgui.separator_text("Renderer")
                imgui.text(f"Vertex num: {t.get_vertex_count()}")
                imgui.text(f"Chunk num: {t.path_count}")
                imgui.text(f"Chunk visible num: {t.path_count_visible}")
                imgui.text(f"Chunk size: {imgui.TurtleRenderer.get_size_chunk()}")
                t.show_turtle = imgui.checkbox("Dessine tortue",t.show_turtle)
                t.turtle_size = imgui.slider_float("Taille", t.turtle_size, 0.001, 1)
            if imgui.collapsing_header("Application"):
                imgui.separator_text("Fenetre")
                imgui.text("DeltaTime: {}".format(round(deltaTime,7)))
                imgui.text("FPS: {}".format(round(1/deltaTime,3)))
                imgui.text("Height: {}".format(round(w.height,3)))
                imgui.text("Width: {}".format(round(w.width,3)))
                cursorW, cursorH = glfw.get_cursor_pos(w.instance().getWindow())
                imgui.separator_text("Cursor")
                imgui.text("Height: {}".format(round(cursorH,3)))
                imgui.text("Width: {}".format(round(cursorW,3)))
            if imgui.collapsing_header("ImGui"):
                imgui.draw_imgui_info_widget()
            if imgui.collapsing_header("Camera"):
                pos = [camera.x,camera.y]
                newPos = imgui.slider_float2("Position",pos,-5,5)
                camera.x = newPos[0]
                camera.y = newPos[1]
                camera.target_zoom = imgui.slider_float("Zoom",camera.target_zoom,0.5,10)
                camera.zoom_speed = imgui.slider_float("ZoomSpeed",camera.zoom_speed,0.1,10)
                camera.move_speed = imgui.slider_float("MoveSpeed",camera.move_speed,0.1,100)
                camera.smooth_damping = imgui.slider_float("SmoothDamping",camera.smooth_damping,0.1,1)
                camera.use_old_cam = imgui.checkbox("Ancienne camera",camera.use_old_cam)
                imgui.separator_text("Data")
                if camera._last_mouse_pos==None:
                    imgui.text(f"Last mouse pos: (None;None)")
                    imgui.text(f"Position de la souris en coordonné monde avant le zoom: (None;None)")
                else:
                    imgui.text(f"Last mouse pos: ({camera._last_mouse_pos[0]};{camera._last_mouse_pos[1]})")
                    imgui.text(f"Position de la souris en coordonné monde avant le zoom: ({round(camera._last_mouse_pos[2][0],3)};{round(camera._last_mouse_pos[2][1],3)})")
                
            imgui.end()
