#pragma once

#include "common.hpp"

class TextureBase
{
protected:
    GLuint id;
    GLenum type;
    TextureBase(GLenum);
    ~TextureBase();
public:
    void bind() const;
    void unbind() const;
};

class Texture: public TextureBase
{
public:
    Texture();
    void allocate(GLsizei, GLsizei, GLenum) const;
    template <typename T>
    void buffer(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, const T* data) const
    {
        glTextureSubImage2D(id, 0, x, y, width, height, format, GetGLTypeEnum<T>::value, data);
    }
};

class TextureArray: public TextureBase
{
public:
    TextureArray();
    void allocate(GLsizei, GLsizei, GLsizei, GLenum) const;
    template <typename T>
    void buffer(GLint layer, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, const T* data) const
    {
        glTextureSubImage3D(id, 0, x, y, layer, width, height, 1, format, GetGLTypeEnum<T>::value, data);
    }
};