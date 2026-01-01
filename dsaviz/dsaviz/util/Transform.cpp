#include "Transform.hpp"
#include <dsaviz/util/Transform.hpp>

namespace dsaviz {

Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale)
    : position(position), rotation(rotation), scale(scale), matrix(1.0f),
      matrixUpToDate(false) {}

const glm::vec3 &Transform::getPosition() const { return position; }

const glm::quat &Transform::getRotation() const { return rotation; }

const glm::vec3 &Transform::getScale() const { return scale; }

void Transform::setPosition(const glm::vec3 &p) {
  position = p;
  matrixUpToDate = false;
}

void Transform::setRotation(const glm::quat &r) {
  rotation = r;
  matrixUpToDate = false;
}

void Transform::setScale(const glm::vec3 &s) {
  scale = s;
  matrixUpToDate = false;
}

void Transform::updateMatrix() const {
  if (matrixUpToDate)
    return;

  glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
  glm::mat4 r = glm::mat4_cast(rotation);
  glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);

  matrix = t * r * s;
  matrixUpToDate = true;
}

glm::mat4 Transform::getMatrix() const {
  updateMatrix();
  if (parent)
    return parent->getMatrix() * matrix;
  else
    return matrix;
}
  void Transform::rotate(float angleRadians, const glm::vec3 &axis) {
    setRotation(glm::normalize(
        glm::angleAxis(angleRadians, glm::normalize(axis)) * rotation));
  }

glm::vec3 Transform::getLocalDirection(const glm::vec3 &dir) const {
  return glm::normalize(glm::vec3(getMatrix() * glm::vec4(dir, 0.0f)));
}

glm::vec3 Transform::getForward() const {
  return getLocalDirection(glm::vec3(0, 0, -1));
}
glm::vec3 Transform::getBackward() const {
  return getLocalDirection(glm::vec3(0, 0, 1));
}
glm::vec3 Transform::getRight() const {
  return getLocalDirection(glm::vec3(1, 0, 0));
}
glm::vec3 Transform::getLeft() const {
  return getLocalDirection(glm::vec3(-1, 0, 0));
}
glm::vec3 Transform::getUp() const {
  return getLocalDirection(glm::vec3(0, 1, 0));
}
glm::vec3 Transform::getDown() const {
  return getLocalDirection(glm::vec3(0, -1, 0));
}

void Transform::lookAt(const glm::vec3 &target, const glm::vec3 &up) {
  glm::mat4 lookAtMatrix = glm::lookAt(position, target, up);
  glm::quat newRotation = glm::quat_cast(glm::inverse(lookAtMatrix));
  setRotation(newRotation);
}

} // namespace dsaviz