#pragma once

#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL_opengl.h>

#include "../console/logger.hpp"

template <typename T>
struct GetGLTypeEnum;

#define pair(type, val) \
template<> \
struct GetGLTypeEnum<type> \
{ inline static const GLenum value = val; };

pair(GLbyte, GL_BYTE)
pair(GLubyte, GL_UNSIGNED_BYTE)
pair(GLshort, GL_SHORT)
pair(GLushort, GL_UNSIGNED_SHORT)
pair(GLint, GL_INT)
pair(GLuint, GL_UNSIGNED_INT)
pair(GLfloat, GL_FLOAT)
pair(GLdouble, GL_DOUBLE)

#undef pair

inline Logger openglLogger{"OpenGL"};