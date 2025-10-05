import sys
import OpenGL.GL as gl
import glfw

from imgui_bundle import imgui, immapp

def main():
    if not glfw.init():
        print("Impossible d'initialiser GLFW")
        sys.exit(1)

    window = glfw.create_window(1280, 720, "Docking Test - ImGui Bundle", None, None)
    glfw.make_context_current(window)

    imgui.create_context()
    io = imgui.get_io()
    io.config_flags |= imgui.ConfigFlags_.docking_enable
    io.config_flags |= imgui.ConfigFlags_.viewports_enable  # pour tester multi-viewport

    runner_params = immapp.RunnerParams()
    runner_params.app_window_params.window_title = "Docking Test"
    runner_params.app_window_params.window_geometry.size = (1280, 720)

    def gui():
        imgui.dock_space_over_viewport()
        imgui.begin("Contrôles fractale")
        imgui.text("Zoom, itérations...")
        imgui.end()
        imgui.begin("Palette")
        imgui.text("Paramètres couleurs")
        imgui.end()
        imgui.begin("Logs")
        imgui.text("Messages de debug")
        imgui.end()

    # Important : lier ta fonction ici
    runner_params.callbacks.show_gui = gui

    immapp.run(runner_params)

    glfw.terminate()

if __name__ == "__main__":
    main()
