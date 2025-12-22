#pragma once
#include <dsaViz/renderState.hpp>
#include <dsaViz/shaderProgram.hpp>
#include <dsaViz/texture.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>

namespace dsaViz {

class RenderText : public RenderState {
public:
    RenderText(GLFWwindow* windowHandle) : vao(0), vbo(0), windowHandle(windowHandle) {};
    ~RenderText() override = default;

    /// @brief Sets up the render state by generating an MSDF atlas.
    void setup() override {
        setup(64.0f);
    };

    void setup(float size);
    /// @brief Renders the text using the generated atlas.
    void render() override {
        render(textToRender, 0.0f, 0.0f, 1.0f);
    }
    void render(const std::string& text, float startX = -0.9f, float startY = 0.8f, float scale = 1.0f);

    /// @brief Loads a font and generates an MSDF atlas.
    bool generateAtlas(const char* fontFilename, float size);

private:
    float textRenderScale;
    GLFWwindow* windowHandle;
    Texture textureAtlas;
    ShaderProgram msdfShaderProgram;

    GLuint vao = 0;
    GLuint vbo = 0;

const char* vertexShaderSource = R"(
    #version 330 core
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

const char* fragmentShaderSource = R"(
    #version 330 core
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

    std::vector<GlyphInfo> glyphs;
    std::string textToRender = "This is dsaViz rendering MSDF text!\n!@#$%^&*()_+1234567890";
};

} // namespace dsaViz
