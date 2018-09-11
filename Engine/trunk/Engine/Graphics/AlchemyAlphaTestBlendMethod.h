#pragma once

#include "AlchemyEffectInfoBlendMethodInterface.h"


namespace alchemy
{
	class CAlphaTestBlendMethod :
		public EFFECTINFOBLENDMETHOD
	{
	public:
		CAlphaTestBlendMethod(const FLOAT* fRef = ALCHEMY_NULL);
		virtual ~CAlphaTestBlendMethod();

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
		FLOAT						           m_fAlphaRef;
		CEffectCode::CONSTANT		           m_CONSTANT;
        static const CEffectCode::VARIABLETYPE sm_TEMPORARY;   
	};
}