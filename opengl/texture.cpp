#include "texture.hpp"

#include "shader.hpp"

Texture::Texture()
{
    glGenTextures(1, &id);
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    unbind();
}

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::allocate(GLsizei width, GLsizei height, GLenum format) const
{
    bind();
    glTextureStorage2D(id, 1, format, width, height);
    unbind();
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

template <>
void Program::set(const GLchar* name, const Texture& texture)
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