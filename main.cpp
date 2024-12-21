#include "opengl/shader.hpp"

#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

const int WindowWidth = 1000;
const int WindowHeight = 1000;

struct VertexIn
{
    float origin[3];
    float size[3];
    float rotation[3];
    float uv_east[4];
    float uv_south[4];
    float uv_west[4];
    float uv_north[4];
    float uv_up[4];
    float uv_down[4];
};

std::vector<VertexIn> vertices = {{
    -1.f, -1.f, -1.f,
    2.f, 2.f, 2.f,
    0.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 1.f,
    0.f, 0.f, 1.f, 1.f,
    0.f, 0.f, 1.f, 1.f,
    0.f, 0.f, 1.f, 1.f,
    0.f, 0.f, 1.f, 1.f,
    0.f, 0.f, 1.f, 1.f,
}};

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
    glDepthFunc(GL_LEQUAL);

    glViewport(0, 0, WindowWidth, WindowHeight);

    Program prog("../shaders/vertex.glsl", "../shaders/geometry.glsl", "../shaders/fragment.glsl", GL_POINTS);

    prog.input.setVertices(vertices);
    prog.input.loadMemoryModel(
        &VertexIn::origin,
        &VertexIn::size,
        &VertexIn::rotation,
        &VertexIn::uv_east,
        &VertexIn::uv_south,
        &VertexIn::uv_west,
        &VertexIn::uv_north,
        &VertexIn::uv_up,
        &VertexIn::uv_down
    );

    // stbi_set_flip_vertically_on_load(true);
    stbi_flip_vertically_on_write(true);

    int texWidth, texHeight, nChannels;
    unsigned char* tex = stbi_load("../.cache/cirno_fumo.png", &texWidth, &texHeight, &nChannels, 0);
    if (tex == NULL)
    {
        logger.error("{}", stbi_failure_reason());
        exit(-1);
    }

    Texture texture{};
    texture.allocate(texWidth, texWidth, GL_RGBA8);
    texture.buffer(0, 0, texWidth, texWidth, GL_RGBA, tex);

    stbi_image_free(tex);

    prog.set<const Texture&>("altas", texture);

    bool running = true;
    int frame = 0;
    while (running)
    {
        SDL_Event event;
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
                    stbi_write_png("screenshot.png", WindowWidth, WindowHeight, 3, img, 0);
                    delete[] img;
                }
            }
            else if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        glClearColor(0.5f, 0.5f, 0.5f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        prog.set("frame", frame);
        frame++;

        prog.draw();

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);

    return 0;
}