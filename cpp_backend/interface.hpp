#pragma once

#ifndef CPP_BACKEND_INTERFACE_HPP
#define CPP_BACKEND_INTERFACE_HPP

#include <array>
#include <pybind11/functional.h>

#include "imgui.h"
#include "glad/glad.h"

extern bool showScreenshotPreview;
void openScreenshotPreview();

void updateColorEditTimer(float delta_time);

void initFBO(int w, int h);

void setExplorerClickedCallback(const std::function<void(int,int)> &callback);

int getFBO();

void registerTexture();
void saveFBOToFile(const char* filename);

void renderScreenshotPreview();

void renderMenuBar();
void renderDockSpace();

void beginShaderPreview();
void endShaderPreview();

void renderTextureInformation();

void beginRenderShaderToFBO();
void endRenderShaderToFBO();

const char* getSelectedFractalesID();
void setSelectedFractalesID(const char* id);
std::array<float, 2> getFBOPos();
std::array<float, 2> getFBOSize();
std::array<int, 2> getFrameSize();
float getMenuBarHeight();

void setShowProgress(bool show);
void renderProgressBar();

int getDepth();

static GLuint prototypeTexture;
static const char* prototypeTexturePath = "prototype.png";
typedef struct {
   GLuint TextureID;
   const char* TexturePath;
   const char* name;
   const char* id;
} Fractale;

static std::vector<Fractale> fractales;

//void addFractale(const char* id,const char* name,const char* path);

#endif //CPP_BACKEND_INTERFACE_HPP