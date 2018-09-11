#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashContext3D.h"
#include "AlchemyFlashVertexDeclaration.h"
#include "AlchemyFlashVertexBuffer.h"
#include "AlchemyFlashIndexBuffer.h"
#include "AlchemyFlashProgram3D.h"
#include "AlchemyFlashEffect.h"
#include "AlchemyFlashTexture.h"

using namespace alchemy;
CFlashContext3D::CFlashContext3D(ICompiler& Compiler, UINT uTotalRenderDataCount) :
m_uTotalRenderDataCount(uTotalRenderDataCount),
m_uCurrentRenderDataCount(0),
//m_uProgramVertexParameterCount(0),
//m_uProgramFragmentParameterCount(0),
m_uTextureFlag(0),
//m_uCurrentTextureFlag(0),
m_uVertexBufferDirtyFlag(0),
m_uVertexBufferIndexFlag(0),
m_uVertexBufferCacheFlag(0),
m_uRenderStateStackCount(0),
m_uRenderStateFlag(0),
m_pVertexDeclaration(ALCHEMY_NULL),
m_uViewportWidth(1),
m_uViewportHeight(1),
m_Compiler(Compiler)
{
	m_RenderSystemInfo.uRenderTargetCount   = 1;
	m_RenderSystemInfo.uMaxVertexCount      = 65535;
	m_RenderSystemInfo.uMaxVertexIndexCount = 524287;

	UINT uTotalTextureCount = MAXINUM_TEXTURE_COUNT * m_uTotalRenderDataCount;
	ALCHEMY_DEBUG_NEW(m_pRenderDataQueue          , RENDERDATA[                              m_uTotalRenderDataCount                          ]);
	ALCHEMY_DEBUG_NEW(m_pfProgramVertexParameter  , FLOAT[MAXINUM_VERTEX_PARAMETER_COUNT   * m_uTotalRenderDataCount * VERTEX_PARAMETER_SIZE  ]);
	ALCHEMY_DEBUG_NEW(m_pfProgramFragmentParameter, FLOAT[MAXINUM_FRAGMENT_PARAMETER_COUNT * m_uTotalRenderDataCount * FRAGMENT_PARAMETER_SIZE]);
	ALCHEMY_DEBUG_NEW(m_puProgramTexture          , UINT16[                                       uTotalTextureCount                          ]);

	UINT i;
	for(i = 0; i < uTotalTextureCount; ++ i)
		m_puProgramTexture[i] = 0;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::FILL_MODE] = 0;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::SHADE_MODE] = 0;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::DEPTH_WRITEE_ENABLE] = 1;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::DEPTH_FUNC] = IRenderState::LESS;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::ALPHA_FUNC] = IRenderState::LESS;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::SRC_BLEND] = IRenderState::ONE;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::DEST_BLEND] = IRenderState::ZERO;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::ALPHA_REF] = 0;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::ALPHA_FUNC] = IRenderState::LESS;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::CULL_MODE] = IRenderState::CULL_NONE;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_FUNC] = IRenderState::ALWAYS;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_FAIL] = IRenderState::KEEP;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_Z_FAIL] = IRenderState::KEEP;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_PASS] = IRenderState::KEEP;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_REF] = 0;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_READ_MASK] = 0xff;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_WRITE_MASK] = 0xff;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::CCW_STENCIL_FAIL] = IRenderState::KEEP;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::CCW_STENCIL_Z_FAIL] = IRenderState::KEEP;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::CCW_STENCIL_PASS] = IRenderState::KEEP;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[IRenderState::CCW_STENCIL_FUNC] = IRenderState::ALWAYS;

	m_CurrentRenderData.DeviceStateBlock.uDirtyFlag = 0;

	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::FILL_MODE] = 0;

	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::SHADE_MODE] = 0;

	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::DEPTH_WRITEE_ENABLE] = 1;
	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::DEPTH_FUNC] = IRenderState::LESS;

	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::ALPHA_FUNC] = IRenderState::LESS;

	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::SRC_BLEND] = IRenderState::ONE;
	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::DEST_BLEND] = IRenderState::ZERO;

	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::ALPHA_REF] = 0;
	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::ALPHA_FUNC] = IRenderState::LESS;

	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::CULL_MODE] = IRenderState::CULL_NONE;

	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_FUNC] = IRenderState::ALWAYS;
	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_FAIL] = IRenderState::KEEP;
	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_Z_FAIL] = IRenderState::KEEP;
	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_PASS] = IRenderState::KEEP;

	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_REF] = 0;
	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_READ_MASK] = 0xff;
	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::STENCIL_WRITE_MASK] = 0xff;

	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::CCW_STENCIL_FAIL] = IRenderState::KEEP;
	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::CCW_STENCIL_Z_FAIL] = IRenderState::KEEP;
	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::CCW_STENCIL_PASS] = IRenderState::KEEP;
	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::CCW_STENCIL_FUNC] = IRenderState::ALWAYS;

	m_OldRenderData.DeviceStateBlock.uRenderState[IRenderState::COLOR_WRITE_ENABLE] = ALCHEMY_FLAG(IRenderState::ALPHA) | ALCHEMY_FLAG(IRenderState::RED) | ALCHEMY_FLAG(IRenderState::GREEN) | ALCHEMY_FLAG(IRenderState::BLUE);

	m_OldRenderData.DeviceStateBlock.uDirtyFlag = 0;

	for(i = 0; i < MAXINUM_VERTEX_INDEX_COUNT; ++ i)
	{
		m_CurrentRenderData.VertexData[i].VertexBufferData.uVertexBuffer = 0;
		m_OldRenderData    .VertexData[i].VertexBufferData.uVertexBuffer = 0;
	}

	m_CurrentRenderData.uRenderTarget            = 0;
	m_OldRenderData    .uRenderTarget            = 0;

	m_CurrentRenderData.uRenderSurface           = 0;
	m_OldRenderData    .uRenderSurface           = 0;

	m_CurrentRenderData.uIndexBuffer             = 0;
	m_OldRenderData    .uIndexBuffer             = 0;

	m_CurrentRenderData.ProgramData.uProgram     = 0;
	m_OldRenderData    .ProgramData.uProgram     = 0;

	m_CurrentRenderData.ProgramData.uTextureFlag = 0;
	m_OldRenderData    .ProgramData.uTextureFlag = 0;

	__ClearRenderData(m_CurrentRenderData);
	__ClearRenderData(m_OldRenderData    );
}

CFlashContext3D::~CFlashContext3D(void)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pRenderDataQueue);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pfProgramVertexParameter);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pfProgramFragmentParameter);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puProgramTexture);
}

void CFlashContext3D::Destroy()
{
	CRenderSystem::Destroy();

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pRenderDataQueue);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pfProgramVertexParameter);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pfProgramFragmentParameter);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puProgramTexture);

#ifdef AS3_H
	AS3_Release(m_Object);
	AS3_Release(m_CreateResourceMethod);
	AS3_Release(m_DestroyResourceMethod);
	AS3_Release(m_RenderMethod);
#endif
}

bool CFlashContext3D::CheckFormat(FORMATTYPE Type, UINT uFormat)
{
	switch(Type)
	{
	case CRenderSystem::TEXTURE:
		switch(uFormat)
		{
		case CTexture::R8G8B8A8:
			return true;
			break;
		case CTexture::RED32:
			return false;
			break;
		case CTexture::RED32FLOAT:
			return false;
			break;
		}
		break;

	case CRenderSystem::DEPTH_STENCIL:
		return false;
		break;

	case CRenderSystem::INDEX_BUFFER:
		switch(uFormat)
		{
		case CIndexBuffer::BIT16:
			return true;
			break;
		case CIndexBuffer::BIT32:
			return false;
			break;
		}
		break;
	}

	return false;
}

ICompiler& CFlashContext3D::GetCompiler()
{
	return m_Compiler;
}

bool CFlashContext3D::GetViewport(PUINT puWidth, PUINT puHeight)
{
	if(puWidth)
		*puWidth = m_uViewportWidth;

	if(puHeight)
		*puHeight = m_uViewportHeight;

	return true;
}

void CFlashContext3D::SetViewport(UINT uWidth, UINT uHeight)
{
	m_uViewportWidth = uWidth;
	m_uViewportHeight = uHeight;

	UINT uEventType;
	if( ALCHEMY_GET_EVENT_TYPE(uEventType, RESET_VIEWPORT, CRenderSystem) )
	{
		CEvent Event(uEventType);
		DispatchEvent(Event);
	}
}

/*CGraphicsResource::HANDLE CFlashContext3D::GetRenderTarget()
{
	return m_CurrentRenderData.uRenderTarget;
}*/

bool CFlashContext3D::Clear(ARGBCOLOR Color, UINT uFlag)
{
	if(uFlag)
	{
		ALCHEMY_SET_BIT(m_CurrentRenderData.uFlag, CLEAR_DIRTY);

		m_CurrentRenderData.ClearData.Color = Color;
		m_CurrentRenderData.ClearData.uFlag = uFlag;
	}

	return true;
}

bool CFlashContext3D::ResetTexture()
{
	if(m_uTextureFlag)
	{
		CFlashTexture* pTexture;
		UINT uBegin = MininumBit( static_cast<UINT8>(m_uTextureFlag) ) - 1, uEnd = MaxinumBit( static_cast<UINT8>(m_uTextureFlag) ), uIndex = m_uCurrentRenderDataCount * MAXINUM_TEXTURE_COUNT + uBegin;
		for(UINT i = uBegin; i < uEnd; ++ i)
		{
			if( ALCHEMY_TEST_BIT(m_uTextureFlag, i) )
			{
				if( ALCHEMY_TEST_BIT(m_CurrentRenderData.ProgramData.uTextureFlag, i) )
				{
					pTexture = static_cast<CFlashTexture*>( GetResource(m_puProgramTexture[uIndex], CGraphicsResource::TEXTURE) );
					if( pTexture && pTexture->IsActivate() )
						static_cast<CFlashGraphicsBuffer<CTexture>*>(pTexture)->Reset();

					ALCHEMY_CLEAR_BIT(m_CurrentRenderData.ProgramData.uTextureFlag, i);
				}

				m_puProgramTexture[uIndex] = 0;
			}

			++ uIndex;
		}

		m_CurrentRenderData.uFlag |= m_uTextureFlag << TEXTURE_DIRTY_SHIFT;

		m_uTextureFlag = 0;
	}

	return true;
}

bool CFlashContext3D::RegisterRenderState()
{
	m_uRenderStateFlag = 0;

	m_uRenderStateStackCount = 0;

	return true;
}

bool CFlashContext3D::RestoreRenderState()
{
	CRenderSystem::RENDERSTATE State;
	UINT uValue;
	for(UINT i = 0; i < m_uRenderStateStackCount; ++ i)
	{
		State  = m_RenderStateStack[i];
		uValue = m_RenderStateValueStack[i];

		m_CurrentRenderData.DeviceStateBlock.uRenderState[State] = uValue;

		ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.DeviceStateBlock.uDirtyFlag, State, uValue != m_OldRenderData.DeviceStateBlock.uRenderState[State]);
	}

	m_uRenderStateFlag = 0;

	m_uRenderStateStackCount = 0;

	return true;
}

bool CFlashContext3D::SetRenderState(const RENDERSTATEINFO* pRenderStateInfos, UINT uRenderStateInfoCount, bool bIsRegister)
{
	if(bIsRegister)
	{
		m_uRenderStateFlag = 0;

		m_uRenderStateStackCount = 0;
	}
	
	const RENDERSTATEINFO* pRenderStateInfo;
	for(UINT i = 0; i < uRenderStateInfoCount; ++ i)
	{
		pRenderStateInfo = &pRenderStateInfos[i];
		if(pRenderStateInfo->State < TOTAL_RENDER_STATE)
		{
			if( !ALCHEMY_TEST_BIT(m_uRenderStateFlag, pRenderStateInfo->State) )
			{
				m_RenderStateStack[m_uRenderStateStackCount] = pRenderStateInfo->State;

				m_RenderStateValueStack[m_uRenderStateStackCount] = m_CurrentRenderData.DeviceStateBlock.uRenderState[pRenderStateInfo->State];

				ALCHEMY_SET_BIT(m_uRenderStateFlag, pRenderStateInfo->State);

				++ m_uRenderStateStackCount;
			}

			m_CurrentRenderData.DeviceStateBlock.uRenderState[pRenderStateInfo->State] = pRenderStateInfo->uValue;

			ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.DeviceStateBlock.uDirtyFlag, pRenderStateInfo->State, pRenderStateInfo->uValue != m_OldRenderData.DeviceStateBlock.uRenderState[pRenderStateInfo->State]);
		}
		else
		{
			ALCHEMY_DEBUG_WARNING("CFlashContext3D::SetRenderState: Error state.");

			return false;
		}
	}
	
	return true;
}

bool CFlashContext3D::SetRenderTarget(CGraphicsResource::HANDLE Texture, UINT uSurface, UINT uIndex)
{
	m_CurrentRenderData.uRenderTarget = Texture;

	m_CurrentRenderData.uRenderSurface = uSurface;

	ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.uFlag, RENDER_TARGET_DIRTY, Texture != m_OldRenderData.uRenderTarget || uSurface != m_OldRenderData.uRenderSurface);

	return true;
}

bool CFlashContext3D::SetVertexDeclaration(CGraphicsResource::HANDLE VertexDeclaration)
{
	if(m_pVertexDeclaration)
		m_pVertexDeclaration->Release();

	m_pVertexDeclaration = static_cast<CVertexDeclaration*>( GetResource(VertexDeclaration, CGraphicsResource::VERTEX_DECLARATION) );
	if(m_pVertexDeclaration)
	{
		if(m_pVertexDeclaration->GetVertexElementCount() < MAXINUM_VERTEX_INDEX_COUNT)
		{
			m_pVertexDeclaration->AddRef();

			return true;
		}
		else
		{
			m_pVertexDeclaration = ALCHEMY_NULL;

			ALCHEMY_DEBUG_WARNING("CFlashContext3D::SetVertexDeclaration: Current vertex description is not supported.");

			return false;
		}
	}

	ALCHEMY_DEBUG_WARNING("CFlashContext3D::SetVertexDeclaration: Invalid vertex declaration handle.");

	return false;
}

bool CFlashContext3D::SetStreamSource(CGraphicsResource::HANDLE VertexBuffer, UINT uStream)
{
	ALCHEMY_CLEAR_BIT(m_uVertexBufferCacheFlag, uStream);

	CFlashVertexBuffer* pVertexBuffer = static_cast<CFlashVertexBuffer*>( GetResource(VertexBuffer, CGraphicsResource::VERTEX_BUFFER) );
	if(pVertexBuffer)
	{
		if( !pVertexBuffer->IsActivate() )
		{
			pVertexBuffer->Activate();

			ALCHEMY_SET_BIT(m_uVertexBufferDirtyFlag, uStream);
		}

		m_VertexBuffer[uStream] = VertexBuffer;
	}
	else
	{
		//ALCHEMY_DEBUG_WARNING("CFlashContext3D::SetStreamSource: Invalid vertex buffer handle.");

		//return false;
		m_VertexBuffer[uStream] = 0;

		ALCHEMY_CLEAR_BIT(m_uVertexBufferDirtyFlag, uStream);
	}

	//m_VertexBuffer[uStream] = VertexBuffer;

	return true;
}

bool CFlashContext3D::SetIndices(CGraphicsResource::HANDLE IndexBuffer)
{
	CFlashIndexBuffer* pIndexBuffer = static_cast<CFlashIndexBuffer*>( GetResource(IndexBuffer, CGraphicsResource::INDEX_BUFFER) );
	if(pIndexBuffer)
		pIndexBuffer->Activate();
	else
	{
		ALCHEMY_DEBUG_WARNING("CFlashContext3D::SetIndices: Invalid index buffer handle.");

		return false;
	}

	m_CurrentRenderData.uIndexBuffer = IndexBuffer;

	ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.uFlag, INDEX_BUFFER_DIRTY, IndexBuffer != m_OldRenderData.uIndexBuffer);

	return true;
}

bool CFlashContext3D::SetShader(CGraphicsResource::HANDLE Shader)
{
	m_CurrentRenderData.ProgramData.uProgram = Shader;

	ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.uFlag, PROGRAM_DIRTY, Shader != m_OldRenderData.ProgramData.uProgram);

	return true;
}


bool CFlashContext3D::SetShaderParameters(IEffect::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uCount)
{
	UINT i, uCurrentIndex;

	if( ALCHEMY_TEST_FLAG(Type, IEffect::VERTEX_SHADER) )
	{
		//m_CurrentRenderData.ProgramData.uVertexParameterSize[m_CurrentRenderData.ProgramData.uVertexParameterCount] = uCount;
		//m_CurrentRenderData.ProgramData.uVertexIndex        [m_CurrentRenderData.ProgramData.uVertexParameterCount] = uIndex;

		//++ m_CurrentRenderData.ProgramData.uVertexParameterCount;

		uCurrentIndex = MAXINUM_VERTEX_PARAMETER_COUNT * m_uCurrentRenderDataCount * VERTEX_PARAMETER_SIZE + uIndex;

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset, uCurrentIndex);

		for(i = 0; i < uCount; ++ i)
			m_pfProgramVertexParameter[uCurrentIndex ++] = pfValues[i];

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount, uCurrentIndex);
	}

	if( ALCHEMY_TEST_FLAG(Type, IEffect::FRAGMENT_SHADER) )
	{
		//m_CurrentRenderData.ProgramData.uFragmentParameterSize[m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uCount;
		//m_CurrentRenderData.ProgramData.uFragmentIndex        [m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uIndex;

		//++ m_CurrentRenderData.ProgramData.uFragmentParameterCount;

		uCurrentIndex = MAXINUM_FRAGMENT_PARAMETER_COUNT * m_uCurrentRenderDataCount * FRAGMENT_PARAMETER_SIZE + uIndex;

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset, uCurrentIndex);

		for(i = 0; i < uCount; ++ i)
			m_pfProgramFragmentParameter[uCurrentIndex ++] = pfValues[i];

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount, uCurrentIndex);
	}

	return true;
}

bool CFlashContext3D::SetShaderMatrixArray(IEffect::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount)
{
	UINT i, j, uValueIndex, uCurrentIndex, uSize = uCount * uRow * VERTEX_PARAMETER_SIZE, uCurrentOffset = ALCHEMY_MININUM(uRow * uColumn, uOffset), uOffsetStep = uOffset - uCurrentOffset;

	if( ALCHEMY_TEST_FLAG(Type, IEffect::VERTEX_SHADER) )
	{
		//m_CurrentRenderData.ProgramData.uVertexParameterSize[m_CurrentRenderData.ProgramData.uVertexParameterCount] = uSize;
		//m_CurrentRenderData.ProgramData.uVertexIndex        [m_CurrentRenderData.ProgramData.uVertexParameterCount] = uIndex;

		//++ m_CurrentRenderData.ProgramData.uVertexParameterCount;

		uCurrentIndex = MAXINUM_VERTEX_PARAMETER_COUNT * m_uCurrentRenderDataCount * VERTEX_PARAMETER_SIZE + uIndex;

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset, uCurrentIndex);

		uValueIndex = 0;
		for(i = 0; i < uCount; ++ i)
		{
			for(j = 0; j < uCurrentOffset; ++ j)
				m_pfProgramVertexParameter[uCurrentIndex ++] = pfValues[uValueIndex ++];

			uValueIndex += uOffsetStep;
		}

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount, uCurrentIndex);
		/*uMatrixIndex = 0;
		for(i = 0; i < uCount; ++ i)
		{
			uRowIndex = uMatrixIndex;
			for(j = 0; j < uRow; ++ j)
			{
				uValueIndex = uRowIndex;
				for(k = 0; k < uColumn; ++ k)
				{
					m_pfProgramVertexParameter[uCurrentIndex ++] = pfValues[uValueIndex];

					uValueIndex += uRowOffset;
				}

				while(k ++ < VERTEX_PARAMETER_SIZE)
					m_pfProgramVertexParameter[uCurrentIndex ++] = 0.0f;

				++ uRowIndex;
			}

			uMatrixIndex += uOffset;
		}*/
	}

	if( ALCHEMY_TEST_FLAG(Type, IEffect::FRAGMENT_SHADER) )
	{
		//m_CurrentRenderData.ProgramData.uFragmentParameterSize[m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uSize;
		//m_CurrentRenderData.ProgramData.uFragmentIndex        [m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uIndex;

		//++ m_CurrentRenderData.ProgramData.uFragmentParameterCount;

		uCurrentIndex = MAXINUM_FRAGMENT_PARAMETER_COUNT * m_uCurrentRenderDataCount * FRAGMENT_PARAMETER_SIZE + uIndex;

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset, uCurrentIndex);

		uValueIndex = 0;
		for(i = 0; i < uCount; ++ i)
		{
			for(j = 0; j < uCurrentOffset; ++ j)
				m_pfProgramFragmentParameter[uCurrentIndex ++] = pfValues[uValueIndex ++];

			uValueIndex += uOffsetStep;
		}

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount, uCurrentIndex);
		/*uMatrixIndex = 0;
		for(i = 0; i < uCount; ++ i)
		{
			uRowIndex = uMatrixIndex;
			for(j = 0; j < uRow; ++ j)
			{
				uValueIndex = uRowIndex;
				for(k = 0; k < uColumn; ++ k)
				{
					m_pfProgramFragmentParameter[uCurrentIndex ++] = pfValues[uValueIndex];

					uValueIndex += uRowOffset;
				}

				while(k ++ < FRAGMENT_PARAMETER_SIZE)
					m_pfProgramFragmentParameter[uCurrentIndex ++] = 0.0f;

				++ uRowIndex;
			}

			uMatrixIndex += uOffset;
		}*/
	}

	return true;
}

bool CFlashContext3D::SetShaderMatrixPointerArray(IEffect::EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount)
{
	UINT i, j, uCurrentIndex, uSize = uCount * uRow * VERTEX_PARAMETER_SIZE, uOffset = uRow * uColumn;

	PFLOAT pfValues;

	if( ALCHEMY_TEST_FLAG(Type, IEffect::VERTEX_SHADER) )
	{
		//m_CurrentRenderData.ProgramData.uVertexParameterSize[m_CurrentRenderData.ProgramData.uVertexParameterCount] = uSize;
		//m_CurrentRenderData.ProgramData.uVertexIndex        [m_CurrentRenderData.ProgramData.uVertexParameterCount] = uIndex;

		//++ m_CurrentRenderData.ProgramData.uVertexParameterCount;

		uCurrentIndex = MAXINUM_VERTEX_PARAMETER_COUNT * m_uCurrentRenderDataCount * VERTEX_PARAMETER_SIZE + uIndex;

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset, uCurrentIndex);

		for(i = 0; i < uCount; ++ i)
		{
			pfValues = ppfValues[i];
			if(pfValues)
			{
				for(j = 0; j < uOffset; ++ j)
					m_pfProgramVertexParameter[uCurrentIndex ++] = pfValues[j];
			}
		}

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount, uCurrentIndex);
		/*for(i = 0; i < uCount; ++ i)
		{
			pfValues = ppfValues[i];

			if(!pfValues)
			{
				for(j = 0; j < uRow; ++ j)
				{
					for(k = 0; k < uColumn; ++ k)
						m_pfProgramVertexParameter[uCurrentIndex ++] = k == j ? 1.0f : 0.0f;

					while(k ++ < VERTEX_PARAMETER_SIZE)
						m_pfProgramVertexParameter[uCurrentIndex ++] = 0.0f;
				}

				continue;
			}

			uRowIndex = 0;
			for(j = 0; j < uRow; ++ j)
			{
				uValueIndex = uRowIndex;
				for(k = 0; k < uColumn; ++ k)
				{
					m_pfProgramVertexParameter[uCurrentIndex ++] = pfValues[uValueIndex];

					uValueIndex += uRowOffset;
				}

				while(k ++ < VERTEX_PARAMETER_SIZE)
					m_pfProgramVertexParameter[uCurrentIndex ++] = 0.0f;

				++ uRowIndex;
			}
		}*/
	}

	if( ALCHEMY_TEST_FLAG(Type, IEffect::FRAGMENT_SHADER) )
	{
		//m_CurrentRenderData.ProgramData.uFragmentParameterSize[m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uSize;
		//m_CurrentRenderData.ProgramData.uFragmentIndex        [m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uIndex;

		//++ m_CurrentRenderData.ProgramData.uFragmentParameterCount;

		uCurrentIndex = MAXINUM_FRAGMENT_PARAMETER_COUNT * m_uCurrentRenderDataCount * FRAGMENT_PARAMETER_SIZE + uIndex;

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset, uCurrentIndex);

		for(i = 0; i < uCount; ++ i)
		{
			pfValues = ppfValues[i];
			if(pfValues)
			{
				for(j = 0; j < uOffset; ++ j)
					m_pfProgramFragmentParameter[uCurrentIndex ++] = pfValues[j];
			}
		}

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount, uCurrentIndex);
		/*for(i = 0; i < uCount; ++ i)
		{
			pfValues = ppfValues[i];

			if(!pfValues)
			{
				for(j = 0; j < uRow; ++ j)
				{
					for(k = 0; k < uColumn; ++ k)
						m_pfProgramFragmentParameter[uCurrentIndex ++] = k == j ? 1.0f : 0.0f;

					while(k ++ < VERTEX_PARAMETER_SIZE)
						m_pfProgramFragmentParameter[uCurrentIndex ++] = 0.0f;
				}

				continue;
			}

			uRowIndex = 0;
			for(j = 0; j < uRow; ++ j)
			{
				uValueIndex = uRowIndex;
				for(k = 0; k < uColumn; ++ k)
				{
					m_pfProgramFragmentParameter[uCurrentIndex ++] = pfValues[uValueIndex];

					uValueIndex += uRowOffset;
				}

				while(k ++ < FRAGMENT_PARAMETER_SIZE)
					m_pfProgramFragmentParameter[uCurrentIndex ++] = 0.0f;

				++ uRowIndex;
			}
		}*/
	}

	return true;
}

bool CFlashContext3D::SetShaderConstants(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE ConstantBuffer, UINT uIndex)
{
	return false;
}

bool CFlashContext3D::SetTexture(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE Texture, UINT uIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration)
{
	if(uIndex >= MAXINUM_TEXTURE_COUNT)
	{
		ALCHEMY_DEBUG_WARNING("CFlashContext3D::SetProgramTexture: Index out of range.");

		return false;
	}

	if(Texture)
	{
		CFlashTexture* pTexture = static_cast<CFlashTexture*>( GetResource(Texture, CGraphicsResource::TEXTURE) );
		if(pTexture)
		{
			if( !pTexture->IsActivate() )
			{
				ALCHEMY_SET_BIT(m_CurrentRenderData.ProgramData.uTextureFlag, uIndex);

				pTexture->Activate();
			}

			ALCHEMY_SET_BIT(m_uTextureFlag, uIndex);
		}
		else
		{
			if( ALCHEMY_TEST_BIT(m_CurrentRenderData.ProgramData.uTextureFlag, uIndex) )
			{
				pTexture = static_cast<CFlashTexture*>( GetResource(m_puProgramTexture[uIndex], CGraphicsResource::TEXTURE) );
				if( pTexture && pTexture->IsActivate() )
					static_cast<CFlashGraphicsBuffer<CTexture>*>(pTexture)->Reset();

				ALCHEMY_CLEAR_BIT(m_CurrentRenderData.ProgramData.uTextureFlag, uIndex);
			}

			ALCHEMY_CLEAR_BIT(m_uTextureFlag, uIndex);
		}
		/*else
		{
			ALCHEMY_DEBUG_WARNING("CFlashContext3D::SetProgramTexture: Invalid texture handle.");

			return false;
		}*/
	}

	UINT uTextureIndex = m_uCurrentRenderDataCount * MAXINUM_TEXTURE_COUNT + uIndex;
	m_puProgramTexture[uTextureIndex] = Texture;

	//ALCHEMY_SET_BIT(m_uCurrentTextureFlag, uIndex);

	uTextureIndex = uTextureIndex < MAXINUM_TEXTURE_COUNT ? (uIndex + (m_uTotalRenderDataCount - 1) * MAXINUM_TEXTURE_COUNT) : uTextureIndex - MAXINUM_TEXTURE_COUNT;

	/*if( !ALCHEMY_TEST_BIT(m_uTextureFlag, uIndex) )
	{
		m_puProgramTexture[uTextureIndex] = 0;

		ALCHEMY_SET_BIT(m_uTextureFlag, uIndex);
	}*/

	if( ALCHEMY_TEST_BIT(m_OldRenderData.uFlag, TEXTURE_DIRTY_SHIFT + uIndex) )
		ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.uFlag, TEXTURE_DIRTY_SHIFT + uIndex, m_puProgramTexture[uTextureIndex] != Texture);
	else
		ALCHEMY_SET_BIT(m_CurrentRenderData.uFlag, TEXTURE_DIRTY_SHIFT + uIndex);

	return true;
}

/*bool CFlashContext3D::DrawTriangles(CRenderSystem::TRIANGLETYPE Type, UINT uStartVertex, UINT uTriangleCount)
{
	if(!m_pVertexDeclaration)
	{
		ALCHEMY_DEBUG_WARNING("No vertex declaration");

		return false;
	}

	m_CurrentRenderData.uStart         = uStartVertex;
	m_CurrentRenderData.uTriangleCount = uTriangleCount;

	__PushRenderData();

	return true;
}*/

bool CFlashContext3D::DrawIndexedTriangles(CRenderSystem::TRIANGLETYPE Type, UINT uStartIndex, UINT uTriangleCount)
{
	if(!m_pVertexDeclaration)
	{
		ALCHEMY_DEBUG_WARNING("No vertex declaration");

		return false;
	}

	m_CurrentRenderData.uStart         = uStartIndex;
	m_CurrentRenderData.uTriangleCount = uTriangleCount;

	ALCHEMY_SET_BIT(m_CurrentRenderData.uFlag, DRAW_INDEXED_TRIANGLES);

	__PushRenderData();

	return true;
}

bool CFlashContext3D::Present()
{
	return false;
}

void CFlashContext3D::SubmitRenderDataQueueToRender()
{
	ALCHEMY_FLASH_CALL(m_RenderMethod, m_Object);

	CFlashVertexBuffer* pVertexBuffer;
	CFlashIndexBuffer * pIndexBuffer;
	CFlashTexture* pTexture;
	LPRENDERDATA pRenderData;
	UINT i, j, uEnd, uTextureIndex = 0;
	UINT16 uHandle;
	UINT8 uFlag;
	for(i = 0; i < m_uCurrentRenderDataCount; ++ i)
	{
		pRenderData = &m_pRenderDataQueue[i];

		uFlag = static_cast<UINT8>(pRenderData->uFlag >> VERTEX_BUFFER_DIRTY_SHIFT);
		if(uFlag)
		{
			uEnd = MaxinumBit(uFlag);
			for(j = MininumBit(uFlag) - 1; j < uEnd; ++ j)
			{
				if( ALCHEMY_TEST_BIT(uFlag, j) )
				{
					pVertexBuffer = static_cast<CFlashVertexBuffer*>( GetResource(pRenderData->VertexData[j].VertexBufferData.uVertexBuffer, CGraphicsResource::VERTEX_BUFFER) );

					if( pVertexBuffer->IsActivate() )
						pVertexBuffer->Reset();
				}
			}
		}

		pIndexBuffer = static_cast<CFlashIndexBuffer*>( GetResource(pRenderData->uIndexBuffer, CGraphicsResource::INDEX_BUFFER) );
		
		if( pIndexBuffer->IsActivate() )
			pIndexBuffer->Reset();

		uFlag = static_cast<UINT8>(pRenderData->ProgramData.uTextureFlag);
		if(uFlag)
		{
			uEnd = MaxinumBit(uFlag);
			j = MininumBit(uFlag) - 1;
			uTextureIndex += j;
			while(j < uEnd)
			{
				if( ALCHEMY_TEST_BIT(uFlag, j) )
				{
					uHandle = m_puProgramTexture[uTextureIndex];
					if(uHandle)
					{
						pTexture = static_cast<CFlashTexture*>( GetResource(uHandle, CGraphicsResource::TEXTURE) );

						if( pTexture->IsActivate() )
							pTexture->Reset();

						//m_puProgramTexture[uTextureIndex] = 0;
					}
				}

				++ uTextureIndex;

				++ j;
			}

			uTextureIndex += MAXINUM_TEXTURE_COUNT - uEnd;
		}
		else
			uTextureIndex += MAXINUM_TEXTURE_COUNT;
	}

	m_uCurrentRenderDataCount        = 0;

	//m_uProgramVertexParameterCount   = 0;
	//m_uProgramFragmentParameterCount = 0;
}

CGraphicsResource* CFlashContext3D::_Create(CGraphicsResource::TYPE Type)
{
	CGraphicsResource* pResource = ALCHEMY_NULL;

	switch(Type)
	{
	case CGraphicsResource::VERTEX_DECLARATION:
		ALCHEMY_DEBUG_NEW(pResource, CFlashVertexDeclaration);
		break;

	case CGraphicsResource::VERTEX_BUFFER:
		ALCHEMY_DEBUG_NEW(pResource, CFlashVertexBuffer);

//#ifdef AS3_H
//		CFlashVertexBuffer* pVertexBuffer = static_cast<CFlashVertexBuffer*>(pResource);
//		AS3_CallT(m_CreateResourceMethod, m_Object, "IntType, IntType, IntType", pVertexBuffer, static_cast<CFlashObjectInterface*>(pVertexBuffer), Type);
//#endif
		break;
	case CGraphicsResource::INDEX_BUFFER:
		ALCHEMY_DEBUG_NEW(pResource, CFlashIndexBuffer);

//#ifdef AS3_H
//		CFlashIndexBuffer* pIndexBuffer = static_cast<CFlashIndexBuffer*>(pResource);
//		AS3_CallT(m_CreateResourceMethod, m_Object, "IntType, IntType, IntType", pIndexBuffer, static_cast<CFlashObjectInterface*>(pIndexBuffer), Type);
//#endif
		break;
	case CGraphicsResource::SHADER:
		ALCHEMY_DEBUG_NEW(pResource, CFlashProgram3D);

//#ifdef AS3_H
//		CFlashProgram3D* pProgram = static_cast<CFlashProgram3D*>(pResource);
//		AS3_CallT(m_CreateResourceMethod, m_Object, "IntType, IntType, IntType", pProgram, static_cast<CFlashObjectInterface*>(pProgram), Type);
//#endif
		break;
	case CGraphicsResource::EFFECT:
		ALCHEMY_DEBUG_NEW(pResource, CFlashEffect);

//#ifdef AS3_H
//		CFlashEffect* pEffect = static_cast<CFlashEffect*>(pResource);
//		AS3_CallT(m_CreateResourceMethod, m_Object, "IntType, IntType, IntType", pEffect, static_cast<CFlashObjectInterface*>(pEffect), Type);
//#endif
		break;
	case CGraphicsResource::TEXTURE:
		ALCHEMY_DEBUG_NEW(pResource, CFlashTexture);
//#ifdef AS3_H
//		CFlashTexture* pTexture = static_cast<CFlashTexture*>(pResource);
//		AS3_CallT(m_CreateResourceMethod, m_Object, "IntType, IntType, IntType", pTexture, static_cast<CFlashObjectInterface*>(pTexture), Type);
//#endif
		break;
    default:
        break;
	}

	return pResource;
}


/*void CFlashContext3D::__CommitProgramTextureChange(UINT uTextureFlag)
{
	uTextureFlag = uTextureFlag & m_uTextureFlag;

	UINT uTextureMask, uBegin, uEnd, uIndex;
	CFlashTexture* pTexture;

	uTextureMask = (uTextureFlag | m_CurrentRenderData.ProgramData.uTextureFlag) ^ uTextureFlag;
	if(uTextureMask)
	{
		uBegin = MininumBit( static_cast<UINT8>(uTextureMask) ) - 1;
		uEnd = MaxinumBit( static_cast<UINT8>(uTextureMask) );
		uIndex = m_uCurrentRenderDataCount * MAXINUM_TEXTURE_COUNT + uBegin;
		for(UINT i = uBegin; i < uEnd; ++ i)
		{
			if( ALCHEMY_TEST_BIT(uTextureMask, i) )
			{
				pTexture = static_cast<CFlashTexture*>( GetResource(m_puProgramTexture[uIndex], CGraphicsResource::TEXTURE) );
				if( pTexture && pTexture->IsActivate() )
					static_cast<CFlashGraphicsBuffer<CTexture>*>(pTexture)->Reset();

				m_puProgramTexture[uIndex] = 0;
			}

			++ uIndex;
		}

		m_CurrentRenderData.ProgramData.uTextureFlag &= ~uTextureMask;

		m_CurrentRenderData.uFlag &= ~(uTextureMask << TEXTURE_DIRTY_SHIFT);
	}

	uTextureMask = m_uTextureFlag ^ uTextureFlag;
	if(uTextureMask)
	{
		uBegin = MininumBit( static_cast<UINT8>(uTextureMask) ) - 1;
		uEnd = MaxinumBit( static_cast<UINT8>(uTextureMask) );
		uIndex = m_uCurrentRenderDataCount * MAXINUM_TEXTURE_COUNT + uBegin;
		for(UINT i = uBegin; i < uEnd; ++ i)
		{
			if( ALCHEMY_TEST_BIT(uTextureMask, i) )
			{
				if( ALCHEMY_TEST_BIT(m_CurrentRenderData.ProgramData.uTextureFlag, i) )
				{
					pTexture = static_cast<CFlashTexture*>( GetResource(m_puProgramTexture[uIndex], CGraphicsResource::TEXTURE) );
					if( pTexture && pTexture->IsActivate() )
						static_cast<CFlashGraphicsBuffer<CTexture>*>(pTexture)->Reset();

					ALCHEMY_CLEAR_BIT(m_CurrentRenderData.ProgramData.uTextureFlag, i);
				}

				m_puProgramTexture[uIndex] = 0;
			}

			++ uIndex;
		}

		//m_CurrentRenderData.uFlag &= ~( ( uTextureFlag ^ ( (m_CurrentRenderData.uFlag >> TEXTURE_DIRTY_SHIFT) && 0xff ) ) << TEXTURE_DIRTY_SHIFT );

		m_CurrentRenderData.uFlag |= uTextureMask << TEXTURE_DIRTY_SHIFT;
	}

	m_uTextureFlag = uTextureFlag;
}*/

void CFlashContext3D::__ClearRenderData(RENDERDATA& RenderData)
{
	RenderData.DeviceStateBlock.uDirtyFlag                     = 0;

	//RenderData.ProgramData.uVertexParameterCount               = 0;
	//RenderData.ProgramData.uFragmentParameterCount             = 0;

	RenderData.ProgramData.uDirtyVertexParameterBufferOffset   = ~0;
	RenderData.ProgramData.uDirtyFragmentParameterBufferOffset = ~0;

	RenderData.ProgramData.uDirtyVertexParameterBufferCount    = 0;
	RenderData.ProgramData.uDirtyFragmentParameterBufferCount  = 0;

	RenderData.ProgramData.uTextureFlag                        = 0;

	RenderData.uFlag                                           = 0;
}

void CFlashContext3D::__PushRenderData()
{
	CFlashVertexBuffer* pVertexBuffer;
	LPVERTEXDATA pVertexData;
	LPVERTEXBUFFERDATA pCurrentVertexBufferData, pOldVertexBufferData;
	const CVertexDeclaration::VERTEXELEMENT* pVertexElement = m_pVertexDeclaration->GetVertexElements();
	//const UINT* pOrder = m_pVertexDeclaration->GetVertexElementOrderMap();
	UINT uVertexElementCount = m_pVertexDeclaration->GetVertexElementCount(), uStreamDirtyFlag = 0, uVertexBufferIndexFlag = 0, /*uOrder, */i;
	for(i = 0; i < uVertexElementCount; ++ i)
	{
		//uOrder                   = pOrder[i];

		ALCHEMY_SET_BIT(uVertexBufferIndexFlag, pVertexElement->uIndex);

		pVertexData              = &m_CurrentRenderData.VertexData[pVertexElement->uIndex];
		pCurrentVertexBufferData = &pVertexData->VertexBufferData;
		pOldVertexBufferData     = &m_OldRenderData.VertexData[pVertexElement->uIndex].VertexBufferData;

		pCurrentVertexBufferData->uVertexBuffer  = m_VertexBuffer[pVertexElement->uStream];
		//pCurrentVertexBufferData->uIndex         = pVertexElement->uIndex;
		pCurrentVertexBufferData->uOffsetInBytes = pVertexElement->uOffset;

		pVertexData->Type                        = pVertexElement->Type;

		if(pCurrentVertexBufferData->uVertexBuffer != pOldVertexBufferData->uVertexBuffer || pCurrentVertexBufferData->uOffsetInBytes != pOldVertexBufferData->uOffsetInBytes)
			ALCHEMY_SET_BIT(m_CurrentRenderData.uFlag, VERTEX_INDEX_DIRTY_SHIFT + pVertexElement->uIndex);

		if( ALCHEMY_TEST_BIT(m_uVertexBufferDirtyFlag, pVertexElement->uStream) )
		{
			ALCHEMY_SET_BIT(uStreamDirtyFlag, pVertexElement->uIndex);

			ALCHEMY_CLEAR_BIT(m_uVertexBufferDirtyFlag, pVertexElement->uStream);
		}
		else if( ALCHEMY_TEST_BIT(m_uVertexBufferCacheFlag, pVertexElement->uStream) )
		{
			pVertexBuffer = static_cast<CFlashVertexBuffer*>( GetResource(pCurrentVertexBufferData->uVertexBuffer, CGraphicsResource::VERTEX_BUFFER) );

			if( !pVertexBuffer->IsActivate() )
			{
				pVertexBuffer->Activate();

				ALCHEMY_SET_BIT(uStreamDirtyFlag, pVertexElement->uIndex);
			}

			ALCHEMY_CLEAR_BIT(m_uVertexBufferCacheFlag, pVertexElement->uStream);
		}

		++ pVertexElement;
	}

	ALCHEMY_CLEAR_FLAG(m_uVertexBufferIndexFlag, uVertexBufferIndexFlag);

	UINT uEnd;
	if(m_uVertexBufferIndexFlag)
	{
		uEnd = MaxinumBit( static_cast<UINT8>(m_uVertexBufferIndexFlag) );
		for(i = MininumBit( static_cast<UINT8>(m_uVertexBufferIndexFlag) ) - 1; i < uEnd; ++ i)
		{
			if( ALCHEMY_TEST_BIT(m_uVertexBufferIndexFlag, i) )
			{
				ALCHEMY_SET_BIT(m_CurrentRenderData.uFlag, VERTEX_INDEX_DIRTY_SHIFT + i);

				m_CurrentRenderData.VertexData[i].VertexBufferData.uVertexBuffer = 0;
			}
		}
	}

	m_uVertexBufferIndexFlag = uVertexBufferIndexFlag;

	/*while(i < MAXINUM_VERTEX_INDEX_COUNT)
	{
		if(m_OldRenderData.VertexData[i].VertexBufferData.uVertexBuffer)
		{
			ALCHEMY_SET_BIT(m_CurrentRenderData.uFlag, VERTEX_INDEX_DIRTY_SHIFT + i);

			m_CurrentRenderData.VertexData[i].VertexBufferData.uVertexBuffer = 0;
		}

		++ i;
	}*/

	if(m_uVertexBufferDirtyFlag)
	{
		m_uVertexBufferCacheFlag |= m_uVertexBufferDirtyFlag;
		uEnd = MaxinumBit( static_cast<UINT8>(m_uVertexBufferDirtyFlag) );
		for(i = MininumBit( static_cast<UINT8>(m_uVertexBufferDirtyFlag) ) - 1; i < uEnd; ++ i)
		{
			if( ALCHEMY_TEST_BIT(m_uVertexBufferDirtyFlag, i) )
			{
				pVertexBuffer = static_cast<CFlashVertexBuffer*>( GetResource(m_VertexBuffer[i], CGraphicsResource::VERTEX_BUFFER) );

				if( pVertexBuffer->IsActivate() )
					static_cast<CFlashGraphicsBuffer<CGraphicsBuffer<CVertexBuffer>::CLocker>*>(pVertexBuffer)->Reset();
			}
		}

		m_uVertexBufferDirtyFlag = 0;
	}

	//ALCHEMY_CLEAR_MASK2(m_CurrentRenderData.uFlag, MAXINUM_VERTEX_INDEX_COUNT - 1, VERTEX_BUFFER_DIRTY_SHIFT);

	m_CurrentRenderData.uFlag |= uStreamDirtyFlag << VERTEX_BUFFER_DIRTY_SHIFT;

	//__CommitProgramTextureChange(m_uCurrentTextureFlag);
	//m_uCurrentTextureFlag = 0;

	m_pRenderDataQueue[m_uCurrentRenderDataCount] = m_CurrentRenderData;

	if(++ m_uCurrentRenderDataCount == m_uTotalRenderDataCount)
		SubmitRenderDataQueueToRender();

	m_OldRenderData = m_CurrentRenderData;

	__ClearRenderData(m_CurrentRenderData);

	//CommitProgramTextureChange(0);
}

#ifdef AS3_H
AS3TYPE CFlashContext3D::Init()
{
	return AS3_Array("PtrType, PtrType, PtrType, PtrType, PtrType", 
		m_pRenderDataQueue, 
		&m_uCurrentRenderDataCount, 
		m_pfProgramVertexParameter, 
		//&m_uProgramVertexParameterCount, 
		m_pfProgramFragmentParameter, 
		//&m_uProgramFragmentParameterCount, 
		m_puProgramTexture);
}

void CFlashContext3D::Fill(AS3TYPE Object, AS3TYPE MethodArray)
{
	m_Object = Object;

	AS3_ArrayValue(MethodArray, "AS3ValType, AS3ValType, AS3ValType", &m_CreateResourceMethod, &m_DestroyResourceMethod, &m_RenderMethod);

	AS3_Acquire(m_Object);
	AS3_Acquire(m_RenderMethod);
	AS3_Acquire(m_CreateResourceMethod);
	AS3_Acquire(m_DestroyResourceMethod);
}
#endif

#ifdef AS3_H
bool CFlashContext3D::_RemoveResource(CGraphicsResource::HANDLE Handle, CGraphicsResource::TYPE Type)
{
	int nHandle = Handle, nType = Type;

	AS3_CallT(m_DestroyResourceMethod, m_Object, "IntType, IntType", nHandle, nType);

	return CGraphicsResourcePool::_RemoveResource(Handle, Type);
}
#endif