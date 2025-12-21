#pragma once

namespace dsaViz {
    class RenderState {
        public:
            virtual ~RenderState() = default;
            virtual void setup() = 0; 
            virtual void render() = 0; 
    };
}
