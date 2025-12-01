cmake_minimum_required(VERSION 3.21)
project(cpp_backend LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain")

# === Python 3.11.9 detection ===
set(Python_ROOT_DIR "C:/Users/natha/AppData/Local/Programs/Python/Python311")
set(Python_EXECUTABLE "${Python_ROOT_DIR}/python.exe")
set(Python_INCLUDE_DIR "${Python_ROOT_DIR}/include")
set(Python_LIBRARY "${Python_ROOT_DIR}/libs/python311.lib")

# === Force CMake to use your system Python 3.11 instead of vcpkg's one ===
set(VCPKG_PREFER_SYSTEM_LIBS ON)
set(Python3_ROOT_DIR "C:/Users/natha/AppData/Local/Programs/Python/Python311")
set(Python3_EXECUTABLE "${Python3_ROOT_DIR}/python.exe")

# Force CMake to ignore vcpkg Python
set(Python3_FIND_REGISTRY LAST)
set(Python3_FIND_VIRTUALENV FIRST)
set(Python3_FIND_STRATEGY LOCATION)
set(Python3_FIND_IMPLEMENTATIONS "CPython")

find_package(Python3 3.11 EXACT COMPONENTS Interpreter Development REQUIRED)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# === Try to find pybind11 ===
execute_process(
        COMMAND "${Python3_EXECUTABLE}" -m pybind11 --cmakedir
        OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
list(APPEND CMAKE_PREFIX_PATH "${PYBIND11_CMAKE_DIR}")
find_package(pybind11 CONFIG REQUIRED)


# If not found, fallback to local git clone
if(NOT pybind11_FOUND)
    message(WARNING "pybind11 not found via vcpkg — falling back to local clone...")
    add_subdirectory("D:/NSI/pybind11")
endif()

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
        include
        ${Python_INCLUDE_DIR}
)

# === Sources ===
set(SRC_FILES
        wrapper.cpp
        turtle_renderer.cpp
        turtle_renderer.hpp
        interface.cpp
        interface.hpp
        shader.cpp
        shader.hpp
        glad.c
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)

# === Create module ===
if(COMMAND pybind11_add_module)
    message(STATUS "Using pybind11_add_module")
    pybind11_add_module(cpp_backend ${SRC_FILES})
else()
    message(WARNING "pybind11_add_module not found — using fallback add_library()")
    add_library(cpp_backend MODULE ${SRC_FILES})
    target_include_directories(cpp_backend PRIVATE ${Python_INCLUDE_DIR})
endif()

target_link_libraries(cpp_backend PRIVATE
        glfw
        OpenGL::GL
        ${Python_LIBRARY}
)

# === Output config ===
set_target_properties(cpp_backend PROPERTIES
        PREFIX ""
        SUFFIX ".pyd"
)

add_subdirectory(nativefiledialog-extended)
target_link_libraries(cpp_backend PRIVATE nfd)