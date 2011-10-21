#pragma once

#include "AlchemyEffectInfoBlendMethodInterface.h"

namespace alchemy
{
	class CEffectInfoAddition :
		public CEffectInfoBlendMethodInterface
	{
	public:
		CEffectInfoAddition(void);
		~CEffectInfoAddition(void);

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