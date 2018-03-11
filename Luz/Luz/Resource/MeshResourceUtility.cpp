#include "stdafx.h"
#include "MeshResourceUtility.h"

namespace Resource
{
    Vertex0::Vertex0()
    {
    
    }

    Vertex0::Vertex0(float(&position)[3], float(&normal)[3], float(&uv)[2])
    {
        memcpy_s(Position, 12, position, 12);
        memcpy_s(Normal, 12, normal, 12);
        memcpy_s(UV, 8, uv, 8);
    }

    bool operator==(const Vertex0& lhs, const Vertex0& rhs)
    {
        return
            lhs.Position[0] == rhs.Position[0] && lhs.Position[1] == rhs.Position[1] && lhs.Position[2] == rhs.Position[2] &&
            lhs.Normal[0] == rhs.Normal[0] && lhs.Normal[1] == rhs.Normal[1] && lhs.Normal[2] == rhs.Normal[2] &&
            lhs.UV[0] == rhs.UV[0] && lhs.UV[1] == rhs.UV[1];
    }

    bool operator!=(const Vertex0& lhs, const Vertex0& rhs)
    {
        return !(lhs == rhs);
    }

    Vertex1::Vertex1()
    {

    }

    Vertex1::Vertex1(float(&position)[3], float(&normal)[3], float(&uv)[2])
    {
        memcpy_s(Position, 12, position, 12);
        memcpy_s(Normal, 12, normal, 12);
        memcpy_s(UV, 8, uv, 8);
    }

    bool operator==(const Vertex1& lhs, const Vertex1& rhs)
    {
        return
            lhs.Position[0] == rhs.Position[0] && lhs.Position[1] == rhs.Position[1] && lhs.Position[2] == rhs.Position[2] &&
            lhs.Normal[0] == rhs.Normal[0] && lhs.Normal[1] == rhs.Normal[1] && lhs.Normal[2] == rhs.Normal[2] &&
            lhs.Tangent[0] == rhs.Tangent[0] && lhs.Tangent[1] == rhs.Tangent[1] && lhs.Tangent[2] == rhs.Tangent[2] &&
            lhs.UV[0] == rhs.UV[0] && lhs.UV[1] == rhs.UV[1];
    }

    bool operator!=(const Vertex1& lhs, const Vertex1& rhs)
    {
        return !(lhs == rhs);
    }

    Vertex2::Vertex2()
    {

    }

    Vertex2::Vertex2(float(&position)[3], float(&normal)[3], float(&uv)[2])
    {
        memcpy_s(Position, 12, position, 12);
        memcpy_s(Normal, 12, normal, 12);
        memcpy_s(UV, 8, uv, 8);
    }

    bool operator==(const Vertex2& lhs, const Vertex2& rhs)
    {
        return
            lhs.BlendIndices[0] == rhs.BlendIndices[0] && lhs.BlendIndices[1] == rhs.BlendIndices[1] && lhs.BlendIndices[2] == rhs.BlendIndices[2] && lhs.BlendIndices[3] == rhs.BlendIndices[3] &&
            lhs.BlendWeights[0] == rhs.BlendWeights[0] && lhs.BlendWeights[1] == rhs.BlendWeights[1] && lhs.BlendWeights[2] == rhs.BlendWeights[2] && lhs.BlendWeights[3] == rhs.BlendWeights[3] &&
            lhs.Position[0] == rhs.Position[0] && lhs.Position[1] == rhs.Position[1] && lhs.Position[2] == rhs.Position[2] &&
            lhs.Normal[0] == rhs.Normal[0] && lhs.Normal[1] == rhs.Normal[1] && lhs.Normal[2] == rhs.Normal[2] &&
            lhs.Tangent[0] == rhs.Tangent[0] && lhs.Tangent[1] == rhs.Tangent[1] && lhs.Tangent[2] == rhs.Tangent[2] &&
            lhs.UV[0] == rhs.UV[0] && lhs.UV[1] == rhs.UV[1];
    }

    bool operator!=(const Vertex2& lhs, const Vertex2& rhs)
    {
        return !(lhs == rhs);
    }

    JointBlend::JointBlend()
    {
    
    }
}

