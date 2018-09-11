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
}

void CShader::Destroy()
{
	if(m_pCode)
	{
		m_pCode->Release();
		
		m_pCode = ALCHEMY_NULL;
	}
	
	if(m_pAdditionalCode)
	{
		m_pAdditionalCode->Release();
		
		m_pCode = ALCHEMY_NULL;
	}
}