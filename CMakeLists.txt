cmake_minimum_required(VERSION 3.15)
project(PixelSorter LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

include(FetchContent)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
)

FetchContent_MakeAvailable(glfw glm)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE external/stb)

add_executable(PixelSorter
    src/main.cpp
    src/image.h
)

target_compile_definitions(PixelSorter PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(PixelSorter PRIVATE OpenGL::GL glfw glm stb)
