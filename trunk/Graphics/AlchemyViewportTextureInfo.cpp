#include "AlchemyViewportTextureInfo.h"

using namespace alchemy;

CViewportTextureInfo::CViewportTextureInfo(CRenderSystem& RenderSystem) :
m_RenderSystem(RenderSystem),
m_nShift(0)
{

}

CViewportTextureInfo::~CViewportTextureInfo()
{

}

bool CViewportTextureInfo::GetSize(PUINT puWidth, PUINT puHeight, PUINT puDepth)
{
	m_RenderSystem.GetViewport(puWidth, puHeight);

	if(m_nShift > 0)
	{
		if(puWidth)
			*puWidth <<= m_nShift;

		if(puHeight)
			*puHeight <<= m_nShift;
	}
	else
	{
		if(puWidth)
			*puWidth >>= - m_nShift;

		if(puHeight)
			*puHeight >>= - m_nShift;
	}

	if(puDepth)
		*puDepth = 1;

	return true;
}