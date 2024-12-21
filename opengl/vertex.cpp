#include "vertex.hpp"

VertexInput::VertexInput(GLenum mode):
    drawMode(mode)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void VertexInput::bindVBO() const
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void VertexInput::unbindVBO() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexInput::bindEBO() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

void VertexInput::unbindEBO() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VertexInput::draw() const
{
    bind();
    if (vertexCount)
    {
        if (indexCount)
        {
            glDrawElements(drawMode, indexCount, indexType, 0);
        }
        else
        {
            glDrawArrays(drawMode, 0, vertexCount);
        }
    }
    else
    {
        openglLogger.info("VBO has size 0 when draw call is invoked, skip.");
    }
    unbind();
}

void VertexInput::bind() const
{
    glBindVertexArray(VAO);
}

void VertexInput::unbind() const
{
    glBindVertexArray(0);
}

VertexInput::~VertexInput()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}