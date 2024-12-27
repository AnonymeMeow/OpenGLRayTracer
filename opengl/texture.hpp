#pragma once

#include "common.hpp"

class Texture
{
    GLuint id;
public:
    Texture();
    void bind() const;
    void unbind() const;

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
    void allocate(GLsizei, GLsizei, GLenum) const;

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexSubImage2D.xhtml
    template <is_gl_type T>
        requires (!std::is_same_v<T, GLdouble>)
    void buffer(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, const T* data) const
    {
        glTextureSubImage2D(id, 0, x, y, width, height, format, gl_type_enum_v<T>, data);
    }

    ~Texture();
};