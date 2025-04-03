cmake_minimum_required(VERSION 3.28)
project(OpenGLRayTracer)

set(CMAKE_CXX_STANDARD 23)

add_executable(RayTracer)

add_subdirectory(console)
add_subdirectory(model)
add_subdirectory(opengl)
add_subdirectory(view)

target_sources(RayTracer
  PUBLIC
  main.cpp
)

find_package(SDL2 REQUIRED)
target_link_libraries(RayTracer PRIVATE SDL2::SDL2)

find_package(OpenGL REQUIRED)
target_link_libraries(RayTracer PRIVATE OpenGL::GL)

target_link_libraries(RayTracer PRIVATE jsoncpp)