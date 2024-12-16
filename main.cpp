#include "opengl/shader.hpp"

#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

const int WindowWidth = 1000;
const int WindowHeight = 1000;

struct VertexIn
{
    float vertexCoord[3];
    float textureCoord[2];
};

std::vector<VertexIn> vertices = {
    {-1.f,  1.f, -1.f, 0.f, 0.f},
    { 1.f,  1.f, -1.f, 1.f, 0.f},
    { 1.f, -1.f, -1.f, 1.f, 1.f},
    {-1.f, -1.f, -1.f, 0.f, 1.f},

    { 1.f,  1.f, -1.f, 0.f, 0.f},
    { 1.f,  1.f,  1.f, 1.f, 0.f},
    { 1.f, -1.f,  1.f, 1.f, 1.f},
    { 1.f, -1.f, -1.f, 0.f, 1.f},
    
    { 1.f,  1.f,  1.f, 0.f, 0.f},
    {-1.f,  1.f,  1.f, 1.f, 0.f},
    {-1.f, -1.f,  1.f, 1.f, 1.f},
    { 1.f, -1.f,  1.f, 0.f, 1.f},
    
    {-1.f,  1.f,  1.f, 0.f, 0.f},
    {-1.f,  1.f, -1.f, 1.f, 0.f},
    {-1.f, -1.f, -1.f, 1.f, 1.f},
    {-1.f, -1.f,  1.f, 0.f, 1.f},

    {-1.f,  1.f,  1.f, 0.f, 0.f},
    { 1.f,  1.f,  1.f, 1.f, 0.f},
    { 1.f,  1.f, -1.f, 1.f, 1.f},
    {-1.f,  1.f, -1.f, 0.f, 1.f},

    {-1.f, -1.f, -1.f, 0.f, 0.f},
    { 1.f, -1.f, -1.f, 1.f, 0.f},
    { 1.f, -1.f,  1.f, 1.f, 1.f},
    {-1.f, -1.f,  1.f, 0.f, 1.f},
};

std::vector<unsigned int> indices = {
    0, 1, 2,
    0, 2, 3,
    4, 5, 6,
    4, 6, 7,
    8, 9, 10,
    8, 10, 11,
    12, 13, 14,
    12, 14, 15,
    16, 17, 18,
    16, 18, 19,
    20, 21, 22,
    20, 22, 23,
};

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

    Program prog("../shaders/vertex.glsl", "../shaders/geometry.glsl", "../shaders/fragment.glsl", GL_TRIANGLES);

    prog.input.setVertice(vertices);
    prog.input.setIndice(indices);
    prog.input.loadMemoryModel(&VertexIn::vertexCoord, &VertexIn::textureCoord);

    int texWidth, texHeight, nChannels;
    unsigned char* tex = stbi_load("../.cache/cirno_fumo.png", &texWidth, &texHeight, &nChannels, 0);
    if (tex == NULL)
    {
        logger.error("{}", stbi_failure_reason());
        exit(-1);
    }

    TextureArray texture{};
    texture.allocate(texWidth, texHeight, 1, GL_RGBA8);
    texture.buffer(0, 0, 0, texWidth, texHeight, GL_RGBA, tex);

    stbi_image_free(tex);

    prog.set<const TextureBase&>("altas", texture);

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