#pragma once

#ifndef IMGUI_PY_BACKEND_TURTLE_RENDERER_HPP
#define IMGUI_PY_BACKEND_TURTLE_RENDERER_HPP

#include <algorithm>
#include <vector>
#include <cmath>
#include <string>

#include "glad/glad.h"

struct PathRange {
    int start;      // index in points
    int length;     // number of points
    float minx, miny, maxx, maxy;
};

class Turtle {
public:
   Turtle(float x_percent = 0.0f, float y_percent = 0.0f, float angle_deg = 0.0f);
   ~Turtle() = default;

   // movement in percent (0..100)
   void forward(float distance_percent);
   void backward(float distance_percent);
   void right(float angle_deg);
   void left(float angle_deg);
   void goto_pos(float x_percent, float y_percent, bool draw = true);

   // pen
   void penup();
   void pendown();

   // accessors
   [[nodiscard]] float getX() const { return x; } // normalized world coords
   [[nodiscard]] float getY() const { return y; }
   [[nodiscard]] float getAngle() const { return angle; }
   void setAngle(float a) {angle=a;normalizeAngle();}
   [[nodiscard]] bool isPenDown() const { return pen_down; }
   [[nodiscard]] bool isShowTurtle() const { return show_turtle; }
   void setShowTurtle(const bool v) {show_turtle = v;}
   [[nodiscard]] float getTurtleSize() const { return turtle_size; }
   void setTurtleSize(const float v) {turtle_size=std::max(0.0f,v);}

   // geometry access
   const std::vector<float>& getVertexBuffer() const { return vertices; } // flat floats
   size_t getVertexCount() const { return vertex_count; }                // number of points
   unsigned int getVisiblePathCount() const { return path_count_visible; }
   void setVisiblePathCount(const unsigned int v) { path_count_visible = v; }
   unsigned int getPathCount() const { return path_count; }
   void setPathCount(const unsigned int v) { path_count = v; }

   // path access
   // fillPathRanges(out, include_open): fills out with PathRange entries (no extra alloc inside if out reserved)
   void fillPathRanges(std::vector<PathRange>& out, bool include_open = true) const;

   // convenience: check if there is open path
   bool hasOpenPath() const { return current_path_start >= 0; }

   void nettoyer();

   // singleton
   static Turtle& instance();

private:
   void normalizeAngle();
   void append_point_world(float wx, float wy);
   void start_new_path_if_needed();
   void finalize_current_path_if_needed();
   std::pair<float,float> last_point() const;

private:
   // normalized internal coords (value/100)
   float x, y;
   float angle;
   bool pen_down;
   bool show_turtle;
   float turtle_size;

   // geometry
   std::vector<float> vertices; // flat floats: x,y,x,y..
   unsigned int path_count_visible;
   unsigned int path_count;
   size_t vertex_count;

   // paths with AABB
   std::vector<PathRange> path_ranges;
   int current_path_start; // -1 if none
   // current open path AABB (updated on append)
   float current_minx, current_miny, current_maxx, current_maxy;

   static Turtle* s_instance;
};

class TurtleRenderer {
public:
   TurtleRenderer();
   ~TurtleRenderer();

   void render();
   void cleanup();

   void setCamera(float camera_x, float camera_y, float zoom_);

   // initialize glad / GL functions if needed (static)
   static bool initializeGlad();

   void ensureInitialized();

   static int getSizeChunk();
private:
   void initializeGLResources();
   void createDefaultShaders();
   void updateViewMatrix(); // fill view_matrix (col-major)
   void drawTurtle(const Turtle &t);

private:
   // GL objects
   GLuint shader_program = 0;
   GLuint path_vao = 0, path_vbo = 0, path_ebo = 0;
   GLuint turtle_vao = 0, turtle_vbo = 0;

   // camera
   float camera_x = 0.0f, camera_y = 0.0f, zoom = 1.0f;
   float view_matrix[9]; // column-major mat3

   bool initialized = false;
   static bool glad_initialized;
   const uint32_t primitive_restart_index = 0xFFFFFFFFu;
};

#endif //IMGUI_PY_BACKEND_TURTLE_RENDERER_HPP