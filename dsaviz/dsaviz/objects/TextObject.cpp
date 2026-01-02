#include <dsaviz/objects/TextObject.hpp>
#include <dsaviz/util/Transform.hpp>

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
  FontMetrics fontMetrics = this->font->getFontMetrics();
  float x = 0.0f;
  float y = 0.0f;
  for (char c : text) {
    if (c == '\n') {
      y += 1.0f;
      x = 0;
  }
    const GlyphMetrics& glyphMetrics = this->font->getGlyphMetrics(c);
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

void TextObject::submit(Renderer &renderer) {
  renderer.submit(RenderCommand{RenderCommandType::Text, transform.getMatrix(),
                                Color::toVec3(color), &font->getFontAtlas(),
                                &quadData});
}
} // namespace dsaviz