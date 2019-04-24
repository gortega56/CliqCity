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


	template<unsigned int Dimension>
	static void CreateRange(float* pMin, float* pMax, const float* pPositions, const unsigned int nPositions)
	{
		float min[Dimension], max[Dimension];

		for (unsigned int j = 0; j < Dimension; ++j)
		{
			min[j] = (std::numeric_limits<float>::max)();
			max[j] = (std::numeric_limits<float>::min)();
		}

		for (unsigned int i = 0; i < nPositions; i += Dimension)
		{
			for (unsigned int j = 0; j < Dimension; ++j)
			{
				min[j] = std::min<float>(min[j], pPositions[i + j]);
				max[j] = std::max<float>(max[j], pPositions[i + j]);
			}
		}

		for (unsigned int j = 0; j < Dimension; ++j)
		{
			pMin[j] = std::min(min[j], pMin[j]);
			pMax[j] = std::max(max[j], pMax[j]);
		}
	}

	void CreateRange2D(float* pMin, float* pMax, const float* pPositions, const unsigned int nPositions)
	{
		CreateRange<2>(pMin, pMax, pPositions, nPositions);
	}

	void CreateRange3D(float* pMin, float* pMax, const float* pPositions, const unsigned int nPositions)
	{
		CreateRange<3>(pMin, pMax, pPositions, nPositions);
	}
}