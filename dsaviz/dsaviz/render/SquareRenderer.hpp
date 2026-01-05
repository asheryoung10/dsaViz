#pragma once
#include <dsaviz/util/glm.hpp>
#include <dsaviz/render/Shader.hpp>
#include <vector>

namespace dsaviz {

class SquareRenderer {
public:
    void initialize(int segmentsPerCorner, float cornerRadius, float outlineRadius);
    void render(const glm::mat4& transform, const glm::vec3& color, const glm::vec3& outlineColor);

private:
    unsigned int vao = 0;
    unsigned int vbo = 0;
    int vertexCount = 0;

    unsigned int outlineVao = 0;
    unsigned int outlineVbo = 0;
    int outlineVertexCount = 0;

    Shader shader;

    static std::vector<glm::vec2> generateSquareVertices(int segments, float length, float cornerRadius);
};

} // namespace dsaviz