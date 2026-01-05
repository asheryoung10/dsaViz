#pragma once
#include <dsaviz/util/glm.hpp>
#include <dsaviz/render/Shader.hpp>
#include <vector>

namespace dsaviz {

class CircleRenderer {
public:
    void initialize(int segments, float outlineRadius);
    void render(const glm::mat4& transform, const glm::vec3& color, const glm::vec3& outlineColor);

private:
    unsigned int vao = 0;
    unsigned int vbo = 0;
    int vertexCount = 0;

    unsigned int outlineVao = 0;
    unsigned int outlineVbo = 0;
    int outlineVertexCount = 0;

    Shader shader;

    static std::vector<glm::vec2> generateCircleVertices(int segments, float radius);
};

} // namespace dsaviz