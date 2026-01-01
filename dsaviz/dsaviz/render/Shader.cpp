#include <dsaviz/render/Shader.hpp>
#include <glad/gl.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>

namespace dsaviz {

static std::string readFile(const std::string &filepath) {
  std::ifstream file(filepath, std::ios::in | std::ios::binary);
  if (!file) {
    spdlog::error("Failed to open shader file '{}'", filepath);
    return "";
  }

  std::ostringstream ss;
  ss << file.rdbuf();
  return ss.str();
}
Shader::Shader(const std::string &vertexShader,
                             const std::string &fragmentShader,
                             bool areFilepaths) {
  setupShader((vertexShader), fragmentShader, areFilepaths);
}
bool Shader::setupShader(const std::string &vertexShader,
                                const std::string &fragmentShader,
                                bool areFilepaths) {

  if (id != 0) {
    spdlog::warn("Overwritting without destroying opengl shader program!");
    glDeleteProgram(id);
    id = 0;
  }
  std::string vertSource;
  std::string fragSource;

  if (areFilepaths) {
    vertSource = readFile(vertexShader);
    fragSource = readFile(fragmentShader);
  } else {
    vertSource = vertexShader;
    fragSource = fragmentShader;
  }

  if (vertSource.empty() || fragSource.empty()) {
    spdlog::critical("Shader source is empty. Cannot create program.");
    id = 0;
    return false;
  }

  id = glCreateProgram();

  compileShader(vertSource.c_str(), true);
  compileShader(fragSource.c_str(), false);

  glLinkProgram(id);

  GLint success;
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    GLint logLength = 0;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);

    std::string log(logLength, '\0');
    glGetProgramInfoLog(id, logLength, nullptr, log.data());

    spdlog::critical("Shader program linking failed:\n{}", log);
    glDeleteProgram(id);
    id = 0;
    return false;
  }

  return true;
}

void Shader::bind() { glUseProgram(id); }

unsigned int Shader::getID() { return id; }

static GLint getLocation(unsigned int program, const std::string &name) {
  GLint loc = glGetUniformLocation(program, name.c_str());
  if (loc == -1) {
    spdlog::warn("Uniform '{}' not found in shader program {}", name, program);
  }
  return loc;
}

void Shader::uploadInt(const std::string &variableName, int value) {
  glUniform1i(getLocation(id, variableName), value);
}

void Shader::uploadFloat(const std::string &variableName, float value) {
  glUniform1f(getLocation(id, variableName), value);
}

void Shader::uploadVec2(const std::string &variableName, float x,
                               float y) {
  glUniform2f(getLocation(id, variableName), x, y);
}

void Shader::uploadVec2(const std::string &variableName,
                               const glm::vec2 &vector) {
  glUniform2f(getLocation(id, variableName), vector.x, vector.y);
}

void Shader::uploadVec3(const std::string &variableName, float x,
                               float y, float z) {
  glUniform3f(getLocation(id, variableName), x, y, z);
}

void Shader::uploadVec3(const std::string &variableName,
                               const glm::vec3 &vector) {
  glUniform3f(getLocation(id, variableName), vector.x, vector.y, vector.z);
}

void Shader::uploadVec4(const std::string &variableName, float x,
                               float y, float z, float w) {
  glUniform4f(getLocation(id, variableName), x, y, z, w);
}

void Shader::uploadVec4(const std::string &variableName,
                               const glm::vec4 &vector) {
  glUniform4f(getLocation(id, variableName), vector.x, vector.y, vector.z,
              vector.w);
}

void Shader::uploadMat4(const std::string &variableName,
                               const glm::mat4 &matrix) {
  glUniformMatrix4fv(getLocation(id, variableName), 1, GL_FALSE,
                     glm::value_ptr(matrix));
}

void Shader::compileShader(const char *shaderSource,
                                  bool isVertexShader) {
  GLuint shader =
      glCreateShader(isVertexShader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
  glShaderSource(shader, 1, &shaderSource, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    GLint logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

    std::string log(logLength, '\0');
    glGetShaderInfoLog(shader, logLength, nullptr, log.data());

    spdlog::critical("{} shader compilation failed:\n{}",
                     isVertexShader ? "Vertex" : "Fragment", log);

    glDeleteShader(shader);
    return;
  }

  glAttachShader(id, shader);
  glDeleteShader(shader);
}
} // namespace dsaviz