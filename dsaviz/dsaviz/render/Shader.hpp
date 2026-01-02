#pragma once
#include <string>
#include <dsaviz/util/glm.hpp>

namespace dsaviz { 
    class Shader {
        public:
        Shader() = default;
        /// Creates a shader program with either source strings for files dictated by boolean flag.
        Shader(const std::string& vertexShader, const std::string& fragmentShader, bool areFilepaths);

        bool setupShader(const std::string& vertexShader, const std::string& fragmentShader, bool areFilepaths);

        /// Set this as the active program.
        void bind();

        unsigned int getID();

        void uploadInt(const std::string& variableName, int value);
        void uploadFloat(const std::string& variableName, float value);
        void uploadVec2(const std::string& variableName, float x, float y);
        void uploadVec2(const std::string& variableName, const glm::vec2& vector);
        void uploadVec3(const std::string& variableName, float x, float y, float z);
        void uploadVec3(const std::string& variableName, const glm::vec3& vector);
        void uploadVec4(const std::string& variableName, float x, float y, float z, float w);
        void uploadVec4(const std::string& variableName, const glm::vec4& vector);
        void uploadMat4(const std::string& variableName, const glm::mat4& matrix);

        private:
        unsigned int id = 0;
        void compileShader(const char* shaderSource, bool isVertexShader);
    };
}