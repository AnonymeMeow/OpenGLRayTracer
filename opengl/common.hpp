#pragma once

#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL_opengl.h>

#include "../console/logger.hpp"

template <typename T>
struct _get_gl_type_enum;

template <GLenum E>
struct _get_gl_enum_type;

#define pair(t, v) \
template<> \
struct _get_gl_type_enum<t> \
{ inline static constexpr GLenum value = v; }; \
template<> \
struct _get_gl_enum_type<v> \
{ using type = t; };

pair(GLbyte, GL_BYTE)
pair(GLubyte, GL_UNSIGNED_BYTE)
pair(GLshort, GL_SHORT)
pair(GLushort, GL_UNSIGNED_SHORT)
pair(GLint, GL_INT)
pair(GLuint, GL_UNSIGNED_INT)
pair(GLfloat, GL_FLOAT)
pair(GLdouble, GL_DOUBLE)

#undef pair

template <typename T>
inline constexpr GLenum gl_type_enum_v = _get_gl_type_enum<T>::value;

template <GLenum E>
using gl_enum_type_t = typename _get_gl_enum_type<E>::type;

template <typename T>
concept is_gl_type = requires {
    gl_type_enum_v<T>;
};

template <typename T, typename... U>
concept is_one_of = (std::is_same_v<T, U> || ...);

inline Logger openglLogger{"OpenGL"};