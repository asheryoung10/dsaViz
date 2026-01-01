#pragma once
#include <miniaudio.h>
#include <cstddef>

namespace dsaviz {
class Audio {
public:
    Audio();
    ~Audio();

    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    void playFile(const char* path);
    void playFileStreamed(const char* path);

private:
    ma_engine engine{};
    bool initialized = false;
};
}