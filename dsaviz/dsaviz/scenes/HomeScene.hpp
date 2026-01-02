#pragma once
#include <dsaviz/core/Scene.hpp>

namespace dsaviz {
    class HomeScene : public Scene {
        public:
        void initialize(VizContext& context);
        void update(VizContext& context) override;
    };

}