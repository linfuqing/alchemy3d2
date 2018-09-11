#include "AlchemyD3D9Effect.h"
#include "AlchemyD3D9Device.h"

using namespace alchemy;

CD3D9Effect::CD3D9Effect(void) : 
m_pPasses(ALCHEMY_NULL)
{
}

CD3D9Effect::~CD3D9Effect(void)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pPasses);
}

void CD3D9Effect::Destroy()
{
	if(m_pPasses)
	{
		UINT uPassInfoCount = GetPassInfoCount();
		LPPASS pPass;
		for(UINT i = 0; i < uPassInfoCount; ++ i)
		{
			pPass = &m_pPasses[i];
			if(pPass->pVertexShader)
				_Release(*pPass->pVertexShader);

			if(pPass->pPixelShader)
				_Release(*pPass->pPixelShader);
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_pPasses);
	}

	CEffect::Destroy();
}

bool CD3D9Effect::GetShader(alchemy::UINT uPass, alchemy::UINT uRenderTargetIndex, CShader** ppOutputShader, CShader** ppOutputAdditionalShader)
{
	if(ppOutputShader)
	{
		if(m_pPasses && m_pPassInfos && m_puRenderTargetIndexCount && m_puRenderTargetIndexOffset && uPass < m_uPassCount && uRenderTargetIndex < m_puRenderTargetIndexCount[uPass])
		{
			LPPASS pPass = &m_pPasses[m_puRenderTargetIndexOffset[uPass] + uRenderTargetIndex];
			*ppOutputShader =  pPass->pVertexShader;
			*ppOutputAdditionalShader =  pPass->pPixelShader;

			return true;
		}

		return false;
	}

	return true;
}

bool CD3D9Effect::_Create()
{
	if(!CEffect::m_uPassCount)
	{
		ALCHEMY_DEBUG_WARNING("CD3D9Effect::_Create: No pass.");

		return false;
	}

	CD3D9Device& Device = static_cast<CD3D9Device&>( GetGraphicsResourcePool() );

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pPasses);

	alchemy::UINT uPassInfoCount = GetPassInfoCount();

	ALCHEMY_DEBUG_NEW(m_pPasses, PASS[uPassInfoCount]);
	LPPASS pPass;

	for(alchemy::UINT i = 0; i < uPassInfoCount; ++ i)
	{
		pPass = &m_pPasses[i];
		pPass->pVertexShader = ALCHEMY_NULL;
		pPass->pPixelShader = ALCHEMY_NULL;

		if( !(pPass->pVertexShader = static_cast<CD3D9Shader*>( Device.CreateShader(
			IEffect::VERTEX_SHADER, 
			Device.GetCompiler(), 
			static_cast<const CHAR*>(CEffect::m_pCodes[i]          ),
			ALCHEMY_NULL) ) ) )
		{
			ALCHEMY_DEBUG_WARNING("CD3D9Effect::_Create: Vertex shader has been created fail.");

			return false;
		}

		_AddRef(*pPass->pVertexShader);

		if( !(pPass->pPixelShader = static_cast<CD3D9Shader*>( Device.CreateShader(
			IEffect::FRAGMENT_SHADER, 
			Device.GetCompiler(), 
			static_cast<const CHAR*>(CEffect::m_pAdditionalCodes[i] ),
			ALCHEMY_NULL) ) ) )
		{
			ALCHEMY_DEBUG_WARNING("CD3D9Effect::_Create: Pixel shader has been created fail.");

			return false;
		}

		_AddRef(*pPass->pPixelShader);
	}

	return true;
}