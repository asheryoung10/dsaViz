#include <dsaviz/render/SquareRenderer.hpp>
#include <glad/gl.h>
#include <cmath>
#include <spdlog/spdlog.h>

namespace dsaviz {
static const char *CIRCLE_VERT_SRC = R"(#version 330 core

layout(location = 0) in vec2 aPos;

uniform mat4 uTransform;

void main() {
    gl_Position = uTransform * vec4(aPos, 0.0, 1.0);
}
)";
static const char *CIRCLE_FRAG_SRC = R"(#version 330 core

out vec4 FragColor;

uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

void SquareRenderer::initialize(int segmentsPerCorner, float cornerRadius, float outlineRadius) {
  if (outlineRadius < 0.0f)
    outlineRadius = 0.0f;
  if (outlineRadius > 0.4f)
    outlineRadius = 0.4f;
  // Generate vertices
  std::vector<glm::vec2> vertices = generateSquareVertices(
      segmentsPerCorner, 0.5f - outlineRadius - cornerRadius, cornerRadius);
  std::vector<glm::vec2> outerVertices = generateSquareVertices(
      segmentsPerCorner, 0.5f - outlineRadius - cornerRadius, cornerRadius + outlineRadius);
  std::vector<glm::vec2> outlineVertices;
  for (size_t i = 0; i < vertices.size(); ++i) {
    outlineVertices.push_back(outerVertices[i]);
    outlineVertices.push_back(vertices[i]);
  }
  outlineVertices.push_back(outlineVertices[0]);
  outlineVertices.push_back(outlineVertices[1]);
  vertexCount = static_cast<int>(vertices.size());
  outlineVertexCount = static_cast<int>(outlineVertices.size());

  // Create VAO + VBO
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2),
               vertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, // attribute index
                        2, // vec2
                        GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);

  glBindVertexArray(0);

  glGenVertexArrays(1, &outlineVao);
  glGenBuffers(1, &outlineVbo);
  glBindVertexArray(outlineVao);
  glBindBuffer(GL_ARRAY_BUFFER, outlineVbo);
  glBufferData(GL_ARRAY_BUFFER, outlineVertices.size() * sizeof(glm::vec2),
               outlineVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
  glBindVertexArray(0);

  // Load shader (you can replace with your own shader paths)
  shader.setupShader(CIRCLE_VERT_SRC, CIRCLE_FRAG_SRC, false);
  spdlog::info("SquareRenderer initialized with {} segments.",
               segmentsPerCorner);
}

void SquareRenderer::render(const glm::mat4 &transform,
                            const glm::vec3 &color, const glm::vec3& outlineColor) {
  shader.bind();
  shader.uploadMat4("uTransform", transform);
  shader.uploadVec3("uColor", color);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
  glBindVertexArray(outlineVao);
  spdlog::info("{} {} {}", outlineColor.x, outlineColor.y, outlineColor.z);
  shader.uploadVec3("uColor", outlineColor);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, outlineVertexCount);
  glBindVertexArray(0);
}

std::vector<glm::vec2>
SquareRenderer::generateSquareVertices(int segments, float length,
                                       float cornerRadius) {
  std::vector<glm::vec2> vertices;

  auto generateCornerArc = [&](float xMul, float yMul, float angle_start = 0.0f) {
    for (int i = 1; i <= segments; ++i) {
      float theta = (float)i / (float)segments * glm::half_pi<float>() + angle_start;
      float x = xMul * length + cornerRadius * cosf(theta);
      float y = yMul * length + cornerRadius * sinf(theta);
      vertices.emplace_back(x, y);
    }
  };

  vertices.reserve(8 + segments * 4);
  vertices.push_back(glm::vec2(length + cornerRadius, length));
  generateCornerArc(1, 1, 0.0f);
  vertices.push_back(glm::vec2(length, length + cornerRadius));

  vertices.push_back(glm::vec2(-length, length + cornerRadius));
  generateCornerArc(-1, 1, glm::half_pi<float>());
  vertices.push_back(glm::vec2(-length - cornerRadius, length));

  vertices.push_back(glm::vec2(-length - cornerRadius, -length));
    generateCornerArc(-1, -1, glm::pi<float>());
  vertices.push_back(glm::vec2(-length, -length - cornerRadius));

  vertices.push_back(glm::vec2(length, -length - cornerRadius));
    generateCornerArc(1, -1, glm::pi<float>() * 1.5f);
  vertices.push_back(glm::vec2(length + cornerRadius, -length));

  return vertices;
}

} // namespace dsaviz