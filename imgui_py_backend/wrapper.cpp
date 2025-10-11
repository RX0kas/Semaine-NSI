#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

namespace py = pybind11;


void init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void destroy_context() {
    ImGui::DestroyContext();
}

void new_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void render() {
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/////////////////////////////////////////////////////

void begin(const std::string& name) {
    ImGui::Begin(name.c_str());
}

void end_window() {
    ImGui::End();
}

void text(const std::string& content) {
    ImGui::Text("%s", content.c_str());
}

void show_demo_window(bool show = true) {
    static bool open = show;
    ImGui::ShowDemoWindow(&open);
}

// Simple fonction de dockspace global
void dockspace() {
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
}

bool collapsing_header(const std::string& label, int flags = 0) {
    return ImGui::CollapsingHeader(label.c_str(), flags);
}

void separator_text(const std::string& label) {
    ImGui::SeparatorText(label.c_str());
}

std::pair<bool, bool> checkbox(const std::string& label, bool value) {
    bool v = value;
    bool changed = ImGui::Checkbox(label.c_str(), &v);
    return {changed, v};
}

std::pair<bool, float> slider_float(const std::string& label, float value,
                                   float v_min, float v_max,
                                   const std::string& format = "%.3f", int flags = 0) {
    float v = value;
    bool changed = ImGui::SliderFloat(label.c_str(), &v, v_min, v_max, format.c_str(), flags);
    return {changed, v};
}

std::pair<bool, std::array<float, 2>> slider_float2(const std::string& label,
                                                   std::array<float, 2> value,
                                                   float v_min, float v_max,
                                                   const std::string& format = "%.3f", int flags = 0) {
    float v[2] = {value[0], value[1]};
    bool changed = ImGui::SliderFloat2(label.c_str(), v, v_min, v_max, format.c_str(), flags);
    return {changed, {v[0], v[1]}};
}



// --- Module Python ---
PYBIND11_MODULE(imgui_py_backend, m) {
    m.doc() = "Python bindings pour ImGui avec backends C++ OpenGL3 + GLFW";

    m.def("init_imgui", &init_imgui);
    m.def("destroy_context", &destroy_context);
    m.def("new_frame", &new_frame);
    m.def("render", &render);
    m.def("shutdown", &shutdown);

    m.def("begin", &begin, py::arg("name"));
    m.def("end", &end_window);
    m.def("text", &text, py::arg("content"));
    m.def("show_demo_window", &show_demo_window, py::arg("show") = true);
    m.def("dock_space", &dockspace);
    m.def("collapsing_header", &collapsing_header, py::arg("label"), py::arg("flags") = 0);
    m.def("separator_text", &separator_text, py::arg("label"));
    m.def("checkbox", &checkbox, py::arg("label"), py::arg("value"));
    m.def("slider_float", &slider_float, py::arg("label"), py::arg("value"),
          py::arg("v_min"), py::arg("v_max"),
          py::arg("format") = "%.3f", py::arg("flags") = 0);
    m.def("slider_float2", &slider_float2, py::arg("label"), py::arg("value"),
          py::arg("v_min"), py::arg("v_max"),
          py::arg("format") = "%.3f", py::arg("flags") = 0);
}
