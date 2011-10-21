#include "AlchemyEffect.h"

using namespace alchemy;

CEffect::CEffect(void)
{
}

CEffect::~CEffect(void)
{
}

void CEffect::Destroy()
{
	ALCHEMY_DEBUG_DELETE(m_pCode);
}
