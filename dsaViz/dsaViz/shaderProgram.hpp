#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace dsaViz {

    class ShaderProgram {
    public:
        ShaderProgram();
        ~ShaderProgram();

        // Non-copyable
        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;

        // Movable
        ShaderProgram(ShaderProgram&& other) noexcept;
        ShaderProgram& operator=(ShaderProgram&& other) noexcept;

        // Creation
        bool loadFromSource(
            const std::string& vertexSrc,
            const std::string& fragmentSrc
        );

        bool loadFromFiles(
            const std::string& vertexPath,
            const std::string& fragmentPath
        );

        // Usage
        void bind() const;
        void unbind() const;

        // Uniforms
        void setInt(const std::string& name, int value);
        void setFloat(const std::string& name, float value);
        void setVec3(const std::string& name, const glm::vec3& v);
        void setVec4(const std::string& name, const glm::vec4& v);
        void setVec4(const std::string& name, float x, float y, float z, float w);
        void setMat4(const std::string& name, const glm::mat4& m);

        // Info
        bool isValid() const;
        GLuint id() const;

        // Explicit cleanup
        void destroy();

    private:
        GLuint m_id;
        std::unordered_map<std::string, GLint> m_uniformCache;

        GLuint compileShader(GLenum type, const std::string& source);
        GLint getUniformLocation(const std::string& name);
        static std::string readFile(const std::string& path);
    };

}
