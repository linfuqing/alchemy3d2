#pragma once

#include "AlchemyTexture.h"

#include "../Math/AlchemyMatrix4x4.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IEffect)
	{
	public:
		typedef enum
		{
			VERTEX_SHADER   = 0x1,
			FRAGMENT_SHADER = 0x2,
			PROGRAM         = 0x3
		}EFFECTTYPE;
		
		typedef enum
		{
			TEXTURE2D,
			TEXTURE3D,
			TEXTURE_CUBE,
			
			TOTAL_SAMPLER_TYPE
		}SAMPLERTYPE;
		
		typedef enum
		{
			NONE = 0,
			POINT, 
			LINEAR,
			
			TOTAL_SAMPLER_FILTER
		}SAMPLERFILTER;
		
		typedef enum 
		{
			WRAP = 0,
			MIRROR,
			CLAMP,
			
			TOTAL_SAMPLER_ADDRESS_MODE
		}SAMPLERADDRESSMODE;
		
		typedef struct
		{
			SAMPLERTYPE        Type;
			SAMPLERFILTER      MinFilter;
			SAMPLERFILTER      MagFilter;
			SAMPLERFILTER      MipFilter;
			SAMPLERADDRESSMODE AddressU;
			SAMPLERADDRESSMODE AddressV;
			SAMPLERADDRESSMODE AddressW;
		}SAMPLERDECLARATION, * LPSAMPLERDECLARATION;

		//virtual bool SetMatrix(EFFECTTYPE Type, UINT uIndex, const MATRIX4X4& Matrix) = 0;
		virtual bool SetShaderParameters(EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uCount) = 0;

		virtual bool SetShaderMatrixArray(EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount) = 0;
		virtual bool SetShaderMatrixPointerArray(EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValue, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount) = 0;
		
		virtual bool SetShaderConstants(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE ConstantBuffer, UINT uIndex) = 0;

		virtual bool SetTexture(EFFECTTYPE Type, CGraphicsResource::HANDLE Texture, UINT uIndex, const SAMPLERDECLARATION* pSamplerDeclaration) = 0;
	};

	ALCHEMY_INLINE bool SetShaderMatrix(IEffect& Effect, IEffect::EFFECTTYPE Type, const MATRIX4X4& Matrix, UINT uIndex)
	{
		return Effect.SetShaderMatrixArray(Type, uIndex, Matrix.fValue, 4, 4, 4, 16, 1);
	}
}