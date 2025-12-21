#pragma once
#include <dsaViz/renderState.hpp>
#include <dsaViz/shaderProgram.hpp>
#include <dsaViz/texture.hpp>
#include <spdlog/spdlog.h>
#include <vector>
#include <glad/gl.h>

namespace dsaViz {

/// @brief Render state for rendering a triangle.
class RenderTriangle : public RenderState {
public:
    RenderTriangle() : vao(0), vbo(0), ebo(0) {}

    ~RenderTriangle() override {
        if (ebo) glDeleteBuffers(1, &ebo);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (vao) glDeleteVertexArrays(1, &vao);
    }

    /// @brief Sets up the render state.
    void setup() override {
        spdlog::info("Setting up RenderTriangle state.");

        // Compile shader
        shaderProgram.createFromSource(vertexShaderSource, fragmentShaderSource);

        // Generate buffers
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // position
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // color
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    /// @brief Renders the triangle.
    void render() override {
        spdlog::trace("Rendering triangle.");

        shaderProgram.use();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aColor;
        out vec3 ourColor;
        void main()
        {
            gl_Position = vec4(aPos, 1.0);
            ourColor = aColor;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec3 ourColor;
        void main()
        {
            FragColor = vec4(ourColor, 1.0);
        }
    )";

    std::vector<float> vertices = {
        // Positions        // Colors
         0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Top vertex (Red)
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom left vertex (Green)
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // Bottom right vertex (Blue)
    };

    std::vector<unsigned int> indices = {
        0, 1, 2 // Triangle
    };

    ShaderProgram shaderProgram;
    Texture texture;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

}
