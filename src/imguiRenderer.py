import glfw
from time import time
from imgui_bundle import imgui, hello_imgui

# TODO: Reecrire entierement le backend:
# OpenGL: https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_opengl3.cpp et https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_opengl3.h
# GLFW: https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_glfw.cpp et https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_glfw.h
class ImGuiRenderer:
    def __init__(self, window):
        pass
        """imgui.create_context()

        io = imgui.get_io()
        io.config_flags |= imgui.ConfigFlags_.docking_enable
        io.config_flags |= imgui.ConfigFlags_.viewports_enable

        self.__window = window
        self.__showDebug = False
        self.__lastPressed = 0.0
        self.__timer = 0.1  # anti-spam clavier"""

    def newFrame(self):
        pass

    def endFrame(self):
        pass

    def show_debug_window(self):
        """if self.__showDebug:
            imgui.begin("Debug")
            imgui.text("Debug ON (press F3 to toggle)")
            imgui.end()"""
        pass
