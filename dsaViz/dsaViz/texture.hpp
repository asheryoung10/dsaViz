#pragma once
#include <glad/gl.h>
#include <string>

namespace dsaViz {

    enum class TextureFormat {
        R8,
        RGB8,
        RGBA8
    };

    class Texture {
    public:
        Texture();
        ~Texture();

        // Non-copyable (OpenGL resource)
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Movable
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        // Creation / loading
        bool loadFromFile(const std::string& path, bool flipY = true);
        bool createFromMemory(
            int width,
            int height,
            TextureFormat format,
            const unsigned char* pixels
        );
        bool createEmpty(
            int width,
            int height,
            TextureFormat format
        );

        // I/O
        bool writeToFile(const std::string& path) const;

        // Usage
        void bind(unsigned int unit = 0) const;
        void unbind() const;

        // Info
        bool isValid() const;
        GLuint id() const;
        int width() const;
        int height() const;
        TextureFormat format() const;

        // Explicit cleanup
        void destroy();

    private:
        GLuint m_id;
        int m_width;
        int m_height;
        TextureFormat m_format;

        GLenum toGLInternalFormat(TextureFormat format) const;
        GLenum toGLDataFormat(TextureFormat format) const;
    };

}
