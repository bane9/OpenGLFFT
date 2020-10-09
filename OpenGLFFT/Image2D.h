#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdexcept>
#include <string_view>
#include "glad.h"
#include <type_traits>
#include "checkGl.h"
#include <vector>

class Image2D
{
public:
    Image2D() = delete;

    Image2D(std::string_view path, bool normalize = false)
    {
        auto img_data = stbi_load(path.data(), &width, &height, &channels, 0);

        using namespace std::string_literals;
        
        if (!img_data)
        {
            auto err = "Failed to load image at path: "s + path.data();
            throw std::runtime_error(err);
        }

        data.resize(get_total_size());

        for (std::size_t i = 0; i < get_total_size(); i++)
        {
            data[i] = img_data[i];
            if (normalize) data[i] /= 255.0f;
        }

        stbi_image_free(img_data);

    }

    Image2D(int width, int height, int channels)
        : width(width), height(height), channels(channels)
    {
        data.resize(get_total_size());
    }

    Image2D(const Image2D& rhs)
    {
        *this = rhs;
    }

    Image2D(Image2D&& rhs) noexcept
    {
        *this = std::move(rhs);
    }

    Image2D& operator=(const Image2D& rhs)
    {
        width = rhs.width;
        height = rhs.height;
        channels = rhs.channels;
        data = rhs.data;

        return *this;
    }

    Image2D& operator=(Image2D&& rhs) noexcept
    {
        width = rhs.width;
        height = rhs.height;
        channels = rhs.channels;
        data = std::move(rhs.data);
        ImageID = rhs.ImageID;

        rhs.width = 0;
        rhs.height = 0;
        rhs.ImageID = 0;

        return *this;
    }

    ~Image2D()
    {
        cleanup_gpu();
    }

    float& get_pixel(int x, int y, int channel)
    {
        return (data.data() + (x + width * y) * channels)[channel];
    }

    float get_pixel(int x, int y, int channel) const
    {
        return (data.data() + (x + width * y) * channels)[channel];
    }

    void bind(GLuint slot)
    {
        glCheck(glBindImageTexture(slot, ImageID, 0, GL_FALSE, 0, GL_READ_WRITE, channelsToInternalFormat()));
    }

    void unbind()
    {
        if (ImageID)
        {
            glCheck(glBindTexture(GL_TEXTURE_2D, 0));
        }
    }

    void bindAsFrameBuffer()
    {
        glCheck(glGenFramebuffers(1, &fboId));
        glCheck(glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId));
        glCheck(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, ImageID, 0));

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    void upload()
    {
        cleanup_gpu();
        
        glCheck(glGenTextures(1, &ImageID));
        glCheck(glBindTexture(GL_TEXTURE_2D, ImageID));
        glCheck(glTexImage2D(GL_TEXTURE_2D, 0, channelsToInternalFormat(),
            width, height, 0, channelsToFormat(), GL_FLOAT, data.data()));

        glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        glCheck(glGenerateMipmap(GL_TEXTURE_2D));
    }

    [[nodiscard]] int get_width() const noexcept { return width; }
    [[nodiscard]] int get_height() const noexcept { return height; }
    [[nodiscard]] int get_channels() const noexcept { return channels; }
    [[nodiscard]] float* get_data() noexcept { return data.data(); }
    [[nodiscard]] const float* get_data() const noexcept { return data.data(); }
    [[nodiscard]] std::size_t get_total_size() const noexcept { return (std::size_t)width * height * channels; }
    [[nodiscard]] GLuint get_binding() const noexcept { return ImageID; }

private:

    void cleanup_gpu()
    {
        if (ImageID) 
        {
            glDeleteTextures(1, &ImageID);
        }

        if (fboId)
        {
            glDeleteFramebuffers(1, &fboId);
        }
    }

    GLenum channelsToFormat()
    {
        switch (channels)
        {
        case 4:
            return GL_RGBA;
        case 3:
            return GL_RGB;
        case 2:
            return GL_RG;
        case 1:
            return GL_RED;
        default:
            throw std::logic_error("Invalid channel count present in the image");
        }
    }

    GLenum channelsToInternalFormat()
    {
        switch (channels)
        {
        case 4:
            return GL_RGBA32F;
        case 3:
            return GL_RGB32F;
        case 2:
            return GL_RG32F;
        case 1:
            return GL_R32F;
        default:
            throw std::logic_error("Invalid channel count present in the image");
        }
    }

private:
    GLuint ImageID = 0;
    GLuint fboId = 0;

private:
    std::vector<float> data;
    int width;
    int height;
    int channels;

};
