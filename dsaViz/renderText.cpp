#include <dsaViz/renderText.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_image.h>

#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdf-atlas-gen/AtlasGenerator.h>
#include <msdf-atlas-gen/FontGeometry.h>
#include <msdf-atlas-gen/TightAtlasPacker.h>

using namespace msdf_atlas;
using namespace msdfgen;

namespace dsaViz {

void RenderText::setup() {
    if (!generateAtlas("../assets/CascadiaCode.ttf")) {
        spdlog::error("Failed to generate MSDF atlas");
        return;
    }

    msdfShaderProgram.createFromSource(vertexShaderSource, fragmentShaderSource);
    // Setup a simple quad for rendering a single glyph (can be instanced later)
    float quadVertices[] = {
        // positions  // UVs
        0.0f, 1.0f,   0.0f, 1.0f,
        0.0f, 0.0f,   0.0f, 0.0f,
        1.0f, 0.0f,   1.0f, 0.0f,

        0.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 0.0f,   1.0f, 0.0f,
        1.0f, 1.0f,   1.0f, 1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // UV
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void RenderText::render() {
    msdfShaderProgram.use();
    textureAtlas.bind(GL_TEXTURE0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    msdfShaderProgram.uploadInt("uTexture", 0);
    msdfShaderProgram.uploadColor("textColor", 1.0f, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(vao);

    // For simplicity, render single quad at origin
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

bool RenderText::generateAtlas(const char* fontFilename) {
    FreetypeHandle* ft = initializeFreetype();
    if (!ft) {
        spdlog::error("Failed to initialize FreeType");
        return false;
    }

    FontHandle* font = loadFont(ft, fontFilename);
    if (!font) {
        spdlog::error("Failed to load font '{}'", fontFilename);
        deinitializeFreetype(ft);
        return false;
    }

    std::vector<GlyphGeometry> glyphsGeometry;
    FontGeometry fontGeometry(&glyphsGeometry);
    fontGeometry.loadCharset(font, 1.0, Charset::ASCII);

    // Edge coloring
    for (auto& g : glyphsGeometry)
        g.edgeColoring(&edgeColoringInkTrap, 3.0, 0);

    TightAtlasPacker packer;
    packer.setDimensionsConstraint(DimensionsConstraint::SQUARE);
    packer.setMinimumScale(32.0);
    packer.setPixelRange(2.0);
    packer.setMiterLimit(1.0);
    packer.pack(glyphsGeometry.data(), glyphsGeometry.size());

    int width = 0, height = 0;
    packer.getDimensions(width, height);

    ImmediateAtlasGenerator<float, 3, msdfGenerator, msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>>
        generator(width, height);

    generator.generate(glyphsGeometry.data(), glyphsGeometry.size());

    const auto& bitmap = generator.atlasStorage();
    BitmapConstRef<msdf_atlas::byte, 3> bitmapRef = bitmap;

    stbi_flip_vertically_on_write(true);
    stbi_write_png("atlas.png", bitmapRef.width, bitmapRef.height, 3, bitmapRef.pixels, bitmapRef.width * 3);

    textureAtlas.createFromData(bitmapRef.width, bitmapRef.height, bitmapRef.pixels);

    destroyFont(font);
    deinitializeFreetype(ft);

    spdlog::info("MSDF atlas generated: {}x{}", width, height);
    return true;
}

} // namespace dsaViz
