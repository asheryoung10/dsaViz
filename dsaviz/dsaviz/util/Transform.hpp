#pragma once
#include <dsaviz/util/glm.hpp>

namespace dsaviz {

class Transform {
public:
  Transform(glm::vec3 position = glm::vec3(0.0f),
            glm::quat rotation = glm::quat(1, 0, 0, 0),
            glm::vec3 scale = glm::vec3(1.0f));

  Transform *parent = nullptr;

  const glm::vec3 &getPosition() const;
  const glm::quat &getRotation() const;
  const glm::vec3 &getScale() const;

  glm::mat4 getMatrix() const;

  glm::vec3 getLocalDirection(const glm::vec3 &direction) const;
  glm::vec3 getForward() const;
  glm::vec3 getBackward() const;
  glm::vec3 getRight() const;
  glm::vec3 getLeft() const;
  glm::vec3 getUp() const;
  glm::vec3 getDown() const;

  void setPosition(const glm::vec3 &position);
  void setRotation(const glm::quat &rotation);
  void setScale(const glm::vec3 &scale);
  void lookAt(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0, 1, 0));

  void rotate(float angleRadians, const glm::vec3 &axis);

private:
  glm::vec3 position;
  glm::quat rotation;
  glm::vec3 scale;

  mutable glm::mat4 matrix;
  mutable bool matrixUpToDate;

  void updateMatrix() const;
};

} // namespace dsaviz