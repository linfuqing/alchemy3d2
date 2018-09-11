#include "AlchemyFlasccWindow.h"
#include "AlchemyFlasccGraphicsManager.h"

using namespace alchemy;

CFlasccWindow::CFlasccWindow(FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight) :
m_fViewportX(x), 
m_fViewportY(y), 
m_fViewportWidth(fWidth), 
m_fViewportHeight(fHeight)
{
}

CFlasccWindow::~CFlasccWindow(void)
{
}

bool CFlasccWindow::AttachToScreen(bool bIsAsKeyWindow)
{
	return true;
}

bool CFlasccWindow::AddRunLoop(IWindow::IDisplayLink& DisplayLink)
{
	return static_cast<CFlasccGraphicsManager&>(ALCHEMY_GRAPHICS_MANAGER).AddRunLoop(DisplayLink);
}
