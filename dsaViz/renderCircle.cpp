#include <dsaViz/renderCircle.hpp>
#include <dsaViz/camera.hpp>

namespace dsaViz {

void RenderCircle::setup()
{
    shader.loadFromSource(vertexShaderSource, fragmentShaderSource);

    // Unit quad centered at origin
    // Covers [-1, 1] in both axes
    float vertices[] = {
        -1.f, -1.f,
         1.f, -1.f,
         1.f,  1.f,

        -1.f, -1.f,
         1.f,  1.f,
        -1.f,  1.f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    spdlog::info("RenderCircle initialized");
}

void RenderCircle::render(
    glm::vec2 position,
    float rotation,
    glm::vec2 scale,
    glm::vec4 color,
    bool camera
)
{
    shader.bind();

    glm::mat4 transform(1.0f);

    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    transform = glm::rotate(transform, rotation, glm::vec3(0, 0, 1));
    transform = glm::scale(transform, glm::vec3(scale, 1.0f));

    if (camera) {
        transform = context->camera->getViewProjectionMatrix() * transform;
    }

    shader.setMat4("uTransform", transform);
    shader.setVec4("uColor", color);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

} // namespace dsaViz
