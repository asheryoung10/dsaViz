#pragma once
#include <miniaudio.h>
#include <cstddef>


namespace dsaViz {
class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    // Non-copyable
    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;

    // Playback
    void playFile(const char* path);
    void playFileStreamed(const char* path);

private:
    ma_engine engine{};
    bool initialized = false;
};
}