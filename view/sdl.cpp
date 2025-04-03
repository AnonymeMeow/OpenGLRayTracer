#include "sdl.hpp"

#include <SDL2/SDL_opengl.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

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

    stbi_flip_vertically_on_write(true);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glViewport(0, 0, window_width, window_height);
}

void SDL_Context::swap() const
{
    SDL_GL_SwapWindow(window);
}

void SDL_Context::render_loop(Program& prog, void (*call_back)())
{
    bool running = true;
    const Uint8* key_states = SDL_GetKeyboardState(nullptr);
    while (running)
    {
        SDL_Event event;
        struct
        {
            float xrel, yrel;
            float wheel;
            bool left, right;
            bool have_motion, have_wheel;
        } mouse_motion;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }
                else if (event.key.keysym.sym == SDLK_F2)
                {
                    unsigned char* img = new unsigned char[window_width * window_height * 3];
                    glReadPixels(0, 0, window_width, window_height, GL_RGB, GL_UNSIGNED_BYTE, img);
                    stbi_write_png("../.cache/screenshot.png", window_width, window_height, 3, img, 0);
                    delete[] img;
                }
                else if (event.key.keysym.sym == SDLK_r)
                {
                    camera.reset_fov();
                }
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                mouse_motion.xrel = event.motion.xrel;
                mouse_motion.yrel = event.motion.yrel;
                mouse_motion.left = event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT);
                mouse_motion.right = event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT);
                mouse_motion.have_motion = true;
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                mouse_motion.wheel = event.wheel.preciseY;
                mouse_motion.have_wheel = true;
            }
            else if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        float modifier = key_states[SDL_SCANCODE_LCTRL]? camera.ctrl_sensitivity_modifier: 1;

        if (mouse_motion.have_motion)
        {
            if (mouse_motion.left && !mouse_motion.right)
            {
                camera.turn_head(-mouse_motion.xrel, -mouse_motion.yrel, camera.mouse_rotation_sensitivity * modifier);
            }
            else if (mouse_motion.right && !mouse_motion.left)
            {
                camera.move_relative(mouse_motion.xrel, mouse_motion.yrel, 0, camera.mouse_move_sensitivity * modifier);
            }
            mouse_motion.have_motion = false;
        }

        if (mouse_motion.have_wheel)
        {
            camera.zoom(mouse_motion.wheel, camera.mouse_zoom_sensitivity * modifier);
            mouse_motion.have_wheel = false;
        }

        camera.move_relative_yaw(
            key_states[SDL_SCANCODE_A] - key_states[SDL_SCANCODE_D],
            key_states[SDL_SCANCODE_SPACE] - key_states[SDL_SCANCODE_LSHIFT],
            key_states[SDL_SCANCODE_W] - key_states[SDL_SCANCODE_S],
            camera.keyboard_sensitivity * modifier
        );

        glClearColor(0.5f, 0.5f, 0.5f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        prog.set("camera", camera);
        prog.draw();

        // unsigned char* img = new unsigned char[window_width * window_height * 3];
        // glReadPixels(0, 0, window_width, window_height, window_height, GL_UNSIGNED_BYTE, img);
        // stbi_write_png("../.cache/screenshot.png", window_width, window_height, 3, img, 0);
        // delete[] img;
        // return;

        if (call_back)
        {
            call_back();
        }

        swap();
    }
}

SDL_Context::~SDL_Context()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
}