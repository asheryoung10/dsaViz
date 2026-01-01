#pragma once
#include <dsaviz/util/glm.hpp>
namespace dsaviz {
struct Color {
  float r, g, b;
  static Color fromHSV(float h, float s, float v);
  static glm::vec3 toVec3(const Color &color);
};
} // namespace dsaviz