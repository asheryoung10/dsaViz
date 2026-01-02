#pragma once
#include <string>

namespace dsaviz {

enum class TextureFormat { R8, RGB8, RGBA8 };

class Texture {
public:
    Texture() = default;

    explicit Texture(const std::string& textureFilepath, bool flipVertically);
    explicit Texture(const unsigned char* pixelData, int width, int height,
                     TextureFormat format);

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    ~Texture();

    void bind(unsigned int unit) const;
    static void unbind(unsigned int unit);

    bool isValid() const { return id != 0; }

    int getID() const { return id; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    float getAspectRatio() const { return width == 0 ? 0.0f : float(width) / height; }
    TextureFormat getFormat() const { return format; }

    void setFiltering(int minFilter, int magFilter);
    void setWrap(int wrapS, int wrapT);

    bool writeToPNG(const std::string& filepath, bool flipVertically) const;

    void release();
    void swap(Texture& other) noexcept;
    bool createFromFile(const std::string& textureFilepath, bool flipVertically);
    bool createFromMemory(const unsigned char* pixelData, int width,
                          int height, TextureFormat format);

private:
    unsigned int id = 0;
    int width = 0;
    int height = 0;
    TextureFormat format = TextureFormat::RGBA8;
};

} // namespace dsaviz