#pragma once

#include "AlchemyEffectInfoBlendMethodInterface.h"

namespace alchemy
{
	class CAlphaBlendMethod :
		public EFFECTINFOBLENDMETHOD
	{
		typedef enum
		{
			CONSTANT_ONE = 0,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;
	public:
		CAlphaBlendMethod(void);
		~CAlphaBlendMethod(void);

		const CEffectCode::ATTRIBUTE   * GetAttributes(UINT& uAttributeCount);
		const CEffectCode::CONSTANT    * GetConstants (UINT& uConstantCount );
		const CEffectCode::VARIABLETYPE* GetTemporary (UINT& uTemporaryCount);

		bool OutputColorBlendHandler(
			UINT uOutputColor, 
			UINT uSourceColor,
			UINT uDestinationColor,
			const UINT* puAttributes, 
			const UINT* puConstants,
			const UINT* puTemporary,
			CEffectCode::IInstructionSet& ShaderDefiner);

	private:
		static const CEffectCode::CONSTANT sm_CONSTANTS[TOTAL_CONSTANTS];
	};
}