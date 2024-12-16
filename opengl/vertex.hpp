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
    size_t indiceCount = 0, verticeCount = 0;
    GLenum indexType = 0;
    const GLenum drawMode;
    void bindVBO() const;
    void unbindVBO() const;
    void bindEBO() const;
    void unbindEBO() const;
public:
    VertexInput(GLenum);
    template <typename T>
    void setVertice(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW)
    {
        verticeCount = data.size();

        bind();
        bindVBO();
        glBufferData(GL_ARRAY_BUFFER, verticeCount * sizeof(T), data.data(), usage);
        unbind();
        unbindVBO();
    }
    template <typename T>
    void setIndice(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW)
    {
        indiceCount = data.size();
        indexType = GetGLTypeEnum<T>::value;

        bind();
        bindEBO();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiceCount * sizeof(T), data.data(), usage);
        unbind();
        unbindEBO();
    }
    template <int Index = 0, typename T, typename Current, typename... Remaining>
    void loadMemoryModel(Current (T::*current), Remaining (T::*...remaining)) const
    {
        if constexpr(Index == 0)
        {
            bind();
            bindVBO();
            bindEBO();
        }
        size_t offset = (size_t)&(((T*)NULL)->*current);
        size_t stride = (size_t)((T*)NULL + 1);
        GLenum type = GetGLTypeEnum<typename GetArraySize<Current>::Type>::value;
        size_t size = GetArraySize<Current>::Size;
        glVertexAttribPointer(Index, size, type, GL_FALSE, stride, (const void*)offset);
        glEnableVertexAttribArray(Index);
        if constexpr(sizeof...(Remaining))
        {
            loadMemoryModel<Index + 1>(remaining...);
        }
        if constexpr(Index == 0)
        {
            unbind();
            unbindVBO();
            unbindEBO();
        }
    }
    void draw() const;
    void bind() const;
    void unbind() const;
    ~VertexInput();
};