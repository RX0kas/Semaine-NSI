#include "interface.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <nfd.h>
#include "imgui.h"
#include <filesystem>
#include <iostream>
#include <ostream>
#include <GLFW/glfw3.h>

#include "imgui_internal.h"
#include "stb_image_write.h"
#include "stb_image.h"
#include "turtle_renderer.hpp"

Fractale arbre = {prototypeTexture,prototypeTexturePath,"Arbre","arbre"}; // TODO get the screen
Fractale arbre3 = {prototypeTexture,prototypeTexturePath,"Arbre à 3 branches","arbre3"};
Fractale courbe_de_koch_quadratique = {prototypeTexture,prototypeTexturePath,"Courbe de Koch Quadratique","courbe_de_koch_quadratique"};
Fractale courbe_de_koch_quadratique_inv = {prototypeTexture,prototypeTexturePath,"Courbe de Koch Quadratique Inversé","courbe_de_koch_quadratique_inv"};
Fractale flocon_koch = {prototypeTexture,prototypeTexturePath,"Flocon de Koch","flocon_koch"};
Fractale ligne_koch = {prototypeTexture,prototypeTexturePath,"Ligne de Koch","ligne_koch"};


GLuint fbo = 0;
int currentFBOw = 0;
int currentFBOh = 0;

bool showScreenshotPreview = false;
bool inverted = false;
bool isPrinterCompatible() {
   return inverted;
}


void openScreenshotPreview() {
   showScreenshotPreview = true;
}

int getFBO() {
   return fbo;
}

GLuint colorTex = 0;

void initFBO(int w, int h) {
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   if (fbo != 0) {
      glDeleteFramebuffers(1, &fbo);
      fbo = 0;
   }

   if (colorTex != 0) {
      glDeleteTextures(1, &colorTex);
      colorTex = 0;
   }

   glViewport(0, 0, w, h);

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

void saveFBOToFile(const char* filename)
{
   if (fbo == 0 || colorTex == 0) {
      printf("FBO non initialisé.\n");
      return;
   }

   glBindFramebuffer(GL_FRAMEBUFFER, fbo);

   int w = currentFBOw;
   int h = currentFBOh;

   if (w <= 0 || h <= 0) {
      printf("Dimensions FBO invalides (%d x %d).\n", w, h);
      return;
   }

   std::vector<unsigned char> pixels(w * h * 4);

   glReadPixels(
       0, 0, w, h,
       GL_RGBA, GL_UNSIGNED_BYTE,
       pixels.data()
   );

   // Flip vertical (OpenGL origin = bottom-left)
   for (int y = 0; y < h / 2; ++y) {
      int opposite = h - 1 - y;
      for (int x = 0; x < w * 4; ++x) {
         std::swap(pixels[y * w * 4 + x], pixels[opposite * w * 4 + x]);
      }
   }

   if (stbi_write_png(filename, w, h, 4, pixels.data(), w * 4)) {
      printf("Image sauvegardée : %s (%dx%d)\n", filename, w, h);
   } else {
      printf("Échec de l’enregistrement de %s\n", filename);
   }

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderScreenshotPreview() {
   if (showScreenshotPreview) {
      static char name[256] = "capture.png";
      ImGui::SetNextWindowDockID(0, ImGuiCond_Always); // interdit le docking
      ImGui::SetNextWindowCollapsed(false);
      ImGui::SetNextWindowFocus();
      ImGui::Begin("Prévisualisation Screenshot", &showScreenshotPreview,
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

      // Affichage de la texture FBO
      ImVec2 size(static_cast<float>(currentFBOw), static_cast<float>(currentFBOh));

      // Redimensionne automatiquement si trop grand
      float maxW = 600.0f;
      float maxH = 400.0f;
      float scale = std::min(maxW / size.x, maxH / size.y);
      if (scale < 1.0f) size = ImVec2(size.x * scale, size.y * scale);

      // IMPORTANT : inverser les UV en Y pour afficher correctement la texture OpenGL
      ImGui::Image((ImTextureID)(intptr_t)colorTex, size,
                   ImVec2(0,1), ImVec2(1,0));

      ImGui::Separator();

      // Boutons
      if (ImGui::Button("Enregistrer")) {
         nfdchar_t* path = nullptr;
         nfdu8filteritem_t filters[1] = { { "Image", "png" }};
         if (NFD_SaveDialog(&path, filters, 1,nullptr,"capture.png") == NFD_OKAY) {
            saveFBOToFile(path);
            free(path);
         }
      }

      ImGui::SameLine();
      if (ImGui::Button("Annuler")) {
         showScreenshotPreview = false;
      }
      ImGui::SameLine();
      ImGui::Checkbox("Adapter à l'impression",&inverted);
      ImGui::End();
   }
}

void renderTextureInformation() {
   ImGui::Text("Version: %s", glGetString(GL_VERSION));
   ImGui::Text("GPU: %s", glGetString(GL_RENDERER));
   ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));

   ImGui::SeparatorText("Framebuffer");
   glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

   if (status == GL_FRAMEBUFFER_COMPLETE)
      ImGui::Text("FBO OK");
   else
      ImGui::Text("FBO error : 0x%X", status);

   glBindFramebuffer(GL_FRAMEBUFFER, fbo);

   GLint tex;
   glGetFramebufferAttachmentParameteriv(
       GL_FRAMEBUFFER,
       GL_COLOR_ATTACHMENT0,
       GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
       &tex
   );    

   GLint w, h;
   glBindTexture(GL_TEXTURE_2D, tex);
   glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &w);
   glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
   ImGui::Text("Texture: %d",tex);
   ImGui::Text("Height: %d",w);
   ImGui::Text("Width: %d",h);
}


void defaultCallback(int x,int y) {
   std::cout << "Default callback called with (" << x << ";" << y << ")" << std::endl;
}

static std::function callback = &defaultCallback;

void setExplorerClickedCallback(const std::function<void(int,int)> &c) {
   callback = c;
}


static ImVec2 availSize = ImVec2(-1, -1);
static ImGuiID explorerID;

ImGuiID getExplorerID() {
   return explorerID;
}

void beginShaderPreview() {
   ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoTitleBar |  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

   ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
   ImGui::Begin("Explorateur", nullptr, winFlags);
   explorerID = ImGui::GetCurrentWindow()->ID;
}

static ImVec2 g_fbo_pos;
static ImVec2 g_fbo_size;

void endShaderPreview() {
   if (availSize.x > 0 && availSize.y > 0) {

      if ((int)availSize.x != currentFBOw || (int)availSize.y != currentFBOh)
      {
         std::cout << "Recréation du framebuffer avec la taille :" << "(" << availSize.x << ", " << availSize.y << ")" << std::endl;
         currentFBOw = (int)availSize.x;
         currentFBOh = (int)availSize.y;
         initFBO(currentFBOw, currentFBOh);
      }

      auto texID = (ImTextureID)(intptr_t)colorTex;
      ImGui::Image(texID, availSize, ImVec2(0, 1), ImVec2(1, 0));

      ImRect rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

      if (ImGui::IsItemClicked())
      {
         callback(ImGui::GetMousePos().x - rect.Min.x,ImGui::GetMousePos().y - rect.Min.y);
      }
   } else {
      ImGui::Text("No preview available");
   }

   g_fbo_pos  = ImGui::GetItemRectMin();
   g_fbo_size = ImVec2(currentFBOw, currentFBOh);
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

   static bool init_dock = true;
   if (init_dock)
   {
      init_dock = false;

      // Effacer l'ancien dock
      ImGui::DockBuilderRemoveNode(dockID);

      // Créer un dockspace racine
      ImGui::DockBuilderAddNode(
          dockID,
          ImGuiDockNodeFlags_DockSpace
      );

      // Taille = taille fenetre entière
      ImGui::DockBuilderSetNodeSize(
          dockID,
          ImGui::GetIO().DisplaySize
      );

      // Pas de split → un seul dock, 100% de la zone
      ImGuiID centralNode = dockID;

      // Docker la fenêtre dedans
      ImGui::DockBuilderDockWindow("Explorateur", centralNode);

      // Finaliser la construction
      ImGui::DockBuilderFinish(dockID);
   }
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
   if (!glIsTexture((GLuint)(intptr_t)texID)) {
      // draw placeholder or skip
      ImGui::Text("[no tex]");
   } else {
      ImGui::GetWindowDrawList()->AddImage(
          texID,
          p,
          ImVec2(p.x + sz, p.y + sz),
          ImVec2(0,1), // UV min
          ImVec2(1,0)  // UV max
      );
   }
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


static int profondeur = 5;
int getDepth() {
   return profondeur;
}

bool color_changed = false;
std::array<float, 3> pending_color = {1.0f, 1.0f, 1.0f};
float color_edit_timer = 0.0f;
constexpr float COLOR_EDIT_TIMEOUT = 0.5f; // 500ms de délai

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

   // Utiliser la couleur courante de la tortue
   std::array<float, 3> c = Turtle::instance().getColor();
   float color[3] = {c[0],c[1],c[2]};

   bool color_edited = ImGui::ColorEdit3("Couleur", color, ImGuiColorEditFlags_NoInputs);

   if (color_edited) {
       // Stocker la couleur temporairement
       pending_color = {color[0], color[1], color[2]};
       color_changed = true;
   }

   // Appliquer la couleur seulement quand l'éditeur est désactivé
   if (ImGui::IsItemDeactivated() && color_changed) {
       // Sauvegarder l'état avant de changer la couleur
       Turtle::instance().saveStateForUndo();
       // Appliquer la nouvelle couleur
       Turtle::instance().setColor(pending_color[0], pending_color[1], pending_color[2]);
       color_changed = false;
   }

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

void updateColorEditTimer(float delta_time) {
    if (color_changed && color_edit_timer > 0) {
        color_edit_timer -= delta_time;
        if (color_edit_timer <= 0) {
            // Timeout - appliquer la couleur
            Turtle::instance().setColor(pending_color[0], pending_color[1], pending_color[2]);
            color_changed = false;
        }
    }
}

void ensureFBOSize(int w, int h)
{
    if (w <= 0 || h <= 0) return;
    if (w == currentFBOw && h == currentFBOh) return;
    currentFBOw = w; currentFBOh = h;

    // (re)create texture storage sized to w,h
    if (colorTex == 0) glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, currentFBOw, currentFBOh, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (fbo == 0) glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) printf("FBO error: %x\n", status);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static float menuBarHeight = 47.0f;
std::array<int,2> getFrameSize() {
   return {currentFBOw, currentFBOh};
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
   currentFBOw = w;
   currentFBOh = h;
    // Ensure FBO size matches
    ensureFBOSize(w, h);

    // bind FBO and viewport
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, w, h);
   if (isPrinterCompatible()) glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   else glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


void endRenderShaderToFBO() {
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}