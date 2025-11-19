#include <iostream>

#include "turtle_renderer.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "interface.hpp"


namespace py = pybind11;


void init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
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
    int display_w, display_h;
    glfwGetWindowSize(glfwGetCurrentContext(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

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

bool checkbox(const std::string& label, bool value) {
    bool v = value;
    ImGui::Checkbox(label.c_str(), &v);
    return v;
}

float slider_float(const std::string& label, float value,
                                   float v_min, float v_max,
                                   const std::string& format = "%.3f", int flags = 0) {
    float v = value;
    ImGui::SliderFloat(label.c_str(), &v, v_min, v_max, format.c_str(), flags);
    return v;
}

std::array<float, 2> slider_float2(const std::string& label,
                                                   std::array<float, 2> value,
                                                   float v_min, float v_max,
                                                   const std::string& format = "%.3f", int flags = 0) {
    float v[2] = {value[0], value[1]};
    ImGui::SliderFloat2(label.c_str(), v, v_min, v_max, format.c_str(), flags);
    return {v[0], v[1]};
}

// MainMenuBar
bool beginMainMenuBar() {
    return ImGui::BeginMainMenuBar();
}

void endMainMenuBar() {
    ImGui::EndMainMenuBar();
}

// --- Module Python ---
PYBIND11_MODULE(cpp_backend, m) {
    m.doc() = "Python bindings pour ImGui avec backends C++ OpenGL3 + GLFW et backend graphique Turtle";
    ///////////////
    // Interface //
    ///////////////
    m.def("get_selected_fractales_id",&getSelectedFractalesID);
    m.def("set_selected_fractales_id",&setSelectedFractalesID,py::arg("id"));
    m.def("render_menu_bar",&renderMenuBar);
    m.def("render_dock_space",&renderDockSpace);
    m.def("get_fbo",&getFBO);
    m.def("begin_render_shader_to_fbo",&beginRenderShaderToFBO);
    m.def("end_render_shader_to_fbo",&endRenderShaderToFBO);
    m.def("init_fbo",&initFBO,py::arg("w"),py::arg("h"));
    m.def("get_profondeur",&getDepth);
    m.def("renderProgressBar",&renderProgressBar);
    m.def("setShowProgress",&setShowProgress,py::arg("show"));

    ///////////
    // ImGui //
    ///////////
    // Shader preview
    m.def("begin_shader_preview",&beginShaderPreview);
    m.def("end_shader_preview",&endShaderPreview);

    m.def("get_fbo_screen_pos",&getFBOPos);
    m.def("get_fbo_screen_size",&getFBOSize);
    m.def("get_frame_size",&getFrameSize);
    m.def("get_menu_bar_height",&getMenuBarHeight);
    // other
    m.def("init_imgui", &init_imgui);
    m.def("destroy_context", &destroy_context);
    m.def("new_frame", &new_frame);
    m.def("render", &render);
    m.def("shutdown", &shutdown);

    m.def("begin", &begin, py::arg("name"));
    m.def("end", &end_window);
    m.def("open_popup",[](const char* name){ImGui::OpenPopup(name);},py::arg("name"));
    m.def("button",[](const char* name){return ImGui::Button(name);},py::arg("name"));
    m.def("text", &text, py::arg("content"));
    m.def("show_demo_window", &show_demo_window, py::arg("show") = true);
    m.def("dock_space", &dockspace);
    m.def("collapsing_header", &collapsing_header, py::arg("label"), py::arg("flags") = 0);
    m.def("separator_text", &separator_text, py::arg("label"));
    m.def("checkbox", &checkbox, py::arg("label"), py::arg("value"));

    // MainMenuBar
    m.def("begin_main_menu_bar",&beginMainMenuBar);
    m.def("end_main_menu_bar",&endMainMenuBar);

    m.def("slider_float", [](const char* label, float v, float v_min, float v_max, const char* format = "%.3f") {
        float val = v;
        ImGui::SliderFloat(label, &val, v_min, v_max, format);
        return val;
    }, py::arg("label"), py::arg("v"), py::arg("v_min"), py::arg("v_max"), py::arg("format") = "%.3f");

    m.def("slider_float2", [](const char* label, std::array<float, 2> values, float v_min, float v_max, const char* format = "%.3f") {
        std::array<float, 2> val = values;
        ImGui::SliderFloat2(label, val.data(), v_min, v_max, format);
        return val;
    }, py::arg("label"), py::arg("values"), py::arg("v_min"), py::arg("v_max"), py::arg("format") = "%.3f");

    m.def("slider_float3", [](const char* label, std::array<float, 3> values, float v_min, float v_max, const char* format = "%.3f") {
        std::array<float, 3> val = values;
        ImGui::SliderFloat3(label, val.data(), v_min, v_max, format);
        return val;
    }, py::arg("label"), py::arg("values"), py::arg("v_min"), py::arg("v_max"), py::arg("format") = "%.3f");

    m.def("slider_float4", [](const char* label, std::array<float, 4> values, float v_min, float v_max, const char* format = "%.3f") {
        std::array<float, 4> val = values;
        ImGui::SliderFloat4(label, val.data(), v_min, v_max, format);
        return val;
    }, py::arg("label"), py::arg("values"), py::arg("v_min"), py::arg("v_max"), py::arg("format") = "%.3f");

    m.def("slider_int", [](const char* label, int v, int v_min, int v_max, const char* format = "%d") {
        int val = v;
        ImGui::SliderInt(label, &val, v_min, v_max, format);
        return val;
    }, py::arg("label"), py::arg("v"), py::arg("v_min"), py::arg("v_max"), py::arg("format") = "%d");

    m.def("slider_int2", [](const char* label, std::array<int, 2> values, int v_min, int v_max, const char* format = "%d") {
        std::array<int, 2> val = values;
        ImGui::SliderInt2(label, val.data(), v_min, v_max, format);
        return val;
    }, py::arg("label"), py::arg("values"), py::arg("v_min"), py::arg("v_max"), py::arg("format") = "%d");

    m.def("slider_int3", [](const char* label, std::array<int, 3> values, int v_min, int v_max, const char* format = "%d") {
        std::array<int, 3> val = values;
        ImGui::SliderInt3(label, val.data(), v_min, v_max, format);
        return val;
    }, py::arg("label"), py::arg("values"), py::arg("v_min"), py::arg("v_max"), py::arg("format") = "%d");

    m.def("slider_int4", [](const char* label, std::array<int, 4> values, int v_min, int v_max, const char* format = "%d") {
        std::array<int, 4> val = values;
        ImGui::SliderInt4(label, val.data(), v_min, v_max, format);
        return val;
    }, py::arg("label"), py::arg("values"), py::arg("v_min"), py::arg("v_max"), py::arg("format") = "%d");


    // ==== Inputs ====
    m.def("input_text", [](const char* label, const std::string& text, ImGuiInputTextFlags flags = 0) {
        std::string buffer = text;
        buffer.resize(256);
        ImGui::InputText(label, buffer.data(), buffer.size(), flags);
        return std::string(buffer);
    }, py::arg("label"), py::arg("text"), py::arg("flags") = 0);

    m.def("input_float", [](const char* label, float v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f") {
        float val = v;
        ImGui::InputFloat(label, &val, step, step_fast, format);
        return val;
    }, py::arg("label"), py::arg("v"), py::arg("step") = 0.0f, py::arg("step_fast") = 0.0f, py::arg("format") = "%.3f");

    m.def("input_int", [](const char* label, int v, int step = 1, int step_fast = 100) {
        int val = v;
        ImGui::InputInt(label, &val, step, step_fast);
        return val;
    }, py::arg("label"), py::arg("v"), py::arg("step") = 1, py::arg("step_fast") = 100);

    m.def("input_float2", [](const char* label, std::array<float, 2> values, const char* format = "%.3f") {
        std::array<float, 2> val = values;
        ImGui::InputFloat2(label, val.data(), format);
        return val;
    }, py::arg("label"), py::arg("values"), py::arg("format") = "%.3f");

    m.def("input_int2", [](const char* label, std::array<int, 2> values) {
        std::array<int, 2> val = values;
        ImGui::InputInt2(label, val.data());
        return val;
    }, py::arg("label"), py::arg("values"));

    // Image
    m.def("image", [](uintptr_t texture_id, std::array<float, 2> size,
                      std::array<float, 2> uv0 = {0.0f, 1.0f},
                      std::array<float, 2> uv1 = {1.0f, 0.0f}) {
        ImGui::Image(
            texture_id,
            ImVec2(size[0], size[1]),
            ImVec2(uv0[0], uv0[1]),
            ImVec2(uv1[0], uv1[1])
        );
    }, py::arg("texture_id"), py::arg("size"),
       py::arg("uv0") = std::array<float, 2>{0.0f, 1.0f},
       py::arg("uv1") = std::array<float, 2>{1.0f, 0.0f});

    m.def("begin_child", [](const std::string& str_id) {
        return ImGui::BeginChild(str_id.c_str());
    }, py::arg("str_id"));

    m.def("end_child", []() {
        ImGui::EndChild();
    });

    m.def("draw_imgui_info_widget",[] {
        ImGui::Text("ImGui version: %s",IMGUI_VERSION);
        bool iht = ImGuiBackendFlags_RendererHasTextures!=0;
        ImGui::Checkbox("IMGUI_HAS_TEXTURES",&iht);
        bool ihv = ImGuiBackendFlags_RendererHasViewports!=0;
        ImGui::Checkbox("IMGUI_HAS_VIEWPORT",&ihv);
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        bool ihd = io.ConfigFlags & ImGuiConfigFlags_DockingEnable;
        ImGui::Checkbox("IMGUI_HAS_DOCK",&ihd);
    });


    ////////////
    // Turtle //
    ////////////
    py::class_<Turtle>(m, "Turtle")
        .def(py::init<float, float, float>(),
             py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("angle") = 0.0f)
        // Mouvements
        .def("forward", &Turtle::forward)
        .def("backward", &Turtle::backward)
        .def("right", &Turtle::right)
        .def("left", &Turtle::left)
        .def("goto", &Turtle::goto_pos, py::arg("x"), py::arg("y"), py::arg("draw") = true)
        // Contrôle du stylo
        .def("penup", &Turtle::penup)
        .def("pendown", &Turtle::pendown)
        .def("up", &Turtle::penup)
        .def("down", &Turtle::pendown)
        // Propriétés
        .def_property_readonly("x", &Turtle::getX)
        .def_property_readonly("y", &Turtle::getY)
        .def_property("angle", &Turtle::getAngle,&Turtle::setAngle)
        .def_property_readonly("pen_down", &Turtle::isPenDown)
        .def_property("show_turtle", &Turtle::isShowTurtle, &Turtle::setShowTurtle)
        .def_property("turtle_size", &Turtle::getTurtleSize, &Turtle::setTurtleSize)
        .def_property_readonly("path_count",&Turtle::getPathCount)
        .def_property_readonly("path_count_visible",&Turtle::getVisiblePathCount)
        // Données
        .def("get_vertices", &Turtle::getVertexBuffer,
             py::return_value_policy::reference_internal)
        .def("get_vertex_count", &Turtle::getVertexCount)
        // Singleton
        .def_static("get_turtle", &Turtle::instance,
                    py::return_value_policy::reference);

    // --- Classe TurtleRenderer ---
    py::class_<TurtleRenderer>(m, "TurtleRenderer")
        .def(py::init<>())
        .def("render", &TurtleRenderer::render)
        .def("cleanup", &TurtleRenderer::cleanup)
        .def("set_camera", &TurtleRenderer::setCamera,
             py::arg("x"), py::arg("y"), py::arg("zoom"))
        .def_static("initialize_glad", &TurtleRenderer::initializeGlad)
        .def_static("get_size_chunk", &TurtleRenderer::getSizeChunk);

    // Fonctions utilitaires globales
    m.def("create_turtle", []() -> Turtle& {
        return Turtle::instance();
    }, py::return_value_policy::reference);

    m.def("get_turtle", []() -> Turtle& {
        return Turtle::instance();
    }, py::return_value_policy::reference);

    m.def("initialize_glad", &TurtleRenderer::initializeGlad);
}
