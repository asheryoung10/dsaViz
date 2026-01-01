#pragma once
#include <dsaviz/core/Scene.hpp>

namespace dsaviz {
    class HomeScene : public Scene {
        public:
        HomeScene();
        void update(VizContext& context) override;
    };

}