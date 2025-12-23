#include <dsaViz/renderText.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdf-atlas-gen/AtlasGenerator.h>
#include <msdf-atlas-gen/FontGeometry.h>
#include <msdf-atlas-gen/TightAtlasPacker.h>

using namespace msdf_atlas;
using namespace msdfgen;

namespace dsaViz {

void RenderText::setup(float size) {
    if (!generateAtlas("../assets/palatinolinotype_roman.ttf", size)) {
        spdlog::error("Failed to generate MSDF atlas");
        return;
    }

    msdfShaderProgram.loadFromSource(vertexShaderSource, fragmentShaderSource);

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
    msdfShaderProgram.bind();
    textureAtlas.bind(GL_TEXTURE0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    msdfShaderProgram.setInt("uTexture", 0);
    msdfShaderProgram.setVec4("textColor", 1.0f, 1.0f, 1.0f, 1.0f);
    int framebufferWidth = 1000;
    int framebufferHeight = 1000;
    float aspectRatio = 1.0f;
    glm::mat4 projection;
    if(framebufferWidth > framebufferHeight) {
        aspectRatio = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
        projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
    }else {
        aspectRatio = static_cast<float>(framebufferHeight) / static_cast<float>(framebufferWidth);
        projection = glm::ortho(-1.0f, 1.0f, -aspectRatio, aspectRatio, -1.0f, 1.0f);
    }
    msdfShaderProgram.setMat4("uTransform", projection);

    glBindVertexArray(vao);

    float x = startX;
    float y = startY - scale;

    for (char c : text) {
        if(c == '\n') {
            x = startX;
            y -= newLineVerticalShift * scale;
            continue;
        }
        if (c < 32 || c > 126)  {
            spdlog::warn("Skipping unsupported character code: {}", static_cast<int>(c));
            continue; // skip unsupported ASCII
        }
    
        const GlyphMetric& g = glyphMetrics[c - 32];
        if(g.width > 0 && g.height > 0) {
            float xpos = x + g.offsetX * scale;
            float ypos = y + g.offsetY * scale;
            float w = g.width * scale;
            float h = g.height * scale;

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
        }
        x += g.advance * scale;
    }
    glBindVertexArray(0);
}


bool RenderText::generateAtlas(const char *fontFilename, float size) {
    bool success = false;
    // Initialize instance of FreeType library
    if (msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype()) {
        // Load font file
        if (msdfgen::FontHandle *font = msdfgen::loadFont(ft, fontFilename)) {
            // Storage for glyph geometry and their coordinates in the atlas
            std::vector<GlyphGeometry> glyphs;
            // FontGeometry is a helper class that loads a set of glyphs from a single font.
            // It can also be used to get additional font metrics, kerning information, etc.
            FontGeometry fontGeometry(&glyphs);
            FontMetrics metrics;
            getFontMetrics(metrics, font);
            float normalizationFactor = metrics.emSize;
            newLineVerticalShift = metrics.lineHeight;
            // Load a set of character glyphs:
            // The second argument can be ignored unless you mix different font sizes in one atlas.
            // In the last argument, you can specify a charset other than ASCII.
            // To load specific glyph indices, use loadGlyphs instead.
            fontGeometry.loadCharset(font, 1.0, Charset::ASCII);
            // Apply MSDF edge coloring. See edge-coloring.h for other coloring strategies.
            const double maxCornerAngle = 3.0;
            for (GlyphGeometry &glyph : glyphs)
                glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
            // TightAtlasPacker class computes the layout of the atlas.
            TightAtlasPacker packer;
            // Set atlas parameters:
            // setDimensions or setDimensionsConstraint to find the best value
            packer.setDimensionsConstraint(DimensionsConstraint::SQUARE);
            // setScale for a fixed size or setMinimumScale to use the largest that fits
            packer.setMinimumScale(size);
            // setPixelRange or setUnitRange
            packer.setPixelRange(2.0);
            packer.setMiterLimit(1.0);
            // Compute atlas layout - pack glyphs
            packer.pack(glyphs.data(), glyphs.size());
            // Get final atlas dimensions
            int atlasWidth = 0, atlasHeight = 0;
            packer.getDimensions(atlasWidth, atlasHeight);
            // The ImmediateAtlasGenerator class facilitates the generation of the atlas bitmap.
            ImmediateAtlasGenerator<
                float, // pixel type of buffer for individual glyphs depends on generator function
                3, // number of atlas color channels
                msdfGenerator, // function to generate bitmaps for individual glyphs
                BitmapAtlasStorage<byte, 3> // class that stores the atlas bitmap
                // For example, a custom atlas storage class that stores it in VRAM can be used.
            > generator(atlasWidth, atlasHeight);
            // GeneratorAttributes can be modified to change the generator's default settings.
            GeneratorAttributes attributes;
            generator.setAttributes(attributes);
            generator.setThreadCount(4);
            // Generate atlas bitmap
            generator.generate(glyphs.data(), glyphs.size());
            // The atlas bitmap can now be retrieved via atlasStorage as a BitmapConstRef.
            // The glyphs array (or fontGeometry) contains positioning data for typesetting text.
            BitmapConstRef<byte, 3> bitmapRef = generator.atlasStorage();
            textureAtlas.createFromMemory(bitmapRef.width, bitmapRef.height, TextureFormat::RGB8, bitmapRef.pixels);

            glyphMetrics.resize(glyphs.size());
            for (size_t i = 0; i < glyphs.size(); ++i) {
                const auto& g = glyphs[i];
                GlyphMetric& metric = glyphMetrics[i];

                // Get glyph rectangle in atlas (pixel coordinates)
                double l, b, r, t;
                g.getQuadAtlasBounds(l, b, r, t);
                metric.u0 = float(l) / float(atlasWidth);
                metric.v0 = float(b) / float(atlasHeight);
                metric.u1 = float(r) / float(atlasWidth);
                metric.v1 = float(t) / float(atlasHeight);

                // Get glyph quad for placement offsets (relative to baseline)
                g.getQuadPlaneBounds(l, b, r, t);
                metric.offsetX = l;
                metric.offsetY = b;
                metric.width = r - l;
                metric.height = t - b;
                metric.advance = float(g.getAdvance());
            }


            // Cleanup
            msdfgen::destroyFont(font);
        }
        msdfgen::deinitializeFreetype(ft);
    }
    return true;
}

} // namespace dsaViz
