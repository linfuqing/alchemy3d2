#pragma once

#include "AlchemyEffectInfoBlendMethodInterface.h"

namespace alchemy
{
	class CEffectInfoSub :
		public EFFECTINFOBLENDMETHOD
	{
	public:
		typedef enum
		{
			SOURCE_ALPHA,
			DESTINATION_ALPHA,
			BLEND_ALPHA
		}BLENDFLAG;
		CEffectInfoSub(BLENDFLAG BlendFlag = SOURCE_ALPHA);
		~CEffectInfoSub(void);

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
		BLENDFLAG			m_BlendFlag;
	};
}