#pragma once

#include "AlchemyEffectInfoBlendMethodInterface.h"

namespace alchemy
{
	class CFresnelBlendMethod :
		public CEffectInfoBlendMethodInterface
	{
		typedef enum
		{
			ATTRIBUTE_FACING = 0,

			TOTAL_ATTRIBUTES
		}ATTRIBUTEINDEX;

		typedef enum
		{
			CONSTANT_ONE = 0,
			CONSTANT_FRESNEL_BIAS,
			CONSTANT_FRESNEL_POW,
			CONSTANT_INVERSE_BIAS,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_REFLECTION = 0,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;
	public:
		CFresnelBlendMethod(void);
		~CFresnelBlendMethod(void);

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
			CEffectCode::CInstructionSetInterface& ShaderDefiner);

	protected:
		FLOAT                                  m_fBias;
		FLOAT                                  m_fPow;

	private:
		static const CEffectCode::ATTRIBUTE    sm_ATTRIBUTES[TOTAL_ATTRIBUTES];
		static const CEffectCode::VARIABLETYPE sm_TEMPORARY [TOTAL_TEMPORARY ];

		CEffectCode::CONSTANT                  m_Constants  [TOTAL_CONSTANTS ];

		FLOAT                                  m_fInverseBias;
	};
}