#include "AlchemyFlasccStdafx.h"
#include "AlchemyFlasccEffect.h"
#include "AlchemyFlasccContext3D.h"

using namespace alchemy;

CFlasccEffect::CFlasccEffect(void) : 
m_ppPass(ALCHEMY_NULL)
{
}

CFlasccEffect::~CFlasccEffect(void)
{
}

void CFlasccEffect::Destroy()
{
	if(m_ppPass)
	{
		UINT uPassInfoCount = GetPassInfoCount();
		CFlasccProgram3D* pProgram;
		for(UINT i = 0; i < uPassInfoCount; ++ i)
		{
			pProgram = m_ppPass[i];
			if(pProgram)
				_Release(*pProgram);
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPass);
	}

	CEffect::Destroy();
}

bool CFlasccEffect::GetShader(UINT uPass, UINT uRenderTargetIndex, CShader** ppOutputShader, CShader** ppOutputAdditionalShader)
{
	if(ppOutputAdditionalShader)
		*ppOutputAdditionalShader = ALCHEMY_NULL;

	if(ppOutputShader)
	{
		if(m_ppPass && m_pPassInfos && m_puRenderTargetIndexCount && m_puRenderTargetIndexOffset && uPass < m_uPassCount && uRenderTargetIndex < m_puRenderTargetIndexCount[uPass])
		{
			*ppOutputShader =  m_ppPass[m_puRenderTargetIndexOffset[uPass] + uRenderTargetIndex];

			return true;
		}

		return false;
	}

	return true;
}

bool CFlasccEffect::_Create()
{
	if(!CEffect::m_uPassCount)
	{
		ALCHEMY_DEBUG_WARNING("CFlasccEffect::_Create: No pass.");

		return false;
	}

	CFlasccContext3D& Context3D = static_cast<CFlasccContext3D&>( GetGraphicsResourcePool() );

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPass);

	UINT uPassInfoCount = GetPassInfoCount();

	ALCHEMY_DEBUG_NEW(m_ppPass, CFlasccProgram3D*[uPassInfoCount]);
	CFlasccProgram3D* pProgram;

	for(UINT i = 0; i < uPassInfoCount; ++ i)
	{
		if( !( pProgram = m_ppPass[i] = static_cast<CFlasccProgram3D*>( Context3D.CreateShader(
			IEffect::PROGRAM, 
			Context3D.GetCompiler(), 
			static_cast<const CHAR*>(CEffect::m_pCodes[i]          ),
			static_cast<const CHAR*>(CEffect::m_pAdditionalCodes[i]) ) ) ) )
		{
			ALCHEMY_DEBUG_WARNING("CFlasccEffect::_Create: Program3D has been created fail.");

			return false;
		}

		_AddRef(*pProgram);
	}

	return true;
}