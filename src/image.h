#ifndef PIXELSORTER_IMAGE_H
#define PIXELSORTER_IMAGE_H

#include <iostream>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <filesystem>
#include <glm/glm.hpp>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class Image {
public:
    Image(const char* path) {
        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("Error loading Image");
        }

        data = stbi_load(path, &width, &height, &channels, 0);
    }

    // Prevent copying
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    // Getters
    const int &get_width() const { return width; }
    const int &get_height() const { return height; }
    const int &get_channels() const { return channels; }

    std::vector<glm::vec3> get_data() {
        if (!data) throw std::runtime_error("Error: Failed to get pixel data");

        const size_t size = static_cast<size_t>(width * height);
        std::vector<glm::vec3> pixels;
        pixels.reserve(size);

        for (auto i = 0; i < size; ++i) {
            const auto index = i * channels;
            pixels.push_back({
                data[index + 0] / 255.f, // Red
                data[index + 1] / 255.f, // Green
                data[index + 2] / 255.f  // Blue
            });
        }

        return pixels;
    }

    ~Image() { // free the data
        stbi_image_free(data);
    }
private:
    uint8_t* data = nullptr;

    int width, height, channels;
};

#endif //PIXELSORTER_IMAGE_H
