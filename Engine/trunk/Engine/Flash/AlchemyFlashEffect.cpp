#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashEffect.h"
#include "AlchemyFlashContext3D.h"

#include "../Core/AlchemyUtils.h"

using namespace alchemy;

CFlashEffect::CFlashEffect(void) :
m_ppPass(ALCHEMY_NULL)//,
//m_pCurrentPass(ALCHEMY_NULL),
//m_uTextureFlag(0)
{
}

CFlashEffect::~CFlashEffect(void)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPass);
}

void CFlashEffect::Destroy()
{
	if(m_ppPass)
	{
		UINT uPassInfoCount = GetPassInfoCount();
		CFlashProgram3D* pProgram;
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

/*bool CFlashEffect::SetMatrix(IEffect::EFFECTTYPE Type, UINT uIndex, const MATRIX4X4& Matrix)
{
	if(m_pCurrentPass)
		return static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SetShaderMatrixArray(Type, uIndex, Matrix.fValue, 4, 4, 4, 16, 1);;

	return false;
}

bool CFlashEffect::SetParameter(IEffect::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfData, INT nSize)
{
	if(m_pCurrentPass)
		return static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SetShaderParameters(Type, uIndex, pfData, nSize);

	return false;
}

bool CFlashEffect::SetTexture(IEffect::EFFECTTYPE Type, UINT uIndex, CGraphicsResource::HANDLE Texture)
{
	if(m_pCurrentPass)
	{
		ALCHEMY_SET_BIT(m_uTextureFlag, uIndex);

		return static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SetTexture(Texture, uIndex);
	}

	return false;
}

bool CFlashEffect::SetMatrixArray(IEffect::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount)
{
	if(m_pCurrentPass)
		return static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SetShaderMatrixArray(Type, uIndex, pfValues, uRow, uColumn, uRowOffset, uOffset, uCount);

	return false;
}

bool CFlashEffect::SetMatrixPointerArray(IEffect::EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount)
{
	if(m_pCurrentPass)
		return static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SetShaderMatrixPointerArray(Type, uIndex, ppfValues, uRow, uColumn, uRowOffset, uCount);

	return false;
}

bool CFlashEffect::Begin()
{
	return true;
}

bool CFlashEffect::BeginPass(UINT uIndex)
{
	if(uIndex < CEffect::m_uPassCount)
	{
		CFlashProgram3D* pProgram = m_ppPass[uIndex];

		if(pProgram)
		{
			m_pCurrentPass = pProgram;

			m_uTextureFlag = 0;

			return static_cast<CRenderSystem&>( GetGraphicsResourcePool() ).SetShader( pProgram->GetResourceHandle() );
		}
	}

	return false;
}

bool CFlashEffect::EndPass()
{
	//static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).CommitProgramTextureChange(0);

	return true;
}

bool CFlashEffect::End()
{
	return true;
}

bool CFlashEffect::SetPass(CFlashProgram3D* pPass, UINT uIndex)
{
	if(uIndex >= CEffect::m_uPassCount)
	{
		ALCHEMY_DEBUG_WARNING("Error index.");

		return false;
	}

	if(m_ppPass[uIndex])
		m_ppPass[uIndex]->Release();

	if(pPass)
		pPass->AddRef();

	m_ppPass[uIndex] = pPass;

	return true;
}

bool CFlashEffect::CommitChange()
{
	//static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).CommitProgramTextureChange(m_uTextureFlag);

	//m_uTextureFlag = 0;

	return true;
}*/

bool CFlashEffect::GetShader(UINT uPass, UINT uRenderTargetIndex, CShader** ppOutputShader, CShader** ppOutputAdditionalShader)
{
	if(ppOutputAdditionalShader)
		*ppOutputAdditionalShader = ALCHEMY_NULL;

	if(ppOutputShader)
	{
		if(m_ppPass && CEffect::m_pPassInfos && m_puRenderTargetIndexCount && m_puRenderTargetIndexOffset && uPass < m_uPassCount && uRenderTargetIndex < m_puRenderTargetIndexCount[uPass])
		{
			*ppOutputShader =  m_ppPass[m_puRenderTargetIndexOffset[uPass] + uRenderTargetIndex];

			return true;
		}

		return false;
	}

	return true;
}

#ifdef AS3_H
AS3TYPE CFlashEffect::Init()
{
	return AS3_Array("IntType", CEffect::m_uPassCount);
}
#endif

bool CFlashEffect::_Create()
{
	if(!CEffect::m_uPassCount)
	{
		ALCHEMY_DEBUG_WARNING("CFlashEffect::_Create: No pass.");

		return false;
	}

	CFlashContext3D& Context3D = static_cast<CFlashContext3D&>( GetGraphicsResourcePool() );

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPass);

	UINT uPassInfoCount = GetPassInfoCount();

	ALCHEMY_DEBUG_NEW(m_ppPass, CFlashProgram3D*[uPassInfoCount]);
	CFlashProgram3D* pProgram;

	for(UINT i = 0; i < uPassInfoCount; ++ i)
	{
		if( !( pProgram = m_ppPass[i] = static_cast<CFlashProgram3D*>( Context3D.CreateShader(
			IEffect::PROGRAM, 
			Context3D.GetCompiler(), 
			static_cast<const CHAR*>(CEffect::m_pCodes[i]          ),
			static_cast<const CHAR*>(CEffect::m_pAdditionalCodes[i]) ) ) ) )
		{
			ALCHEMY_DEBUG_WARNING("CFlashEffect::_Create: Program has been created fail.");

			return false;
		}

		_AddRef(*pProgram);

		//pProgram->AddRef();
	}

	Context3D.CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType(), GetResourceHandle() );

	return true;
}