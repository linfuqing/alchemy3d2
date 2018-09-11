#pragma once

#include "AlchemyEffectCode.h"
#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CSkyPassDefiner :
		public CEffectCode::CPassDefiner
	{
	public:
		class CSkyEffect : 
			public IEffectInfo
		{
			typedef enum
			{
				TOTAL_SAMPLERS
			}SAMPLERINDEX;

			typedef enum
			{
				TOTAL_ATTRIBUTES
			}ATTRIBUTEINDEX;

			typedef enum
			{
				VARYING_POSITIONT = 0,

				TOTAL_VARYING
			}VARYINGINDEX;

			typedef enum
			{
				CONSTANT_WORLD_VIEW_PROJECTION_MATRIX,

				TOTAL_CONSTANTS
			}CONSTANSINDEX;

			typedef enum
			{
				TEMPORARY_POS = 0,

				TOTAL_TEMPORARY
			}TEMPORARYINDEX;

			typedef enum
			{
				ATTRIBUTE_VARIABLE_POSITION = 0,

				TOTAL_ATTRIBUTE_VARIABLES
			}ATTRIBUTEVARIABLEINDEX;

			typedef enum
			{
				TOTAL_CONSTANT_VARIABLES = 0
			}CONSTANTVARIABLEINDEX;

		public:
			CSkyEffect();
			virtual ~CSkyEffect();

			void SetOffset(const FLOAT& ZOffset);

			UINT                                             GetEffectInfoFlag    (                             );

			const IEffectInfo::SAMPLER                        *GetSamplers          (UINT& uSamplerCount          );

			const CEffectCode::ELEMENTDECLARATION            * GetAttributes        (UINT& uAttributeCount        );
			const CEffectCode::ELEMENTDECLARATION            * GetVarying           (UINT& uVaryingCount          );

			const CEffectCode::GLOBALVARIABLE              * GetConstants         (UINT& uConstantCount         );

			const CEffectCode::TEMPORARYDECLARATION        * GetTemporary         (UINT& uTemporaryCount        );

			const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* GetAttributeVariables(UINT& uAttributeVariableCount);

			const CEffectCode::CONSTANTVARIABLEDECLARATION * GetConstantVariables (UINT& uConstantVariableCount );

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

		private:
			CEffectCode::GLOBALVARIABLE*							m_pConstant;
			UINT													m_uConstantNum;

			static CEffectCode::ELEMENTDECLARATION           sm_VARYING          [TOTAL_VARYING           ];
			static CEffectCode::GLOBALVARIABLE               sm_CONSTANS         [TOTAL_CONSTANTS         ];
			static CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES];
			static CEffectCode::TEMPORARYDECLARATION		   sm_TEMPRORAYS	   [TOTAL_TEMPORARY];
		};

		CSkyPassDefiner( CEffectCode& Code, const FLOAT* pZOffset = ALCHEMY_NULL );
		virtual ~CSkyPassDefiner();

	private:
		CSkyEffect*		m_pSkyEffect;
	};
}