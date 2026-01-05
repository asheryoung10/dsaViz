#pragma once
#include <dsaviz/util/Transform.hpp>
#include <dsaviz/util/Color.hpp>
#include <dsaviz/render/Renderer.hpp>

namespace dsaviz {
    class VizObject {
        public:
        Transform transform;
        Color color;
        Color outlineColor;
        virtual void submit(Renderer& renderer) = 0;
        virtual ~VizObject() = default;
    };
}