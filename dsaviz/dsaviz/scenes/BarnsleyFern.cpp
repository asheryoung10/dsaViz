#include <dsaviz/scenes/BarnsleyFern.hpp>
#include <imgui.h>
#include <dsaviz/objects/TextureObject.hpp>
#include <dsaviz/util/Random.hpp>
#include <dsaviz/util/FrameTimeTracker.hpp>

#include <algorithm>
#include <vector>
#include <memory>

namespace dsaviz {

void BarnsleyFern::drawSceneUI() {
  ImGuiViewport *vp = ImGui::GetMainViewport();

  ImVec2 panelPos(vp->WorkPos.x + vp->WorkSize.x * 0.5f,
                  vp->WorkPos.y + vp->WorkSize.y - 16.0f);

  ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always, ImVec2(0.5f, 1.0f));

  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoFocusOnAppearing;
  ImGui::Begin("BarnsleyFernSetup", nullptr, flags);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 6));
  
  bool changed = false;
  float newProb[4] = {};
  memcpy(newProb, prob, sizeof(float) * 4);
  if (ImGui::SliderFloat4("Probabilites", newProb, 0.0, 1.0f) ) {
    float sum = newProb[0] + newProb[1] + newProb[2] + newProb[3];
    float difference = (1 - sum) / 4;
    newProb[0] += difference;
    newProb[1] += difference;
    newProb[2] += difference;
    newProb[3] += difference;

    memcpy(prob, newProb, sizeof(float) * 4);
    changed = true;
  }

  static bool firstTime = true;
  ImGui::SameLine();
    if(ImGui::Button("Reset")) {
        firstTime = true;
    }
  if (ImGui::SliderInt("Width", &width, 8, 2048)) {
    changed = true;
  }
  if (ImGui::SliderInt("Height", &height, 8, 2048)) {
    changed = true;
  }
    if (ImGui::SliderInt("Interations", &iterationCount, 10, 1000000)) {
        changed = true;
  }

  if(firstTime) {
    firstTime = false;
    prob[0] = 0.01;
    prob[1] = 0.7;
    prob[2] = 0.15;
    prob[3] = 0.15;
    changed  = true;
  }
  if (changed) {
    if (imageData != nullptr)
      free(imageData);

    imageData = (uint8_t *)malloc((size_t)(width * height));
    memset(imageData, 0, (size_t)(width * height));

    // Barnsley fern parameters
    float x = 0.0f;
    float y = 0.0f;

    for (int i = 0; i < iterationCount; ++i) {
      float nextX, nextY;
      float r = rand() % 100;
      r /= 100;
      

      if (r < prob[0]) {
        nextX = 0.0f;
        nextY = 0.16f * y;
      } else if (r < prob[0] + prob[1]) {
        nextX = 0.85f * x + 0.04f * y;
        nextY = -0.04f * x + 0.85f * y + 1.6f;
      } else if (r < prob[0] + prob[1] + prob[2]) {
        nextX = 0.20f * x - 0.26f * y;
        nextY = 0.23f * x + 0.22f * y + 1.6f;
      } else {
        nextX = -0.15f * x + 0.28f * y;
        nextY = 0.26f * x + 0.24f * y + 0.44f;
      }

      x = nextX;
      y = nextY;

      // Normalize to texture space
      int px = (int)((x + 2.1820f) / 4.8378f * (width - 1));
      int py = (int)((1.0f - (y / 9.9983f)) * (height - 1));

      if (px >= 0 && px < width && py >= 0 && py < height) {
        size_t index = (size_t)py * width + px;

        // Accumulate brightness (clamped)
        if (imageData[index] < 255)
          imageData[index] += 5;
      }
    }

    texture.createFromMemory(imageData, width, height, TextureFormat::R8);
    texture.setNearestFiltering();
  }

  

  ImGui::PopStyleVar();

  ImGui::End();
}
void BarnsleyFern::drawSetupUI() {}
void BarnsleyFern::update(VizContext &context) { drawSceneUI(); }
void BarnsleyFern::render(Renderer &render) {
  if (!texture.isValid())
    return;
  TextureObject textureObject(&texture);
  textureObject.submit(render);
}

} // namespace dsaviz
