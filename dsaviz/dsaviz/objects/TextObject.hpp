#pragma once
#include <dsaviz/core/VizObject.hpp>
#include <dsaviz/text/Font.hpp>

namespace dsaviz {
    class TextObject : public VizObject {
        public:
            TextObject(const std::string& text, const Font& font);
            void setText(const std::string& text, const Font& font);
            void submit(Renderer &renderer) override;

        private:
        std::string text;
        const Font* font;
        std::vector<float> quadData; // x, y, width, height, u0, v0, u1, v1,
    };
}