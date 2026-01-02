#include <dsaviz/render/TextRenderer.hpp>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

namespace dsaviz {
    static const char * FONT_VERTEX = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 uTransform;

out vec2 vTexCoord;

void main() {
    vTexCoord = aUV;
    gl_Position = uTransform * vec4(aPos, 0.0, 1.0);
}
)";

static const char * FONT_FRAGMENT = R"(#version 330 core
in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3 uColor;

/* Median helper */
float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

const float smoothing = 0.01;

void main()
{
    vec3 msdf = texture(uTexture, vTexCoord).rgb;
    float dist = median(msdf.r, msdf.g, msdf.b);
    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);
    FragColor = vec4(uColor.rgb, alpha);
}
)";

void TextRenderer::initialize() {
    // Vertex format:
    // vec2 position
    // vec2 uv
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Dynamic because text changes often
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );

    // UV
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

    // Load shader (adjust path to your system)
    shader.setupShader(
        FONT_VERTEX,
        FONT_FRAGMENT,
        false
    );
}

void TextRenderer::render(const glm::mat4 &transform,
                          const glm::vec3 &color,
                          const std::vector<float>* quadData,
                          const Texture* fontAtlas) {

    if (!quadData || quadData->empty() || !fontAtlas)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        quadData->size() * sizeof(float),
        quadData->data(),
        GL_DYNAMIC_DRAW
    );

    shader.bind();
    shader.uploadMat4("uTransform", transform);
    shader.uploadVec3("uColor", color);
    shader.uploadInt("uTexture", 0);

    fontAtlas->bind(0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(quadData->size() / 4));
    glBindVertexArray(0);
}

} // namespace dsaviz
