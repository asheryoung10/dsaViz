#pragma once
#include <glad/gl.h>
#include <dsaViz/dsaContext.hpp>
#include <dsaViz/shaderProgram.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace dsaViz {

class RenderCircle {
public:
    RenderCircle(DSAContext* context)
        : context(context)
    {
        setup();
    }

    ~RenderCircle()
    {
        if (vbo) glDeleteBuffers(1, &vbo);
        if (vao) glDeleteVertexArrays(1, &vao);
    }

    void render(
        glm::vec2 position,
        float rotation,
        glm::vec2 scale,
        glm::vec4 color,
        bool camera
    );

private:
    void setup();

private:
    DSAContext* context;
    ShaderProgram shader;

    GLuint vao = 0;
    GLuint vbo = 0;

    const char* vertexShaderSource = R"(#version 330 core
        layout (location = 0) in vec2 aPos;

        uniform mat4 uTransform;

        out vec2 vLocalPos;

        void main()
        {
            vLocalPos = aPos;
            gl_Position = uTransform * vec4(aPos, 0.0, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(#version 330 core
in vec2 vLocalPos;
out vec4 FragColor;

uniform vec4 uColor;

void main()
{
    float dist = length(vLocalPos);

    // Pixel width for AA
    float aa = fwidth(dist);

    // Alpha only around the edge
    float alpha = 1.0 - smoothstep(1.0 - aa, 1.0 + aa, dist);

    // Hard reject fully outside pixels
    if (alpha <= 0.0)
        discard;

    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}

    )";
};

} // namespace dsaViz
