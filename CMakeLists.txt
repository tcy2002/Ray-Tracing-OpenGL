cmake_minimum_required(VERSION 3.22)
project(final)

set(CMAKE_CXX_STANDARD 14)

set(EXECUTABLE_OUTPUT_PATH ..)

link_directories(env/lib/x64)
include_directories(env/include)
include_directories(.)

add_executable(
        final
        main.cpp
        config/config.h
        config/config.cpp
        shader/shader.h
        shader/shader.cpp
        shader/shaderBuf.h
        model/model.h
        model/model.cpp
        texture/texture.h
        texture/texture.cpp
        loader/loader.h
        loader/loader.cpp
        material/material.h
        material/material.cpp
        bvh/bvh.h
        bvh/bvh.cpp
        scene/scene.h
        scene/scene.cpp)

target_link_libraries(
        final
        glew32.lib
        libfreeglut.a
        libopengl32.a)
