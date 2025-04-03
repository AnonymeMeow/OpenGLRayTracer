cmake_minimum_required(VERSION 3.28)
project(OpenGLRayTracer)

set(CMAKE_CXX_STANDARD 23)

add_library(RayTracer OBJECT)

add_subdirectory(console)
add_subdirectory(model)
add_subdirectory(opengl)
add_subdirectory(view)

find_package(SDL2 REQUIRED)
target_link_libraries(RayTracer PUBLIC SDL2::SDL2)

find_package(OpenGL REQUIRED)
target_link_libraries(RayTracer PUBLIC OpenGL::GL)

target_link_libraries(RayTracer PUBLIC jsoncpp)

add_executable(RayTracerExec main.cpp)

target_link_libraries(RayTracerExec RayTracer)