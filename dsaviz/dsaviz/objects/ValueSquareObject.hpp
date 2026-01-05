#pragma once
#include <dsaviz/core/VizObject.hpp>
#include <dsaviz/objects/SquareObject.hpp>
#include <dsaviz/objects/TextObject.hpp>
#include <dsaviz/core/VizContext.hpp>
#include <dsaviz/util/Color.hpp>

namespace dsaviz {
    class ValueSquareObject : public VizObject {
        public:
        ValueSquareObject(int value, VizContext& context);
        void submit(Renderer& renderer) override;
        void setValue(int value);
        int getValue() const;
        void setTextColor(Color color);
        private:
        int value;
        // Must come before textobject for correct initialization order.
        VizContext& context;
        SquareObject squareObject;
        TextObject textObject;


    };
}