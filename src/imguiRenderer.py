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
                imgui.separator_text("Position")
                pos = [camera.x,camera.y]
                newPos = imgui.slider_float2("Position",pos,-5,5)
                camera.x = newPos[0]
                camera.y = newPos[1]
                camera.enable_position_clamp = imgui.checkbox("Enable Position Clamp",camera.enable_position_clamp)
                camera.position_limit = imgui.slider_float("Position limit",camera.position_limit,10**1,10**10)
                
                imgui.separator_text("Camera & Interpolation")
                camera.zoom_speed = imgui.slider_float("ZoomSpeed",camera.zoom_speed,0.1,10)
                camera.zoom_interp = "exp" if imgui.checkbox("Zoom Exponential Interpolation",camera.zoom_interp=="exp") else "linear"
                camera.zoom_wheel_base  = imgui.slider_float("Zoom Wheel Base",camera.zoom_wheel_base,0.01,2)
                camera.zoom_sensitivity  = imgui.slider_float("Zoom Sensitivity",camera.zoom_sensitivity,0.1,10)
                camera.follow_interp_speed = imgui.slider_float("Follow Interp Speed",camera.follow_interp_speed,1,100)
                camera.zoom_follow_strength = imgui.slider_float("Zoom Follow Strength",camera.zoom_follow_strength,0,2)
                camera.clear_follow_on_input = imgui.checkbox("Clear Follow On Input",camera.clear_follow_on_input)
                
                imgui.separator_text("Dragging")
                camera.pan_speed = imgui.slider_float("Pan Speed",camera.pan_speed,0.1,10)
                camera.pan_smoothing = imgui.slider_float("Pan Smoothing",camera.pan_smoothing,0.0,1)
            if imgui.collapsing_header("OpenGL"):
                imgui.render_texture_information()
                if imgui.button("screenshot"):
                    imgui.open_preview_screenshot()

            if imgui.collapsing_header("Undo/Redo API"):
                t = imgui.get_turtle()
                imgui.text(f"Memory for state: 104o")
                imgui.text(f"Memory for redo stack: {t.get_memory_redo()}o")
                imgui.text(f"Memory for undo stack: {t.get_memory_undo()}o")
                imgui.text(f"Size for redo stack: {t.get_size_redo()}")
                imgui.text(f"Size for undo stack: {t.get_size_undo()}")
                if imgui.button("Undo") and t.can_undo():
                    t.undo()
                if imgui.button("Redo") and t.can_redo():
                    t.redo()

            imgui.end()
