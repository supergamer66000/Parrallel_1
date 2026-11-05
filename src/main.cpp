#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <chrono>
#include <thread>
#include <glm/glm.hpp>

#include "glm/geometric.hpp"
#include "image.h"

int main() {
    const char* PATH = ""; // PATH TO IMAGE
    Image image(PATH);
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

    bool running = true;
    for (int i = 0; i < thread_count; ++i) {
        const auto start = i * local_height;
        const auto end = (i == thread_count - 1) ? height : start + local_height;

        constexpr float SPEED = 0.005;
        
        threads.emplace_back([&, start, end] {
           while (running) {
                for (int y = start; y < end; ++y) {
                    for (int x = 1; x < width; ++x) {
                        
                        /* CODE GOES HERE */

                        auto sleep_start = std::chrono::steady_clock::now();
                        auto target_duration = std::chrono::duration<double, std::milli>(SPEED);

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
