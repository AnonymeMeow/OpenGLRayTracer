#include "texture.hpp"
#include "shader.hpp"

TextureBase::TextureBase(GLenum set_type):
    type(set_type)
{
    glGenTextures(1, &id);
}

void TextureBase::bind() const
{
    glBindTexture(type, id);
}

void TextureBase::unbind() const
{
    glBindTexture(type, 0);
}

TextureBase::~TextureBase()
{
    glDeleteTextures(1, &id);
}

Texture::Texture():
    TextureBase(GL_TEXTURE_2D)
{}

void Texture::allocate(GLsizei width, GLsizei height, GLenum format) const
{
    bind();
    glTextureStorage2D(id, 1, format, width, height);
    unbind();
}

TextureArray::TextureArray():
    TextureBase(GL_TEXTURE_2D_ARRAY)
{}

void TextureArray::allocate(GLsizei width, GLsizei height, GLsizei depth, GLenum format) const
{
    bind();
    glTextureStorage3D(id, 1, format, width, height, depth);
    unbind();
}

template <>
void Program::set(const GLchar* name, const TextureBase& texture)
{
    if (boundTextures.contains(name))
    {
        boundTextures[name].first = &texture;
        return;
    }
    int index = boundTextures.size() + 1;
    set(name, index);
    boundTextures[name] = {&texture, index};
}