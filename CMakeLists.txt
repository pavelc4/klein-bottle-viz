cmake_minimum_required(VERSION 3.10)
project(klein-bottle-viz)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package (OpenGL REQUIRED)
find_package (GLUT REQUIRED)

set (SOURCES
  src/main.cpp
  src/KleinMath.cpp
  src/Renderer.cpp
)

add_executable (${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    GLUT::GLUT
)
