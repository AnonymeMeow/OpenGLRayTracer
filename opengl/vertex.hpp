#pragma once

#include "common.hpp"

#include <cstddef>
#include <vector>

class VertexInput
{
    template <typename T>
    struct GetArraySize
    {
        using Type = T;
        inline static const size_t Size = 1;
    };

    template <typename T, size_t N>
    struct GetArraySize<T[N]>
    {
        using Type = T;
        inline static const size_t Size = N;
    };

    GLuint VAO, VBO, EBO;
    size_t indexCount = 0, vertexCount = 0;
    GLenum indexType = 0;
    const GLenum drawMode;
    void bindVBO() const;
    void unbindVBO() const;
    void bindEBO() const;
    void unbindEBO() const;
public:
    VertexInput(GLenum);
    template <typename T>
    void setVertices(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW)
    {
        vertexCount = data.size();

        bind();
        bindVBO();
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(T), data.data(), usage);
        unbind();
        unbindVBO();
    }
    template <typename T>
    void setIndices(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW)
    {
        indexCount = data.size();
        indexType = GetGLTypeEnum<T>::value;

        bind();
        bindEBO();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(T), data.data(), usage);
        unbind();
        unbindEBO();
    }
    template <typename T, typename... Members>
    void loadMemoryModel(Members (T::* const... members)) const
    {
        bind();
        bindVBO();
        bindEBO();
        GLuint index = 0;
        (
            (
                glVertexAttribPointer(
                    index,
                    GetArraySize<Members>::Size,
                    GetGLTypeEnum<typename GetArraySize<Members>::Type>::value,
                    GL_FALSE,
                    sizeof(T),
                    static_cast<const void*>(&(((T*)nullptr)->*members))
                ),
                glEnableVertexAttribArray(index++)
            ), ...
        );
        unbind();
        unbindVBO();
        unbindEBO();
    }
    void draw() const;
    void bind() const;
    void unbind() const;
    ~VertexInput();
};