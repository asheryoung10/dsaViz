#define MINIAUDIO_IMPLEMENTATION
#include <dsaViz/audioEngine.hpp>

#include <stdexcept>
#include <spdlog/spdlog.h>

namespace dsaViz {
AudioEngine::AudioEngine()
{
    ma_engine_config config = ma_engine_config_init();

    if (ma_engine_init(&config, &engine) != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize miniaudio engine");
    }

    initialized = true;
}

AudioEngine::~AudioEngine()
{
    if (initialized) {
        ma_engine_uninit(&engine);
    }
}

void AudioEngine::playFile(const char* path)
{
    if (!initialized) return;

    spdlog::info("Playing audio file {}", path);
    ma_engine_play_sound(&engine, path, nullptr);
}


}