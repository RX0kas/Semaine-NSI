#include "interface.hpp"
#define STB_IMAGE_IMPLEMENTATION

#include "imgui.h"
#include <iostream>
#include <ostream>
#include <string>
#include <GLFW/glfw3.h>

#include "imgui_internal.h"
#include "stb_image.h"
#include "turtle_renderer.hpp"

Fractale arbre = {prototypeTexture,prototypeTexturePath,"Arbre","arbre"};
Fractale arbre3 = {prototypeTexture,prototypeTexturePath,"Arbre à 3 branches","arbre3"};
Fractale courbe_de_koch_quadratique = {prototypeTexture,prototypeTexturePath,"Courbe de Koch Quadratique","courbe_de_koch_quadratique"};
Fractale courbe_de_koch_quadratique_inv = {prototypeTexture,prototypeTexturePath,"Courbe de Koch Quadratique Inversé","courbe_de_koch_quadratique_inv"};
Fractale flocon_koch = {prototypeTexture,prototypeTexturePath,"Flocon de Koch","flocon_koch"};
Fractale ligne_koch = {prototypeTexture,prototypeTexturePath,"Ligne de Koch","ligne_koch"};

GLuint fbo = 0;
int getFBO() {
   return fbo;
}

GLuint colorTex = 0;

void initFBO(int w, int h) {
   if (fbo != 0) {
      glDeleteFramebuffers(1, &fbo);
      fbo = 0;
   }

   if (colorTex != 0) {
      glDeleteTextures(1, &colorTex);
      colorTex = 0;
   }

   glGenFramebuffers(1, &fbo);
   glGenTextures(1, &colorTex);

   glBindTexture(GL_TEXTURE_2D, colorTex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

   GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   if (status != GL_FRAMEBUFFER_COMPLETE)
      printf("FBO error\n");

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void on_resize(int width, int height) {

}


static ImVec2 availSize = ImVec2(-1, -1);

void beginShaderPreview() {
   ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;

   ImGui::Begin("Explorateur", nullptr, winFlags);
}

static ImVec2 g_fbo_pos;
static ImVec2 g_fbo_size;


void endShaderPreview() {
   if (availSize.x > 0 && availSize.y > 0) {
      ImTextureID texID = (ImTextureID)(intptr_t)colorTex;
      ImGui::Image(texID, availSize, ImVec2(0, 1), ImVec2(1, 0));
   } else {
      ImGui::Text("No preview available");
   }

   g_fbo_pos  = ImGui::GetItemRectMin();
   g_fbo_size = ImGui::GetItemRectSize();
   ImGui::End();
}

std::array<float, 2> getFBOPos() {
   std::array<float, 2> val = {g_fbo_pos.x, g_fbo_pos.y};
   return val;
}

std::array<float, 2> getFBOSize() {
   std::array<float, 2> val = {g_fbo_size.x, g_fbo_size.y};
   return val;
}


void renderDockSpace()
{
   ImGuiViewport* vp = ImGui::GetMainViewport();
   ImGui::SetNextWindowPos(vp->Pos);
   ImGui::SetNextWindowSize(vp->Size);
   ImGui::SetNextWindowViewport(vp->ID);

   ImGuiWindowFlags host_flags =
       ImGuiWindowFlags_NoTitleBar |
       ImGuiWindowFlags_NoCollapse |
       ImGuiWindowFlags_NoResize |
       ImGuiWindowFlags_NoMove |
       ImGuiWindowFlags_NoBringToFrontOnFocus |
       ImGuiWindowFlags_NoNavFocus;

   // remove padding so dockspace fills whole viewport
   ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
   ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
   ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
   ImGui::Begin("MainDockspace", nullptr, host_flags);
   ImGui::PopStyleVar(3);

   // Create the DockSpace (use size 0,0 so it fills the host window)
   ImGuiID dockID = ImGui::GetID("MyDockspace");
   ImGui::DockSpace(dockID, ImVec2(0,0), ImGuiDockNodeFlags_PassthruCentralNode);

   ImGui::End();
}

static bool showProgress = false;

void setShowProgress(bool show) {showProgress = show;}

void renderProgressBar() {
   if (!showProgress) {return;}
   std::cout << "Draw" << std::endl;
   ImGuiWindowFlags flags =
       ImGuiWindowFlags_NoTitleBar |
       ImGuiWindowFlags_NoCollapse |
       ImGuiWindowFlags_NoResize |
       ImGuiWindowFlags_NoMove;
   ImGui::Begin("ProgressBar",nullptr,flags);
   ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(0.0f, 0.0f), "");
   ImGui::End();
}

const char* selectedFractaleID = arbre.id;

const char* getSelectedFractalesID() {
   return selectedFractaleID;
}
void setSelectedFractalesID(const char* id) {
   selectedFractaleID = id;
}

void loadTexture(const char* path, GLuint* tex) {
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   int w, h, channels;
   unsigned char* data = stbi_load(path, &w, &h, &channels, 4);
   if (!data) {
      printf("Erreur chargement texture: %s\n", path);
      return;
   }
   printf("Chargement texture: %s\n", path);

   glGenTextures(1, tex);
   glBindTexture(GL_TEXTURE_2D, *tex);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // important

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

   stbi_image_free(data);
}


void registerTexture() {
   loadTexture(prototypeTexturePath, &prototypeTexture);
}

void renderPreviewFractale(float sz,Fractale fractale) {
   ImVec2 p = ImGui::GetCursorScreenPos();
   ImTextureID texID = (ImTextureID)(intptr_t)prototypeTexture;
   ImGui::GetWindowDrawList()->AddImage(
    texID,
    p,
    ImVec2(p.x + sz, p.y + sz),
    ImVec2(0,1), // UV min
    ImVec2(1,0)  // UV max
);
   ImGui::Dummy(ImVec2(sz, sz)); // Reserver de l'espace
   ImGui::SameLine();
   if (ImGui::MenuItem(fractale.name))
      setSelectedFractalesID(fractale.id);

}

void renderPreviewFractalesMenu() {
   if (ImGui::BeginPopup("previewfractalesmenu"/*, ImGuiWindowFlags_MenuBar*/))
   {
      float sizeScale = 2.0f;
      float sz = ImGui::GetTextLineHeight()*sizeScale;
      ImGuiStyle& style = ImGui::GetStyle();

      ImGui::PushFont(nullptr,style.FontSizeBase*sizeScale);

      renderPreviewFractale(sz,arbre);
      renderPreviewFractale(sz,arbre3);
      renderPreviewFractale(sz,courbe_de_koch_quadratique);
      renderPreviewFractale(sz,courbe_de_koch_quadratique_inv);
      renderPreviewFractale(sz,flocon_koch);
      renderPreviewFractale(sz,ligne_koch);

      ImGui::PopFont();
      ImGui::EndPopup();
   }
}

float color[3] = {1,1,1};
static int profondeur = 5;
int getDepth() {
   return profondeur;
}


void renderMenuBar() {
   ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

   ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 100)); // hauteur multiple
   ImGui::Begin("Options", nullptr, flags);

   ImGui::Text("Fractale: %s",selectedFractaleID);
   ImGui::OpenPopupOnItemClick("previewfractalesmenu");
   ImGui::SameLine();
   if (ImGui::Button("Changer"))
      ImGui::OpenPopup("previewfractalesmenu");
   renderPreviewFractalesMenu();

   ImGui::SameLine();
   ImGui::BeginDisabled();
   float taille = 1.0f;
   float width = ImGui::GetContentRegionAvail().x * 0.2f;
   ImGui::PushItemWidth(width);
   ImGui::InputFloat("Taille",&taille,0.01);
   taille = std::max(taille,0.0f);
   ImGui::EndDisabled();
   ImGui::SameLine();
   ImGui::ColorEdit3("Couleur", color, ImGuiColorEditFlags_NoInputs);
   ImGui::SameLine();
   ImGui::BeginDisabled();
   float pos[2] = {0,0};
   ImGui::InputFloat2("Position",pos);
   ImGui::EndDisabled();

   // Next Line
   ImGui::SliderInt("Profondeur",&profondeur,1,10);
   ImGui::SameLine();
   ImGui::BeginDisabled();
      float epaisseur = 1.0f;
      ImGui::SliderFloat("Epaisseur",&epaisseur,0.0f,10.0f,"%.1f");
      ImGui::SameLine();
      float degrad[3] = {1,1,1};
      ImGui::ColorEdit3("Dégradé", degrad, ImGuiColorEditFlags_NoInputs);
      ImGui::SameLine();
      bool en = true;
      ImGui::Checkbox("Activé",&en);
      ImGui::SameLine();
      float zoom = 1.0f;
      ImGui::SliderFloat("Zoom",&zoom,0.0f,1.0f);
   ImGui::EndDisabled();
   ImGui::SameLine();
   if (ImGui::Button("Nettoyez")) {
      Turtle::instance().nettoyer();
   }
   ImGui::End();
}


static int fboWidth = 0, fboHeight = 0;

void ensureFBOSize(int w, int h)
{
    if (w <= 0 || h <= 0) return;
    if (w == fboWidth && h == fboHeight) return;
    fboWidth = w; fboHeight = h;

    // (re)create texture storage sized to w,h
    if (colorTex == 0) glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (fbo == 0) glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) printf("FBO error: %x\n", status);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static int frameSize_x = 0, frameSize_y = 0;
static float menuBarHeight = 47.0f;
std::array<int,2> getFrameSize() {
   return {frameSize_x, frameSize_y};
}

float getMenuBarHeight() {return menuBarHeight;}

void beginRenderShaderToFBO()
{
    // 1) Try content region avail (correct when inside window after menu bar)
    ImVec2 contentAvail = ImGui::GetContentRegionAvail();

    // 2) Fallback: compute from window size minus menubar and paddings
    if (contentAvail.x <= 0.0f || contentAvail.y <= 0.0f) {
        ImVec2 ws = ImGui::GetWindowSize();
        ImGuiStyle& style = ImGui::GetStyle();
        menuBarHeight = ImGui::GetFrameHeight(); // height of menu bar / frame
        contentAvail.x = ws.x - style.WindowPadding.x * 2.0f;
        contentAvail.y = ws.y - menuBarHeight - style.WindowPadding.y * 2.0f;
    }
   menuBarHeight = ImGui::GetFrameHeight();
    // clamp to >= 1
    int w = std::max(1, (int)floorf(contentAvail.x + 0.5f));
    int h = std::max(1, (int)floorf(contentAvail.y + 0.5f));
    availSize = ImVec2((float)w, (float)h);
   frameSize_x = w;
   frameSize_y = h;
    // Ensure FBO size matches
    ensureFBOSize(w, h);

    // bind FBO and viewport
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


void endRenderShaderToFBO() {
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}