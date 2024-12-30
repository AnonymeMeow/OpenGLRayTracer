#pragma once

#include "../model/cube.hpp"
#include "texture.hpp"
#include "vertex.hpp"

template <typename... Ts>
struct _are_all_the_same
{
    inline static constexpr bool value = sizeof...(Ts) <= 1;
};

template <typename T, typename... Us>
struct _are_all_the_same<T, T, Us...>
{
    inline static constexpr bool value = _are_all_the_same<T, Us...>::value;
};

template <typename... Ts>
concept are_all_the_same = _are_all_the_same<Ts...>::value;

template <typename T>
concept gl_uniform_type = is_one_of<T, GLint, GLuint, GLfloat, GLdouble>;

class Program
{
    const GLuint id;
    std::map<const std::string, std::pair<const Texture*, GLuint>> boundTextures{};
    VertexInput input;
    void link() const;
public:
    Program(const fs::path&, const fs::path&, GLenum);
    Program(const fs::path&, const fs::path&, const fs::path&, GLenum);
    void activate() const;
    void deactivate() const;
    template <gl_uniform_type... Args>
    void set(const GLchar*, Args...)
    requires are_all_the_same<Args...> && (sizeof...(Args) <= 4);
    template <typename T>
    void set(const GLchar*, const T&)
    requires (!gl_uniform_type<T>);
    template <gl_floating_point T = GLfloat>
    void set_input()
    {
        struct Vertex
        {
            T coord[2];
        };
        std::vector<Vertex> vertices{
            {1., 1.},
            {-1., 1.},
            {-1., -1.},
            {1., -1.}
        };
        input.loadMemoryModel<Vertex>(&Vertex::coord);
        input.setVertices(vertices);
        input.setIndices(std::vector<GLubyte>{0, 1, 2, 2, 3, 0});
    }
    template <gl_floating_point P = GLfloat, gl_floating_point T = GLfloat>
    void set_input(const CubeArray<P, T>& cubes)
    {
        input.loadMemoryModel<Cube<P, T>>(
            &Cube<P, T>::origin,
            &Cube<P, T>::size,
            &Cube<P, T>::rotation,
            &Cube<P, T>::east,
            &Cube<P, T>::south,
            &Cube<P, T>::west,
            &Cube<P, T>::north,
            &Cube<P, T>::up,
            &Cube<P, T>::down
        );
        input.setVertices(cubes);
    };
    void draw() const;
    ~Program();
};