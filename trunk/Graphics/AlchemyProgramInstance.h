#pragma once

#include "AlchemyGraphicsResource.h"
#include "AlchemySceneManager.h"

namespace alchemy
{
	class CEffectInfoInstance;
	class CSceneObject;
	class CProgramInstance : public CReferenceCounter
	{
	public:
		CProgramInstance(CSceneManager::RENDERENTRYTYPE EffectType);
		virtual ~CProgramInstance(void);

		UINT GetEffectInfoCount()const;

		void AddEffectInfoInstance(CEffectInfoInstance& EffectInfoInstance);

		bool Create(/*CEffectCode::CPassDefiner& PassDefiner*/);

		void UpdatePassLogic(CSceneObject* pSceneObject);
		
		void SubmitRenderEntry();

		virtual CGraphicsResource::HANDLE GetRenderTarget() const;
		
		virtual bool OnPreRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag);
		virtual bool OnRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag);
		virtual bool OnEndRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag);

		virtual void OnAddedToSceneObject();

	protected:
		bool	__IsValuable() const;

	protected:
		CSceneManager::RENDERENTRYTYPE			m_EffectType;
		CSceneManager*						m_pSceneManager;
		CVector<CEffectInfoInstance*>		m_EffectInfos;
	};


	ALCHEMY_INLINE bool	CProgramInstance::__IsValuable() const
	{
		return m_pSceneManager && m_EffectType != CSceneManager::EFFECT_TYPE_COUNT;
	}

	ALCHEMY_INLINE UINT CProgramInstance::GetEffectInfoCount() const
	{
		return m_EffectInfos.GetLength();
	}

}