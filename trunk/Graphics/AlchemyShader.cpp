#include "AlchemyShader.h"
#include "AlchemyRenderSystem.h"
#include "../Core/AlchemyUtils.h"

using namespace alchemy;

CShader::CShader(void) :
m_pCode(ALCHEMY_NULL),
m_pAdditionalCode(ALCHEMY_NULL)
{
}

CShader::~CShader(void)
{
	Destroy();
}

void CShader::Destroy()
{
}