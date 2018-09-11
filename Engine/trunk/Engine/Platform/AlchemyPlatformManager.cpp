#include "AlchemyPlatformManager.h"

using namespace alchemy;

CPlatformManager::CRenderSystemEvent::CRenderSystemEvent(CRenderSystem& RenderSystem, UINT uType, bool bIsBubbles) :
m_RenderSystem(RenderSystem), 
CEvent(uType, bIsBubbles)
{
}

CPlatformManager::CRenderSystemEvent::~CRenderSystemEvent()
{

}

CPlatformManager::CPlatformManager(void) : 
CSingleton<CPlatformManager>(*this)
{
}

CPlatformManager::~CPlatformManager(void)
{
}

UINT CPlatformManager::CreateWindow(FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight)
{
	IWindow* pWindow = _CreateWindow(x, y, fWidth, fHeight);

	if(pWindow)
		return m_Windows.Add(pWindow) + 1;

	return 0;
}

CPool<IWindow*>::CIterator CPlatformManager::_GetWindowIterator(UINT uIndex)
{
	return m_Windows.GetIterator(uIndex);
}