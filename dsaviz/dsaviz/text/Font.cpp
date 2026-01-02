#include <dsaviz/text/Font.hpp>

#include <msdf-atlas-gen/AtlasGenerator.h>
#include <msdf-atlas-gen/FontGeometry.h>
#include <msdf-atlas-gen/TightAtlasPacker.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include <spdlog/spdlog.h>

using namespace msdfgen;
using namespace msdf_atlas;

namespace dsaviz {

bool Font::setFont(const std::string& fontFilePath, float fontResolution) {
    this->fontFilePath = fontFilePath;
    return setFontResolution(fontResolution);
}

bool Font::setFontResolution(float resolution) {
    fontResolution = resolution;
    GlyphMetricsMap.clear();

    // Initialize FreeType
    FreetypeHandle* ft = initializeFreetype();
    if (!ft) {
        spdlog::error("Failed to initialize FreeType");
        return false;
    }

    FontHandle* font = loadFont(ft, fontFilePath.c_str());
    if (!font) {
        spdlog::error("Failed to load font: {}", fontFilePath);
        deinitializeFreetype(ft);
        return false;
    }

    std::vector<GlyphGeometry> glyphs;
    FontGeometry fontGeometry(&glyphs);

    // Load font metrics
    msdfgen::FontMetrics msdfMetrics;
    msdfgen::getFontMetrics(msdfMetrics, font, msdfgen::FONT_SCALING_EM_NORMALIZED);

    float normalizationFactor = 1 / (msdfMetrics.ascenderY - msdfMetrics.descenderY);
    fontMetrics.ascent  = msdfMetrics.ascenderY * normalizationFactor;
    fontMetrics.descent = msdfMetrics.descenderY * normalizationFactor;
    fontMetrics.lineGap =
        (msdfMetrics.lineHeight - (msdfMetrics.ascenderY - msdfMetrics.descenderY)) * normalizationFactor;

    // Load ASCII charset
    fontGeometry.loadCharset(font, 1.0, Charset::ASCII);

    // Edge coloring
    const double maxCornerAngle = 5.0;
    for (GlyphGeometry& glyph : glyphs) {
        glyph.edgeColoring(&edgeColoringInkTrap, maxCornerAngle, 0);
    }

    // Pack atlas
    TightAtlasPacker packer;
    packer.setDimensionsConstraint(DimensionsConstraint::SQUARE);
    packer.setMinimumScale(fontResolution);
    packer.setPixelRange(2.0);
    packer.setMiterLimit(1.0);
    packer.pack(glyphs.data(), glyphs.size());

    int atlasWidth = 0, atlasHeight = 0;
    packer.getDimensions(atlasWidth, atlasHeight);

    // Generate atlas bitmap (MSDF, RGB)
    ImmediateAtlasGenerator<
        float,
        3,
        msdfGenerator,
        BitmapAtlasStorage<byte, 3>>
        generator(atlasWidth, atlasHeight);

    GeneratorAttributes attributes;
    generator.setAttributes(attributes);
    generator.setThreadCount(4);
    generator.generate(glyphs.data(), glyphs.size());

    BitmapConstRef<byte, 3> bitmap = generator.atlasStorage();

    fontAtlas.createFromMemory(
        bitmap.pixels,
        bitmap.width,
        bitmap.height,
        TextureFormat::RGB8
    );

    // Store glyph metrics
    for (const GlyphGeometry& g : glyphs) {
        char c = static_cast<char>(g.getCodepoint());
        if (c < 32 || c > 126)
            continue;

        GlyphMetrics m{};

        double l, b, r, t;

        // Atlas UVs
        g.getQuadAtlasBounds(l, b, r, t);
        m.u0 = float(l) / float(atlasWidth);
        m.v0 = float(b) / float(atlasHeight);
        m.u1 = float(r) / float(atlasWidth);
        m.v1 = float(t) / float(atlasHeight);

        // Plane bounds (layout)
        g.getQuadPlaneBounds(l, b, r, t);
        m.offsetX = float(l) * normalizationFactor;
        m.offsetY = float(b) * normalizationFactor;
        m.width   = float(r - l) * normalizationFactor;
        m.height  = float(t - b) * normalizationFactor;
        m.advance = float(g.getAdvance()) * normalizationFactor;

        GlyphMetricsMap[c] = m;
    }

    destroyFont(font);
    deinitializeFreetype(ft);

    spdlog::info(
        "Font atlas generated: {} ({}x{}, resolution {})",
        fontFilePath,
        fontAtlas.getWidth(),
        fontAtlas.getHeight(),
        fontResolution
    );
    return true;
}

float Font::getFontResolution() const {
    return fontResolution;
}

const FontMetrics& Font::getFontMetrics() const {
    return fontMetrics;
}

const GlyphMetrics& Font::getGlyphMetrics(char c) const {
    static GlyphMetrics empty{};
    auto it = GlyphMetricsMap.find(c);
    if (it == GlyphMetricsMap.end()) {
        spdlog::warn("Glyph not found for character '{}'", c);
        return empty;
    }
    return it->second;
}

const Texture& Font::getFontAtlas() const {
    return fontAtlas;
}

} // namespace dsaviz
