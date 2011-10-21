#pragma once

#include "AlchemyEffect.h"
#include "AlchemyCamera.h"
#include "AlchemyMaterial.h"
//#include "AlchemyLight.h"
#include "AlchemyVertexDeclaration.h"

namespace alchemy
{
	class CLight;
	class CRenderMethodInterface
	{
	public:
		typedef enum
		{
			POSITION_OFFSET = 0,
			UV_OFFSET,

			ELAPSED_TIME,
			TOTAL_TIME,

			AMBIENT_LIGHT,

			TOTAL_PARAMETERS
		}PARAMETER;

		typedef enum
		{
			BONE_MATRIX_ARRAY = 0,

			TOTAL_ARRAY_PARAMETERS
		}ARRAYPARAMETER;

		CRenderMethodInterface(void) {};
		virtual ~CRenderMethodInterface(void) {};

		virtual bool SetParameter(PARAMETER Index, const FLOAT* pData) = 0;
		virtual bool SetArray(ARRAYPARAMETER Index, const void* pData, UINT uCount, bool bIsPointer) = 0;

		//virtual bool SetWorldMatrix(const CMatrix4x4& Matrix) = 0;
		//virtual bool SetCamera(const CCamera& Camera) = 0;
		//virtual bool SetMaterial(const CMaterial& Material) = 0;
		//virtual bool SetLight(const CLight& Light, UINT uIndex) = 0;

		virtual bool Activate(UINT uPass) = 0;
		//virtual bool CommitTransform() = 0;

		virtual bool SetParameters(const CMatrix4x4& WorldMatrix, const CCamera& Camera, const CMaterial& Material) = 0;

		virtual CEffect& GetEffect() = 0;

		virtual const CEffectCode::ELEMENTPARAMETER* GetVertexElementParameters(UINT uIndex, PUINT puOutCount = ALCHEMY_NULL)const = 0;
	};
}