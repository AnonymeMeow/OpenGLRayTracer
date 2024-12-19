#pragma once

#include "common.hpp"

class Texture
{
    GLuint id;
public:
    Texture();
    void bind() const;
    void unbind() const;
    void allocate(GLsizei, GLsizei, GLenum) const;
    template <typename T>
    void buffer(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, const T* data) const
    {
        glTextureSubImage2D(id, 0, x, y, width, height, format, GetGLTypeEnum<T>::value, data);
    }
    ~Texture();
};