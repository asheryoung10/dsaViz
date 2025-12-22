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
    if (!generateAtlas("../assets/palatinolinotype_roman.ttf")) {
        spdlog::error("Failed to generate MSDF atlas");
        return;
    }

    msdfShaderProgram.createFromSource(vertexShaderSource, fragmentShaderSource);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // We will update VBO dynamically per glyph during render
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // UV
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void RenderText::render(const std::string& text, float startX, float startY, float scale) {
    msdfShaderProgram.use();
    textureAtlas.bind(GL_TEXTURE0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    msdfShaderProgram.uploadInt("uTexture", 0);
    msdfShaderProgram.uploadColor("textColor", 1.0f, 1.0f, 1.0f, 1.0f);
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(windowHandle, &framebufferWidth, &framebufferHeight);
    float aspectRatio = 1.0f;
    glm::mat4 projection;
    if(framebufferWidth > framebufferHeight) {
        aspectRatio = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
        projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
    }else {
        aspectRatio = static_cast<float>(framebufferHeight) / static_cast<float>(framebufferWidth);
        projection = glm::ortho(-1.0f, 1.0f, -aspectRatio, aspectRatio, -1.0f, 1.0f);
    }
    msdfShaderProgram.uploadGLMMatrix("uTransform", projection);

    glBindVertexArray(vao);

    float x = startX;
    float y = startY;

    for (char c : text) {
        if(c == ' ') {
            x += 20.0f * scale; // simple space handling
            continue;
        }
        if(c == '\n') {
            x = startX;
            y -= 64.0f * scale; // simple newline handling
            continue;
        }
        if (c < 32 || c > 126)  {
            spdlog::warn("Skipping unsupported character code: {}", static_cast<int>(c));
            continue; // skip unsupported ASCII
        }
    

        const GlyphInfo& g = glyphs[c - 32];

        float xpos = x + g.offsetX * scale;
        float ypos = y - g.offsetY * scale; // y-down
        spdlog::info("Offsets for char '{}': offsetX = {}, offsetY = {}", c, g.offsetX, g.offsetY);
        float w = (g.u1 - g.u0) * textureAtlas.width() * scale;
        float h = (g.v1 - g.v0) * textureAtlas.height() * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,  g.u0, g.v1 },
            { xpos,     ypos,      g.u0, g.v0 },
            { xpos + w, ypos,      g.u1, g.v0 },

            { xpos,     ypos + h,  g.u0, g.v1 },
            { xpos + w, ypos,      g.u1, g.v0 },
            { xpos + w, ypos + h,  g.u1, g.v1 }
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += w + g.advance * scale;
    }

    glBindVertexArray(0);
}

bool RenderText::generateAtlas(const char* fontFilename) {
    // Initialize FreeType
    FreetypeHandle* ft = initializeFreetype();
    if (!ft) return false;

    FontHandle* font = loadFont(ft, fontFilename);
    if (!font) { deinitializeFreetype(ft); return false; }

    std::vector<GlyphGeometry> glyphsGeometry;
    FontGeometry fontGeometry(&glyphsGeometry);
    fontGeometry.loadCharset(font, 1.0, Charset::ASCII);

    // Edge coloring
    for (auto& g : glyphsGeometry)
        g.edgeColoring(&edgeColoringInkTrap, 3.0, 0);

    // Pack glyphs
    TightAtlasPacker packer;
    packer.setDimensionsConstraint(DimensionsConstraint::SQUARE);
    double scale = 64;
    packer.setMinimumScale(scale);
    packer.setPixelRange(2.0);
    packer.setMiterLimit(1.0);
    packer.pack(glyphsGeometry.data(), glyphsGeometry.size());

    int atlasWidth = 0, atlasHeight = 0;
    packer.getDimensions(atlasWidth, atlasHeight);

    // Generate atlas bitmap
    ImmediateAtlasGenerator<float, 3, msdfGenerator, BitmapAtlasStorage<byte, 3>> generator(atlasWidth, atlasHeight);
    GeneratorAttributes attributes;
    generator.setAttributes(attributes);
    generator.setThreadCount(4);
    generator.generate(glyphsGeometry.data(), glyphsGeometry.size());

    const auto& bitmap = generator.atlasStorage();
    BitmapConstRef<byte, 3> bitmapRef = bitmap;

    // Save atlas for debugging
    stbi_flip_vertically_on_write(true);
    stbi_write_png("atlas.png", bitmapRef.width, bitmapRef.height, 3, bitmapRef.pixels, bitmapRef.width * 3);

    textureAtlas.createFromData(bitmapRef.width, bitmapRef.height, bitmapRef.pixels);

    // Compute UVs and glyph offsets manually
    glyphs.resize(glyphsGeometry.size());
    for (size_t i = 0; i < glyphsGeometry.size(); ++i) {
        const auto& g = glyphsGeometry[i];
        GlyphInfo& info = glyphs[i];

        // Get glyph rectangle in atlas (pixel coordinates)
        double l, b, r, t;
        g.getQuadAtlasBounds(l, b, r, t);
        info.u0 = float(l) / float(atlasWidth);
        info.v0 = float(b) / float(atlasHeight);
        info.u1 = float(r) / float(atlasWidth);
        info.v1 = float(t) / float(atlasHeight);

        // Get glyph quad for placement offsets (relative to baseline)
        g.getQuadPlaneBounds(l, b, r, t);
        info.offsetX = l * scale;
        info.offsetY = b * -scale;
        info.advance = float(g.getAdvance());
    }

    destroyFont(font);
    deinitializeFreetype(ft);

    spdlog::info("MSDF atlas generated: {}x{}", atlasWidth, atlasHeight);
    return true;
}

} // namespace dsaViz
