#pragma once
#include <glad/gl.h>
#include <spdlog/spdlog.h>

namespace dsaViz {
    /// @brief Class for handling OpenGL shader programs.
    class ShaderProgram {
    public:
        /// @brief Default constructor.
        ShaderProgram() : programID(0) {}

        /// @brief Constructor that creates a shader program from source code.
        /// @param vertexSource
        /// @param fragmentSource 
        ShaderProgram(const char* vertexSource, const char* fragmentSource) : programID(0) {
            createFromSource(vertexSource, fragmentSource);
        }

        /// @brief Destructor to clean up the shader program resource.
        ~ShaderProgram() {
            if (programID != 0) {
                glDeleteProgram(programID);
            }
        }

        /// @brief Creates a shader program from vertex and fragment shader source code.
        bool createFromSource(const char* vertexSource, const char* fragmentSource) {
            GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
            if (vertexShader == 0) return false;

            GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
            if (fragmentShader == 0) {
                glDeleteShader(vertexShader);
                return false;
            }

            programID = glCreateProgram();
            glAttachShader(programID, vertexShader);
            glAttachShader(programID, fragmentShader);
            glLinkProgram(programID);

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            GLint linkStatus;
            glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
            if (linkStatus == GL_FALSE) {
                char infoLog[512];
                glGetProgramInfoLog(programID, 512, nullptr, infoLog);
                spdlog::error("Shader program linking failed: {}", infoLog);
                glDeleteProgram(programID);
                programID = 0;
                return false;
            }
            return true;
        }

        /// @brief Uses the shader program for rendering.
        void use() const {
            glUseProgram(programID);
        }

        /// @brief Gets the OpenGL program ID.
        GLuint getProgramID() const {
            return programID;
        }

    private:
        /// @brief OpenGL program ID.
        GLuint programID;

        /// @brief Compiles a shader from source code.
        /// @param shaderType The type of shader to compile.
        /// @param source The shader source code.
        /// @return The compiled shader ID, or 0 if compilation failed.
        GLuint compileShader(GLenum shaderType, const char* source) {
            GLuint shader = glCreateShader(shaderType);
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);

            GLint compileStatus;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
            if (compileStatus == GL_FALSE) {
                char infoLog[512];
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                if(shaderType == GL_VERTEX_SHADER) {
                    spdlog::error("Vertex shader compilation failed: {}", infoLog);
                } else if(shaderType == GL_FRAGMENT_SHADER) {
                    spdlog::error("Fragment shader compilation failed: {}", infoLog);
                } else {
                    spdlog::error("Shader compilation failed, shader type invalid: {}", infoLog);
                }
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        }
    };
}