#include "shader.hpp"

#include <fstream>

Shader::Shader(GLenum shader_type, const char* source_path):
    type(shader_type),
    id(glCreateShader(shader_type))
{
    std::ifstream src_file(source_path);
    if (!src_file)
    {
        openglLogger.error("Cannot open shader source file: {}.", source_path);
        exit(-1);
    }
    std::string line;
    std::string src_str;
    while (std::getline(src_file, line))
    {
        src_str += line + '\n';
    }
    src_file.close();
    const GLchar* src = src_str.c_str();
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &status);
        char* log_info = new char[status + 1];
        glGetShaderInfoLog(id, status, NULL, log_info);
        openglLogger.error("Shader compilation error: at file {}\n{}", source_path, log_info);
        delete[] log_info;
        exit(-1);
    }
}

Shader::~Shader()
{
    glDeleteShader(id);
}

Program::Program(const char* vertex_source, const char* fragment_source, GLenum drawMode):
    id(glCreateProgram()),
    input(VertexInput(drawMode))
{
    Shader vertex = Shader(GL_VERTEX_SHADER, vertex_source);
    Shader fragment = Shader(GL_FRAGMENT_SHADER, fragment_source);

    glAttachShader(id, vertex.id);
    glAttachShader(id, fragment.id);

    link();

    glDetachShader(id, vertex.id);
    glDetachShader(id, fragment.id);
}

Program::Program(const char* vertex_source, const char* geometry_source, const char* fragment_source, GLenum drawMode):
    id(glCreateProgram()),
    input(VertexInput(drawMode))
{
    Shader vertex = Shader(GL_VERTEX_SHADER, vertex_source);
    Shader geometry = Shader(GL_GEOMETRY_SHADER, geometry_source);
    Shader fragment = Shader(GL_FRAGMENT_SHADER, fragment_source);

    glAttachShader(id, vertex.id);
    glAttachShader(id, geometry.id);
    glAttachShader(id, fragment.id);

    link();

    glDetachShader(id, vertex.id);
    glDetachShader(id, geometry.id);
    glDetachShader(id, fragment.id);
}

void Program::link() const
{
    glLinkProgram(id);

    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &status);
        char* log_info = new char[status + 1];
        glGetProgramInfoLog(id, status, NULL, log_info);
        openglLogger.error("Program linkage error:\n{}", log_info);
        delete[] log_info;
        exit(-1);
    }
}

void Program::activate() const
{
    glUseProgram(id);
}

void Program::deactivate() const
{
    glUseProgram(0);
}

void Program::draw() const
{
    for (auto const& [_, texture]: boundTextures)
    {
        glActiveTexture(GL_TEXTURE0 + texture.second);
        texture.first->bind();
    }
    activate();
    input.draw();
    deactivate();
    for (auto const& [_, texture]: boundTextures)
    {
        glActiveTexture(GL_TEXTURE0 + texture.second);
        texture.first->unbind();
    }
    glActiveTexture(GL_TEXTURE0);
}

Program::~Program()
{
    glDeleteProgram(id);
}

#define PARENS ()

#define GLf GLfloat
#define GLd GLdouble
#define GLi GLint
#define GLui GLuint

#define CONCAT_X(a, b) a ## b

#define GLTYPE(type) CONCAT_X(GL, type)

#define GEN_ARGS(...) GEN_ARGS_I(__VA_ARGS__)
#define GEN_ARGS_I(n, ...) __VA_OPT__(GEN_ARGS_X PARENS (__VA_ARGS__),) v ## n
#define GEN_ARGS_X() GEN_ARGS_I

#define GEN_PARAM(type, ...) GEN_PARAM_I(type, __VA_ARGS__)
#define GEN_PARAM_I(type, n, ...) __VA_OPT__(GEN_PARAM_X PARENS (type, __VA_ARGS__),) GLTYPE(type) v ## n
#define GEN_PARAM_X() GEN_PARAM_I

#define UNIFORM_SETTER(type, n, ...) \
template <> \
void Program::set(const GLchar* name, GEN_PARAM(type, n, __VA_ARGS__)) \
{ \
    activate(); \
    GLint location = glGetUniformLocation(id, name); \
    if (location == -1) \
    { \
        openglLogger.error("Cannot find uniform variable {} in shader.", name); \
        exit(-1); \
    } \
    glUniform ## n ## type (location, GEN_ARGS(n, __VA_ARGS__)); \
    deactivate(); \
}

#define GEN_UNIFORM_SETTER_TYPE(type, ...) GEN_UNIFORM_SETTER_TYPE_I(type, __VA_ARGS__)
#define GEN_UNIFORM_SETTER_TYPE_I(type, n, ...) \
UNIFORM_SETTER(type, n, __VA_ARGS__) \
__VA_OPT__(GEN_UNIFORM_SETTER_TYPE_X PARENS (type, __VA_ARGS__))
#define GEN_UNIFORM_SETTER_TYPE_X() GEN_UNIFORM_SETTER_TYPE_I

#define GEN_UNIFORM_SETTER(...) GEN_UNIFORM_SETTER_I(__VA_ARGS__)
#define GEN_UNIFORM_SETTER_I(type, ...) \
GEN_UNIFORM_SETTER_TYPE(type, 4, 3, 2, 1) \
__VA_OPT__(GEN_UNIFORM_SETTER_X PARENS (__VA_ARGS__))
#define GEN_UNIFORM_SETTER_X() GEN_UNIFORM_SETTER_I

#define EXPAND(x) EXPAND1(EXPAND1(EXPAND1(EXPAND1(x))))
#define EXPAND1(x) EXPAND2(EXPAND2(EXPAND2(EXPAND2(x))))
#define EXPAND2(x) EXPAND3(EXPAND3(EXPAND3(EXPAND3(x))))
#define EXPAND3(x) EXPAND4(EXPAND4(EXPAND4(EXPAND4(x))))
#define EXPAND4(x) x

EXPAND(GEN_UNIFORM_SETTER(f, d, i, ui))