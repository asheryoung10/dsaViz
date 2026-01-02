#pragma once
#include <string.h>
#include <dsaviz/render/Texture.hpp>
#include <unordered_map>

namespace dsaviz {

struct FontMetrics {
  float ascent;
  float descent;
  float lineGap;
};

struct GlyphMetrics {
  float u0, v0, u1, v1;
  float offsetX, offsetY;
  float width, height;
  float advance;
};

class Font {
    public:
    /// Needs opengl context to create font atlas texture 
    bool setFont(const std::string& fontFilePath, float fontResolution);
    bool setFontResolution(float fontResolution);
    float getFontResolution() const;
    const FontMetrics& getFontMetrics() const;
    const GlyphMetrics& getGlyphMetrics(char c) const;
    const Texture& getFontAtlas() const;
private:
    float fontResolution;
    std::string fontFilePath;
    FontMetrics fontMetrics;
    std::unordered_map<char, GlyphMetrics> GlyphMetricsMap;
    Texture fontAtlas;
};
} // namespace dsaviz