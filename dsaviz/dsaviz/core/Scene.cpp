#include <dsaviz/core/Scene.hpp>
namespace dsaviz {
    void Scene::render(Renderer &renderer) {
        for (auto& obj : vizObjects) {
            obj->submit(renderer);
        }
    }
}