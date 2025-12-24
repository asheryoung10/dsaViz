#pragma once
#include <random>
#include <cstdint>
#include <glm/glm.hpp>

namespace dsaViz {

class Random {
public:
    // Non-deterministic seed
    Random()
        : rng(std::random_device{}())
    {}

    // Deterministic seed
    explicit Random(uint32_t seed)
        : rng(seed)
    {}

    // Integers
    int intRange(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }

    // Floats
    float floatRange(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }

    // 0..1
    float unit()
    {
        return floatRange(0.0f, 1.0f);
    }

    // Random boolean
    bool chance(float probability)
    {
        return unit() < probability;
    }

    // Random vec2
    glm::vec2 vec2(float min, float max)
    {
        return {
            floatRange(min, max),
            floatRange(min, max)
        };
    }

    // Random direction (unit circle)
    glm::vec2 direction2D()
    {
        float angle = floatRange(0.0f, glm::two_pi<float>());
        return { std::cos(angle), std::sin(angle) };
    }

private:
    std::mt19937 rng;
};

} // namespace dsaViz
