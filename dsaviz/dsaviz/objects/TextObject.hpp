#pragma once
#include <dsaviz/core/VizObject.hpp>
#include <font.hpp>

namespace dsaviz {
    class TextObject : VizObject {
        public:
            TextObject()
            void submit (Renderer &renderer) override;

        private:
        std::vector<float> quadData; // x, y, u, v, width, height per quad
        Font& font;
    }
}