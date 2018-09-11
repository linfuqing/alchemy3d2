#include "AlchemyFlasccStdafx.h"
#include "AlchemyFlasccContext3D.h"
#include "AlchemyFlasccGraphicsManager.h"
#include "AlchemyFlasccAGALAssembler.h"
#include "AlchemyFlasccVertexDeclaration.h"
#include "AlchemyFlasccVertexBuffer3D.h"
#include "AlchemyFlasccIndexBuffer3D.h"
#include "AlchemyFlasccTexture.h"
#include "AlchemyFlasccProgram3D.h"
#include "AlchemyFlasccEffect.h"

using namespace alchemy;

CFlasccContext3D::CViewportResetHandler::CViewportResetHandler(CFlasccContext3D& Parent) :
m_Parent(Parent)
{

}

CFlasccContext3D::CViewportResetHandler::~CViewportResetHandler()
{

}

void CFlasccContext3D::CViewportResetHandler::Handler(const CEvent& Event)
{
	const CFlasccGraphicsManager::CViewportEvent& ViewportEvent = static_cast<const CFlasccGraphicsManager::CViewportEvent&>(Event);

	m_Parent.SetViewport(ViewportEvent.GetViewportWidth(), ViewportEvent.GetViewportHeight(), m_Parent.m_nAntiAlias);
}

CFlasccContext3D::CFlasccContext3D(UINT uRenderSystemIndex) :
m_uRenderSystemIndex(uRenderSystemIndex), 
m_uTextureFlag(0),
m_uVertexBufferIndexFlag(0),
m_uRenderStateStackCount(0),
m_uRenderStateFlag(0),
m_pVertexDeclaration(ALCHEMY_NULL),
m_uViewportWidth(1),
m_uViewportHeight(1), 
m_nAntiAlias(0), 
m_ViewportResetHandler(*this)
{
	ALCHEMY_DEBUG_NEW(m_pCompiler, CFlasccAGALAssembler);

	m_RenderSystemInfo.uRenderTargetCount   = 1;
	m_RenderSystemInfo.uMaxVertexCount      = 65535;
	m_RenderSystemInfo.uMaxVertexIndexCount = 524287;

	UINT i;
	for(i = 0; i < MAXINUM_TEXTURE_COUNT; ++ i)
		m_uProgramTexture[i] = 0;

	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::FILL_MODE] = 0;

	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::SHADE_MODE] = 0;

	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::DEPTH_WRITEE_ENABLE] = 1;
	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::DEPTH_FUNC] = IRenderState::LESS;

	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::ALPHA_FUNC] = IRenderState::LESS;

	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::SRC_BLEND] = IRenderState::ONE;
	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::DEST_BLEND] = IRenderState::ZERO;

	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::ALPHA_REF] = 0;
	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::ALPHA_FUNC] = IRenderState::LESS;

	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::CULL_MODE] = IRenderState::CULL_NONE;

	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_FUNC] = IRenderState::ALWAYS;
	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_FAIL] = IRenderState::KEEP;
	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_Z_FAIL] = IRenderState::KEEP;
	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_PASS] = IRenderState::KEEP;

	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_REF] = 0;
	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_READ_MASK] = 0xff;
	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_WRITE_MASK] = 0xff;

	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::CCW_STENCIL_FAIL] = IRenderState::KEEP;
	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::CCW_STENCIL_Z_FAIL] = IRenderState::KEEP;
	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::CCW_STENCIL_PASS] = IRenderState::KEEP;
	m_CurrentRenderData.RenderStateBlock.uRenderState[IRenderState::CCW_STENCIL_FUNC] = IRenderState::ALWAYS;

	m_CurrentRenderData.RenderStateBlock.uDirtyFlag = 0;

	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::FILL_MODE] = 0;

	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::SHADE_MODE] = 0;

	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::DEPTH_WRITEE_ENABLE] = 1;
	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::DEPTH_FUNC] = IRenderState::LESS;

	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::ALPHA_FUNC] = IRenderState::LESS;

	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::SRC_BLEND] = IRenderState::ONE;
	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::DEST_BLEND] = IRenderState::ZERO;

	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::ALPHA_REF] = 0;
	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::ALPHA_FUNC] = IRenderState::LESS;

	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::CULL_MODE] = IRenderState::CULL_NONE;

	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_FUNC] = IRenderState::ALWAYS;
	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_FAIL] = IRenderState::KEEP;
	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_Z_FAIL] = IRenderState::KEEP;
	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_PASS] = IRenderState::KEEP;

	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_REF] = 0;
	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_READ_MASK] = 0xff;
	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::STENCIL_WRITE_MASK] = 0xff;

	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::CCW_STENCIL_FAIL] = IRenderState::KEEP;
	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::CCW_STENCIL_Z_FAIL] = IRenderState::KEEP;
	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::CCW_STENCIL_PASS] = IRenderState::KEEP;
	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::CCW_STENCIL_FUNC] = IRenderState::ALWAYS;

	m_OldRenderData.RenderStateBlock.uRenderState[IRenderState::COLOR_WRITE_ENABLE] = ALCHEMY_FLAG(IRenderState::ALPHA) | ALCHEMY_FLAG(IRenderState::RED) | ALCHEMY_FLAG(IRenderState::GREEN) | ALCHEMY_FLAG(IRenderState::BLUE);

	m_OldRenderData.RenderStateBlock.uDirtyFlag = 0;

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

	__ClearRenderData(m_CurrentRenderData);
	__ClearRenderData(m_OldRenderData    );
}

CFlasccContext3D::~CFlasccContext3D(void)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pCompiler);
}

bool CFlasccContext3D::CheckFormat(FORMATTYPE Type, UINT uFormat)
{
	switch(Type)
	{
	case CRenderSystem::TEXTURE:
		switch(uFormat)
		{
		case CTexture::DEFAULT:
			return true;
			break;
		case CTexture::FLOAT1:
			return false;
			break;
		case CTexture::FLOAT4:
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

ICompiler& CFlasccContext3D::GetCompiler()
{
	return *m_pCompiler;
}

bool CFlasccContext3D::GetViewport(PUINT puWidth, PUINT puHeight)
{
	if(puWidth)
		*puWidth = m_uViewportWidth;

	if(puHeight)
		*puHeight = m_uViewportHeight;

	return true;
}

bool CFlasccContext3D::Clear(ARGBCOLOR Color, UINT uFlag)
{
	if(uFlag)
	{
#ifdef AS3_H
		inline_as3(
			"import alchemy.core.AlchemyGraphicsManager;\n"
			"import alchemy.core.AlchemyRenderSystem;\n"
			"var renderSystem = AlchemyGraphicsManager.instance.getRenderSystem(%0);\n"
			"if(renderSystem) renderSystem.clear(%1, %2, %3, %4);\n"
			: : "r"(m_uRenderSystemIndex), "r"(Color), "r"(uFlag), "r"(m_CurrentRenderData.uRenderTarget), "r"(m_CurrentRenderData.uRenderSurface)
			);
#endif
		ALCHEMY_CLEAR_BIT(m_CurrentRenderData.uFlag, RENDER_TARGET_DIRTY);
	}

	return true;
}

bool CFlasccContext3D::ResetTexture()
{
	if(m_uTextureFlag)
	{
		UINT uBegin = MininumBit(m_uTextureFlag) - 1, uEnd = MaxinumBit(m_uTextureFlag);
		for(UINT i = uBegin; i < uEnd; ++ i)
		{
			if( ALCHEMY_TEST_BIT(m_uTextureFlag, i) )
				m_uProgramTexture[i] = 0;
		}

		m_CurrentRenderData.uFlag |= m_uTextureFlag << TEXTURE_DIRTY_SHIFT;

		m_uTextureFlag = 0;
	}

	return true;
}

bool CFlasccContext3D::RegisterRenderState()
{
	m_uRenderStateFlag = 0;

	m_uRenderStateStackCount = 0;

	return true;
}

bool CFlasccContext3D::RestoreRenderState()
{
	IRenderState::RENDERSTATE State;
	UINT uValue;
	for(UINT i = 0; i < m_uRenderStateStackCount; ++ i)
	{
		State  = m_RenderStateStack[i];
		uValue = m_RenderStateValueStack[i];

		m_CurrentRenderData.RenderStateBlock.uRenderState[State] = uValue;

		ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.RenderStateBlock.uDirtyFlag, State, uValue != m_OldRenderData.RenderStateBlock.uRenderState[State]);
	}

	m_uRenderStateFlag = 0;

	m_uRenderStateStackCount = 0;

	return true;
}

bool CFlasccContext3D::SetRenderState(const RENDERSTATEINFO* pRenderStateInfos, UINT uRenderStateInfoCount, bool bIsRegister)
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

				m_RenderStateValueStack[m_uRenderStateStackCount] = m_CurrentRenderData.RenderStateBlock.uRenderState[pRenderStateInfo->State];

				ALCHEMY_SET_BIT(m_uRenderStateFlag, pRenderStateInfo->State);

				++ m_uRenderStateStackCount;
			}

			m_CurrentRenderData.RenderStateBlock.uRenderState[pRenderStateInfo->State] = pRenderStateInfo->uValue;

			ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.RenderStateBlock.uDirtyFlag, pRenderStateInfo->State, pRenderStateInfo->uValue != m_OldRenderData.RenderStateBlock.uRenderState[pRenderStateInfo->State]);
		}
		else
		{
			ALCHEMY_DEBUG_WARNING("CFlasccContext3D::SetRenderState: Error state.");

			return false;
		}
	}

	return true;
}

bool CFlasccContext3D::SetRenderTarget(CGraphicsResource::HANDLE Texture, UINT uSurface, UINT uIndex)
{
	if(!uIndex)
	{
		m_CurrentRenderData.uRenderTarget = Texture;

		m_CurrentRenderData.uRenderSurface = uSurface;

		ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.uFlag, RENDER_TARGET_DIRTY, Texture != m_OldRenderData.uRenderTarget || uSurface != m_OldRenderData.uRenderSurface);

		return true;
	}

	return false;
}

bool CFlasccContext3D::SetVertexDeclaration(CGraphicsResource::HANDLE VertexDeclaration)
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

bool CFlasccContext3D::SetStreamSource(CGraphicsResource::HANDLE VertexBuffer, UINT uStream)
{
	m_VertexBuffer[uStream] = VertexBuffer;

	return true;
}

bool CFlasccContext3D::SetIndices(CGraphicsResource::HANDLE IndexBuffer)
{
	m_CurrentRenderData.uIndexBuffer = IndexBuffer;

	ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.uFlag, INDEX_BUFFER_DIRTY, IndexBuffer != m_OldRenderData.uIndexBuffer);

	return true;
}

bool CFlasccContext3D::SetShader(CGraphicsResource::HANDLE Shader)
{
	m_CurrentRenderData.ProgramData.uProgram = Shader;

	ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.uFlag, PROGRAM_DIRTY, Shader != m_OldRenderData.ProgramData.uProgram);

	return true;
}

bool CFlasccContext3D::SetShaderParameters(IEffect::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uCount)
{
	UINT i, uCurrentIndex;

	if( ALCHEMY_TEST_FLAG(Type, IEffect::VERTEX_SHADER) )
	{
		uCurrentIndex = uIndex;

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset, uCurrentIndex);

		for(i = 0; i < uCount; ++ i)
			m_fProgramVertexParameter[uCurrentIndex ++] = pfValues[i];

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount, uCurrentIndex);
	}

	if( ALCHEMY_TEST_FLAG(Type, IEffect::FRAGMENT_SHADER) )
	{
		uCurrentIndex = uIndex;

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset, uCurrentIndex);

		for(i = 0; i < uCount; ++ i)
			m_fProgramFragmentParameter[uCurrentIndex ++] = pfValues[i];

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount, uCurrentIndex);
	}

	return true;
}

bool CFlasccContext3D::SetShaderMatrixArray(IEffect::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount)
{
	UINT i, j, uValueIndex, uCurrentIndex, uCurrentOffset = ALCHEMY_MININUM(uRow * uColumn, uOffset), uOffsetStep = uOffset - uCurrentOffset;

	if( ALCHEMY_TEST_FLAG(Type, IEffect::VERTEX_SHADER) )
	{
		uCurrentIndex = uIndex;

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset, uCurrentIndex);

		uValueIndex = 0;
		for(i = 0; i < uCount; ++ i)
		{
			for(j = 0; j < uCurrentOffset; ++ j)
				m_fProgramVertexParameter[uCurrentIndex ++] = pfValues[uValueIndex ++];

			uValueIndex += uOffsetStep;
		}

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount, uCurrentIndex);
	}

	if( ALCHEMY_TEST_FLAG(Type, IEffect::FRAGMENT_SHADER) )
	{
		uCurrentIndex = uIndex;

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset, uCurrentIndex);

		uValueIndex = 0;
		for(i = 0; i < uCount; ++ i)
		{
			for(j = 0; j < uCurrentOffset; ++ j)
				m_fProgramFragmentParameter[uCurrentIndex ++] = pfValues[uValueIndex ++];

			uValueIndex += uOffsetStep;
		}

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount, uCurrentIndex);
	}

	return true;
}

bool CFlasccContext3D::SetShaderMatrixPointerArray(IEffect::EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount)
{
	UINT i, j, uCurrentIndex, uOffset = uRow * uColumn;

	PFLOAT pfValues;

	if( ALCHEMY_TEST_FLAG(Type, IEffect::VERTEX_SHADER) )
	{
		uCurrentIndex = uIndex;

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferOffset, uCurrentIndex);

		for(i = 0; i < uCount; ++ i)
		{
			pfValues = ppfValues[i];
			if(pfValues)
			{
				for(j = 0; j < uOffset; ++ j)
					m_fProgramVertexParameter[uCurrentIndex ++] = pfValues[j];
			}
		}

		m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyVertexParameterBufferCount, uCurrentIndex);
	}

	if( ALCHEMY_TEST_FLAG(Type, IEffect::FRAGMENT_SHADER) )
	{
		uCurrentIndex = uIndex;

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset = ALCHEMY_MININUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferOffset, uCurrentIndex);

		for(i = 0; i < uCount; ++ i)
		{
			pfValues = ppfValues[i];
			if(pfValues)
			{
				for(j = 0; j < uOffset; ++ j)
					m_fProgramFragmentParameter[uCurrentIndex ++] = pfValues[j];
			}
		}

		m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount = ALCHEMY_MAXINUM(m_CurrentRenderData.ProgramData.uDirtyFragmentParameterBufferCount, uCurrentIndex);
	}

	return true;
}

bool CFlasccContext3D::SetShaderConstants(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE ConstantBuffer, UINT uIndex)
{
	return false;
}

bool CFlasccContext3D::SetTexture(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE Texture, UINT uIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration)
{
	if(uIndex >= MAXINUM_TEXTURE_COUNT)
	{
		ALCHEMY_DEBUG_WARNING("CFlasccContext3D::SetTexture: Index out of range.");

		return false;
	}

	if(Texture)
		ALCHEMY_SET_BIT(m_uTextureFlag, uIndex);
	else
		ALCHEMY_CLEAR_BIT(m_uTextureFlag, uIndex);

	if( ALCHEMY_TEST_BIT(m_OldRenderData.uFlag, TEXTURE_DIRTY_SHIFT + uIndex) )
		ALCHEMY_SET_BIT_VALUE(m_CurrentRenderData.uFlag, TEXTURE_DIRTY_SHIFT + uIndex, m_uProgramTexture[uIndex] != Texture);
	else
		ALCHEMY_SET_BIT(m_CurrentRenderData.uFlag, TEXTURE_DIRTY_SHIFT + uIndex);

	m_uProgramTexture[uIndex] = Texture;

	return true;
}

bool CFlasccContext3D::DrawIndexedTriangles(CRenderSystem::TRIANGLETYPE Type, UINT uStartIndex, UINT uTriangleCount)
{
	if(!m_pVertexDeclaration)
	{
		ALCHEMY_DEBUG_WARNING("CFlasccContext3D::DrawIndexedTriangles: No vertex declaration");

		return false;
	}

	m_CurrentRenderData.uStart         = uStartIndex;
	m_CurrentRenderData.uTriangleCount = uTriangleCount;

	LPVERTEXDATA pVertexData;
	LPVERTEXBUFFERDATA pCurrentVertexBufferData, pOldVertexBufferData;
	const CVertexDeclaration::VERTEXELEMENT* pVertexElement = m_pVertexDeclaration->GetVertexElements();
	UINT uVertexElementCount = m_pVertexDeclaration->GetVertexElementCount(), uStreamDirtyFlag = 0, uVertexBufferIndexFlag = 0, i;
	for(i = 0; i < uVertexElementCount; ++ i)
	{
		ALCHEMY_SET_BIT(uVertexBufferIndexFlag, pVertexElement->uIndex);

		pVertexData              = &m_CurrentRenderData.VertexData[pVertexElement->uIndex];
		pCurrentVertexBufferData = &pVertexData->VertexBufferData;
		pOldVertexBufferData     = &m_OldRenderData.VertexData[pVertexElement->uIndex].VertexBufferData;

		pCurrentVertexBufferData->uVertexBuffer  = m_VertexBuffer[pVertexElement->uStream];
		pCurrentVertexBufferData->uOffsetInBytes = pVertexElement->uOffset;

		pVertexData->Type                        = pVertexElement->Type;

		if(pCurrentVertexBufferData->uVertexBuffer != pOldVertexBufferData->uVertexBuffer || pCurrentVertexBufferData->uOffsetInBytes != pOldVertexBufferData->uOffsetInBytes)
			ALCHEMY_SET_BIT(m_CurrentRenderData.uFlag, VERTEX_INDEX_DIRTY_SHIFT + pVertexElement->uIndex);

		++ pVertexElement;
	}

	ALCHEMY_CLEAR_FLAG(m_uVertexBufferIndexFlag, uVertexBufferIndexFlag);

	UINT uEnd;
	if(m_uVertexBufferIndexFlag)
	{
		uEnd = MaxinumBit(m_uVertexBufferIndexFlag);
		for(i = MininumBit(m_uVertexBufferIndexFlag) - 1; i < uEnd; ++ i)
		{
			if( ALCHEMY_TEST_BIT(m_uVertexBufferIndexFlag, i) )
			{
				ALCHEMY_SET_BIT(m_CurrentRenderData.uFlag, VERTEX_INDEX_DIRTY_SHIFT + i);

				m_CurrentRenderData.VertexData[i].VertexBufferData.uVertexBuffer = 0;
			}
		}
	}

	m_uVertexBufferIndexFlag = uVertexBufferIndexFlag;

#ifdef AS3_H
	inline_as3(
		"import alchemy.core.AlchemyGraphicsManager;\n"
		"import alchemy.core.AlchemyRenderSystem;\n"
		"var renderSystem = AlchemyGraphicsManager.instance.getRenderSystem(%0);\n"
		"if(renderSystem) renderSystem.draw(%1, ram, %2, %3, %4);\n"
		: : "r"(m_uRenderSystemIndex), "r"(&m_CurrentRenderData), "r"(m_fProgramVertexParameter), "r"(m_fProgramFragmentParameter), "r"(m_uProgramTexture)
		);
#endif

	m_OldRenderData = m_CurrentRenderData;

	__ClearRenderData(m_CurrentRenderData);

	return true;
}

bool CFlasccContext3D::Present()
{
#ifdef AS3_H
	inline_as3(
		"import alchemy.core.AlchemyGraphicsManager;\n"
		"import alchemy.core.AlchemyRenderSystem;\n"
		"var renderSystem = AlchemyGraphicsManager.instance.getRenderSystem(%0);\n"
		"if(renderSystem) renderSystem.present();\n"
		: : "r"(m_uRenderSystemIndex)
		);
#endif

	return true;
}

void CFlasccContext3D::SetViewport(UINT uWidth, UINT uHeight, INT nAntiAlias)
{
	m_uViewportWidth = uWidth;
	m_uViewportHeight = uHeight;

	m_nAntiAlias = nAntiAlias;

#ifdef AS3_H
	inline_as3(
		"import alchemy.core.AlchemyGraphicsManager;\n"
		"import alchemy.core.AlchemyRenderSystem;\n"
		"var renderSystem = AlchemyGraphicsManager.instance.getRenderSystem(%0);\n"
		"if(renderSystem) renderSystem.configureBackBuffer(%1, %2, %3);\n"
		: : "r"(m_uRenderSystemIndex), "r"(uWidth), "r"(uHeight), "r"(nAntiAlias)
		);
#endif

	UINT uEventType;
	if( ALCHEMY_GET_EVENT_TYPE(uEventType, RESET_VIEWPORT, CRenderSystem) )
	{
		CEvent Event(uEventType);
		DispatchEvent(Event);
	}
}

bool CFlasccContext3D::AddEventListenerForViewportReset()
{
	UINT uEventType;
	if( ALCHEMY_GET_EVENT_TYPE(uEventType, RESET_VIEWPORT, CFlasccGraphicsManager) )
	{
		ALCHEMY_GRAPHICS_MANAGER.AddEventListener(uEventType, m_ViewportResetHandler);

		return true;
	}

	return false;
}

CGraphicsResource* CFlasccContext3D::_Create(CGraphicsResource::TYPE Type)
{
	CGraphicsResource* pResource = ALCHEMY_NULL;

	switch(Type)
	{
	case CGraphicsResource::VERTEX_DECLARATION:
		ALCHEMY_DEBUG_NEW(pResource, CFlasccVertexDeclaration);
		break;
	case CGraphicsResource::VERTEX_BUFFER:
		ALCHEMY_DEBUG_NEW(pResource, CFlasccVertexBuffer3D);
		break;
	case CGraphicsResource::INDEX_BUFFER:
		ALCHEMY_DEBUG_NEW(pResource, CFlasccIndexBuffer3D);
		break;
	case CGraphicsResource::SHADER:
		ALCHEMY_DEBUG_NEW(pResource, CFlasccProgram3D);
		break;
	case CGraphicsResource::EFFECT:
		ALCHEMY_DEBUG_NEW(pResource, CFlasccEffect);
		break;
	case CGraphicsResource::TEXTURE:
		ALCHEMY_DEBUG_NEW(pResource, CFlasccTexture);
		break;
	default:
		break;
	}

	return pResource;
}

bool CFlasccContext3D::_RemoveResource(CGraphicsResource::HANDLE Handle, CGraphicsResource::TYPE Type)
{
#ifdef AS3_H
	UINT uHandle = Handle;
	INT nType = Type;

	inline_as3(
		"import alchemy.core.AlchemyGraphicsManager;\n"
		"import alchemy.core.AlchemyRenderSystem;\n"
		"var renderSystem = AlchemyGraphicsManager.instance.getRenderSystem(%0);\n"
		"if(renderSystem) renderSystem.removeResource(%1, %2);\n"
		: : "r"(m_uRenderSystemIndex), "r"(uHandle), "r"(nType)
		);
#endif
	
	return CGraphicsResourcePool::_RemoveResource(Handle, Type);
}

void CFlasccContext3D::__ClearRenderData(RENDERDATA& RenderData)
{
	RenderData.RenderStateBlock.uDirtyFlag                     = 0;

	RenderData.ProgramData.uDirtyVertexParameterBufferOffset   = ~0;
	RenderData.ProgramData.uDirtyFragmentParameterBufferOffset = ~0;

	RenderData.ProgramData.uDirtyVertexParameterBufferCount    = 0;
	RenderData.ProgramData.uDirtyFragmentParameterBufferCount  = 0;

	RenderData.uFlag                                           = 0;
}