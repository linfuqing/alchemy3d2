#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CHardwareSkinEffect :
		public EFFECTINFO
	{
		typedef enum
		{
			TOTAL_SAMPLERS = 0
		}SAMPLERINDEX;

		typedef enum
		{
			TOTAL_VARYING = 0
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_BONE_MATRIX_ARRAY = 0,

			CONSTANT_ONE,

			TOTAL_CONSTANT
		}CONSTANTINDEX;

		typedef enum
		{
			TEMPORARY_LAST_WEIGHT = 0,

			TEMPORARY_VARIABLE,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;
	public:
		CHardwareSkinEffect(void);
		~CHardwareSkinEffect(void);

		UINT                                             GetEffectInfoFlag    (                             );

		const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          );

		const CEffectCode::ELEMENTDECLARATION            * GetAttributes        (UINT& uAttributeCount        );
		const CEffectCode::ELEMENTDECLARATION            * GetVarying           (UINT& uVaryingCount          );

		const CEffectCode::GLOBALVARIABLE              * GetConstants         (UINT& uConstantCount         );

		const CEffectCode::TEMPORARYDECLARATION        * GetTemporary         (UINT& uTemporaryCount        );

		const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* GetAttributeVariables(UINT& uAttributeVariableCount);

		const CEffectCode::CONSTANTVARIABLEDECLARATION * GetConstantVariables (UINT& uConstantVariableCount );

		bool Create(UINT uBonesCount, UINT uVertexInfluence, const CEffectCode::ELEMENTDECLARATION* pSkinElementParameters, UINT uSkinElementParameterCount, const MATRIX4X4* pBoneMatrix = ALCHEMY_NULL);

		bool ShaderDefineHandler(
			UINT uOutputColor, 
			const UINT* puSamplers, 
			const UINT* puAttributes, 
			const UINT* puConstants, 
			const CEffectCode::VARYING* pVarying, 
			const UINT* puTemporary,
			const CEffectCode::SHADERVARIABLE* pAttributeVariables,
			const CEffectCode::SHADERVARIABLE* pConstantVariables,
			CEffectCode::IInstructionSet& VertexShaderDefiner,
			CEffectCode::IInstructionSet& FragmentShaderDefiner,
			CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

		void Destroy();

	private:
		static const CEffectCode::TEMPORARYDECLARATION sm_TEMPORARY[TOTAL_TEMPORARY ];

		CEffectCode::GLOBALVARIABLE                    m_Constants [TOTAL_CONSTANT  ];

		CEffectCode::LPELEMENTDECLARATION                  m_pAttributes;

		CEffectCode::LPATTRIBUTEVARIABLEDECLARATION    m_pAttributeVariables;

		UINT m_uBonesCount;
		UINT m_uVertexInfluence;
		UINT m_uSkinElementParameterCount;

		UINT m_uBlendAttributeCount;
	};
}