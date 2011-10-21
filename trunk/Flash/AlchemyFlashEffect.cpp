#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashEffect.h"
#include "AlchemyFlashContext3D.h"

#include "../Core/AlchemyUtils.h"

using namespace alchemy;

CFlashEffect::CFlashEffect(void) :
m_ppPass(ALCHEMY_NULL),
m_pCurrentPass(ALCHEMY_NULL),
m_uTextureFlag(0)
{
}

CFlashEffect::~CFlashEffect(void)
{
}

void CFlashEffect::Destroy()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPass);
}

bool CFlashEffect::SetMatrix(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const MATRIX4X4& Matrix)
{
	if(m_pCurrentPass)
		return static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).PushProgramMatrixArray(Type, uIndex, Matrix.fValue, 4, 4, 4, 16, 1);;

	return false;
}

bool CFlashEffect::SetParameter(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfData, INT nSize)
{
	if(m_pCurrentPass)
		return static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).PushProgramParameter(Type, uIndex, pfData, nSize);

	return false;
}

bool CFlashEffect::SetTexture(CEffectInterface::EFFECTTYPE Type, UINT uIndex, CGraphicsResource::HANDLE Texture)
{
	if(m_pCurrentPass)
	{
		ALCHEMY_SET_BIT(m_uTextureFlag, uIndex);

		return static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SetProgramTexture(Texture, uIndex);
	}

	return false;
}

bool CFlashEffect::SetMatrixArray(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount)
{
	if(m_pCurrentPass)
		return static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).PushProgramMatrixArray(Type, uIndex, pfValues, uRow, uColumn, uRowOffset, uOffset, uCount);

	return false;
}

bool CFlashEffect::SetMatrixPointerArray(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount)
{
	if(m_pCurrentPass)
		return static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).PushProgramMatrixPointerArray(Type, uIndex, ppfValues, uRow, uColumn, uRowOffset, uCount);

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
	static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).CommitProgramTextureChange(0);

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
	static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).CommitProgramTextureChange(m_uTextureFlag);

	//m_uTextureFlag = 0;

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
		ALCHEMY_DEBUG_WARNING("No pass.");

		return false;
	}

	CFlashContext3D& Context3D = static_cast<CFlashContext3D&>( GetGraphicsResourcePool() );

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPass);

	ALCHEMY_DEBUG_NEW(m_ppPass, CFlashProgram3D*[CEffect::m_uPassCount]);

	if(CEffect::m_pEffectCode)
	{
		STRING VertexShaderCode, FragmentShaderCode;
		const CEffectCode::PASS* pPass;

		for(UINT i = 0; i < CEffect::m_uPassCount; ++ i)
		{
			pPass = CEffect::m_pEffectCode->GetPass(i + 1);

			if(pPass && 
				CEffect::m_pEffectCode->BuildShaderFunctionCode(VertexShaderCode  , pPass->uVertexShader  ) == CEffectInterface::VERTEX_SHADER && 
				CEffect::m_pEffectCode->BuildShaderFunctionCode(FragmentShaderCode, pPass->uFragmentShader) == CEffectInterface::FRAGMENT_SHADER)
			{
				if( !( m_ppPass[i] = static_cast<CFlashProgram3D*>( Context3D.CreateShader(
					CEffectInterface::PROGRAM, 
					CEffect::m_pEffectCode->GetCompiler(), 
					static_cast<const CHAR*>(VertexShaderCode),
					static_cast<const CHAR*>(FragmentShaderCode),
					CEffect::m_ppCompilationErrors) ) ) )
				{
					ALCHEMY_DEBUG_WARNING("CFlashEffect::_Create: Program has been created fail.");

					return false;
				}
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CFlashEffect::_Create: Error pass.");

				return false;
			}
		}
	}
	else
	{
		for(UINT i = 0; i < CEffect::m_uPassCount; ++ i)
			m_ppPass[i] = ALCHEMY_NULL;
	}

	Context3D.CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType() );

	return true;
}