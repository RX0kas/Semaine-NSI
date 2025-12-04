#include "interface.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <math.h>
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

Fractale arbre = {arbreTexture,arbreTexturePath,"Arbre","arbre",0};
Fractale arbre3 = {prototypeTexture,prototypeTexturePath,"Arbre à 3 branches","arbre3",1};
Fractale courbe_de_koch_quadratique = {prototypeTexture,prototypeTexturePath,"Courbe de Koch Quadratique","courbe_de_koch_quadratique",2};
Fractale courbe_de_koch_quadratique_inv = {prototypeTexture,prototypeTexturePath,"Courbe de Koch Quadratique Inversé","courbe_de_koch_quadratique_inv",3};
Fractale flocon_koch = {prototypeTexture,prototypeTexturePath,"Flocon de Koch","flocon_koch",4};
Fractale ligne_koch = {prototypeTexture,prototypeTexturePath,"Ligne de Koch","ligne_koch",5};
Fractale courbe_carre_sierpinski = {arbreTexture,arbreTexturePath,"Courbe Carre Sierpinski","courbe_carre_sierpinski",6};
Fractale courbe_de_hilbert = {arbreTexture,arbreTexturePath,"Courbe de hilbert","courbe_de_hilbert",7};
Fractale courbe_sierpinski = {arbreTexture,arbreTexturePath,"Courbe Sierpinski","courbe_sierpinski",8};
Fractale courbe_du_dragon = {arbreTexture,arbreTexturePath,"Courbe du dragon","courbe_du_dragon",9};
Fractale courbe_en_fleche_sierpinski = {arbreTexture,arbreTexturePath,"Courbe en fleche sierpinski","courbe_en_fleche_sierpinski",10};
Fractale courbe_levy = {arbreTexture,arbreTexturePath,"Courbe Levy","courbe_levy",11};

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float angle = 0;
float getAngle() {return angle * (180.0 / M_PI);}

static float longueur = 100;
float getLongueur() {return longueur;}

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

void beginShaderPreview() {
   ImGuiWindowFlags winFlags =
       ImGuiWindowFlags_NoTitleBar |
       ImGuiWindowFlags_NoCollapse |
       ImGuiWindowFlags_NoMove |
       ImGuiWindowFlags_NoResize |
       ImGuiWindowFlags_NoScrollbar |
       ImGuiWindowFlags_NoScrollWithMouse |
       ImGuiWindowFlags_NoBringToFrontOnFocus;

   // Let dockspace position the window
   //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
   //ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
   //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
   //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
   //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
   //ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

   ImGui::Begin("Explorateur", nullptr, winFlags);
}

static ImVec2 g_fbo_pos;

std::array<float, 2> getFBOPos() {
   std::array<float, 2> val = {g_fbo_pos.x, g_fbo_pos.y};
   return val;
}

void renderDockSpace()
{
   const ImGuiViewport* viewport = ImGui::GetMainViewport();
   ImGui::SetNextWindowPos(viewport->WorkPos);
   ImGui::SetNextWindowSize(viewport->WorkSize);
   ImGui::SetNextWindowViewport(viewport->ID);
   ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
   ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
   ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

   ImGuiWindowFlags host_window_flags = 0;
   host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
   host_window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
   host_window_flags |= ImGuiWindowFlags_NoDocking;
   host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

   ImGui::Begin("DockSpace Demo", nullptr, host_window_flags);
   ImGui::PopStyleVar(3);

   ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
   ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

   static bool init_dock = true;
   if (init_dock)
   {
      init_dock = false;
      ImGui::DockBuilderRemoveNode(dockspace_id);
      ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

      ImGuiID dock_main_id = dockspace_id;
      ImGui::DockBuilderDockWindow("Explorateur", dock_main_id);

      ImGui::DockBuilderFinish(dockspace_id);
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
   loadTexture(arbreTexturePath, &arbreTexture);
}

void renderPreviewFractale(float sz,Fractale fractale) {
   ImVec2 p = ImGui::GetCursorScreenPos();
   auto texID = (ImTextureID)(intptr_t)prototypeTexture;
   switch (fractale.numID) {
      case 0: // Arbre
         texID = (ImTextureID)(intptr_t)arbreTexture;
         break;
      default: ;
   }
  /* if (!glIsTexture((GLuint)(intptr_t)texID)) {
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
   ImGui::SameLine();*/
   if (ImGui::MenuItem(fractale.name))
      setSelectedFractalesID(fractale.id);

}

void renderPreviewFractalesMenu() {
   if (ImGui::BeginPopup("previewfractalesmenu"/*, ImGuiWindowFlags_MenuBar*/))
   {
      float sizeScale = 2.0f;
      float sz = ImGui::GetTextLineHeight()*sizeScale;
      ImGuiStyle& style = ImGui::GetStyle();

      ImGui::PushFont(nullptr,style.FontSizeBase*2);

      renderPreviewFractale(sz,arbre);
      renderPreviewFractale(sz,arbre3);
      renderPreviewFractale(sz,courbe_de_koch_quadratique);
      renderPreviewFractale(sz,courbe_de_koch_quadratique_inv);
      renderPreviewFractale(sz,flocon_koch);
      renderPreviewFractale(sz,ligne_koch);
      renderPreviewFractale(sz,courbe_carre_sierpinski);
      renderPreviewFractale(sz,courbe_de_hilbert);
      renderPreviewFractale(sz,courbe_sierpinski);
      renderPreviewFractale(sz,courbe_du_dragon);
      renderPreviewFractale(sz,courbe_en_fleche_sierpinski);
      renderPreviewFractale(sz,courbe_levy);
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
   ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

   //ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 100)); // hauteur multiple
   ImGui::Begin("Options", nullptr, flags);

   ImGui::Text("Fractale: %s",selectedFractaleID);
   ImGui::OpenPopupOnItemClick("previewfractalesmenu");
   if (ImGui::Button("Changer"))
      ImGui::OpenPopup("previewfractalesmenu");
   renderPreviewFractalesMenu();

   // Utiliser la couleur courante de la tortue
   std::array<float, 3> c = Turtle::instance().getColor();
   float color[3] = {c[0],c[1],c[2]};
   bool color_edited = ImGui::ColorEdit3("Couleur", color, ImGuiColorEditFlags_NoInputs);
   //printf("color before set color %f,%f,%f\n", color[0], color[1], color[2]);

   if (color_edited) {
       // Stocker la couleur temporairement
       pending_color = {color[0], color[1], color[2]};
       color_changed = true;
   }

   // appliquer la couleur seulement quand l'éditeur est désactivé
   if (ImGui::IsItemDeactivated() && color_changed) {
       Turtle::instance().saveStateForUndo();
       Turtle::instance().setColor(pending_color[0], pending_color[1], pending_color[2]);
       color_changed = false;
   }

   ImGui::SliderInt("Profondeur",&profondeur,1,10);
   ImGui::SliderFloat("Longueur",&longueur,10,300);
   ImGui::SliderAngle("Angle",&angle,0,360);

   if (ImGui::Button("Nettoyez")) {
      Turtle::instance().nettoyer();
   }

   if (ImGui::Button("Annuler") && Turtle::instance().canUndo())
      Turtle::instance().undo();

   ImGui::SameLine();
   if (ImGui::Button("Refaire") && Turtle::instance().canRedo())
      Turtle::instance().redo();

   if (ImGui::Button("Capture"))
      showScreenshotPreview = true;

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

void ensureFBOSize(int w, int h) {
   if (w <= 0 || h <= 0) return;
   if (w == currentFBOw && h == currentFBOh) return;

   currentFBOw = w;
   currentFBOh = h;

   if (colorTex == 0) {
      glGenTextures(1, &colorTex);
   }

   glBindTexture(GL_TEXTURE_2D, colorTex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

   // Set proper texture parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   if (fbo == 0) {
      glGenFramebuffers(1, &fbo);
   }

   glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

   GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   if (status != GL_FRAMEBUFFER_COMPLETE) {
      printf("FBO error: 0x%x\n", status);
   }

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glBindTexture(GL_TEXTURE_2D, 0);
}

static float menuBarHeight = 47.0f;
std::array<int,2> getFrameSize() {
   return {currentFBOw, currentFBOh};
}

float getMenuBarHeight() {return menuBarHeight;}

/*void beginRenderShaderToFBO()
{
   // Récupération correcte de la zone de rendu disponible
   ImVec2 cr_min = ImGui::GetWindowContentRegionMin();
   ImVec2 cr_max = ImGui::GetWindowContentRegionMax();
   ImVec2 win_pos = ImGui::GetWindowPos();

   float avail_w = cr_max.x - cr_min.x;
   float avail_h = cr_max.y - cr_min.y;

   // fallback si quelque chose échoue
   if (avail_w < 1.f || avail_h < 1.f) {
      avail_w = ImGui::GetWindowSize().x;
      avail_h = ImGui::GetWindowSize().y;
   }

   int w = std::max(1, (int)floorf(avail_w));
   int h = std::max(1, (int)floorf(avail_h));

   currentFBOw = w;
   currentFBOh = h;

   ensureFBOSize(w, h);

   glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   glViewport(0, 0, w, h);

   if (isPrinterCompatible()) glClearColor(1,1,1,1);
   else glClearColor(0.1f,0.1f,0.1f,1);

   glClear(GL_COLOR_BUFFER_BIT);
}*/

static int w,h;

void beginRenderShaderToFBO() {
   // 1) Obtenir la vraie taille finale
   ImVec2 contentAvail = ImGui::GetContentRegionAvail();

   w = (int)contentAvail.x;
   h = (int)contentAvail.y;

   ensureFBOSize(w, h);

   glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   glViewport(0, 0, w, h);
   if (isPrinterCompatible()) {
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   } else {
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
   }

   glClear(GL_COLOR_BUFFER_BIT);
}



void endRenderShaderToFBO()
{
   if (currentFBOw > 0 && currentFBOh > 0 && colorTex != 0) {
      // Draw the texture at the current cursor position
      ImGui::Image((ImTextureID)(intptr_t)colorTex,
                   ImVec2(static_cast<float>(currentFBOw), static_cast<float>(currentFBOh)),
                   ImVec2(0,1), ImVec2(1,0));

      // Store position for click handling
      g_fbo_pos = ImGui::GetItemRectMin();

      // Click handling
      ImRect rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
      if (ImGui::IsItemClicked()) {
         ImVec2 mousePos = ImGui::GetMousePos();
         callback(mousePos.x - rect.Min.x, mousePos.y - rect.Min.y);
      }
   } else {
      ImGui::Text("No preview available");
   }
}

void endShaderPreview() {
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   // 2) Affichage dans la fenêtre
   ImGui::Image((ImTextureID)(intptr_t)colorTex,
                ImVec2(static_cast<float>(w), static_cast<float>(h)),
                ImVec2(0,1), ImVec2(1,0));

   ImGui::End();
     /*if (currentFBOw > 0 && currentFBOh > 0) {
      // Get content region - this accounts for tab bar if present
      ImVec2 contentMin = ImGui::GetWindowContentRegionMin();

      // Set cursor to the beginning of the content region (after tab bar)
      ImGui::SetCursorPos(contentMin);

      // Draw the texture - use the content size, not the FBO size
      ImGui::Image((ImTextureID)(intptr_t)colorTex,
                ImVec2(w, h),
                ImVec2(0,1), ImVec2(1,0));

      // Store the actual position and size for click handling
      g_fbo_pos = ImGui::GetItemRectMin();

      // Click handling
      ImRect rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
      if (ImGui::IsItemClicked()) {
         ImVec2 mousePos = ImGui::GetMousePos();
         callback(mousePos.x - rect.Min.x, mousePos.y - rect.Min.y);
      }
   } else {
      ImGui::Text("No preview available");
   }*/

   // Pop all the style vars we pushed
   //ImGui::PopStyleVar(5);
   //ImGui::End();
}


/*void endRenderShaderToFBO()
{
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}*/