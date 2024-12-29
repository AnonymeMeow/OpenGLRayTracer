#pragma once

#include "camera.hpp"
#include "../console/logger.hpp"

#include <SDL2/SDL.h>

class SDL_Context
{
    inline static const Logger sdl_logger{"SDL"};
    const int window_width;
    const int window_height;
    SDL_Window* const window;
    SDL_GLContext const gl_context;
public:
    Camera camera;
    SDL_Context(int, int, const std::string&, Camera&&);
    void swap() const;
    ~SDL_Context();
};