#include <dsaviz/render/CircleRenderer.hpp>
#include <glad/gl.h>
#include <cmath>
#include <spdlog/spdlog.h>

namespace dsaviz {
  static const char* CIRCLE_VERT_SRC = R"(#version 330 core

layout(location = 0) in vec2 aPos;

uniform mat4 uTransform;

void main() {
    gl_Position = uTransform * vec4(aPos, 0.0, 1.0);
}
)";
static const char* CIRCLE_FRAG_SRC = R"(#version 330 core

out vec4 FragColor;

uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

void CircleRenderer::initialize(int segments, float outlineRadius) {
    if(outlineRadius < 0.0f) outlineRadius = 0.0f;
    if(outlineRadius > 0.4f) outlineRadius = 0.4f;
    // Generate vertices
    std::vector<glm::vec2> vertices = generateCircleVertices(segments, 0.5f - outlineRadius);
    std::vector<glm::vec2> outerVertices = generateCircleVertices(segments, 0.5f);
    std::vector<glm::vec2> outlineVertices;
    for(size_t i = 0; i < vertices.size(); ++i) {
        outlineVertices.push_back(outerVertices[i]);
        outlineVertices.push_back(vertices[i]);
    }
    vertexCount = static_cast<int>(vertices.size());
    outlineVertexCount = static_cast<int>(outlineVertices.size());

    // Create VAO + VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(glm::vec2),
                 vertices.data(),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                  // attribute index
        2,                  // vec2
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec2),
        (void*)0
    );

    glBindVertexArray(0);

    glGenVertexArrays(1, &outlineVao);
    glGenBuffers(1, &outlineVbo);
    glBindVertexArray(outlineVao);
    glBindBuffer(GL_ARRAY_BUFFER, outlineVbo);
    glBufferData(GL_ARRAY_BUFFER,
                 outlineVertices.size() * sizeof(glm::vec2),
                 outlineVertices.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec2),
        (void*)0
    );
    glBindVertexArray(0);

    // Load shader (you can replace with your own shader paths)
    shader.setupShader(
        CIRCLE_VERT_SRC,
        CIRCLE_FRAG_SRC,
        false
    );
    spdlog::info("CircleRenderer initialized with {} segments.", segments);
}

void CircleRenderer::render(const glm::mat4& transform, const glm::vec3& color) {
    shader.bind();
    shader.uploadMat4("uTransform", transform);
    shader.uploadVec3("uColor", color);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    shader.uploadVec3("uColor", glm::vec3(0.0f, 0.0f, 0.0f)); // Outline color (black)
    glBindVertexArray(outlineVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, outlineVertexCount);
    glBindVertexArray(0);
}

std::vector<glm::vec2> CircleRenderer::generateCircleVertices(int segments, float radius) {
    if (segments < 3) segments = 3;

    std::vector<glm::vec2> vertices;
    vertices.reserve(segments + 2);

    // Center vertex for triangle fan
    vertices.emplace_back(0.0f, 0.0f);

    float angleStep = 2.0f * 3.1415926535f / segments;

    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        vertices.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
    }

    return vertices;
}

} // namespace dsaviz