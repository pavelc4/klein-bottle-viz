cmake_minimum_required(VERSION 3.10)
project(klein-bottle-viz)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

set (SOURCES
  src/main.cpp
  src/KleinMath.cpp
  src/KleinMesh.cpp
  src/DemoScene.cpp
  src/FontRenderer.cpp
  src/Overlay.cpp
  src/Stats.cpp
  src/Renderer.cpp
)

add_executable (${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    OpenGL::GLU
    SDL2::SDL2
)
