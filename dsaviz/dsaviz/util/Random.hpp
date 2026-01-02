#pragma once
#include <random>
#include <cstdint>
#include <dsaviz/util/glm.hpp>
#include <dsaviz/util/Color.hpp>

namespace dsaviz {

class Random {
public:
    Random()
        : rng(std::random_device{}())
    {}

    explicit Random(uint32_t seed)
        : rng(seed)
    {}

    int intRange(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }

    float floatRange(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }

    float unit()
    {
        return floatRange(0.0f, 1.0f);
    }

    bool chance(float probability)
    {
        return unit() < probability;
    }

    glm::vec2 vec2(float min, float max)
    {
        return {
            floatRange(min, max),
            floatRange(min, max)
        };
    }

    glm::vec2 direction2D()
    {
        float angle = floatRange(0.0f, glm::two_pi<float>());
        return { std::cos(angle), std::sin(angle) };
    }

    glm::vec3 vec3(float min, float max)
    {
        return {
            floatRange(min, max),
            floatRange(min, max),
            floatRange(min, max)
        };
    }

    Color color() {
        return {
            floatRange(0, 1),
            floatRange(0, 1),
            floatRange(0, 1)

        };
    }




private:
    std::mt19937 rng;
};

} // namespace dsaViz
