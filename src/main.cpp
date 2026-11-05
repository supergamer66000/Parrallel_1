#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <chrono>
#include <thread>
#include <glm/glm.hpp>

#include "glm/geometric.hpp"
#include "image.h"

constexpr int PASSES = 5;

const float calc_brightness(const glm::vec3 &color);
const bool can_sort(const glm::vec3 &pixel, const float &min, const float &max);

int main() {
    Image image("../gettyimages-174557857-612x612.jpg");
    const auto width = image.get_width();
    const auto height = image.get_height();
    const auto SIZE = width * height;
    auto data = image.get_data();

    // OpenGL and GLFW stuff
    GLFWwindow* window = nullptr;
    if (!glfwInit()) throw std::runtime_error("Error: Could not initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    window = glfwCreateWindow(width, height, "PixelSorter", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetErrorCallback([] (int code, const char* message) {
        std::cout << "GLFW error " << code << ": " << message << std::endl;
    });

    glPixelZoom(1.f, -1.f);
    glRasterPos2f(-1.f, 1.f);
    // ------------- //


    const auto thread_count = std::thread::hardware_concurrency() - 4; // Leave 2 cores avaiabl

    std::vector<glm::vec3> output = data;
    std::vector<std::jthread> threads;
    threads.reserve(thread_count);

    const auto local_height = height / thread_count;

    bool running = true;
    for (int i = 0; i < thread_count; ++i) {
        const auto start = i * local_height;
        const auto end = (i == thread_count - 1) ? height : start + local_height;

        threads.emplace_back([&, start, end] {
            uint32_t total_passes = 0;
            const float min = 0.2;
            const float max = 0.6;

            while (running) {
                bool swapped = false;

                for (int y = start; y < end; ++y) {
                    for (int x = 1; x < width; ++x) {
                        const int idx = y * width + x;

                        const auto current_pixel = output[idx];
                        const auto prev_pixel = output[idx - 1];
                        if (can_sort(current_pixel, min, max) && can_sort(prev_pixel, min, max)) {
                                if (calc_brightness(prev_pixel) > calc_brightness(current_pixel)) {
                                output[idx] = prev_pixel;
                                output[idx - 1] = current_pixel;
                                swapped = true;
                            }

                        }

                        if (!swapped) output[idx] = current_pixel;


                        auto sleep_start = std::chrono::steady_clock::now();
                        auto target_duration = std::chrono::duration<double, std::milli>(0.005);

                        // Spin phase captures the precise fractional deadline (Idk)
                        while (std::chrono::steady_clock::now() - sleep_start < target_duration) {
                    #if defined(_MSC_VER)
                            __nop(); // MSVC optimization barrier
                    #elif defined(__GNUC__) || defined(__clang__)
                            asm volatile("nop"); // GCC/Clang optimization barrier
                    #endif
                        }
                    }
                }
            }
        });
    }

    // THIS IS ON THE MAIN THREAD
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(width, height, GL_RGB, GL_FLOAT, output.data());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    running = false;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


const float calc_brightness(const glm::vec3 &color) {
    return ((color.x + color.y + color.z) / 3.f) + 0.1;
}

const bool can_sort(const glm::vec3 &pixel, const float &min, const float &max) {
    const auto brightness = calc_brightness(pixel);
    return (brightness >= min && brightness <= max);
}
