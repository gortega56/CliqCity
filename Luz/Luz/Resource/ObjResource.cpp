#include "stdafx.h"
#include "ObjResource.h"


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
        memset(Data, static_cast<T>(0), sizeof(T) * N);
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

    static const char* g_positionFormat = "v %f %f %f";
    static const char* g_normalFormat = "vn %f %f %f";
    static const char* g_uvFormat = "vt %f %f";
    static const char* g_faceFormatPTN = "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d";
    static const char* g_faceFormatPN = "f %d//%d %d//%d %d//%d %d//%d";
    static const char* g_faceFormatPT = "f %d/%d %d/%d %d/%d %d/%d";

    static const u32 g_maxChar = 100;
    
    static void ReadFace(const char* line, i32(&dest)[12], bool& isTri);

    static void ReadFormat(const char* line, const char* format, u32(&dest)[9]);
    static void ReadFormat(const char* line, const char* format, float(&dest)[3]);
    static void ReadFormat(const char* line, const char* format, float(&dest)[2]);

    static void ReadFace(const char* line, i32(&dest)[12], bool& isTri)
    {
        i32 idx = 0, verts = 0;
        memset(dest, -1, sizeof(i32) * 12);

        std::stringstream lss = std::stringstream(line);
        std::string ltok, ttok;

        while (std::getline(lss, ltok, ' '))
        {
            if (ltok[0] == 'f') continue;

            i32 attempts = 0;

            std::stringstream tss = std::stringstream(ltok);
            while (std::getline(tss, ttok, '/'))
            {
                sscanf_s(ttok.c_str(), "%d", &dest[idx++]);
                attempts += 1;
            }

            // Handle "%d/%d" format
            if (attempts == 2) idx += 1;

            verts += 1;
        }

        isTri = (verts == 3);
    }

    static void ReadFormat(const char* line, const char* format, u32(&dest)[9])
    {
        sscanf_s(line, format, &dest[0], &dest[1], &dest[2], &dest[3], &dest[4], &dest[5], &dest[6], &dest[7], &dest[8]);
    }

    static void ReadFormat(const char* line, const char* format, float(&dest)[3])
    {
        sscanf_s(line, format, &dest[0], &dest[1], &dest[2]);
    }

    static void ReadFormat(const char* line, const char* format, float(&dest)[2])
    {
        sscanf_s(line, format, &dest[0], &dest[1]);
    }

    std::shared_ptr<const Obj> Obj::Load(const std::wstring& filename)
    {
        std::shared_ptr<Obj> pResource;

        auto filename_str = std::string(filename.begin(), filename.end());
        std::ifstream fileStream(filename_str.c_str());
        if (fileStream.is_open())
        {
            using Position = TArray<float, 3>;
            using Normal = TArray<float, 3>;
            using UV = TArray<float, 2>;
            using Face = TArray<i32, 12>;

            using VertexIndirect = TArray<u32, 3>;
            using VertexIndirectHash = TArrayHash<u32, 3>;
            using VertexIndirectEqual = TArrayEqual<u32, 3>;

            char line[g_maxChar];
            std::vector<Position> positions;
            std::vector<Normal> normals;
            std::vector<UV> uvs;
            std::vector<Face> faces;
            std::unordered_map<VertexIndirect, u32, VertexIndirectHash, VertexIndirectEqual> indirects;

            pResource = std::make_shared<Obj>();

            while (fileStream.good())
            {
                fileStream.getline(line, g_maxChar);

                if (line[0] == 'v' && line[1] == 'n')
                {
                    normals.emplace_back();
                    ReadFormat(line, g_normalFormat, normals.back().Data);
                }
                else if (line[0] == 'v' && line[1] == 't')
                {
                    uvs.emplace_back();
                    ReadFormat(line, g_uvFormat, uvs.back().Data);
                }
                else if (line[0] == 'v')
                {
                    positions.emplace_back();
                    ReadFormat(line, g_positionFormat, positions.back().Data);
                }
                else if (line[0] == 'f')
                {
                    faces.emplace_back();
                    Face* pFace = &faces.back();

                    bool isTri = false;
                    ReadFace(line, pFace->Data, isTri);

                    if (isTri)
                    {
                        for (size_t i = 0; i < 3; ++i)
                        {
                            i32 p = pFace->Data[3 * i + 0] - 1;
                            i32 t = pFace->Data[3 * i + 1];
                            i32 n = pFace->Data[3 * i + 2];
                            
                            // -1 is invalid... don't ruin it.
                            if (t != -1) t -= 1;
                            if (n != -1) n -= 1;

                            VertexIndirect vi;
                            vi.Data[0] = p;
                            vi.Data[1] = t;
                            vi.Data[2] = n;

                            auto iter = indirects.find(vi);
                            if (iter == indirects.end())
                            {
                                // New vertex: Create and store for look up later
                                u32 index = static_cast<u32>(pResource->m_vertices.size());
                                pResource->m_vertices.emplace_back();
                                pResource->m_indices.push_back(index);
                                indirects.insert({ vi, index });

                                Vertex* pVertex = &pResource->m_vertices[index];
                                memcpy_s(pVertex->Position, sizeof(Position), positions[vi.Data[0]].Data, sizeof(Position));
                                if (vi.Data[1] != -1) memcpy_s(pVertex->UV, sizeof(UV), uvs[vi.Data[1]].Data, sizeof(UV));
                                if (vi.Data[2] != -1) memcpy_s(pVertex->Normal, sizeof(Normal), normals[vi.Data[2]].Data, sizeof(Normal));
                            }
                            else
                            {
                                // Existing vertex.. just push the index
                                pResource->m_indices.push_back(iter->second);
                            }
                        }
                    }
                    else
                    {

                    }
                }
            }

            fileStream.close();
        }

        return pResource;
    }

    Obj::Obj()
    {

    }

    Obj::~Obj()
    {

    }
}