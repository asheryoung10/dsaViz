#include "TextObject.hpp"
#include <dsaviz/objects/TextObject.hpp>
#include <dsaviz/util/Transform.hpp>
#include <spdlog/spdlog.h>

namespace dsaviz {
TextObject::TextObject(const std::string &text, const Font &font)
    : text(text), font(&font) {
  setText(text, font);
}

void TextObject::setText(const std::string &text, const Font &font) {
  this->text = text;
  this->font = &font;
  quadData.clear();
  quadData.reserve(text.size() * 12);
  std::vector<float> widths = getRowWidths(text);
  float largestWidth = widths[0];
  for(int i = 1; i < widths.size(); i++) {
    if(largestWidth < widths[i]) largestWidth = widths[i];
  }
  float originY;
  switch (verticalTextOrigin) {
  case (VerticalTextOrigin::Baseline):
    originY = 0;
    break;
  case (VerticalTextOrigin::Top):
    originY = -font.getFontMetrics().ascent;
    break;
  case (VerticalTextOrigin::Center):
    originY = -font.getFontMetrics().ascent + float(widths.size()) / 2;
    break;
  case (VerticalTextOrigin::Bottom):
    originY = -font.getFontMetrics().ascent + float(widths.size());
    break;
  }
  float originX;
  switch (horizontalTextOrigin) {
    case(HorizontalTextOrigin::Left): originX = 0; break;
    case(HorizontalTextOrigin::Center): originX = -largestWidth/2; break;
    case(HorizontalTextOrigin::Right): originX = -largestWidth; break;
  }
  float alignmentMultiplier;
  switch(textAlignment) {
    case(TextAlignment::Left): alignmentMultiplier = 1.0f; break;
    case(TextAlignment::Center): alignmentMultiplier = 0.5f; break;
    case(TextAlignment::Right): alignmentMultiplier = 0.0f; break;
  }

  FontMetrics fontMetrics = this->font->getFontMetrics();
  if (text.size() < 1)
    return;
  int i = 0;
  float x = originX + (largestWidth - widths[i]) * alignmentMultiplier;
  float y = originY;
  for (char c : text) {
    if (c == '\n') {
      y -= 1.0f;
      i++;
      x = originX + (largestWidth -widths[i]) * alignmentMultiplier;
    }
    const GlyphMetrics &glyphMetrics = this->font->getGlyphMetrics(c);
    quadData.push_back(x + glyphMetrics.offsetX);
    quadData.push_back(y + glyphMetrics.offsetY + glyphMetrics.height);
    quadData.push_back(glyphMetrics.u0);
    quadData.push_back(glyphMetrics.v1);

    quadData.push_back(x + glyphMetrics.offsetX);
    quadData.push_back(y + glyphMetrics.offsetY);
    quadData.push_back(glyphMetrics.u0);
    quadData.push_back(glyphMetrics.v0);

    quadData.push_back(x + glyphMetrics.offsetX + glyphMetrics.width);
    quadData.push_back(y + glyphMetrics.offsetY + glyphMetrics.height);
    quadData.push_back(glyphMetrics.u1);
    quadData.push_back(glyphMetrics.v1);

    quadData.push_back(x + glyphMetrics.offsetX + glyphMetrics.width);
    quadData.push_back(y + glyphMetrics.offsetY + glyphMetrics.height);
    quadData.push_back(glyphMetrics.u1);
    quadData.push_back(glyphMetrics.v1);

    quadData.push_back(x + glyphMetrics.offsetX);
    quadData.push_back(y + glyphMetrics.offsetY);
    quadData.push_back(glyphMetrics.u0);
    quadData.push_back(glyphMetrics.v0);

    quadData.push_back(x + glyphMetrics.offsetX + glyphMetrics.width);
    quadData.push_back(y + glyphMetrics.offsetY);
    quadData.push_back(glyphMetrics.u1);
    quadData.push_back(glyphMetrics.v0);
    x += glyphMetrics.advance;
  }
}

std::vector<float> TextObject::getRowWidths(const std::string &text) {
  std::vector<float> widths;
  if (text.size() < 1)
    return widths;
  float width = 0;
  char firstChar = text[0];
  char lastChar;
  for (int i = 0; i < text.size(); i++) {
    if (text[i] == '\n') {
      lastChar = text[i - 1];
      width -= this->font->getGlyphMetrics(firstChar).offsetX;
      const GlyphMetrics &lastMetrics = this->font->getGlyphMetrics(lastChar);
      width -= (lastMetrics.advance - lastMetrics.offsetX - lastMetrics.width);
      widths.push_back(width);
      width = 0;
      if(text.size() - 1 > i) firstChar = text[i+1];
      continue;
    }
    const GlyphMetrics &glyphMetrics = this->font->getGlyphMetrics(text[i]);
    width += glyphMetrics.advance;
  }
  if (text[text.size() - 1] != '\n') {
    lastChar = text[text.size() - 1];
    width -= this->font->getGlyphMetrics(firstChar).offsetX;
    const GlyphMetrics &lastMetrics = this->font->getGlyphMetrics(lastChar);
    width -= (lastMetrics.advance - lastMetrics.offsetX - lastMetrics.width);
    widths.push_back(width);
  }

  return widths;
}

void TextObject::submit(Renderer &renderer) {
  renderer.submit(RenderCommand{RenderCommandType::Text, transform.getMatrix(),
                                Color::toVec3(color), Color::toVec3(outlineColor), &font->getFontAtlas(),
                                &quadData});
}
} // namespace dsaviz