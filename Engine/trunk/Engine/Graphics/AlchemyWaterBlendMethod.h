#pragma once

#include "AlchemyEffectInfoBlendMethodInterface.h"
#include "AlchemyARGBColor.h"

namespace alchemy
{
	class CWaterBlendMethod :
		public EFFECTINFOBLENDMETHOD
	{
		typedef enum
		{
			ATTRIBUTE_POSITION = 0,
			ATTRIBUTE_FACING,

			TOTAL_ATTRIBUTES
		}ATTRIBUTEINDEX;

		typedef enum
		{
			TOTAL_VARYING = 0
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_ONE = 0,
			CONSTANT_FRESNEL_BIAS,
			CONSTANT_FRESNEL_POW,
			CONSTANT_INVERSE_BIAS,
			CONSTANT_DEEP,
			CONSTANT_COLOR,
			CONSTANT_DEEP_COLOR,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_COLOR = 0,
			TEMPORARY_REFLECTION,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;
	public:
		CWaterBlendMethod(
			const FLOAT* pfBias = ALCHEMY_NULL, 
			const FLOAT* pfPow = ALCHEMY_NULL, 
			const FLOAT* pfDeep = ALCHEMY_NULL,
			const FLOAT4* pColor = ALCHEMY_NULL, 
			const FLOAT4* pDeepColor = ALCHEMY_NULL);
		~CWaterBlendMethod(void);

		const CEffectCode::ATTRIBUTE   * GetAttributes(UINT& uAttributeCount);
		const CEffectCode::CONSTANT    * GetConstants (UINT& uConstantCount );
		const CEffectCode::VARIABLETYPE* GetTemporary (UINT& uTemporaryCount);

		bool OutputColorBlendHandler(
			UINT uOutputColor, 
			UINT uSourceColor,
			UINT uDestinationColor,
			const UINT* puAttributes, 
			const UINT* puConstant,
			const UINT* puTemporary,
			CEffectCode::IInstructionSet& ShaderDefiner);

	private:
		static const CEffectCode::VARIABLETYPE sm_TEMPORARY [TOTAL_TEMPORARY ];
		static const CEffectCode::ATTRIBUTE    sm_ATTRIBUTES[TOTAL_ATTRIBUTES];

		CEffectCode::CONSTANT                   m_Constants [TOTAL_CONSTANTS ];

		FLOAT                                   m_fBias;
		FLOAT                                   m_fPow;
		FLOAT                                   m_fInverseBias;

		FLOAT                                   m_fDeep;
		CARGBColor                              m_Color;
		CARGBColor                              m_DeepColor;
	};
}