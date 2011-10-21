#pragma once

#include "AlchemyEffectInfoBlendMethodInterface.h"

namespace alchemy
{
	class CPixelFogBlendMethod : 
		public CEffectInfoBlendMethodInterface
	{
		typedef enum
		{
			ATTRIBUTE_POSITION,

			TOTAL_ATTRIBUTES
		}ATTRIBUTEINDEX;

		typedef enum
		{
			TEMPORARY_DISTANCE,
			TEMPORARY_SCENE_COLOR,
			TEMPORARY_FOG_COLOR,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			CONSTANT_LOCAL_CAMERA_POS,
			CONSTANT_ONE,

			CONSTANT_PARAMETER1,
			CONSTANT_PARAMETER2,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

	public:
		typedef enum
		{
			LINEAR,
			EXP,
			EXP2
		}FOG_TYPE;

		CPixelFogBlendMethod(FOG_TYPE FogType);
		virtual ~CPixelFogBlendMethod();

		bool Create(const FLOAT* pStart, const FLOAT* pEnd, const FLOAT* pDensity);

		virtual const CEffectCode::ATTRIBUTE   * GetAttributes(UINT& uAttributeCount);
		virtual const CEffectCode::CONSTANT    * GetConstants (UINT& uConstantCount );
		virtual const CEffectCode::VARIABLETYPE* GetTemporary (UINT& uTemporaryCount);

		virtual bool OutputColorBlendHandler(
			UINT uOutputColor, 
			UINT uSourceColor,
			UINT uDestinationColor,
			const UINT* puAttributes, 
			const UINT* puConstant,
			const UINT* puTemporary,
			CEffectCode::CInstructionSetInterface& ShaderDefiner);

	private:
		FLOAT													m_fStart;
		FLOAT													m_fEnd;
		FLOAT													m_fDistance;

		FLOAT													m_fFogParameter;
		FOG_TYPE												m_FogType;

		CEffectCode::CONSTANT									m_Constans[TOTAL_CONSTANTS];

		static const CEffectCode::ATTRIBUTE						sm_ATTRIBUTEVARIABLES[TOTAL_ATTRIBUTES];
		static const CEffectCode::VARIABLETYPE					sm_TEMPORARYVARIABLES[TOTAL_TEMPORARY];
	};
}