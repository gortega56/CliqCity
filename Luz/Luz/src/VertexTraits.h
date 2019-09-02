#pragma once
#ifndef VERTEXTRAITS_H
#define VERTEXTRAITS_H

namespace Geometry
{
    template<typename T>
    struct VertexTraits
    {
        template<typename C> static constexpr decltype(std::declval<C>().Position, bool()) HasPosition(char) { return true; }
        template<typename C> static constexpr decltype(std::declval<C>().Normal, bool()) HasNormal(char) { return true; }
        template<typename C> static constexpr decltype(std::declval<C>().UV, bool()) HasUV(char) { return true; }
        template<typename C> static constexpr decltype(std::declval<C>().Tangent, bool()) HasTangent(char) { return true; }
        template<typename C> static constexpr decltype(std::declval<C>().BlendWeights, bool()) HasBlendWeights(char) { return true; }
        template<typename C> static constexpr decltype(std::declval<C>().BlendIndices, bool()) HasBlendIndices(char) { return true; }

        template<typename C> static constexpr bool HasPosition(...) { return false; }
        template<typename C> static constexpr bool HasNormal(...) { return false; }
        template<typename C> static constexpr bool HasUV(...) { return false; }
        template<typename C> static constexpr bool HasTangent(...) { return false; }
        template<typename C> static constexpr bool HasBlendWeights(...) { return false; }
        template<typename C> static constexpr bool HasBlendIndices(...) { return false; }

        static constexpr bool Position = HasPosition<T>(char());
        static constexpr bool Normal = HasNormal<T>(char());
        static constexpr bool UV = HasUV<T>(char());
        static constexpr bool Tangent = HasTangent<T>(char());
        static constexpr bool BlendWeight = HasBlendWeights<T>(char());
        static constexpr bool BlendIndices = HasBlendIndices<T>(char());


        //static const bool Position = false;
        //static const bool Normal = false;
        //static const bool UV = false;
        //static const bool Tangent = false;
        //static const bool BlendWeights = false;
        //static const bool BlendIndices = false;
    };
}


#endif