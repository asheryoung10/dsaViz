#pragma once
#include <dsaviz/util/glm.hpp>
#include <dsaviz/render/Texture.hpp>

namespace dsaviz {
    enum class RenderCommandType {
        Circle,
        Square,
        Text
    };
    struct RenderCommand {
        RenderCommandType type;
        glm::mat4 transform;
        glm::vec3 color;
        glm::vec3 outlineColor;

        // Below is only for textured quads. (Used for text rendering)
        const Texture* glyphAtlas = nullptr;
        const std::vector<float>* glyphQuadData = nullptr; // x, y, u, v, width, height per quad, shared transform.
    };
}