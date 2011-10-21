#pragma once

#include "AlchemyProgramInstance.h"

namespace alchemy
{
	class CEnvironmentProgram;
	class CPositionEffect;		
	class CEnvironmentalEffect;	
	class CTextureEffect;		
	class CMeasureColorEffect;
	class CEnvironmentDisplayProgram :
		public CProgramInstance
	{
	public:
		CEnvironmentDisplayProgram();
		virtual ~CEnvironmentDisplayProgram();

		bool Create(CEnvironmentProgram& EnvironmentProgram, UINT uSamplerIndex, CEffectCode::CPassDefiner& PassDefiner, CEffectCode::CPassDefiner::BLENDMETHOD blendArgus = CEffectCode::CPassDefiner::SELECTARG1);

		virtual bool OnPreRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag);
		virtual bool OnRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag);
		virtual void OnAddedToSceneObject();

	protected:
		UINT				m_uSamplerIndex;
		LPTEXTURE			m_pEnvironmentTexture;
		CMatrix4x4			m_matCameraWorld;

		CPositionEffect*		m_pPosEffect;
		CEnvironmentalEffect*	m_pEnvironmentEffect;
		CTextureEffect*			m_pTextureEffect;
	};
}