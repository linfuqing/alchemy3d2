#pragma once

#include "AlchemyEffectInfoBlendMethodInterface.h"

namespace alchemy
{
	class CBidirectionalBlendMethod : 
		public EFFECTINFOBLENDMETHOD
	{
		typedef enum
		{
			TEMPORARY_COLOR = 0,

			TOTAL_TEMPORARY
		}TEMPORARY;

		typedef enum
		{
			CONSTANT_ONE = 0,

			TOTAL_CONSTANT
		}CONSTANTINDEX;
	public:
		typedef enum
		{
			POSITIVE,
			NEGATIVE
		}BLENDMETHOD;

		CBidirectionalBlendMethod(BLENDMETHOD BlendMethod, UINT uChannelIndex = 3);
		virtual ~CBidirectionalBlendMethod();

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
		UINT									m_uChannelIndex;
		BLENDMETHOD							    m_eBlendMethod;
		static const CEffectCode::CONSTANT		sm_CONSTANTS[TOTAL_CONSTANT];
		static const CEffectCode::VARIABLETYPE  sm_TEMPORARY[TOTAL_TEMPORARY];
	};
}