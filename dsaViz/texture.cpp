#include "dsaViz/texture.hpp"

#include <spdlog/spdlog.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <vector>

namespace dsaViz {
    Texture::Texture()
        : m_id(0), m_width(0), m_height(0), m_format(TextureFormat::RGBA8) {}

    Texture::~Texture() {
        destroy();
    }

    Texture::Texture(Texture&& other) noexcept {
        *this = std::move(other);
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        destroy();
        m_id = other.m_id;
        m_width = other.m_width;
        m_height = other.m_height;
        m_format = other.m_format;

        other.m_id = 0;
        other.m_width = 0;
        other.m_height = 0;
        return *this;
    }

    bool Texture::loadFromFile(const std::string& path, bool flipY) {
        stbi_set_flip_vertically_on_load(flipY);

        int w, h, channels;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
        if (!data) {
            spdlog::error("Failed to load image '{}'", path);
            return false;
        }

        TextureFormat fmt;
        switch (channels) {
        case 1: fmt = TextureFormat::R8; break;
        case 3: fmt = TextureFormat::RGB8; break;
        case 4: fmt = TextureFormat::RGBA8; break;
        default:
            stbi_image_free(data);
            spdlog::error("Unsupported channel count {}", channels);
            return false;
        }

        bool ok = createFromMemory(w, h, fmt, data);
        stbi_image_free(data);
        return ok;
    }

    bool Texture::createFromMemory(
        int width,
        int height,
        TextureFormat format,
        const unsigned char* pixels
    ) {
        destroy();

        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            toGLInternalFormat(format),
            width,
            height,
            0,
            toGLDataFormat(format),
            GL_UNSIGNED_BYTE,
            pixels
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        m_width = width;
        m_height = height;
        m_format = format;
        return true;
    }

    bool Texture::createEmpty(int width, int height, TextureFormat format) {
        return createFromMemory(width, height, format, nullptr);
    }

    bool Texture::writeToFile(const std::string& path) const {
        if (!isValid()) return false;

        GLenum dataFormat = toGLDataFormat(m_format);
        int channels =
            (m_format == TextureFormat::R8) ? 1 :
            (m_format == TextureFormat::RGB8) ? 3 : 4;

        std::vector<unsigned char> pixels(m_width * m_height * channels);

        glBindTexture(GL_TEXTURE_2D, m_id);
        glGetTexImage(GL_TEXTURE_2D, 0, dataFormat, GL_UNSIGNED_BYTE, pixels.data());
        glBindTexture(GL_TEXTURE_2D, 0);

        int stride = m_width * channels;
        return stbi_write_png(
            path.c_str(),
            m_width,
            m_height,
            channels,
            pixels.data(),
            stride
        ) != 0;
    }

    void Texture::bind(unsigned int unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    void Texture::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    bool Texture::isValid() const { return m_id != 0; }
    GLuint Texture::id() const { return m_id; }
    int Texture::width() const { return m_width; }
    int Texture::height() const { return m_height; }
    TextureFormat Texture::format() const { return m_format; }

    void Texture::destroy() {
        if (m_id != 0) {
            glDeleteTextures(1, &m_id);
            m_id = 0;
        }
        m_width = m_height = 0;
    }

    GLenum Texture::toGLInternalFormat(TextureFormat format) const {
        switch (format) {
        case TextureFormat::R8:    return GL_R8;
        case TextureFormat::RGB8:  return GL_RGB8;
        case TextureFormat::RGBA8: return GL_RGBA8;
        }
        return GL_RGBA8;
    }

    GLenum Texture::toGLDataFormat(TextureFormat format) const {
        switch (format) {
        case TextureFormat::R8:    return GL_RED;
        case TextureFormat::RGB8:  return GL_RGB;
        case TextureFormat::RGBA8: return GL_RGBA;
        }
        return GL_RGBA;
    }

}
