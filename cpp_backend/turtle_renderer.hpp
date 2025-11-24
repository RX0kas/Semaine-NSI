#pragma once

#ifndef IMGUI_PY_BACKEND_TURTLE_RENDERER_HPP
#define IMGUI_PY_BACKEND_TURTLE_RENDERER_HPP

#include <algorithm>
#include <vector>
#include <cmath>
#include <string>
#include <pybind11/detail/common.h>

#include "glad/glad.h"

struct PathRange {
    int start;      // index in points
    int length;     // number of points
    float minx, miny, maxx, maxy;
};

struct State {
   std::vector<float> vertices;
   std::vector<PathRange> path_ranges;
   size_t vertex_count;
   int current_path_start;
   float current_minx, current_miny, current_maxx, current_maxy;
   float x, y, angle;
   bool pen_down;
   bool show_turtle;
   float turtle_size;
   unsigned int path_count_visible;
   unsigned int path_count;
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

   // undo / redo API
   void undo();
   void redo();
   bool canUndo() const;
   bool canRedo() const;
   void clearUndoHistory();         // clear stacks
   void setUndoLimit(size_t limit) { undo_limit = limit; }
   std::vector<State>* getRedoStack() {return &redo_stack;}
   int getMemoryRedoStack() {
      size_t total = 0;
      total += sizeof(redo_stack); // overhead std::vector (pointeur+size+capacity)

      // Pour chaque State, calcul précis
      for (const auto& st : redo_stack) {
         total += memorySizeOfState(st);
      }

      // Ajouter mémoire allouée par redo_stack lui-même
      total += redo_stack.capacity() * sizeof(State);

      return total;
   }
   int getMemoryUndoStack() {
      size_t total = 0;
      total += sizeof(undo_stack); // overhead std::vector (pointeur+size+capacity)

      // Pour chaque State, calcul précis
      for (const auto& st : undo_stack) {
         total += memorySizeOfState(st);
      }

      // Ajouter mémoire allouée par undo_stack lui-même
      total += undo_stack.capacity() * sizeof(State);

      return total;
   }
   int getSizeRedoStack() {return redo_stack.size();}
   int getSizeUndoStack() {return undo_stack.size();}
   void saveStateForUndo();

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
   static size_t memorySizeOfState(const State& s)
   {
      size_t total = 0;

      // Overhead fixe du struct (pointeurs, ints, floats, etc.)
      total += sizeof(State);

      // ---- vertices ----
      // capacity() est ce qui occupe de la mémoire dans le heap
      total += s.vertices.capacity() * sizeof(float);

      // ---- path_ranges ----
      total += s.path_ranges.capacity() * sizeof(PathRange);

      return total;
   }

   void normalizeAngle();
   void append_point_world(float wx, float wy);
   void start_new_path_if_needed();
   void finalize_current_path_if_needed();
   std::pair<float,float> last_point() const;

   // Undo helpers
   void applyState(const State &s);

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

   // Undo/Redo stacks
   std::vector<State> undo_stack;
   std::vector<State> redo_stack;
   size_t undo_limit = 100; // default limit
   bool last_undo_is_clear = false; // Give if the last State in the undo_stack is created by the "nettoyer()" function

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