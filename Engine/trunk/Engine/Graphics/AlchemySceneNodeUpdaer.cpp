#include "AlchemySceneNodeUpdaer.h"
#include "AlchemySceneManager.h"

#include "../Core/AlchemyUpdaterInterface.h"

using namespace alchemy;

CSceneNodeUpdater::CSceneNodeUpdater()
:m_pSceneManager(ALCHEMY_NULL)
{

}

CSceneNodeUpdater::~CSceneNodeUpdater()
{
}

bool CSceneNodeUpdater::Create(CSceneManager* pSceneManager)
{
	Reset();

	m_pSceneManager = pSceneManager ? pSceneManager : CSceneManager::GetInstance();

	return true;
}

void CSceneNodeUpdater::AddUpdater( IUpdater& Updater )
{
	UINT uOutput;

	if( !m_Updaers.GetIndexOf(uOutput, &Updater) )
	{
		Updater.AddRef();
		m_Updaers.Add(&Updater);
	}
}

void CSceneNodeUpdater::RemoveUpdaer( IUpdater& Updater )
{
	UINT uOutput;

	if( m_Updaers.GetIndexOf(uOutput, &Updater) )
	{
		Updater.Release();
		m_Updaers.Remove(uOutput, 1);
	}
}

void CSceneNodeUpdater::Reset()
{
	UINT i, len = m_Updaers.GetLength();
	for(i=0; i<len; ++i)
		m_Updaers[i]->Release();

	m_Updaers.Reset();
}

void CSceneNodeUpdater::Update()
{
	CSceneNode::Update();

	FLOAT fElpaseTime = m_pSceneManager->GetElapsedTime();

	UINT i, len = m_Updaers.GetLength();
	for(i=0; i<len; ++i)
		m_Updaers[i]->Update( fElpaseTime );
}