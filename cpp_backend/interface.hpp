#pragma once

#ifndef CPP_BACKEND_INTERFACE_HPP
#define CPP_BACKEND_INTERFACE_HPP

#include <array>

#include "glad/glad.h"

extern float color[3];

void initFBO(int w, int h);


int getFBO();

void registerTexture();

void renderMenuBar();
void renderDockSpace();

void beginShaderPreview();
void endShaderPreview();

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

static unsigned int fractaleNum = 2;
static GLuint prototypeTexture;
static const char* prototypeTexturePath = "prototype.png";
typedef struct {
   GLuint TextureID;
   const char* TexturePath;
   const char* name;
   const char* id;
} Fractale;

#endif //CPP_BACKEND_INTERFACE_HPP