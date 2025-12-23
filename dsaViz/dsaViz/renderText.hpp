#pragma once
#include <glad/gl.h>
#include <dsaViz/dsaContext.hpp>
#include <dsaViz/shaderProgram.hpp>
#include <dsaViz/texture.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>

namespace dsaViz {

class RenderText{
public:

    RenderText(DSAContext* context, float fontSize) : RenderText(context, fontSize, "../assets/JetBrainsMono-Bold.ttf") {

    }

    RenderText(DSAContext* context, float fontSize, const char* fontFile) {
        this->context = context;
        setup(fontSize, fontFile);
    }

    RenderText(DSAContext* context) : RenderText(context, 256.0f) {}

    ~RenderText() = default;

    void setup(float size, const char* fontFile);

    void render(bool camera) {
        render("Default Text\n!@#$%^&*()", 0.0f, 0.0f, 0.1f, camera);
    }

    #include <format>
    template<typename... Args>
    void renderFmt(float x, float y, float scale, bool camera, glm::vec4 color,
                std::format_string<Args...> fmt,
                Args&&... args)
    {
        std::string s = std::format(fmt, std::forward<Args>(args)...);
        render(s.c_str(), x, y, scale, camera, color);
    }

    void render(const std::string& text, float startX, float startY, float scale, bool camera) {
        render(text, startX, startY, scale, camera, glm::vec4(1.0f));
    }

    void render(const std::string& text, float startX, float startY, float scale, bool camera, glm::vec4 color);

    bool generateAtlas(const char* fontFilename, float size);

private:
    DSAContext* context;
    Texture textureAtlas;
    ShaderProgram msdfShaderProgram;

    GLuint vao = 0;
    GLuint vbo = 0;

const char* vertexShaderSource = 
R"(#version 330 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec2 aTexCoord;
    uniform mat4 uTransform;
    out vec2 vTexCoord;

    void main()
    {
        gl_Position = uTransform * vec4(aPos, 0.0, 1.0);
        vTexCoord = aTexCoord;
    }
)";

const char* fragmentShaderSource = 
R"(#version 330 core
in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 textColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec3 msdf = texture(uTexture, vTexCoord).rgb;
    float sd = median(msdf.r, msdf.g, msdf.b);

    // This is the important part:
    // sd is a normalized signed distance, 0.5 is the edge.
    // We need (sd - 0.5) to know how far from the edge we are.
    float pxRange = 4.0; // your MSDF "range" in pixels

    float w = fwidth(sd);                           // derivative in screen space
    float dist = (sd - 0.5) * pxRange / w;          // signed distance in pixel units
    float alpha = clamp(dist + 0.5, 0.0, 1.0);      // remap to [0,1]

    FragColor = vec4(textColor.rgb, alpha);
}
)";


    struct GlyphInfo {
        float u0, v0, u1, v1; // UV coordinates
        float advance;
        float offsetX, offsetY;
    };

    struct GlyphMetric {
        float u0, v0, u1, v1;
        float offsetX, offsetY;
        float width, height;
        float advance;
    };
    std::vector<GlyphMetric> glyphMetrics;
    float newLineVerticalShift;

    std::vector<GlyphInfo> glyphs;
};

}
