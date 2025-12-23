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

    RenderText(DSAContext* context, float fontSize) {
        this->context = context;
        setup(fontSize);
    }

    RenderText(DSAContext* context) : RenderText(context, 256.0f) {}

    ~RenderText() = default;

    void setup(float size);

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
    uniform vec4 textColor; // color of the text

    float median(float r, float g, float b) {
        return max(min(r,g), min(max(r,g), b));
    }

    void main() {
        vec3 sample = texture(uTexture, vTexCoord).rgb;
        float sd = median(sample.r, sample.g, sample.b);

        // Anti-aliased alpha from MSDF
        float pxRange = 0.02; // adjust for edge softness
        float alpha = smoothstep(0.5 - pxRange, 0.5 + pxRange, sd);

        FragColor = vec4(textColor.rgb, alpha); // correct alpha from MSDF
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
