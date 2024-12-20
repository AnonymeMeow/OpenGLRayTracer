cmake_minimum_required(VERSION 3.11)
project(OpenGLRayTracer)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(. src)

function(append_src_list dir)
    aux_source_directory(${dir} dir_src)
    set(src ${src} ${dir_src} PARENT_SCOPE)
endfunction()

append_src_list(console)
append_src_list(opengl)

add_executable(RayTracer ${src})

find_package(SDL2 REQUIRED)
target_link_libraries(RayTracer PRIVATE SDL2::SDL2)

find_package(OpenGL REQUIRED)
target_link_libraries(RayTracer PRIVATE OpenGL::GL)