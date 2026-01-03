#pragma once
#include <dsaviz/core/VizObject.hpp>
#include <dsaviz/text/Font.hpp>

namespace dsaviz {

enum class VerticalTextOrigin { Top, Center, Bottom, Baseline };
enum class HorizontalTextOrigin { Left, Center, Right };
enum class TextAlignment { Left, Center, Right };

class TextObject : public VizObject {
public:
  TextObject(const std::string &text, const Font &font);
  void setText(const std::string &text, const Font &font);
  void submit(Renderer &renderer) override;

private:
  std::vector<float> getRowWidths(const std::string& text);
  VerticalTextOrigin verticalTextOrigin = VerticalTextOrigin::Center;
  HorizontalTextOrigin horizontalTextOrigin = HorizontalTextOrigin::Center;
  TextAlignment textAlignment = TextAlignment::Center;
  std::string text;
  const Font *font;
  std::vector<float> quadData; // x, y, width, height, u0, v0, u1, v1,
};
} // namespace dsaviz