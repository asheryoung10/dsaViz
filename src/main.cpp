#include <asio.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <miniaudio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define SPDLOG_HEADER_ONLY
#define SPDLOG_NO_FMT
#include <spdlog/spdlog.h>

#include <thread>
#include <atomic>
#include <cmath>

// =======================
// Miniaudio globals
// =======================
ma_device g_audioDevice;
std::atomic<float> g_phase{0.0f};

void audio_callback(ma_device* device, void* output, const void*, ma_uint32 frameCount)
{
    float* out = static_cast<float*>(output);

    const float frequency = 440.0f + glfwGetTime() * 20.0f; // Sweep up over time
    const float sampleRate = (float)device->sampleRate;
    const float increment = 2.0f * 3.14159265f * frequency / sampleRate;

    for (ma_uint32 i = 0; i < frameCount; i++) {
        float sample = std::sin(g_phase);
        g_phase += increment;

        out[i * 2 + 0] = sample * 0.2f; // left
        out[i * 2 + 1] = sample * 0.2f; // right
    }
}

// =======================
// GLFW error callback
// =======================
void glfw_error_callback(int error, const char* description)
{
    spdlog::error("GLFW error {}: {}", error, description);
}

int main()
{
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::info("Starting application");

    // =======================
    // ASIO
    // =======================
    asio::io_context io;
    spdlog::info("Asio io_context created");

    std::thread netThread([&]() {
        spdlog::info("Asio thread running");
        io.run();
        spdlog::info("Asio thread stopped");
    });

    // =======================
    // GLFW + OpenGL
    // =======================
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        spdlog::critical("Failed to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Minimal Stack Test", nullptr, nullptr);
    if (!window) {
        spdlog::critical("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::critical("Failed to initialize glad");
        return -1;
    }
    spdlog::info(
        "OpenGL initialized: {}",
        reinterpret_cast<const char*>(glGetString(GL_VERSION))
    );


    // =======================:w
    // GLM
    // =======================
    glm::mat4 projection = glm::perspective(
        glm::radians(60.0f),
        800.0f / 600.0f,
        0.1f,
        100.0f
    );

    spdlog::info("GLM projection matrix created");

    // =======================
    // Miniaudio
    // =======================
    ma_device_config audioConfig = ma_device_config_init(ma_device_type_playback);
    audioConfig.playback.format   = ma_format_f32;
    audioConfig.playback.channels = 2;
    audioConfig.sampleRate        = 48000;
    audioConfig.dataCallback      = audio_callback;

    if (ma_device_init(nullptr, &audioConfig, &g_audioDevice) != MA_SUCCESS) {
        spdlog::critical("Failed to initialize audio device");
        return -1;
    }

    ma_device_start(&g_audioDevice);
    spdlog::info("Audio device started (440 Hz sine wave)");

    // =======================
    // Main loop
    // =======================
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    // =======================
    // Shutdown
    // =======================
    spdlog::info("Shutting down");

    ma_device_uninit(&g_audioDevice);

    io.stop();
    netThread.join();

    glfwDestroyWindow(window);
    glfwTerminate();

    spdlog::info("Clean shutdown complete");
    return 0;
}
