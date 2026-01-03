#include <dsaviz/objects/ValueSquareObject.hpp>
#include <string>

namespace dsaviz {
ValueSquareObject::ValueSquareObject(int value, VizContext &context)
    : context(context), value(value),
      textObject(std::to_string(value), *context.font) {
  squareObject.transform.parent = &this->transform;
  textObject.transform.parent = &squareObject.transform;
  textObject.color = {1,1,1};
  textObject.transform.setPosition(glm::vec3(0, 0, 0.1));
  textObject.transform.setScale(glm::vec3(0.5));
}

void ValueSquareObject::submit(Renderer &renderer) {
  squareObject.color = this->color;
  squareObject.submit(renderer);
  textObject.submit(renderer);
}

int ValueSquareObject::getValue() const { return value; }

void ValueSquareObject::setValue(int value) {
  this->value = value;
  textObject.setText(std::to_string(value), *context.font);
}

} // namespace dsaviz