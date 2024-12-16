#pragma once

#include "texture.hpp"
#include "vertex.hpp"
#include <map>

class Shader
{
    const GLenum type;
    const GLuint id;
    friend class Program;
public:
    Shader(GLenum, const char*);
    ~Shader();
};

class Program
{
    const GLuint id;
    std::map<const std::string, std::pair<const TextureBase*, GLuint>> boundTextures{};
    void link() const;
public:
    VertexInput input;
    Program(const char*, const char*, GLenum);
    Program(const char*, const char*, const char*, GLenum);
    void activate() const;
    void deactivate() const;
    template <typename... Args>
    void set(const GLchar*, Args...);
    void draw() const;
    ~Program();
};