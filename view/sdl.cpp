#include "sdl.hpp"

#include <SDL2/SDL_opengl.h>

SDL_Context::SDL_Context(
    int window_width,
    int window_height,
    const std::string& window_name,
    Camera&& camera
):
    window_width(window_width),
    window_height(window_height),
    window(SDL_CreateWindow(
        window_name.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_OPENGL
    )),
    gl_context(SDL_GL_CreateContext(window)),
    camera(camera)
{
    if (window == NULL)
    {
        sdl_logger.error("Failed to create window: {}.", SDL_GetError());
        exit(-1);
    }
    if (gl_context == NULL)
    {
        sdl_logger.error("Failed to create GL context: {}.", SDL_GetError());
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glViewport(0, 0, window_width, window_height);
}

void SDL_Context::swap() const
{
    SDL_GL_SwapWindow(window);
}

SDL_Context::~SDL_Context()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
}