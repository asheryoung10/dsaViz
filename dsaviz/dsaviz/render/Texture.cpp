#include <dsaviz/render/Texture.hpp>
#include <glad/gl.h>
#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

namespace dsaviz {

  void Texture::setLinearFiltering() const {
  if (id == 0) {
    spdlog::warn("Attempted to set filtering on invalid texture.");
    return;
  }

  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setNearestFiltering() const {
  if (id == 0) {
    spdlog::warn("Attempted to set filtering on invalid texture.");
    return;
  }

  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
}


static unsigned int toGLInternalFormat(TextureFormat format) {
  switch (format) {
  case TextureFormat::R8:
    return GL_R8;
  case TextureFormat::RGB8:
    return GL_RGB8;
  case TextureFormat::RGBA8:
    return GL_RGBA8;
  }
  return GL_RGBA8;
}

static unsigned int toGLDataFormat(TextureFormat format) {
  switch (format) {
  case TextureFormat::R8:
    return GL_RED;
  case TextureFormat::RGB8:
    return GL_RGB;
  case TextureFormat::RGBA8:
    return GL_RGBA;
  }
  return GL_RGBA;
}

Texture::Texture(const std::string &textureFilepath, bool flipVertically) {
  if (!createFromFile(textureFilepath, flipVertically)) {
    spdlog::critical("Failed to create texture from file '{}'.",
                     textureFilepath);
  }
}

Texture::Texture(const unsigned char *pixelData, int width, int height,
                 TextureFormat format) {
  if (!createFromMemory(pixelData, width, height, format)) {
    spdlog::critical("Failed to create texture from memory.");
  }
}

Texture::Texture(Texture &&other) noexcept
    : id(other.id), width(other.width), height(other.height),
      format(other.format) {
  other.id = 0; // prevent double deletion
  other.width = 0;
  other.height = 0;
}

Texture &Texture::operator=(Texture &&other) noexcept {
  if (this != &other) {
    // Delete existing texture
    if (id != 0) {
      glDeleteTextures(1, &id);
    }

    // Steal data
    id = other.id;
    width = other.width;
    height = other.height;
    format = other.format;

    // Reset other
    other.id = 0;
    other.width = 0;
    other.height = 0;
  }
  return *this;
}

Texture::~Texture() {
  if (id != 0) {
    glDeleteTextures(1, &id);
  }
}

void Texture::bind(unsigned int unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, id);
}

bool Texture::createFromFile(const std::string &textureFilepath,
                             bool flipVertically) {
  stbi_set_flip_vertically_on_load(flipVertically);

  int w, h, channels;
  unsigned char *data =
      stbi_load(textureFilepath.c_str(), &w, &h, &channels, 0);

  if (!data) {
    spdlog::error("Failed to load image '{}'", textureFilepath);
    return false;
  }

  TextureFormat fmt;
  switch (channels) {
  case 1:
    fmt = TextureFormat::R8;
    break;
  case 3:
    fmt = TextureFormat::RGB8;
    break;
  case 4:
    fmt = TextureFormat::RGBA8;
    break;
  default:
    stbi_image_free(data);
    spdlog::error("Unsupported channel count {}", channels);
    return false;
  }

  bool success = createFromMemory(data, w, h, fmt);
  stbi_image_free(data);
  return success;
}

bool Texture::createFromMemory(const unsigned char *pixelData, int width,
                               int height, TextureFormat format) {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexImage2D(GL_TEXTURE_2D, 0, toGLInternalFormat(format), width, height, 0,
               toGLDataFormat(format), GL_UNSIGNED_BYTE, pixelData);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

  this->width = width;
  this->height = height;
  this->format = format;

  return true;
}

bool Texture::writeToPNG(const std::string &filepath,
                         bool flipVertically) const {
  if (id == 0) {
    spdlog::warn("Attempted to write invalid texture to file.");
    return false;
  }

  stbi_flip_vertically_on_write(flipVertically);

  GLenum dataFormat = toGLDataFormat(format);
  int channels = (format == TextureFormat::R8)     ? 1
                 : (format == TextureFormat::RGB8) ? 3
                                                   : 4;

  std::vector<unsigned char> pixels(width * height * channels);

  glBindTexture(GL_TEXTURE_2D, id);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glGetTexImage(GL_TEXTURE_2D, 0, dataFormat, GL_UNSIGNED_BYTE, pixels.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  int stride = width * channels;

  return stbi_write_png(filepath.c_str(), width, height, channels,
                        pixels.data(), stride) != 0;
}

} // namespace dsaviz