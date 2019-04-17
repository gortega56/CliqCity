#include "stdafx.h"
#include "Mesh.h"

namespace Graphics
{
	template<class IndexType>
	static void ReverseWindingOrder(IndexType* pIndices, const unsigned int nIndices)
	{
		//LUZASSERT(nIndices % 3U == 0);

		auto swap = [&](const int i, const int j)
		{
			IndexType temp = pIndices[i];
			pIndices[i] = pIndices[j];
			pIndices[j] = temp;
		};

		for (unsigned int i = 0; i < nIndices; i += 3)
		{
			swap(i + 1, i + 2);
		}
	}

	void ReverseWindingOrder(unsigned char* pIndices, const unsigned int nIndices)
	{
		ReverseWindingOrder<unsigned char>(pIndices, nIndices);
	}

	void ReverseWindingOrder(unsigned short* pIndices, const unsigned int nIndices)
	{
		ReverseWindingOrder<unsigned short>(pIndices, nIndices);
	}

	void ReverseWindingOrder(unsigned int* pIndices, const unsigned int nIndices)
	{
		ReverseWindingOrder<unsigned int>(pIndices, nIndices);
	}
}