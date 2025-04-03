#include "model/scene.hpp"
#include "opengl/shader.hpp"
#include "view/sdl.hpp"

int main()
{
    Logger logger{"Main"};

    auto scene = Scene("../assets/scene.json");
    SDL_Context window(scene.window_size[0], scene.window_size[1], scene.window_name, {-15, 35, -15, -0.4, 0.75, 1, 1, 0.2, 0.003, 0.02, 0.05, 0.2});

    // stbi_set_flip_vertically_on_load(true);

    Texture altas{};
    scene.gen_altas(altas);

    auto cubes = scene.build_cube_array<>();

    Program prog("../shaders/vertex.glsl", "../shaders/geometry.glsl", "../shaders/fragment.glsl", GL_POINTS);

    prog.set_input(cubes);

    // Program prog("../shaders/raytrace/vertex.glsl", "../shaders/raytrace/fragment.glsl", GL_TRIANGLES);

    // prog.set_input<>();

    // Texture ori_size{}, rotation{}, uv{}, material{};

    // TextureCube<> tex_cube(cubes);
    // tex_cube.buffer_to_texture(ori_size, rotation, uv, material);

    // prog.set("cube.origin_size", ori_size);
    // prog.set("cube.rotation", rotation);
    // prog.set("cube.uv", uv);
    // prog.set("cube.material", material);

    prog.set("altas", altas);
    // prog.set("count", (int)cubes.size());

    window.render_loop(prog, nullptr);

    return 0;
}