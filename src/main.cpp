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

#include <msdf-atlas-gen/msdf-atlas-gen.h>

using namespace msdf_atlas;

bool generateAtlas(const char *fontFilename) {
    bool success = false;
    // Initialize instance of FreeType library
    if (msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype()) {
        // Load font file
        if (msdfgen::FontHandle *font = msdfgen::loadFont(ft, fontFilename)) {
            // Storage for glyph geometry and their coordinates in the atlas
            std::vector<GlyphGeometry> glyphs;
            // FontGeometry is a helper class that loads a set of glyphs from a single font.
            // It can also be used to get additional font metrics, kerning information, etc.
            FontGeometry fontGeometry(&glyphs);
            // Load a set of character glyphs:
            // The second argument can be ignored unless you mix different font sizes in one atlas.
            // In the last argument, you can specify a charset other than ASCII.
            // To load specific glyph indices, use loadGlyphs instead.
            fontGeometry.loadCharset(font, 1.0, Charset::ASCII);
            // Apply MSDF edge coloring. See edge-coloring.h for other coloring strategies.
            const double maxCornerAngle = 3.0;
            for (GlyphGeometry &glyph : glyphs)
                glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
            // TightAtlasPacker class computes the layout of the atlas.
            TightAtlasPacker packer;
            // Set atlas parameters:
            // setDimensions or setDimensionsConstraint to find the best value
            packer.setDimensionsConstraint(DimensionsConstraint::SQUARE);
            // setScale for a fixed size or setMinimumScale to use the largest that fits
            packer.setMinimumScale(24.0);
            // setPixelRange or setUnitRange
            packer.setPixelRange(2.0);
            packer.setMiterLimit(1.0);
            // Compute atlas layout - pack glyphs
            packer.pack(glyphs.data(), glyphs.size());
            // Get final atlas dimensions
            int width = 0, height = 0;
            packer.getDimensions(width, height);
            // The ImmediateAtlasGenerator class facilitates the generation of the atlas bitmap.
            ImmediateAtlasGenerator<
                float, // pixel type of buffer for individual glyphs depends on generator function
                3, // number of atlas color channels
                msdfGenerator, // function to generate bitmaps for individual glyphs
                BitmapAtlasStorage<byte, 3> // class that stores the atlas bitmap
                // For example, a custom atlas storage class that stores it in VRAM can be used.
            > generator(width, height);
            // GeneratorAttributes can be modified to change the generator's default settings.
            GeneratorAttributes attributes;
            generator.setAttributes(attributes);
            generator.setThreadCount(4);
            // Generate atlas bitmap
            generator.generate(glyphs.data(), glyphs.size());
            // The atlas bitmap can now be retrieved via atlasStorage as a BitmapConstRef.
            // The glyphs array (or fontGeometry) contains positioning data for typesetting text.
            //success = my_project::submitAtlasBitmapAndLayout(generator.atlasStorage(), glyphs);
            // Cleanup
            msdfgen::destroyFont(font);
        }
        msdfgen::deinitializeFreetype(ft);
    }
    return success;
}


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

    if (!generateAtlas("path/to/font.ttf"))  {
        spdlog::critical("msdf-atlas-gen test FAILED");
        //return -1;
    }

    spdlog::info("msdf-atlas-gen test PASSED");


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
