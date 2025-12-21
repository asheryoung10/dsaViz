#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <dsaViz/logSetup.hpp>

#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdf-atlas-gen/AtlasGenerator.h>
#include <msdfgen/core/BitmapRef.hpp>


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using namespace msdf_atlas;

bool generateAtlas(const char *fontFilename) {
    bool success = false;
    // Initialize instance of FreeType library
    if (msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype()) {
        spdlog::info("FreeType library initialized");
        // Load font file
        if (msdfgen::FontHandle *font = msdfgen::loadFont(ft, fontFilename)) {
            spdlog::info("Font '{}' loaded", fontFilename);
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
            ImmediateAtlasGenerator<float, 3, msdfGenerator, msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>>
            generator(width, height);

            // set attributes, thread count, etc.
            GeneratorAttributes attributes;
            generator.setAttributes(attributes);
            generator.setThreadCount(4);

            // generate the atlas
            generator.generate(glyphs.data(), glyphs.size());

            const auto &bitmap = generator.atlasStorage(); // AtlasStorage reference

            // convert to BitmapConstRef
            msdfgen::BitmapConstRef<msdf_atlas::byte, 3> bitmapRef = bitmap;

            // now you can access width, height, pixels
            stbi_write_png(
                "atlas.png",
                bitmapRef.width,
                bitmapRef.height,
                3,
                bitmapRef.pixels,
                bitmapRef.width * 3
            );


            success = true;
            // Cleanup
            msdfgen::destroyFont(font);
        }else {
            spdlog::error("Failed to load font '{}'", fontFilename);
        }
        msdfgen::deinitializeFreetype(ft);
    }else {
        spdlog::error("Failed to initialize FreeType library");
    }
    return success;
}

void glfw_error_callback(int error, const char* description)
{
    spdlog::error("GLFW error {}: {}", error, description);
}

int main()
{
    dsaViz::setupLogging(spdlog::level::trace);
    spdlog::info("Logging initialized.");

    spdlog::info("Setting glfw error callback.");
    glfwSetErrorCallback(glfw_error_callback);

    spdlog::info("Initializing GLFW.");
    if (!glfwInit()) {
        spdlog::critical("Failed to initialize GLFW.");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    int windowWidth = 800;
    int windowHeight = 800;
    spdlog::info("Creating GLFW window with size {}x{}.", windowWidth, windowHeight);
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "dsaViz", nullptr, nullptr);
    if (!window) {
        spdlog::critical("Failed to create GLFW window.");
        glfwTerminate();
        return -1;
    }
    int windowWidthActual, windowHeightActual;
    glfwGetFramebufferSize(window, &windowWidthActual, &windowHeightActual);
    spdlog::info("GLFW window created with size {}x{}.", windowWidthActual, windowHeightActual);

    spdlog::info("Making OpenGL context current.");
    glfwMakeContextCurrent(window);

    spdlog::info("Setting swap interval to 1.");
    glfwSwapInterval(1);

    spdlog::info("Initializing opengl functions with glad.");
     int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }
    spdlog::info("OpenGL initialized: {}.", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    spdlog::info("GLM projection matrix created.");

    spdlog::info("Starting main loop");
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }
    spdlog::info("Exited main loop.");

    spdlog::info("Destroying GLFW window and terminating.");
    glfwDestroyWindow(window);
    glfwTerminate();

    spdlog::info("Clean shutdown complete.");
    return 0;
}
