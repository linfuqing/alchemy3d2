#pragma once

#include "AlchemyEffectInfoBlendMethodInterface.h"

namespace alchemy
{
	class CEffectInfoModulation :
		public CEffectInfoBlendMethodInterface
	{
	public:
		CEffectInfoModulation(void);
		~CEffectInfoModulation(void);

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
	};
}