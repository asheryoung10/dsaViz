#include <dsaviz/util/Color.hpp>
#include <cmath>

namespace dsaviz {
glm::vec3 Color::toVec3(const Color &color) {
  return glm::vec3(color.r, color.g, color.b);
}
Color Color::fromHSV(float h, float s, float v) {
  float hh = h * 6.0f;
  float c = v * s;
  float x = c * (1.0f - std::fabs(std::fmod(hh, 2.0f) - 1.0f));
  float m = v - c;

  float r1, g1, b1;

  if (hh < 1.0f) {
    r1 = c;
    g1 = x;
    b1 = 0.0f;
  } else if (hh < 2.0f) {
    r1 = x;
    g1 = c;
    b1 = 0.0f;
  } else if (hh < 3.0f) {
    r1 = 0.0f;
    g1 = c;
    b1 = x;
  } else if (hh < 4.0f) {
    r1 = 0.0f;
    g1 = x;
    b1 = c;
  } else if (hh < 5.0f) {
    r1 = x;
    g1 = 0.0f;
    b1 = c;
  } else {
    r1 = c;
    g1 = 0.0f;
    b1 = x;
  }

  return Color{r1 + m, g1 + m, b1 + m};
}
} // namespace dsaviz