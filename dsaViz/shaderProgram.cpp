#include "dsaViz/shaderProgram.hpp"

#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>

namespace dsaViz {

    ShaderProgram::ShaderProgram() : m_id(0) {}

    ShaderProgram::~ShaderProgram() {
        destroy();
    }

    ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept {
        *this = std::move(other);
    }

    ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
        destroy();
        m_id = other.m_id;
        m_uniformCache = std::move(other.m_uniformCache);
        other.m_id = 0;
        return *this;
    }

    bool ShaderProgram::loadFromSource(
        const std::string& vertexSrc,
        const std::string& fragmentSrc
    ) {
        destroy();

        GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSrc);
        if (!vs) return false;

        GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
        if (!fs) {
            glDeleteShader(vs);
            return false;
        }

        m_id = glCreateProgram();
        glAttachShader(m_id, vs);
        glAttachShader(m_id, fs);
        glLinkProgram(m_id);

        glDeleteShader(vs);
        glDeleteShader(fs);

        GLint success;
        glGetProgramiv(m_id, GL_LINK_STATUS, &success);
        if (!success) {
            char log[1024];
            glGetProgramInfoLog(m_id, sizeof(log), nullptr, log);
            spdlog::error("Shader program link failed:\n{}", log);
            destroy();
            return false;
        }

        m_uniformCache.clear();
        return true;
    }

    bool ShaderProgram::loadFromFiles(
        const std::string& vertexPath,
        const std::string& fragmentPath
    ) {
        return loadFromSource(
            readFile(vertexPath),
            readFile(fragmentPath)
        );
    }

    void ShaderProgram::bind() const {
        glUseProgram(m_id);
    }

    void ShaderProgram::unbind() const {
        glUseProgram(0);
    }

    void ShaderProgram::setInt(const std::string& name, int value) {
        glUniform1i(getUniformLocation(name), value);
    }

    void ShaderProgram::setFloat(const std::string& name, float value) {
        glUniform1f(getUniformLocation(name), value);
    }
    
    void ShaderProgram::setVec3(const std::string& name, const glm::vec3& v) {
        glUniform3fv(getUniformLocation(name), 1, &v[0]);
    }

    void ShaderProgram::setVec4(const std::string& name, const glm::vec4& v) {
        glUniform4fv(getUniformLocation(name), 1, &v[0]);
    }

    void ShaderProgram::setVec4(const std::string& name, float x, float y, float z, float w) {
        glUniform4f(getUniformLocation(name), x, y, z, w);
    }

    void ShaderProgram::setMat4(const std::string& name, const glm::mat4& m) {
        glUniformMatrix4fv(
            getUniformLocation(name),
            1,
            GL_FALSE,
            &m[0][0]
        );
    }

    bool ShaderProgram::isValid() const {
        return m_id != 0;
    }

    GLuint ShaderProgram::id() const {
        return m_id;
    }

    void ShaderProgram::destroy() {
        if (m_id != 0) {
            glDeleteProgram(m_id);
            m_id = 0;
        }
        m_uniformCache.clear();
    }

    GLuint ShaderProgram::compileShader(GLenum type, const std::string& source) {
        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[1024];
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            spdlog::error(
                "{} shader compilation failed:\n{}",
                type == GL_VERTEX_SHADER ? "Vertex" : "Fragment",
                log
            );
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    GLint ShaderProgram::getUniformLocation(const std::string& name) {
        auto it = m_uniformCache.find(name);
        if (it != m_uniformCache.end())
            return it->second;

        GLint location = glGetUniformLocation(m_id, name.c_str());
        if (location == -1) {
            spdlog::warn("Uniform '{}' not found", name);
        }

        m_uniformCache[name] = location;
        return location;
    }

    std::string ShaderProgram::readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file) {
            spdlog::error("Failed to open shader file '{}'", path);
            return {};
        }
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

}
