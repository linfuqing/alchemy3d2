#pragma once

#include "AlchemyEffect.h"

namespace alchemy
{
	class CLight;
	class CCamera;
	class CMaterial;
	ALCHEMY_INTERFACE(IRenderMethod)
	{
		typedef enum
		{
			POSITION_OFFSET = 0,
			UV_OFFSET,

			ELAPSED_TIME,
			TOTAL_TIME,

			AMBIENT_LIGHT,

			WORLD_MATRIX,

			INVERSE_WORLD_MATRIX,

			TOTAL_PARAMETERS
		}PARAMETER;

		typedef enum
		{
			BONE_MATRIX_ARRAY = 0,
			INSTANCE_TRANSFORM_ARRAY, 

			TOTAL_ARRAY_PARAMETERS
		}ARRAYPARAMETER;

		virtual UINT GetTextureCount(UINT uPass, UINT uRenderTargetIndex)const = 0;

		virtual const UINT* GetTextureInfo(UINT uPass, UINT uRenderTargetIndex,  UINT& uCount) = 0;

		virtual bool SetParameter(PARAMETER Index, const FLOAT* pData) = 0;
		virtual bool SetArray(IRenderMethod::ARRAYPARAMETER Index, const void* pData, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount, bool bIsPointer) = 0;

		virtual bool Activate(UINT uPass, UINT uRenderTargetIndex) = 0;

		virtual bool CommitChange(const MATRIX4X4* pWorldMatrix, const CMaterial* pMaterial) = 0;

		//virtual bool SetWorldMatrix(const MATRIX4X4& Matrix) = 0;

		//virtual bool SetMaterial(const CMaterial& Material) = 0;

		//virtual CEffect& GetEffect() = 0;

		//virtual const CEffectCode::ELEMENTDECLARATION* GetVertexElements(UINT uIndex, PUINT puOutCount)const = 0;
	};
}