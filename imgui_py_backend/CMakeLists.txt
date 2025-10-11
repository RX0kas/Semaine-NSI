cmake_minimum_required(VERSION 3.21)
project(imgui_py_backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------
# Fix des modules Python
# ---------------------------
cmake_policy(SET CMP0148 NEW)  # Désactive les vieux FindPythonInterp/Libs
set(PYBIND11_FINDPYTHON ON)    # Force pybind11 à utiliser FindPython moderne

# ---------------------------
# Vcpkg
# ---------------------------
set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain")

find_package(pybind11 CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Python COMPONENTS Interpreter Development REQUIRED)

# ---------------------------
# Dossiers include
# ---------------------------
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
        ${Python_INCLUDE_DIRS}
)

# ---------------------------
# Création du module
# ---------------------------
pybind11_add_module(imgui_py_backend
        wrapper.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(imgui_py_backend PRIVATE
        pybind11::module
        glfw
        OpenGL::GL
        ${Python_LIBRARIES}
)

# Supprimer le préfixe "lib" sous Windows
set_target_properties(imgui_py_backend PROPERTIES PREFIX "")

# Pour éviter le _d en debug
set_target_properties(imgui_py_backend PROPERTIES SUFFIX ".pyd")
