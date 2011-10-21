#pragma once

#include "AlchemyTexture.h"

#include "../Math/AlchemyMatrix4x4.h"

namespace alchemy
{
	class CEffectInterface
	{
	public:
		typedef enum
		{
			VERTEX_SHADER   = 0x1,
			FRAGMENT_SHADER = 0x2,
			PROGRAM         = 0x3
		}EFFECTTYPE;

		CEffectInterface(void) {}
		virtual ~CEffectInterface(void) {}

		virtual bool SetMatrix(EFFECTTYPE Type, UINT uIndex, const MATRIX4X4& Matrix) = 0;
		virtual bool SetParameter(EFFECTTYPE Type, UINT uIndex, const FLOAT* pfData, INT nSize) = 0;
		virtual bool SetTexture(EFFECTTYPE Type, UINT uIndex, CGraphicsResource::HANDLE Texture) = 0;

		virtual bool SetMatrixArray(EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount) = 0;
		virtual bool SetMatrixPointerArray(EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount) = 0;
	};
}