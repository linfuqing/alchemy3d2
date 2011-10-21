#pragma once

#include "AlchemyProgramInstance.h"
#include "AlchemyEffectCode.h"

namespace alchemy
{
	class CPositionEffect;
	class CSceneObject;
	class CDistanceInfoEffect;
	class CDepthFieldEffect;
	class CDistanceInfoProgram :
		public CProgramInstance
	{
	public:
		CDistanceInfoProgram();
		virtual ~CDistanceInfoProgram();

		bool Create(const CDepthFieldEffect& DepthEffect,CEffectCode::CPassDefiner& PassDefiner);

		void Destroy();

		virtual CGraphicsResource::HANDLE GetRenderTarget() const;

		virtual bool OnPreRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag);

		virtual void OnAddedToSceneObject();

	private:
		FLOAT						m_fFocusDistance;
		CDistanceInfoEffect*		m_pDistanceInfoEffect;
		CPositionEffect*			m_pPositionEffect;
		CGraphicsResource::HANDLE	m_hRenderTarget;
	};
}