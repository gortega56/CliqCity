#pragma once
#ifndef FBXRESOURCE_H
#define FBXRESOURCE_H

namespace Resource
{
    class LUZ_API Fbx
    {
    public:
        static std::shared_ptr<const Fbx> Load(const std::wstring& filename);
    };
}

#endif // !FBXRESOURCE_H
