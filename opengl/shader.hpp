#pragma once

#include "texture.hpp"
#include "vertex.hpp"
#include <map>

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
    void link() const;
public:
    VertexInput input;
    Program(const char*, const char*, GLenum);
    Program(const char*, const char*, const char*, GLenum);
    void activate() const;
    void deactivate() const;
    template <gl_uniform_type... Args>
    void set(const GLchar*, Args...)
    requires are_all_the_same<Args...> && (sizeof...(Args) <= 4);
    template <typename T>
    void set(const GLchar*, const T&)
    requires (!gl_uniform_type<T>);
    void draw() const;
    ~Program();
};