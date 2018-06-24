#pragma once
#ifndef MESHRESOURCEUTILITY_H
#define MESHRESOURCEUTILITY_H

namespace Resource
{
    template<typename T, size_t N>
    struct TArray
    {
        T Data[N];

        TArray(T(&src)[N]);
        TArray();
    };

    template<typename T, size_t N>
    TArray<T, N>::TArray(T(&src)[N])
    {
        memcpy_s(Data, sizeof(T) * N, src, sizeof(T) * N);
    }

    template<typename T, size_t N>
    TArray<T, N>::TArray()
    {
        memset(Data, 0, sizeof(T) * N);
    }

    template<typename T, size_t N>
    struct TArrayHash
    {
        size_t operator()(const TArray<T, N>& o) const
        {
            size_t hash = 0;
            for (size_t i = 0; i < N; ++i)
            {
                hash = hash * 31 + std::hash<T>{}(o.Data[i]);
            }

            return hash;
        }
    };

    template<typename T, size_t N>
    struct TArrayEqual
    {
        bool operator()(const TArray<T, N>& lhs, const TArray<T, N>& rhs) const
        {
            for (size_t i = 0; i < N; ++i)
            {
                if (lhs.Data[i] != rhs.Data[i]) return false;
            }

            return true;
        }
    };

    struct Vertex0
    {
        float Position[3];
        float Normal[3];
        float UV[2];

        Vertex0();
        Vertex0(float(&position)[3], float(&normal)[3], float(&uv)[2]);
    };

    bool operator==(const Vertex0& lhs, const Vertex0& rhs);
    bool operator!=(const Vertex0& lhs, const Vertex0& rhs);

    struct Vertex1
    {
        float Position[3];
        float Normal[3];
        float Tangent[3];
        float UV[2];

        Vertex1();
        Vertex1(float(&position)[3], float(&normal)[3], float(&uv)[2]);
    };

    bool operator==(const Vertex1& lhs, const Vertex1& rhs);
    bool operator!=(const Vertex1& lhs, const Vertex1& rhs);

    struct Vertex2
    {
        u32 BlendIndices[4];
        float BlendWeights[4];
        float Position[3];
        float Normal[3];
        float Tangent[3];
        float UV[2];

        Vertex2();
        Vertex2(float(&position)[3], float(&normal)[3], float(&uv)[2]);
    };

    bool operator==(const Vertex2& lhs, const Vertex2& rhs);
    bool operator!=(const Vertex2& lhs, const Vertex2& rhs);

    template<class Vertex>
    struct Triangle
    {
        Vertex Vertices[3];
        Triangle()
        {

        }

        Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
        {
            Vertices[0] = v0;
            Vertices[1] = v1;
            Vertices[2] = v2;
        }
    };

    struct JointBlend
    {
        i32 Index;
        float Weight;

        JointBlend();
    };

    template<typename VertexType, typename IndexType>
    struct Mesh
    {
        std::string Name;
        std::vector<VertexType> Vertices;
        std::vector<IndexType> Indices;
    };
};

#endif