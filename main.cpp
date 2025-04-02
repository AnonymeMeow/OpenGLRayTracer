#include "model/scene.hpp"
#include "opengl/shader.hpp"
#include "view/sdl.hpp"

#include <SDL2/SDL.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

const int WindowWidth = 1000;
const int WindowHeight = 1000;

int main()
{
    Logger logger{"Main"};

    auto scene = Scene("../assets/scene.json");
    SDL_Context window(scene.window_size[0], scene.window_size[1], scene.window_name, {-15, 35, -15, -0.4, 0.75, 1, 1, 0.2, 0.003, 0.02, 0.05, 0.2});

    // stbi_set_flip_vertically_on_load(true);
    stbi_flip_vertically_on_write(true);

    Texture altas{};
    scene.gen_altas(altas);

    auto cubes = scene.build_cube_array<>();

    // Program prog("../shaders/vertex.glsl", "../shaders/geometry.glsl", "../shaders/fragment.glsl", GL_POINTS);

    // prog.set_input(cubes);

    Program prog("../shaders/raytrace/vertex.glsl", "../shaders/raytrace/fragment.glsl", GL_TRIANGLES);

    prog.set_input<>();

    Texture ori_size{}, rotation{}, uv{}, material{};

    TextureCube<> tex_cube(cubes);
    tex_cube.buffer_to_texture(ori_size, rotation, uv, material);

    prog.set("cube.origin_size", ori_size);
    prog.set("cube.rotation", rotation);
    prog.set("cube.uv", uv);
    prog.set("cube.material", material);

    prog.set("altas", altas);
    prog.set("count", (int)cubes.size());

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
                    unsigned char* img = new unsigned char[WindowWidth * WindowHeight * 3];
                    glReadPixels(0, 0, WindowWidth, WindowHeight, GL_RGB, GL_UNSIGNED_BYTE, img);
                    stbi_write_png("../.cache/screenshot.png", WindowWidth, WindowHeight, 3, img, 0);
                    delete[] img;
                }
                else if (event.key.keysym.sym == SDLK_r)
                {
                    window.camera.reset_fov();
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

        float modifier = key_states[SDL_SCANCODE_LCTRL]? window.camera.ctrl_sensitivity_modifier: 1;

        if (mouse_motion.have_motion)
        {
            if (mouse_motion.left && !mouse_motion.right)
            {
                window.camera.turn_head(-mouse_motion.xrel, -mouse_motion.yrel, window.camera.mouse_rotation_sensitivity * modifier);
            }
            else if (mouse_motion.right && !mouse_motion.left)
            {
                window.camera.move_relative(mouse_motion.xrel, mouse_motion.yrel, 0, window.camera.mouse_move_sensitivity * modifier);
            }
            mouse_motion.have_motion = false;
        }

        if (mouse_motion.have_wheel)
        {
            window.camera.zoom(mouse_motion.wheel, window.camera.mouse_zoom_sensitivity * modifier);
            mouse_motion.have_wheel = false;
        }

        window.camera.move_relative_yaw(
            key_states[SDL_SCANCODE_A] - key_states[SDL_SCANCODE_D],
            key_states[SDL_SCANCODE_SPACE] - key_states[SDL_SCANCODE_LSHIFT],
            key_states[SDL_SCANCODE_W] - key_states[SDL_SCANCODE_S],
            window.camera.keyboard_sensitivity * modifier
        );

        glClearColor(0.5f, 0.5f, 0.5f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        prog.set("camera", window.camera);
        prog.draw();

        // unsigned char* img = new unsigned char[WindowWidth * WindowHeight * 3];
        // glReadPixels(0, 0, WindowWidth, WindowHeight, GL_RGB, GL_UNSIGNED_BYTE, img);
        // stbi_write_png("../.cache/screenshot.png", WindowWidth, WindowHeight, 3, img, 0);
        // delete[] img;
        // return 0;

        window.swap();
    }

    return 0;
}