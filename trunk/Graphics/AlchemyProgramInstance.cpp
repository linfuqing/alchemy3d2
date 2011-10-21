#include "AlchemyProgramInstance.h"
#include "AlchemyEffectInfoInstance.h"

using namespace alchemy;

CProgramInstance::CProgramInstance(CSceneManager::RENDERENTRYTYPE EffectType)
:m_pSceneManager(ALCHEMY_NULL),
m_EffectType(EffectType)
{

}

CProgramInstance::~CProgramInstance(void)
{
	/*UINT uLen = m_EffectInfos.GetLength();
	CPassInstance** ppPassInstance = ALCHEMY_NULL;
	for (UINT i=0; i<uLen; ++i)
	{
		ppPassInstance = m_EffectInfos.Get(i);
		ALCHEMY_DEBUG_DELETE( *ppPassInstance );
	}*/
}

bool CProgramInstance::Create(/*CEffectCode::CPassDefiner& PassDefiner*/)
{
	//PassDefiner.EndDefine();

	m_pSceneManager = CSceneManager::GetInstance();

	return true;
}

void CProgramInstance::AddEffectInfoInstance(CEffectInfoInstance& EffectInfoInstance)
{
	UINT nIndex = 0;
	if(false == m_EffectInfos.GetIndexOf< BASICEQUAL<CEffectInfoInstance*> >(nIndex,&EffectInfoInstance) )
	{
		m_EffectInfos.Add( &EffectInfoInstance );
		EffectInfoInstance.OnAddToProgram();
	}
}

void CProgramInstance::UpdatePassLogic(CSceneObject* pSceneObject)
{
	UINT uLen = m_EffectInfos.GetLength();
	for (UINT i=0; i<uLen; ++i)
		m_EffectInfos[i]->OnPassLogic( pSceneObject );
}

void CProgramInstance::SubmitRenderEntry()
{
	if( __IsValuable() )
		m_pSceneManager->UnlockRenderEntry(m_EffectType);
}

bool CProgramInstance::OnPreRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag)
{
	return true;
}

bool CProgramInstance::OnRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag)
{

	return true;
}

bool CProgramInstance::OnEndRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag)
{

	return true;
}

CGraphicsResource::HANDLE CProgramInstance::GetRenderTarget() const
{
	return 0;
}

void CProgramInstance::OnAddedToSceneObject()
{

}