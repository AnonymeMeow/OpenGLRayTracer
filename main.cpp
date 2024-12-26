#include "model/camera.hpp"
#include "model/cube.hpp"
#include "model/model.hpp"
#include "model/pose.hpp"
#include "opengl/shader.hpp"

#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

const int WindowWidth = 1000;
const int WindowHeight = 1000;

void push_cubes(std::vector<Cube<>>& cubes, Model::Bone* bone, const PoseTransform& pose, int texWidth, int texHeight)
{
    PoseTransform poseBone = pose * PoseTransform(bone->rotation, bone->pivot);
    for (auto cube: bone->cubes)
    {
        PoseTransform poseCube = poseBone * PoseTransform(cube->rotation, cube->pivot);
        Quaternion origin(0, cube->origin[0], cube->origin[1], cube->origin[2]);
        origin = poseCube * origin;
        if (poseCube.rotation.w < 0)
        {
            poseCube.rotation = - poseCube.rotation;
        }
        cubes.push_back(Cube<>{
            (float)(origin.x), (float)(origin.y), (float)(origin.z),
            (float)(cube->size[0]), (float)(cube->size[1]), (float)(cube->size[2]),
            (float)(poseCube.rotation.x), (float)(poseCube.rotation.y), (float)(poseCube.rotation.z),
            (float)(cube->uv.east[0] / texWidth), (float)(cube->uv.east[1] / texHeight), (float)(cube->uv.east[2] / texWidth), (float)(cube->uv.east[3] / texHeight),
            (float)(cube->uv.south[0] / texWidth), (float)(cube->uv.south[1] / texHeight), (float)(cube->uv.south[2] / texWidth), (float)(cube->uv.south[3] / texHeight),
            (float)(cube->uv.west[0] / texWidth), (float)(cube->uv.west[1] / texHeight), (float)(cube->uv.west[2] / texWidth), (float)(cube->uv.west[3] / texHeight),
            (float)(cube->uv.north[0] / texWidth), (float)(cube->uv.north[1] / texHeight), (float)(cube->uv.north[2] / texWidth), (float)(cube->uv.north[3] / texHeight),
            (float)(cube->uv.up[0] / texWidth), (float)(cube->uv.up[1] / texHeight), (float)(cube->uv.up[2] / texWidth), (float)(cube->uv.up[3] / texHeight),
            (float)(cube->uv.down[0] / texWidth), (float)(cube->uv.down[1] / texHeight), (float)(cube->uv.down[2] / texWidth), (float)(cube->uv.down[3] / texHeight),
        });
    }
    for (auto child: bone->children)
    {
        push_cubes(cubes, child, poseBone, texWidth, texHeight);
    }
}

int main()
{
    Logger logger{"Main"};

    SDL_Window* window = SDL_CreateWindow(
        "RayTrace",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WindowWidth,
        WindowHeight,
        SDL_WINDOW_OPENGL
    );
    if (window == NULL)
    {
        logger.error("{}", SDL_GetError());
        exit(-1);
    }
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL)
    {
        logger.error("{}", SDL_GetError());
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glViewport(0, 0, WindowWidth, WindowHeight);

    // stbi_set_flip_vertically_on_load(true);
    stbi_flip_vertically_on_write(true);

    int texWidth, texHeight, nChannels;
    unsigned char* tex = stbi_load("../.cache/assets/geckolib/textures/entity/winefox.png", &texWidth, &texHeight, &nChannels, 0);
    if (tex == NULL)
    {
        logger.error("{}", stbi_failure_reason());
        exit(-1);
    }

    Texture texture{};

    texture.bind();
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    texture.unbind();

    texture.allocate(texWidth, texHeight, GL_RGBA8);
    texture.buffer(0, 0, texWidth, texHeight, GL_RGBA, tex);

    stbi_image_free(tex);

    std::vector<Cube<>> cubes;

    // "../.cache/assets/geckolib/models/entity/winefox.json"
    Model model("../.cache/assets/geckolib/models/entity/winefox.json", "../.cache/assets/geckolib/textures/entity/winefox.png");
    PoseTransform id_pose{Quaternion(1, 0, 0, 0), Quaternion(0, 0, 0, 0)};
    for (auto bone: model.bones)
    {
        push_cubes(cubes, bone, id_pose, texWidth, texHeight);
    }

    Program prog("../shaders/vertex.glsl", "../shaders/geometry.glsl", "../shaders/fragment.glsl", GL_POINTS);

    prog.input.setVertices(cubes);
    prog.input.loadMemoryModel(
        &Cube<>::origin,
        &Cube<>::size,
        &Cube<>::rotation,
        &Cube<>::uv_east,
        &Cube<>::uv_south,
        &Cube<>::uv_west,
        &Cube<>::uv_north,
        &Cube<>::uv_up,
        &Cube<>::uv_down
    );

    prog.set("altas", texture);

    Camera camera{-15, 35, -15, -0.4, 0.75, 1, 1, 0.2, 0.003, 0.02, 0.05, 0.2};

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

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);

    return 0;
}