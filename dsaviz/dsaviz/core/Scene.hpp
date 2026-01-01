#pragma once
#include <dsaviz/render/Renderer.hpp>
#include <dsaviz/core/VizContext.hpp>
#include <dsaviz/core/VizObject.hpp>

namespace dsaviz {
    class Scene {
        public:
        virtual void update(VizContext& context) = 0;
        void render(Renderer &renderer);
        protected:
        std::vector<std::unique_ptr<VizObject>> vizObjects;
    };
}