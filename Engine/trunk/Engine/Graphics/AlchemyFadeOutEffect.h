#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	/*********************************

	CFadeOutParameterHandler

	**********************************/

	/*class CFadeOutEffect;
	class CFadeOutParameterHandler :
		public CEffectCode::CGlobalInputHandler
	{
	public:
		CFadeOutParameterHandler();
		virtual ~CFadeOutParameterHandler();

		CFadeOutEffect* GetEffect();

		bool Create(const UINT& uModID, const FLOAT& fSystemTime, FLOAT fFadeTime, const CARGBColor& TargetColor);

		virtual bool SetParameters(CEffectInterface& Effect, UINT uPass);

	protected:
		FLOAT			m_fFadeTime;
		FLOAT			m_fCurrentFadeTime;
		CARGBColor		m_fFadeColor;
		CFadeOutEffect*	m_pEffect;
	};*/

	/*********************************

	CFadeOutEffect

	**********************************/
	class CFadeOutEffect :
		public EFFECTINFO
	{
	public:
		typedef enum
		{
			TOTAL_SAMPLERS = 0
		}SAMPLERINDEX;

		typedef enum
		{
			TOTAL_ATTRIBUTES = 0
		}ATTRIBUTEINDEX;

		typedef enum
		{
			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_CUR_TIME,
			TEMPORARY_COLOR,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_DIFFUSE_COLOR = 0,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			CONSTANT_SYSTEM_TIME,

			CONSTANT_TOTAL_TIME,

			CONSTANT_TARGET_COLOR,

			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;

		CFadeOutEffect(const UINT& ModID, const FLOAT& SystemTime, FLOAT TotalTime, const FLOAT4 TargetColor);
		virtual ~CFadeOutEffect();

		virtual UINT                                             GetEffectInfoFlag    (                             );

		virtual const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          );

		virtual const CEffectCode::ELEMENTDECLARATION            * GetAttributes        (UINT& uAttributeCount        );
		virtual const CEffectCode::ELEMENTDECLARATION            * GetVarying           (UINT& uVaryingCount          );

		virtual const CEffectCode::GLOBALVARIABLE              * GetConstants         (UINT& uConstantCount         );

		virtual const CEffectCode::TEMPORARYDECLARATION        * GetTemporary         (UINT& uTemporaryCount        );

		virtual const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* GetAttributeVariables(UINT& uAttributeVariableCount);

		virtual const CEffectCode::CONSTANTVARIABLEDECLARATION * GetConstantVariables (UINT& uConstantVariableCount );

		virtual bool ShaderDefineHandler(
			UINT uOutputColor, 
			const UINT* puSamplers, 
			const UINT* puAttributes, 
			const UINT* puConstants, 
			const CEffectCode::VARYING* pVarying, 
			const UINT* puTemporary,
			const CEffectCode::SHADERVARIABLE* pAttributeVariables,
			const CEffectCode::SHADERVARIABLE* pConstantVariables,
			CEffectCode::IInstructionSet& VertexShaderDefiner,
			CEffectCode::IInstructionSet& FragmentShaderDefiner,
			CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

	private:
		FLOAT4													m_TargetColor;
		FLOAT													m_fTotalTime;
		CEffectCode::CONSTANTVARIABLEDECLARATION				m_ConstantVariables[TOTAL_CONSTANT_VARIABLES ];
		const UINT&												m_ModID;

		static const CEffectCode::TEMPORARYDECLARATION			sm_TEMPORARY[TOTAL_TEMPORARY];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION	sm_ATTRIBUTES[TOTAL_ATTRIBUTE_VARIABLES];
	};
}