#pragma once

#include "AlchemyEffectCode.h"
#include "AlchemyRenderMethodInterface.h"

namespace alchemy
{
	class CEffectInfoBlendMethodInterface
	{
	public:

		CEffectInfoBlendMethodInterface(void) {}
		virtual ~CEffectInfoBlendMethodInterface(void) {}

		virtual const CEffectCode::ATTRIBUTE   * GetAttributes(UINT& uAttributeCount) = 0;
		virtual const CEffectCode::CONSTANT    * GetConstants (UINT& uConstantCount ) = 0;
		virtual const CEffectCode::VARIABLETYPE* GetTemporary (UINT& uTemporaryCount) = 0;

		virtual bool OutputColorBlendHandler(
			UINT uOutputColor, 
			UINT uSourceColor,
			UINT uDestinationColor,
			const UINT* puAttributes, 
			const UINT* puConstant,
			const UINT* puTemporary,
			CEffectCode::CInstructionSetInterface& ShaderDefiner) = 0;
	};
}