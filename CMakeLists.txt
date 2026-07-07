cmake_minimum_required(VERSION 3.16)
project(engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(engine
    engine.cc
    easy_image.cc
    ini_configuration.cc
    l_parser.cc
    lsystem_draw.cpp
    lsystem_replacement.cpp
    Structuren.cpp
    vector3d.cc
    transformations.cpp
    eye_transform.cpp
    projection.cpp
    wireframe_engine.cpp
    wireframe_draw.cpp
    figures3d.cpp
    LSystem3D.cpp
    zbuffer_draw.cpp
    triangulate.cpp
    triangle_draw.cpp
)

target_compile_options(engine PRIVATE -Wall -Wextra)
