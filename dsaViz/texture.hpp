#pragma once 
#include <glad/gl.h>
#include <spdlog/spdlog.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace dsaViz {
    /// @brief Class for handling OpenGL textures.
    class Texture {
    public:
        /// @brief Default constructor.
        Texture() : textureID(0) {}
        /// @brief Destructor to clean up the texture resource.
        ~Texture() {
            if (textureID != 0) {
                glDeleteTextures(1, &textureID);
            }
        }

        /// @brief Creates a texture from an image file.
        /// @param filename  Path to the image file.
        /// @return True if successful, false otherwise.
        bool createFromFile(const char* filename) {
            int width, height, channels;
            unsigned char* data = stbi_load(filename, &width, &height, &channels, 3);
            if (!data) {
                spdlog::error("Failed to load texture from file '{}'", filename);
                return false;
            }
            bool result = createFromData(width, height, data);
            stbi_image_free(data);
            return result;
        }

        /// @brief Creates a texture from raw pixel data.
        /// @param width 
        /// @param height 
        /// @param data 
        /// @return True if successful, false otherwise.
        bool createFromData(int width, int height, const unsigned char* data) {
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);
            return true;
        }

        /// @brief Binds the texture to a specific texture unit.
        /// @param textureUnit The texture unit to bind to.
        void bind(GLenum textureUnit) const {
            glActiveTexture(textureUnit);
            glBindTexture(GL_TEXTURE_2D, textureID);
        }

        /// @brief Gets the OpenGL texture ID.
        GLuint getTextureID() const {
            return textureID;
        }

    private:
        // OpenGL texture ID.
        GLuint textureID;
    };
}