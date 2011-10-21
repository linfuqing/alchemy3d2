#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashContext3D.h"
#include "AlchemyFlashVertexDeclaration.h"
#include "AlchemyFlashVertexBuffer.h"
#include "AlchemyFlashIndexBuffer.h"
#include "AlchemyFlashProgram3D.h"
#include "AlchemyFlashEffect.h"
#include "AlchemyFlashTexture.h"

using namespace alchemy;
CFlashContext3D::CFlashContext3D(CCompilerInterface& Complier, UINT uTotalRenderDataCount) :
m_uTotalRenderDataCount(uTotalRenderDataCount),
m_uCurrentRenderDataCount(0),
m_uProgramVertexParameterCount(0),
m_uProgramFragmentParameterCount(0),
m_uTextureFlag(0),
m_uVertexBufferDirtyFlag(0),
m_uRenderStateStackCount(0),
m_uRenderStateFlag(0),
m_pVertexDeclaration(ALCHEMY_NULL),
m_uViewportWidth(1),
m_uViewportHeight(1),
m_bIsViewportDirty(false),
m_Complier(Complier)
{
	UINT uTotalTextureCount = MAXINUM_TEXTURE_COUNT * m_uTotalRenderDataCount;
	ALCHEMY_DEBUG_NEW(m_pRenderDataQueue          , RENDERDATA[                              m_uTotalRenderDataCount                          ]);
	ALCHEMY_DEBUG_NEW(m_pfProgramVertexParameter  , FLOAT[MAXINUM_VERTEX_PARAMETER_COUNT   * m_uTotalRenderDataCount * VERTEX_PARAMETER_SIZE  ]);
	ALCHEMY_DEBUG_NEW(m_pfProgramFragmentParameter, FLOAT[MAXINUM_FRAGMENT_PARAMETER_COUNT * m_uTotalRenderDataCount * FRAGMENT_PARAMETER_SIZE]);
	ALCHEMY_DEBUG_NEW(m_puProgramTexture          , UINT16[                                       uTotalTextureCount                          ]);

	UINT i;
	for(i = 0; i < uTotalTextureCount; ++ i)
		m_puProgramTexture[i] = 0;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::FILL_MODE] = 0;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::SHADE_MODE] = 0;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::DEPTH_WRITEE_ENABLE] = 1;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::DEPTH_FUNC] = CRenderSystem::LESS;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::ALPHA_FUNC] = CRenderSystem::LESS;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::SRC_BLEND] = CRenderSystem::ONE;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::DEST_BLEND] = CRenderSystem::ZERO;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::ALPHA_REF] = 0;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::ALPHA_FUNC] = CRenderSystem::LESS;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::CULL_MODE] = CRenderSystem::NONE;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_FUNC] = CRenderSystem::ALWAYS;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_FAIL] = CRenderSystem::KEEP;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_Z_FAIL] = CRenderSystem::KEEP;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_PASS] = CRenderSystem::KEEP;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_REF] = 0;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_READ_MASK] = 0xff;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_WRITE_MASK] = 0xff;

	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::CCW_STENCIL_FAIL] = CRenderSystem::KEEP;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::CCW_STENCIL_Z_FAIL] = CRenderSystem::KEEP;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::CCW_STENCIL_PASS] = CRenderSystem::KEEP;
	m_CurrentRenderData.DeviceStateBlock.uRenderState[CRenderSystem::CCW_STENCIL_FUNC] = CRenderSystem::ALWAYS;

	m_CurrentRenderData.DeviceStateBlock.uDirtyFlag = 0;

	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::FILL_MODE] = 0;

	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::SHADE_MODE] = 0;

	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::DEPTH_WRITEE_ENABLE] = 1;
	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::DEPTH_FUNC] = CRenderSystem::LESS;

	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::ALPHA_FUNC] = CRenderSystem::LESS;

	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::SRC_BLEND] = CRenderSystem::ONE;
	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::DEST_BLEND] = CRenderSystem::ZERO;

	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::ALPHA_REF] = 0;
	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::ALPHA_FUNC] = CRenderSystem::LESS;

	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::CULL_MODE] = CRenderSystem::NONE;

	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_FUNC] = CRenderSystem::ALWAYS;
	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_FAIL] = CRenderSystem::KEEP;
	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_Z_FAIL] = CRenderSystem::KEEP;
	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_PASS] = CRenderSystem::KEEP;

	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_REF] = 0;
	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_READ_MASK] = 0xff;
	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::STENCIL_WRITE_MASK] = 0xff;

	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::CCW_STENCIL_FAIL] = CRenderSystem::KEEP;
	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::CCW_STENCIL_Z_FAIL] = CRenderSystem::KEEP;
	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::CCW_STENCIL_PASS] = CRenderSystem::KEEP;
	m_OldRenderData.DeviceStateBlock.uRenderState[CRenderSystem::CCW_STENCIL_FUNC] = CRenderSystem::ALWAYS;

	m_OldRenderData.DeviceStateBlock.uDirtyFlag = 0;

	for(i = 0; i < MAXINUM_VERTEX_INDEX_COUNT; ++ i)
	{
		m_CurrentRenderData.VertexData[i].VertexBufferData.uVertexBuffer = 0;
		m_OldRenderData    .VertexData[i].VertexBufferData.uVertexBuffer = 0;
	}

	m_CurrentRenderData.uRenderTarget        = 0;
	m_OldRenderData    .uRenderTarget        = 0;

	m_CurrentRenderData.uRenderSurface       = 0;
	m_OldRenderData    .uRenderSurface       = 0;

	m_CurrentRenderData.uIndexBuffer         = 0;
	m_OldRenderData    .uIndexBuffer         = 0;

	m_CurrentRenderData.ProgramData.uProgram = 0;
	m_OldRenderData    .ProgramData.uProgram = 0;

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
}

CCompilerInterface& CFlashContext3D::GetComplier()
{
	return m_Complier;
}

bool CFlashContext3D::GetViewport(PUINT puWidth, PUINT puHeight)
{
	if(puWidth)
		*puWidth = m_uViewportWidth;

	if(puHeight)
		*puHeight = m_uViewportHeight;

	return true;
}

CGraphicsResource::HANDLE CFlashContext3D::GetRenderTarget()
{
	return m_CurrentRenderData.uRenderTarget;
}

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

bool CFlashContext3D::SetRenderState(CRenderSystem::RENDERSTATE State, UINT uValue)
{
	if(State < TOTAL_RENDER_STATE)
	{
		if( !ALCHEMY_TEST_BIT(m_uRenderStateFlag, State) )
		{
			m_RenderStateStack[m_uRenderStateStackCount] = State;

			m_RenderStateValueStack[m_uRenderStateStackCount] = m_CurrentRenderData.DeviceStateBlock.uRenderState[State];

			ALCHEMY_SET_BIT(m_uRenderStateFlag, State);

			++ m_uRenderStateStackCount;
		}

		m_CurrentRenderData.DeviceStateBlock.uRenderState[State] = uValue;

		ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.DeviceStateBlock.uDirtyFlag, State, uValue != m_OldRenderData.DeviceStateBlock.uRenderState[State]);

		return true;
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("CFlashContext3D::SetRenderState: Error state.");

		return false;
	}
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
	CFlashVertexBuffer* pVertexBuffer = static_cast<CFlashVertexBuffer*>( GetResource(VertexBuffer, CGraphicsResource::VERTEX_BUFFER) );
	if(pVertexBuffer)
	{
		if( !pVertexBuffer->IsActivate() )
		{
			pVertexBuffer->Activate();

			ALCHEMY_SET_BIT(m_uVertexBufferDirtyFlag, uStream);
		}
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("CFlashContext3D::SetStreamSource: Invalid vertex buffer handle.");

		return false;
	}

	m_VertexBuffer[uStream] = VertexBuffer;

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

bool CFlashContext3D::DrawTriangles(CRenderSystem::TRIANGLETYPE Type, UINT uStartVertex, UINT uTriangleCount)
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
}

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

bool CFlashContext3D::DrawToTexture(CGraphicsResource::HANDLE Source, CEffectCode::SAMPLERFILTER Filter)
{
	CFlashTexture* pTexture = static_cast<CFlashTexture*>( GetResource(Source, CGraphicsResource::TEXTURE) );

	if( pTexture && ALCHEMY_TEST_BIT(pTexture->GetUsage(), CTexture::RENDER_TARGET) )
	{
		if(pTexture->IsActivate() && pTexture->m_bIsDirty)
			SubmitRenderDataQueueToRender();

		pTexture->m_bIsDirty     = true;
		pTexture->m_RenderTarget = m_CurrentRenderData.uRenderTarget;
		pTexture->m_Filter       = Filter;

		return true;
	}

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

		uFlag = static_cast<UINT8>(pRenderData->uFlag >> TEXTURE_DIRTY_SHIFT);
		if(uFlag)
		{
			uEnd = MaxinumBit(uFlag);
			j = MininumBit(uFlag) - 1;
			uTextureIndex += j;
			while(j < uEnd)
			{
				uHandle = m_puProgramTexture[uTextureIndex];
				if(uHandle)
				{
					pTexture = static_cast<CFlashTexture*>( GetResource(uHandle, CGraphicsResource::TEXTURE) );

					if( pTexture->IsActivate() )
						pTexture->Reset();

					m_puProgramTexture[uTextureIndex] = 0;
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

	m_uProgramVertexParameterCount   = 0;
	m_uProgramFragmentParameterCount = 0;
}

bool CFlashContext3D::PushProgramParameter(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValue, UINT uCount)
{
	UINT i;

	if( ALCHEMY_TEST_FLAG(Type, CEffectInterface::VERTEX_SHADER) )
	{
		m_CurrentRenderData.ProgramData.uVertexParameterSize[m_CurrentRenderData.ProgramData.uVertexParameterCount] = uCount;
		m_CurrentRenderData.ProgramData.uVertexIndex        [m_CurrentRenderData.ProgramData.uVertexParameterCount] = uIndex;

		++ m_CurrentRenderData.ProgramData.uVertexParameterCount;

		for(i = 0; i < uCount; ++ i)
			m_pfProgramVertexParameter[m_uProgramVertexParameterCount ++] = pfValue[i];
	}

	if( ALCHEMY_TEST_FLAG(Type, CEffectInterface::FRAGMENT_SHADER) )
	{
		m_CurrentRenderData.ProgramData.uFragmentParameterSize[m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uCount;
		m_CurrentRenderData.ProgramData.uFragmentIndex        [m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uIndex;

		++ m_CurrentRenderData.ProgramData.uFragmentParameterCount;

		for(i = 0; i < uCount; ++ i)
			m_pfProgramFragmentParameter[m_uProgramFragmentParameterCount ++] = pfValue[i];
	}

	return true;
}

bool CFlashContext3D::PushProgramMatrixArray(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount)
{
	UINT i, j, k, uMatrixIndex, uRowIndex, uValueIndex, uSize = uCount * uRow * VERTEX_PARAMETER_SIZE;

	if( ALCHEMY_TEST_FLAG(Type, CEffectInterface::VERTEX_SHADER) )
	{
		m_CurrentRenderData.ProgramData.uVertexParameterSize[m_CurrentRenderData.ProgramData.uVertexParameterCount] = uSize;
		m_CurrentRenderData.ProgramData.uVertexIndex        [m_CurrentRenderData.ProgramData.uVertexParameterCount] = uIndex;

		++ m_CurrentRenderData.ProgramData.uVertexParameterCount;

		uMatrixIndex = 0;
		for(i = 0; i < uCount; ++ i)
		{
			uRowIndex = uMatrixIndex;
			for(j = 0; j < uRow; ++ j)
			{
				uValueIndex = uRowIndex;
				for(k = 0; k < uColumn; ++ k)
				{
					m_pfProgramVertexParameter[m_uProgramVertexParameterCount ++] = pfValues[uValueIndex];

					uValueIndex += uRowOffset;
				}

				while(k ++ < VERTEX_PARAMETER_SIZE)
					m_pfProgramVertexParameter[m_uProgramVertexParameterCount ++] = 0.0f;

				++ uRowIndex;
			}

			uMatrixIndex += uOffset;
		}
	}

	if( ALCHEMY_TEST_FLAG(Type, CEffectInterface::FRAGMENT_SHADER) )
	{
		m_CurrentRenderData.ProgramData.uFragmentParameterSize[m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uSize;
		m_CurrentRenderData.ProgramData.uFragmentIndex        [m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uIndex;

		++ m_CurrentRenderData.ProgramData.uFragmentParameterCount;

		uMatrixIndex = 0;
		for(i = 0; i < uCount; ++ i)
		{
			uRowIndex = uMatrixIndex;
			for(j = 0; j < uRow; ++ j)
			{
				uValueIndex = uRowIndex;
				for(k = 0; k < uColumn; ++ k)
				{
					m_pfProgramFragmentParameter[m_uProgramVertexParameterCount ++] = pfValues[uValueIndex];

					uValueIndex += uRowOffset;
				}

				while(k ++ < FRAGMENT_PARAMETER_SIZE)
					m_pfProgramFragmentParameter[m_uProgramVertexParameterCount ++] = 0.0f;

				++ uRowIndex;
			}

			uMatrixIndex += uOffset;
		}
	}

	return true;
}

bool CFlashContext3D::PushProgramMatrixPointerArray(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount)
{
	UINT i, j, k, uRowIndex, uValueIndex, uSize = uCount * uRow * VERTEX_PARAMETER_SIZE;

	PFLOAT pfValues;

	if( ALCHEMY_TEST_FLAG(Type, CEffectInterface::VERTEX_SHADER) )
	{
		m_CurrentRenderData.ProgramData.uVertexParameterSize[m_CurrentRenderData.ProgramData.uVertexParameterCount] = uSize;
		m_CurrentRenderData.ProgramData.uVertexIndex        [m_CurrentRenderData.ProgramData.uVertexParameterCount] = uIndex;

		++ m_CurrentRenderData.ProgramData.uVertexParameterCount;

		for(i = 0; i < uCount; ++ i)
		{
			pfValues = ppfValues[i];

			if(!pfValues)
			{
				for(j = 0; j < uRow; ++ j)
				{
					for(k = 0; k < uColumn; ++ k)
						m_pfProgramVertexParameter[m_uProgramVertexParameterCount ++] = k == j ? 1.0f : 0.0f;

					while(k ++ < VERTEX_PARAMETER_SIZE)
						m_pfProgramVertexParameter[m_uProgramVertexParameterCount ++] = 0.0f;
				}

				continue;
			}

			uRowIndex = 0;
			for(j = 0; j < uRow; ++ j)
			{
				uValueIndex = uRowIndex;
				for(k = 0; k < uColumn; ++ k)
				{
					m_pfProgramVertexParameter[m_uProgramVertexParameterCount ++] = pfValues[uValueIndex];

					uValueIndex += uRowOffset;
				}

				while(k ++ < VERTEX_PARAMETER_SIZE)
					m_pfProgramVertexParameter[m_uProgramVertexParameterCount ++] = 0.0f;

				++ uRowIndex;
			}
		}
	}

	if( ALCHEMY_TEST_FLAG(Type, CEffectInterface::FRAGMENT_SHADER) )
	{
		m_CurrentRenderData.ProgramData.uFragmentParameterSize[m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uSize;
		m_CurrentRenderData.ProgramData.uFragmentIndex        [m_CurrentRenderData.ProgramData.uFragmentParameterCount] = uIndex;

		++ m_CurrentRenderData.ProgramData.uFragmentParameterCount;

		for(i = 0; i < uCount; ++ i)
		{
			pfValues = ppfValues[i];

			if(!pfValues)
			{
				for(j = 0; j < uRow; ++ j)
				{
					for(k = 0; k < uColumn; ++ k)
						m_pfProgramVertexParameter[m_uProgramVertexParameterCount ++] = k == j ? 1.0f : 0.0f;

					while(k ++ < VERTEX_PARAMETER_SIZE)
						m_pfProgramVertexParameter[m_uProgramVertexParameterCount ++] = 0.0f;
				}

				continue;
			}

			uRowIndex = 0;
			for(j = 0; j < uRow; ++ j)
			{
				uValueIndex = uRowIndex;
				for(k = 0; k < uColumn; ++ k)
				{
					m_pfProgramFragmentParameter[m_uProgramVertexParameterCount ++] = pfValues[uValueIndex];

					uValueIndex += uRowOffset;
				}

				while(k ++ < FRAGMENT_PARAMETER_SIZE)
					m_pfProgramFragmentParameter[m_uProgramVertexParameterCount ++] = 0.0f;

				++ uRowIndex;
			}
		}
	}

	return true;
}

bool CFlashContext3D::SetProgramTexture(CGraphicsResource::HANDLE Texture, UINT uIndex)
{
	if(uIndex >= MAXINUM_TEXTURE_COUNT)
	{
		ALCHEMY_DEBUG_WARNING("CFlashContext3D::SetProgramTexture: Index out of range.");

		return false;
	}

	CFlashTexture* pTexture = static_cast<CFlashTexture*>( GetResource(Texture, CGraphicsResource::TEXTURE) );
	if(pTexture)
	{
		if( !pTexture->IsActivate() )
			pTexture->Activate();
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("CFlashContext3D::SetStreamSource: Invalid vertex buffer handle.");

		return false;
	}

	m_puProgramTexture[m_uCurrentRenderDataCount * MAXINUM_TEXTURE_COUNT + uIndex] = Texture;

	ALCHEMY_SET_BIT(m_uTextureFlag, uIndex);

	ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.uFlag, TEXTURE_DIRTY_SHIFT + uIndex, m_puProgramTexture[(m_uCurrentRenderDataCount - 1) * MAXINUM_TEXTURE_COUNT + uIndex] != Texture);

	return true;
}

void CFlashContext3D::CommitProgramTextureChange(UINT uTextureFlag)
{
	uTextureFlag               = uTextureFlag & m_uTextureFlag;

	UINT uTextureMask          = ~( (m_CurrentRenderData.uFlag >> TEXTURE_DIRTY_SHIFT) ^ uTextureFlag );

	m_CurrentRenderData.uFlag |= (m_uTextureFlag & uTextureMask) << TEXTURE_DIRTY_SHIFT;

	m_uTextureFlag             = uTextureFlag;
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
	}

	return pResource;
}

void CFlashContext3D::__ClearRenderData(RENDERDATA& RenderData)
{
	RenderData.DeviceStateBlock.uDirtyFlag = 0;

	RenderData.ProgramData.uVertexParameterCount   = 0;
	RenderData.ProgramData.uFragmentParameterCount = 0;

	RenderData.uFlag                               = 0;
}

void CFlashContext3D::__PushRenderData()
{
	LPVERTEXDATA pVertexData;
	LPVERTEXBUFFERDATA pCurrentVertexBufferData, pOldVertexBufferData;
	const CVertexDeclaration::VERTEXELEMENT* pVertexElement = m_pVertexDeclaration->GetVertexElements();
	const UINT* pOrder = m_pVertexDeclaration->GetVertexElementOrderMap();
	UINT uVertexElementCount = m_pVertexDeclaration->GetVertexElementCount(), uStreamDirtyFlag = 0, uOrder, i;
	for(i = 0; i < uVertexElementCount; ++ i)
	{
		uOrder                   = pOrder[i];

		pVertexData              = &m_CurrentRenderData.VertexData[uOrder];
		pCurrentVertexBufferData = &pVertexData->VertexBufferData;
		pOldVertexBufferData     = &m_OldRenderData.VertexData[uOrder].VertexBufferData;

		pCurrentVertexBufferData->uVertexBuffer  = m_VertexBuffer[pVertexElement->uStream];
		pCurrentVertexBufferData->uOffsetInBytes = pVertexElement->uOffset;

		pVertexData->Type                        = pVertexElement->Type;

		if(pCurrentVertexBufferData->uVertexBuffer != pOldVertexBufferData->uVertexBuffer || pCurrentVertexBufferData->uOffsetInBytes != pOldVertexBufferData->uOffsetInBytes)
			ALCHEMY_SET_BIT(m_CurrentRenderData.uFlag, VERTEX_INDEX_DIRTY_SHIFT + uOrder);

		if( ALCHEMY_TEST_BIT(m_uVertexBufferDirtyFlag, pVertexElement->uStream) )
		{
			ALCHEMY_SET_BIT(uStreamDirtyFlag, uOrder);

			ALCHEMY_CLEAR_BIT(m_uVertexBufferDirtyFlag, pVertexElement->uStream);
		}

		++ pVertexElement;
	}

	while(i < MAXINUM_VERTEX_INDEX_COUNT)
	{
		if(m_OldRenderData.VertexData[i].VertexBufferData.uVertexBuffer)
		{
			ALCHEMY_SET_BIT(m_CurrentRenderData.uFlag, VERTEX_INDEX_DIRTY_SHIFT + i);

			m_CurrentRenderData.VertexData[i].VertexBufferData.uVertexBuffer = 0;
		}

		++ i;
	}

	//ALCHEMY_CLEAR_MASK2(m_CurrentRenderData.uFlag, MAXINUM_VERTEX_INDEX_COUNT - 1, VERTEX_BUFFER_DIRTY_SHIFT);

	m_CurrentRenderData.uFlag |= uStreamDirtyFlag << VERTEX_BUFFER_DIRTY_SHIFT;

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
	return AS3_Array("PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", 
		m_pRenderDataQueue, 
		&m_uCurrentRenderDataCount, 
		m_pfProgramVertexParameter, 
		&m_uProgramVertexParameterCount, 
		m_pfProgramFragmentParameter, 
		&m_uProgramFragmentParameterCount, 
		m_puProgramTexture,
		&m_uViewportWidth,
		&m_uViewportHeight,
		&m_bIsViewportDirty);
}

void CFlashContext3D::Fill(AS3TYPE Object, AS3TYPE MethodArray)
{
	m_Object = Object;

	AS3_ArrayValue(MethodArray, "AS3ValType, AS3ValType, AS3ValType", &m_CreateResourceMethod, &m_DestroyResourceMethod, &m_RenderMethod);
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