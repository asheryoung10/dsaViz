#include <dsaviz/render/TextureRenderer.hpp>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace dsaviz {

static const char* TEXTURE_VERTEX = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 uTransform;

out vec2 vUV;

void main()
{
    vUV = aUV;
    gl_Position = uTransform * vec4(aPos, 0.0, 1.0);
}
)";

static const char* TEXTURE_FRAGMENT = R"(#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    FragColor = texture(uTexture, vUV);
}
)";

void TextureRenderer::initialize()
{
    // Quad: two triangles, CCW
    // pos(x,y), uv(u,v)
    const float quadVertices[] = {
        // triangle 1
        0.0f, 0.0f,  0.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 1.0f,
        1.0f, 1.0f,  1.0f, 0.0f,

        // triangle 2
        0.0f, 0.0f,  0.0f, 1.0f,
        1.0f, 1.0f,  1.0f, 0.0f,
        0.0f, 1.0f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(quadVertices),
        quadVertices,
        GL_STATIC_DRAW
    );

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );

    // uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)(2 * sizeof(float))
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader.setupShader(
        TEXTURE_VERTEX,
        TEXTURE_FRAGMENT,
        false
    );
}

void TextureRenderer::render(const glm::mat4& transform,
                             const Texture* texture)
{
    if (!texture)
        return;

    glDisable(GL_DEPTH_TEST);

    texture->bind(0);

    shader.bind();
    shader.uploadMat4("uTransform", transform);
    shader.uploadInt("uTexture", 0);


    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

} // namespace dsaviz
