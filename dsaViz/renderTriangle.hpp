#pragma once
#include <dsaViz/renderState.hpp>
#include <dsaViz/shaderProgram.hpp>
#include <dsaViz/texture.hpp>
#include <spdlog/spdlog.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace dsaViz {

/// @brief Render state for rendering a triangle.
class RenderTriangle : public RenderState {
public:
    RenderTriangle(GLFWwindow* windowHandle) : vao(0), vbo(0), ebo(0), windowHandle(windowHandle) {}

    ~RenderTriangle() override {
        if (ebo) glDeleteBuffers(1, &ebo);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (vao) glDeleteVertexArrays(1, &vao);
    }

    /// @brief Sets up the render state.
    void setup() override {
        spdlog::info("Setting up RenderTriangle state.");

        shaderProgram.createFromSource(vertexShaderSource, fragmentShaderSource);
        if (!texture.createFromFile("../assets/atlas.png")) {
            spdlog::error("Failed to load texture");
        }

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // position
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // color
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);


        glBindVertexArray(0);
    }

    /// @brief Renders the triangle.
    void render() override {
        spdlog::trace("Rendering triangle.");
        shaderProgram.use();
        texture.bind(GL_TEXTURE0);
        glBindVertexArray(vao);
        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(windowHandle, &framebufferWidth, &framebufferHeight);
        float aspectRatio = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
        glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
        shaderProgram.uploadGLMMatrix("uTransform", projection);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    GLFWwindow* windowHandle;
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aColor;
        layout(location = 2) in vec2 aTexCoord;
        uniform mat4 uTransform;
        out vec3 ourColor;
        out vec2 ourTexCoord;
        void main()
        {
            gl_Position = uTransform * vec4(aPos, 1.0);
            ourColor = aColor;
            ourTexCoord = aTexCoord;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec3 ourColor;
        in vec2 ourTexCoord;
        uniform sampler2D ourTexture;
        void main()
        {
            FragColor = texture(ourTexture, ourTexCoord);
        }
    )";

    std::vector<float> vertices = {
            // Positions    // Colors     // UVs
        -0.5,  0.5,0,  1,1,1, 0,1, // Top-left
        -0.5, -0.5,0,  1,1,1, 0,0,  // Bottom-left
        0.5, -0.5,0,  1,1,1, 1,0 , // Bottom-right
        0.5,  0.5,0,  1,1,1, 1,1  // Top-right

    };


    std::vector<unsigned int> indices = {
        0, 1, 2 // Triangle
        , 0, 2, 3 // Triangle
    };

    ShaderProgram shaderProgram;
    Texture texture;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

}
