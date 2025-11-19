#include "../turtle_renderer.hpp"
#include <cmath>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <ostream>
#include <sstream>
#include <GLFW/glfw3.h>

#include "interface.hpp"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define DEBUG_LOG(msg) std::cout << "[DEBUG] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl
#define ERROR_LOG(msg) std::cout << "[ERROR] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl

#define SIZE_OF_CHUNK 1024

Turtle* Turtle::s_instance = nullptr;

static inline float percentToWorld(float p) {
    return p * 0.01f;
}

Turtle::Turtle(float ix, float iy, float iangle)
: x(percentToWorld(ix)), y(percentToWorld(iy)), angle(iangle),
  pen_down(true), show_turtle(true), turtle_size(0.05f),
  vertex_count(0), current_path_start(-1),
  current_minx(0), current_miny(0), current_maxx(0), current_maxy(0)
{
    vertices.reserve(4096);

    start_new_path_if_needed();
    append_point_world(x,y);

    if (!s_instance) s_instance = this;
}

Turtle& Turtle::instance() {
    assert(s_instance != nullptr && "Turtle instance not created");
    return *s_instance;
}

void Turtle::append_point_world(float wx, float wy) {
    vertices.push_back(wx);
    vertices.push_back(wy);
    vertex_count += 1;
    // update current open path AABB
    if (current_path_start >= 0) {
        if (vertex_count == static_cast<size_t>(current_path_start) + 1) {
            // first point in this path
            current_minx = current_maxx = wx;
            current_miny = current_maxy = wy;
        } else {
            current_minx = std::min(current_minx, wx);
            current_miny = std::min(current_miny, wy);
            current_maxx = std::max(current_maxx, wx);
            current_maxy = std::max(current_maxy, wy);
        }
    }
}

std::pair<float,float> Turtle::last_point() const {
    if (vertex_count == 0) return {0.0f, 0.0f};
    float lx = vertices[(vertex_count - 1) * 2 + 0];
    float ly = vertices[(vertex_count - 1) * 2 + 1];
    return {lx, ly};
}

void Turtle::start_new_path_if_needed() {
    if (!pen_down) return;
    if (current_path_start == -1) {
        current_path_start = static_cast<int>(vertex_count);
        // set a default AABB to be expanded on first append
        current_minx = current_miny = std::numeric_limits<float>::infinity();
        current_maxx = current_maxy = -std::numeric_limits<float>::infinity();
    }
}

void Turtle::finalize_current_path_if_needed() {
    if (current_path_start >= 0) {
        int len = static_cast<int>(vertex_count) - current_path_start;
        if (len > 0) {
            PathRange pr;
            pr.start = current_path_start;
            pr.length = len;
            pr.minx = current_minx;
            pr.miny = current_miny;
            pr.maxx = current_maxx;
            pr.maxy = current_maxy;
            path_ranges.push_back(pr);
        }
        current_path_start = -1;
        current_minx = current_miny = current_maxx = current_maxy = 0;
    }
}

void Turtle::forward(float distance_percent) {
    float d = percentToWorld(distance_percent);
    float rad = angle * static_cast<float>(M_PI) / 180.0f;
    float nx = x + std::cos(rad) * d;
    float ny = y + std::sin(rad) * d;

    if (pen_down) {
        start_new_path_if_needed();
        // ensure start point present
        if (current_path_start == static_cast<int>(vertex_count)) {
            append_point_world(x, y);
        } else {
            auto lp = last_point();
            if (lp.first != x || lp.second != y) append_point_world(x, y);
        }
        append_point_world(nx, ny);
        if ((vertices.size()/2 - current_path_start) >= SIZE_OF_CHUNK) {
            finalize_current_path_if_needed();
        }
    }
    x = nx; y = ny;


}

void Turtle::backward(float distance_percent) { forward(-distance_percent); }

void Turtle::right(float angle_deg) {
    angle -= angle_deg;
    normalizeAngle();
}
void Turtle::left(float angle_deg) {
    angle += angle_deg;
    normalizeAngle();
}

void Turtle::normalizeAngle() {
    // ramène l’angle dans [0, 360)
    while (angle >= 360.0f) angle -= 360.0f;
    while (angle < 0.0f) angle += 360.0f;
}

void Turtle::goto_pos(float x_percent, float y_percent, bool draw) {
    float nx = percentToWorld(x_percent);
    float ny = percentToWorld(y_percent);

    if (draw && pen_down) {
        start_new_path_if_needed();
        if (current_path_start == static_cast<int>(vertex_count)) {
            append_point_world(x, y);
        } else {
            auto lp = last_point();
            if (lp.first != x || lp.second != y) append_point_world(x,y);
        }
        append_point_world(nx, ny);
    }
    x = nx; y = ny;
}

void Turtle::penup() {
    if (pen_down) {
        finalize_current_path_if_needed();
        pen_down = false;
    }
}

void Turtle::pendown() {
    if (!pen_down) {
        pen_down = true;
        start_new_path_if_needed();
        if (vertex_count == 0) append_point_world(x, y);
        else {
            auto lp = last_point();
            if (lp.first != x || lp.second != y) append_point_world(x, y);
        }
    }
}

void Turtle::nettoyer() {
    vertices.clear();
    path_ranges.clear();
    vertex_count = 0;
    current_path_start = -1;
    current_minx = 0;
    current_miny = 0;
    current_maxx = 0;
    current_maxy = 0;
    path_count_visible = 0;
    path_count = 0;

    vertices.reserve(4096);
    start_new_path_if_needed();
    append_point_world(x,y);
}


// fills vector 'out' with path ranges; include_open adds the current open path if any
void Turtle::fillPathRanges(std::vector<PathRange>& out, bool include_open) const {
    out.clear();
    out.reserve(path_ranges.size() + (include_open && current_path_start>=0 ? 1 : 0));
    for (const auto &p : path_ranges) out.push_back(p);
    if (include_open && current_path_start >= 0) {
        int len = static_cast<int>(vertex_count) - current_path_start;
        if (len > 0) {
            PathRange pr;
            pr.start = current_path_start;
            pr.length = len;
            pr.minx = current_minx;
            pr.miny = current_miny;
            pr.maxx = current_maxx;
            pr.maxy = current_maxy;
            out.push_back(pr);
        }
    }
}

// TurtleRenderer implementation
bool TurtleRenderer::glad_initialized = false;

// Minimal shader sources (very simple)
static const char* vertex_src = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat3 viewmatrix;
void main() {
    vec3 p = viewmatrix * vec3(aPos, 1.0);
    gl_Position = vec4(p.xy, 0.0, 1.0);
}
)";

static const char* fragment_src = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 u_color;
void main() {
    FragColor = u_color;
}
)";

// Simple compile helpers (replace by your project's Shader if available)
static GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len=0; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0'); glGetShaderInfoLog(s, len, nullptr, log.data());
        ERROR_LOG("Shader compile error: " << log);
        glDeleteShader(s);
        return 0;
    }
    return s;
}
static GLuint linkProgram(GLuint v, GLuint f) {
    GLuint p = glCreateProgram();
    glAttachShader(p, v); glAttachShader(p, f);
    glLinkProgram(p);
    GLint ok=0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len=0; glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0'); glGetProgramInfoLog(p, len, nullptr, log.data());
        ERROR_LOG("Program link error: " << log);
        glDeleteProgram(p);
        return 0;
    }
    glDetachShader(p, v); glDetachShader(p, f);
    return p;
}

int TurtleRenderer::getSizeChunk() {
    return SIZE_OF_CHUNK;
}


bool TurtleRenderer::initializeGlad() {
    if (glad_initialized) return true;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
       ERROR_LOG("Failed to initialize GLAD");
        return false;
    }

    glad_initialized = true;
    return true;
}

TurtleRenderer::TurtleRenderer() {
    updateViewMatrix();
}

TurtleRenderer::~TurtleRenderer() {
    cleanup();
}

void TurtleRenderer::ensureInitialized() {
    if (initialized) return;
    initializeGlad();
    createDefaultShaders();
    initializeGLResources();
    updateViewMatrix();
    initialized = true;
}

void TurtleRenderer::createDefaultShaders() {
    if (shader_program != 0) return;
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertex_src);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragment_src);
    if (!vs || !fs) {
        ERROR_LOG("Failed to compile default shaders");
        return;
    }
    shader_program = linkProgram(vs, fs);
    glDeleteShader(vs); glDeleteShader(fs);
    if (!shader_program) ERROR_LOG("Failed to link default shader program");
}

void TurtleRenderer::initializeGLResources() {
    // Create VAO/VBO/EBO for path
    glGenVertexArrays(1, &path_vao);
    glGenBuffers(1, &path_vbo);
    glGenBuffers(1, &path_ebo);

    glBindVertexArray(path_vao);

    glBindBuffer(GL_ARRAY_BUFFER, path_vbo);
    // initial small allocation
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // attribute location 0 -> vec2 aPos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Bind EBO while VAO bound so VAO stores the binding
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, path_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

    // Unbind VAO but keep buffers created
    glBindVertexArray(0);

    // Turtle marker VAO (triangle)
    glGenVertexArrays(1, &turtle_vao);
    glGenBuffers(1, &turtle_vbo);
    glBindVertexArray(turtle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, turtle_vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Ensure primitive restart enabled when drawing; we enable/disable per-frame in render()

    // Interface initialisation
    registerTexture();
}

void TurtleRenderer::cleanup() {
    if (path_ebo) { glDeleteBuffers(1, &path_ebo); path_ebo = 0; }
    if (path_vbo) { glDeleteBuffers(1, &path_vbo); path_vbo = 0; }
    if (path_vao) { glDeleteVertexArrays(1, &path_vao); path_vao = 0; }

    if (turtle_vbo) { glDeleteBuffers(1, &turtle_vbo); turtle_vbo = 0; }
    if (turtle_vao) { glDeleteVertexArrays(1, &turtle_vao); turtle_vao = 0; }

    if (shader_program) { glDeleteProgram(shader_program); shader_program = 0; }
    initialized = false;
}

void TurtleRenderer::setCamera(float camera_x_, float camera_y_, float zoom_) {
    camera_x = camera_x_;
    camera_y = camera_y_;
    zoom = zoom_;
    updateViewMatrix();
}

// view_matrix column-major: [ m00 m10 m20; m01 m11 m21; m02 m12 m22 ] in memory
// we want matrix: [s 0 tx; 0 s ty; 0 0 1] so memory (col-major) = {s,0,0, 0,s,0, tx,ty,1}
void TurtleRenderer::updateViewMatrix() {
    float s = zoom;

    // Translation du modèle vers la caméra
    float tx = -camera_x * s;
    float ty = -camera_y * s;

    // Column-major
    view_matrix[0] = s;     view_matrix[1] = 0.0f; view_matrix[2] = 0.0f;
    view_matrix[3] = 0.0f;  view_matrix[4] = s;    view_matrix[5] = 0.0f;
    view_matrix[6] = tx;    view_matrix[7] = ty;   view_matrix[8] = 1.0f;
}

void TurtleRenderer::drawTurtle(const Turtle &t) {
    // Draw a small triangle oriented by t.getAngle() at (t.getX(), t.getY())
    float size = t.getTurtleSize();
    // triangle in local space
    float hx = size * 0.5f;
    float hy = size * 0.5f;
    float verts[6] = {
        0.0f,  size,
       -hx,   -hy,
        hx,   -hy
    };

    // rotate and translate to world
    float ang = t.getAngle() * 3.14159265f / 180.0f;
    float ca = cosf(ang), sa = sinf(ang);
    float worldVerts[6];
    for (int i = 0; i < 3; ++i) {
        float lx = verts[i*2+0], ly = verts[i*2+1];
        float wx = ca * lx - sa * ly + t.getX();
        float wy = sa * lx + ca * ly + t.getY();
        worldVerts[i*2+0] = wx;
        worldVerts[i*2+1] = wy;
    }

    glUseProgram(shader_program);
    GLint viewLoc = glGetUniformLocation(shader_program, "viewmatrix");
    if (viewLoc != -1) glUniformMatrix3fv(viewLoc, 1, GL_FALSE, view_matrix);

    glBindVertexArray(turtle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, turtle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(worldVerts), worldVerts, GL_DYNAMIC_DRAW);

    glUniform4f(glGetUniformLocation(shader_program, "u_color"), 1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glUseProgram(0);
}

static bool aabbVisible(const PathRange &pr, const float view[9]) {
    // corners in homogeneous coords (x,y,1)
    float corners[4][3] = {
        { pr.minx, pr.miny, 1.0f },
        { pr.maxx, pr.miny, 1.0f },
        { pr.minx, pr.maxy, 1.0f },
        { pr.maxx, pr.maxy, 1.0f }
    };
    float ndc_min_x =  1e9f, ndc_min_y = 1e9f;
    float ndc_max_x = -1e9f, ndc_max_y = -1e9f;

    for (int i = 0; i < 4; ++i) {
        // view is column-major: new = view * vec3
        float tx = view[0]*corners[i][0] + view[3]*corners[i][1] + view[6]*corners[i][2];
        float ty = view[1]*corners[i][0] + view[4]*corners[i][1] + view[7]*corners[i][2];
        float tw = view[2]*corners[i][0] + view[5]*corners[i][1] + view[8]*corners[i][2];
        if (tw == 0.0f) tw = 1e-6f; // avoid div by zero
        float nx = tx / tw;
        float ny = ty / tw;
        ndc_min_x = std::min(ndc_min_x, nx);
        ndc_min_y = std::min(ndc_min_y, ny);
        ndc_max_x = std::max(ndc_max_x, nx);
        ndc_max_y = std::max(ndc_max_y, ny);
    }
    // quick reject if outside [-1,1] in x or y
    if (ndc_max_x < -1.0f || ndc_min_x > 1.0f) return false;
    if (ndc_max_y < -1.0f || ndc_min_y > 1.0f) return false;
    return true;
}


void TurtleRenderer::render() {
    ensureInitialized();

    Turtle &t = Turtle::instance();
    size_t vcount = t.getVertexCount();
    if (vcount == 0) {
        if (t.isShowTurtle()) drawTurtle(t);
        return;
    }

    // 1) get path ranges (including open path)
    std::vector<PathRange> paths;
    t.fillPathRanges(paths, true);
    if (paths.empty()) {
        DEBUG_LOG("No path ranges returned");
        if (t.isShowTurtle()) drawTurtle(t);
        return;
    }

    // 2) cull per path using AABB and view_matrix
    std::vector<const PathRange*> visible_paths;
    visible_paths.reserve(paths.size());
    for (const auto &pr : paths) {
        if (aabbVisible(pr, view_matrix)) visible_paths.push_back(&pr);
    }
    if (visible_paths.empty()) {
        // nothing visible, but draw turtle
        if (t.isShowTurtle()) drawTurtle(t);
        return;
    }
    t.setPathCount(paths.size());
    t.setVisiblePathCount(visible_paths.size());

    // 3) build staging (only visible vertices) and indices local (primitive-restart)
    const auto &vb = t.getVertexBuffer(); // flat floats
    std::vector<float> staging;
    std::vector<uint32_t> indices;
    staging.reserve(1024);
    indices.reserve(1024);

    uint32_t curIndex = 0;
    for (const PathRange* pptr : visible_paths) {
        const PathRange &pr = *pptr;
        int startPt = pr.start;
        int lenPts = pr.length;
        if (lenPts <= 1) continue;

        // copy vertices for this path into staging
        int startFloat = startPt * 2;
        int floatCount = lenPts * 2;
        staging.insert(staging.end(), &vb[startFloat], &vb[startFloat + floatCount]);

        // add indices (rebased)
        for (int i = 0; i < lenPts; ++i) {
            indices.push_back(curIndex++);
        }
        // add restart
        indices.push_back(primitive_restart_index);
    }

    if (indices.empty()) {
        if (t.isShowTurtle()) drawTurtle(t);
        return;
    }

    // 4) upload + draw
    glUseProgram(shader_program);
    GLint viewLoc = glGetUniformLocation(shader_program, "viewmatrix");
    if (viewLoc != -1) glUniformMatrix3fv(viewLoc, 1, GL_FALSE, view_matrix);

    glBindVertexArray(path_vao);

    // VBO (orphan + subdata)
    GLsizeiptr vbytes = static_cast<GLsizeiptr>(staging.size() * sizeof(float));
    glBindBuffer(GL_ARRAY_BUFFER, path_vbo);
    glBufferData(GL_ARRAY_BUFFER, vbytes, nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vbytes, staging.data());

    // EBO
    GLsizeiptr ibytes = static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, path_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibytes, nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ibytes, indices.data());

    // primitive restart draw
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(primitive_restart_index);

    glUniform4f(glGetUniformLocation(shader_program, "u_color"), color[0], color[1], color[2], 1.0f);
    GLsizei indices_count = static_cast<GLsizei>(indices.size());
    glDrawElements(GL_LINE_STRIP, indices_count, GL_UNSIGNED_INT, static_cast<const void *>(nullptr));

    glDisable(GL_PRIMITIVE_RESTART);

    // 5) turtle marker
    if (t.isShowTurtle()) drawTurtle(t);

    // cleanup
    glBindVertexArray(0);
    glUseProgram(0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        ERROR_LOG("GL error after render: " << err);
    }
}