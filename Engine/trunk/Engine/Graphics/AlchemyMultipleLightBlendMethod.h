#pragma once

#include "AlchemyEffectInfoBlendMethodInterface.h"

namespace alchemy
{
	class CMultipleLightBlendMethod :
		public EFFECTINFOBLENDMETHOD
	{
		typedef enum
		{
			TOTAL_ATTRIBUTES = 0
		}ATTRIBUTEINDEX;

		typedef enum
		{
			CONSTANT_MATERIAL_AMBIENT = 0,
			CONSTANT_MATERIAL_DIFFUSE,
			CONSTANT_MATERIAL_SPECULAR,

			CONSTANT_MATERIAL_EMISSIVE,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_DIFFUSE = 0,
			TEMPORARY_SPECLUAR,
			
			TOTAL_TEMPORARY
		}TEMPORARYINDEX;
	public:
		CMultipleLightBlendMethod(bool bIsWriteOverDiffuse = true, bool bIsWriteOverSpecular = true);
		~CMultipleLightBlendMethod(void);

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
		static const CEffectCode::CONSTANT     sm_CONSTANTS[TOTAL_CONSTANTS];
		static const CEffectCode::VARIABLETYPE sm_TEMPORARY[TOTAL_TEMPORARY];
		
		bool m_bIsWriteOverDiffuse;
		bool m_bIsWriteOverSpecular;
	};
}