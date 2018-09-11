#include "AlchemyRenderSystem.h"
#include "AlchemyGraphicsResourcePool.h"

#include "../Core/AlchemyUtils.h"

using namespace alchemy;

CRenderSystem::CRenderTargetViewListener::CRenderTargetViewListener(CGraphicsBuffer<CTexture>& Parent, INT nShift) :
m_Parent(Parent),
m_nShift(nShift)
{

}

CRenderSystem::CRenderTargetViewListener::~CRenderTargetViewListener()
{

}

void CRenderSystem::CRenderTargetViewListener::Handler(const IEventDispatcher::CEvent& Event)
{
	CRenderSystem::__CreateRenderTargetView(m_Parent, m_nShift);
}

CRenderSystem::CRenderSystem(void) :
CGraphicsResourcePool(CGraphicsResource::BEGIN_RENDER_RESOURCE, CGraphicsResource::END_RENDER_RESOURCE)
{
}

CRenderSystem::~CRenderSystem(void)
{
	Destroy();
}

CVertexDeclaration* CRenderSystem::CreateVertexDeclaration(const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount)
{
	CVertexDeclaration* pOutput = static_cast<CVertexDeclaration*>( CreateResource(CGraphicsResource::VERTEX_DECLARATION) );

	if(pOutput)
	{
		if(pVertexElements)
		{
			ALCHEMY_DEBUG_NEW(pOutput->m_pVertexElements, CVertexDeclaration::VERTEXELEMENT[uElementCount]);

			CVertexDeclaration::SortVertexElement(pOutput->m_pVertexElements, pVertexElements, uElementCount);

			pOutput->m_uElementCount = uElementCount;

			pOutput->m_uStreamCount  = CVertexDeclaration::GetVertexElementStreamCount(pVertexElements, uElementCount);

			ALCHEMY_DEBUG_NEW(pOutput->m_puElementEndOfStream, UINT[pOutput->m_uStreamCount]);

			//ALCHEMY_DEBUG_NEW(pOutput->m_puVertexElementOrderMap, UINT[pOutput->m_uElementCount]);

			CVertexDeclaration::LPVERTEXELEMENT pVertexElementTemp = &pOutput->m_pVertexElements[0];
			//pOutput->m_puVertexElementOrderMap[0] = CVertexDeclaration::FindVertexElement(pVertexElementTemp->Usage, pVertexElementTemp->Type, pVertexElementTemp->uOffset, pVertexElementTemp->uStream, pVertexElements, uElementCount);

			UINT uIndex = 0;
			//pOutput->m_puStreamSize[0] = uElementCount;
			for(UINT i = 1; i < uElementCount; ++ i)
			{
				pVertexElementTemp = &pOutput->m_pVertexElements[i];

				if(pOutput->m_pVertexElements[i - 1].uStream != pVertexElementTemp->uStream)
				{
					pOutput->m_puElementEndOfStream[uIndex] = i - 1;

					++ uIndex;
				}

				//pOutput->m_puVertexElementOrderMap[i] = CVertexDeclaration::FindVertexElement(pVertexElementTemp->Usage, pVertexElementTemp->Type, pVertexElementTemp->uOffset, pVertexElementTemp->uStream, pVertexElements, uElementCount);
			}

			pOutput->m_puElementEndOfStream[uIndex] = uElementCount - 1;
		}

		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

LPVERTEXBUFFER CRenderSystem::CreateVertexBuffer(UINT uCount, UINT uStride, UINT uUsage, const void* pData)
{
	LPVERTEXBUFFER pOutput = static_cast<LPVERTEXBUFFER>( CreateResource(CGraphicsResource::VERTEX_BUFFER) );
	if(pOutput)
	{
		pOutput->m_uMemberCount = uCount;
		pOutput->m_uStride      = uStride;
		pOutput->m_uUsage       = uUsage;
		pOutput->m_uByteSize    = uCount * uStride;

		if( pOutput->_Create() )
		{
			if(pData)
			{
				void* pVertices = pOutput->Lock(0);
				if(pVertices)
				{
					ALCHEMY_MEMORY_COPY(pVertices, pData, pOutput->m_uByteSize);

					pOutput->Unlock();
				}
			}

			return pOutput;
		}

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

LPINDEXBUFFER CRenderSystem::CreateIndexBuffer(UINT uCount, UINT uUsage, CIndexBuffer::FORMAT Format, const void* pData)
{
	LPINDEXBUFFER pOutput = static_cast<LPINDEXBUFFER>( CreateResource(CGraphicsResource::INDEX_BUFFER) ) ;
	if(pOutput)
	{
		pOutput->m_uMemberCount  = uCount;
		pOutput->m_uUsage        = uUsage;
		pOutput->m_Format        = Format;
		pOutput->m_uStride       = 1 << Format;
		pOutput->m_uByteSize     = uCount << Format;

		if( pOutput->_Create() )
		{
			if(pData)
			{
				void* pIndices = pOutput->Lock(0);
				if(pIndices)
				{
					ALCHEMY_MEMORY_COPY(pIndices, pData, pOutput->m_uByteSize);

					pOutput->Unlock();
				}
			}

			return pOutput;
		}

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}


CShader* CRenderSystem::CreateShader(IEffect::EFFECTTYPE Type, ICompiler& Compiler, const CHAR* pcCode, const CHAR* pcAdditionalCode)
{
	CShader* pOutput = static_cast<CShader*>( CreateResource(CGraphicsResource::SHADER) );
	if( pOutput)
	{
		pOutput->m_Type  = Type;

		if(pcAdditionalCode)
		{
			Compiler.ProgramString(&pOutput->m_pCode          , IEffect::VERTEX_SHADER  , pcCode          );
			Compiler.ProgramString(&pOutput->m_pAdditionalCode, IEffect::FRAGMENT_SHADER, pcAdditionalCode);
		}
		else
		{
			Compiler.ProgramString(&pOutput->m_pCode, Type, pcCode);

			pOutput->m_pAdditionalCode = ALCHEMY_NULL;
		}

		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CEffect* CRenderSystem::CreateEffect(UINT uPassCount, const UINT* puRenderTargetIndexCount, const CHAR** ppcCodes, const CHAR** ppcAdditionalCodes, const CEffect::PASSINFO* pPassInfos)
{
	if(!puRenderTargetIndexCount || !ppcCodes || !ppcAdditionalCodes || !pPassInfos)
		return ALCHEMY_NULL;

	CEffect* pOutput = static_cast<CEffect*>( CreateResource(CGraphicsResource::EFFECT) );
	if(pOutput)
	{
		UINT i, uOffset = 0;
		ALCHEMY_DEBUG_NEW(pOutput->m_puRenderTargetIndexCount , UINT[uPassCount]);
		ALCHEMY_DEBUG_NEW(pOutput->m_puRenderTargetIndexOffset, UINT[uPassCount]);
		for(i = 0; i < uPassCount; ++ i)
		{
			pOutput->m_puRenderTargetIndexCount[i]  = puRenderTargetIndexCount[i];
			pOutput->m_puRenderTargetIndexOffset[i] = uOffset;

			uOffset += puRenderTargetIndexCount[i];
		}

		ALCHEMY_DEBUG_NEW(pOutput->m_pCodes          , STRING[uOffset]);
		ALCHEMY_DEBUG_NEW(pOutput->m_pAdditionalCodes, STRING[uOffset]);

		for(i = 0; i < uOffset; ++ i)
		{
			pOutput->m_pCodes[i]           = ppcCodes[i];
			pOutput->m_pAdditionalCodes[i] = ppcAdditionalCodes[i];
		}

		pOutput->m_pPassInfoBuffer = &CEffect::CreatePassInfoBuffer(pPassInfos, uOffset);
		pOutput->m_pPassInfos       = reinterpret_cast<CEffect::LPPASSINFO>( pOutput->m_pPassInfoBuffer->GetBufferPointer() );

		pOutput->m_uPassCount       = uPassCount;

		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CEffect* CRenderSystem::CreateEffect(CEffectCode& EffectCode)
{
	CEffect* pOutput = static_cast<CEffect*>( CreateResource(CGraphicsResource::EFFECT) );
	if(pOutput)
	{
		pOutput->m_uPassCount = EffectCode.GetPassCount();
		ALCHEMY_DEBUG_NEW(pOutput->m_puRenderTargetIndexCount, UINT[pOutput->m_uPassCount]);
		pOutput->m_pPassInfoBuffer = &EffectCode.CreatePassInfos(pOutput->m_puRenderTargetIndexCount);
		pOutput->m_pPassInfos      = reinterpret_cast<CEffect::LPPASSINFO>( pOutput->m_pPassInfoBuffer->GetBufferPointer() );
		ALCHEMY_DEBUG_NEW(pOutput->m_puRenderTargetIndexOffset, UINT[pOutput->m_uPassCount]);
		UINT uOffset = 0, i;
		for(i = 0; i < pOutput->m_uPassCount; ++ i)
		{
			pOutput->m_puRenderTargetIndexOffset[i] = uOffset;

			uOffset += pOutput->m_puRenderTargetIndexCount[i];
		}

		ALCHEMY_DEBUG_NEW(pOutput->m_pCodes          , STRING[uOffset]);
		ALCHEMY_DEBUG_NEW(pOutput->m_pAdditionalCodes, STRING[uOffset]);

		ICompiler& Compiler = EffectCode.GetCompiler();
		LPSTRING pVertexShaderCode = pOutput->m_pCodes, pFragmentShaderCode = pOutput->m_pAdditionalCodes;
		const UINT* pRenderTargetInstructionSet;
		const CEffectCode::PASS* pPass;
		const CEffectCode::RENDERTARGETINSTRUCTION* pRenderTargetInstruction;
		for(i = 0; i < pOutput->m_uPassCount; ++ i)
		{
			pPass = EffectCode.GetPass(i + 1);

			if(pPass)
			{
				pRenderTargetInstructionSet = EffectCode.GetRenderTargetInstructionSet(pPass->uRenderTargetInstructionSet);
				if(pRenderTargetInstructionSet)
				{
					pRenderTargetInstruction = EffectCode.GetRenderTargetInstruction(*pRenderTargetInstructionSet);
					while(pRenderTargetInstruction)
					{
						if(!Compiler.BuildShaderFunctionCode(*(pVertexShaderCode ++), IEffect::VERTEX_SHADER, pRenderTargetInstruction->uVertexInstructionIndices, pRenderTargetInstruction->uVertexUserVariables, EffectCode) ||
							!Compiler.BuildShaderFunctionCode(*(pFragmentShaderCode ++), IEffect::FRAGMENT_SHADER, pRenderTargetInstruction->uFragmentInstructionIndices, pRenderTargetInstruction->uFragmentUserVariables, EffectCode) )
						{
							ALCHEMY_DEBUG_WARNING("CRenderSystem::CreateEffect: Code has been created fail.");
						}

						pRenderTargetInstruction = EffectCode.GetRenderTargetInstruction(pRenderTargetInstruction->uNext);
					}
				}
			}
		}

		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

LPTEXTURE CRenderSystem::CreateTexture(UINT uWidth, UINT uHeight, UINT uUsage, CTexture::FORMAT Format, UINT uMipLevel, const void* pData)
{
	if(!uWidth || !uHeight)
		return ALCHEMY_NULL;

	LPTEXTURE pOutput = static_cast<LPTEXTURE>( CreateResource(CGraphicsResource::TEXTURE) );
	if(pOutput)
	{
		pOutput->m_TextureType   = CTexture::NORMAL;

		pOutput->m_uUsage        = uUsage;
		pOutput->m_Format        = Format;
		pOutput->m_uStride       = CTexture::GetFormatSize(Format);

		pOutput->m_uWidth        = uWidth;
		pOutput->m_uHeight       = uHeight;
		pOutput->m_uDepth        = 1;

		pOutput->m_uWidthShift   = ALCHEMY_SHIFT(uWidth);
		pOutput->m_uHeightShift  = ALCHEMY_SHIFT(uHeight);
		pOutput->m_uDepthShift = 0;

        pOutput->m_uMipLevel     = ALCHEMY_MAXINUM(pOutput->m_uWidthShift, pOutput->m_uHeightShift) + 1;
		pOutput->m_uMipLevel     = ALCHEMY_MININUM(static_cast<UINT>( uMipLevel ? uMipLevel : ALCHEMY_TOTAL_BITS(UINT) ), pOutput->m_uMipLevel);

		pOutput->m_uMemberCount = 0;
		for(UINT i = 0; i < pOutput->m_uMipLevel; ++ i)
			pOutput->m_uMemberCount += ALCHEMY_FLAG(pOutput->m_uWidthShift - i) * ALCHEMY_FLAG(pOutput->m_uHeightShift - i);

		pOutput->m_uByteSize = pOutput->m_uMemberCount * pOutput->m_uStride;

		if( pOutput->_Create() )
		{
			if(pData)
			{
				if(pOutput->m_uMipLevel == 1 && ALCHEMY_FLAG(pOutput->m_uWidthShift) == uWidth && ALCHEMY_FLAG(pOutput->m_uHeightShift) == uHeight)
				{
					UINT uBasicPitch, uPitch;
					void* pSurfaceData;
					const UINT8* puData = static_cast<const UINT8*>(pData);

					pSurfaceData = pOutput->Lock(0, 0, &uPitch, ALCHEMY_NULL);

					if(pSurfaceData)
					{
						uBasicPitch = uWidth * pOutput->m_uStride;
						if(uBasicPitch == uPitch)
							ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uBasicPitch * uHeight);
						else
						{
							UINT uOffset = 0;
							for(UINT i = 0; i < uHeight; ++ i)
							{
								ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uBasicPitch);

								uOffset += uPitch;
							}
						}

						pOutput->Unlock();
					}
				}
				else
				{
					UINT i, j, uSourceOffset, uOutputOffset, uOutputPitch, uOutputWidth, uOutputHeight, uSourcePitch = uWidth * pOutput->m_uStride, uSourceWidth = uWidth, uSourceHeight = uHeight;
					void* pOutputData;
					PVOID pSourceData;
					ALCHEMY_DEBUG_NEW(pSourceData, UINT8[ALCHEMY_FLAG(pOutput->m_uWidthShift) * ALCHEMY_FLAG(pOutput->m_uHeightShift) * pOutput->m_uStride]);
					ALCHEMY_MEMORY_COPY(pSourceData, pData, uWidth * uHeight * pOutput->m_uStride);
					for(i = 0; i < pOutput->m_uMipLevel; ++ i)
					{
						pOutputData = pOutput->Lock(i, 0, &uOutputPitch, ALCHEMY_NULL);
						if(pOutputData)
						{
							CTexture::BilerpBlends(
								pOutputData, 
								uOutputPitch, 
								uOutputWidth = pOutput->m_uWidthShift > i ? ALCHEMY_FLAG(pOutput->m_uWidthShift - i) : 1, 
								uOutputHeight = pOutput->m_uHeightShift > i ? ALCHEMY_FLAG(pOutput->m_uHeightShift - i) : 1, 
								pSourceData, 
								uSourcePitch, 
								uSourceWidth, 
								uSourceHeight, 
								2,
								2, 
								1.0f,
								Format);

							uSourcePitch = uOutputWidth * pOutput->m_uStride;
							if(uSourcePitch == uOutputPitch)
								ALCHEMY_MEMORY_COPY(pSourceData, pOutputData, uSourcePitch * uOutputHeight);
							else
							{
								uSourceOffset = 0;
								uOutputOffset = 0;
								for(j = 0; j < uOutputHeight; ++ j)
								{
									ALCHEMY_MEMORY_COPY(reinterpret_cast<PUINT8>(pSourceData) + uSourceOffset, reinterpret_cast<const UINT8*>(pOutputData) + uOutputOffset, uSourcePitch);

									uOutputOffset += uOutputPitch;
									uSourceOffset += uSourcePitch;
								}
							}

							uSourceWidth  = uOutputWidth;
							uSourceHeight = uOutputHeight;

							pOutput->Unlock();
						}
					}
					ALCHEMY_DEBUG_DELETE_ARRAY(pSourceData);
				}
			}

			return pOutput;
		}

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

LPTEXTURE CRenderSystem::CreateRenderTargetView(CTexture::FORMAT Format, INT nShift)
{
	UINT uType;
	if( ALCHEMY_GET_EVENT_TYPE(uType, RESET_VIEWPORT, CRenderSystem) )
	{
		UINT uWidth, uHeight;
		GetViewport(&uWidth, &uHeight);

		if(nShift)
		{
			uWidth  = ALCHEMY_FLAG(ALCHEMY_SHIFT(uWidth ) + nShift);
			uHeight = ALCHEMY_FLAG(ALCHEMY_SHIFT(uHeight) + nShift);
		}

		LPTEXTURE pTexture = CreateTexture(uWidth, uHeight, ALCHEMY_FLAG(CTexture::RENDER_TARGET), Format, 1);
		if(pTexture)
		{
			ALCHEMY_DEBUG_NEW( pTexture->m_pRenderTargetViewListener, CRenderTargetViewListener(*pTexture, nShift) );

			AddEventListener(uType, *pTexture->m_pRenderTargetViewListener);

			return pTexture;
		}

		ALCHEMY_DEBUG_WARNING("CRenderSystem::CreateRenderTargetView: Texture has been created fail.");
	}
	else
		ALCHEMY_DEBUG_WARNING("CRenderSystem::CreateRenderTargetView: Event type has been get fail.");

	return ALCHEMY_NULL;
}

//LPTEXTURE CRenderSystem::CreateTexture(CTexture::ITextureInfo& Info, UINT uUsage, CTexture::FORMAT Format, UINT uMipLevel, const void* pData)
//{
//	UINT uWidth, uHeight;
//
//	Info.GetSize(&uWidth, &uHeight, ALCHEMY_NULL);
//
//	if(!uWidth || !uHeight)
//		return ALCHEMY_NULL;
//
//	LPTEXTURE pOutput = ALCHEMY_NULL;
//	if( pOutput = static_cast<LPTEXTURE>( CreateResource(CGraphicsResource::TEXTURE) ) )
//	{
//		pOutput->m_Type          = CTexture::NORMAL;
//
//		pOutput->m_uUsage        = uUsage;
//		pOutput->m_Format        = Format;
//		pOutput->m_uStride       = CTexture::GetFormatSize(Format);
//
//		pOutput->m_uWidthShift = Shift(uWidth);
//		pOutput->m_uHeightShift = Shift(uHeight);
//		pOutput->m_uDepthShift = 0;
//
//		pOutput->m_uMipLevel     = ALCHEMY_MININUM( uMipLevel ? uMipLevel : ALCHEMY_TOTAL_BITS(UINT), ALCHEMY_MININUM(pOutput->m_uWidthShift, pOutput->m_uHeightShift) );
//
//		pOutput->m_uMemberCount = 0;
//		for(UINT i = 0; i < pOutput->m_uMipLevel; ++ i)
//			pOutput->m_uMemberCount += ( 1 << (pOutput->m_uWidthShift - i) ) * ( 1 << (pOutput->m_uHeightShift - i) );
//
//		pOutput->m_uByteSize = pOutput->m_uMemberCount * pOutput->m_uStride;
//
//		pOutput->m_pTextureInfo = &Info;
//
//		if( pOutput->_Create() )
//		{
//			if(pData)
//			{
//				UINT uMipMapSize, uWidth, uPitch;
//				PVOID pSurfaceData;
//				const UINT8* puData = static_cast<const UINT8*>(pData);
//
//				uWidth = 1 << pOutput->m_uWidthShift;
//				uMipMapSize = uWidth * (1 << pOutput->m_uHeightShift) * pOutput->m_uStride;
//
//				pSurfaceData = pOutput->Lock(0, 0, &uPitch, ALCHEMY_NULL);
//				if(pSurfaceData)
//				{
//					if(uWidth == uPitch)
//						ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
//					else
//					{
//						UINT uOffset = 0;
//						for(UINT i = 0; i < uHeight; ++ i)
//						{
//							ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uWidth);
//
//							uOffset += uPitch;
//						}
//					}
//
//					pOutput->Unlock();
//				}
//			}
//
//			return pOutput;
//		}
//
//		DestroyResource(*pOutput);
//	}
//
//	return ALCHEMY_NULL;
//}

LPTEXTURE CRenderSystem::CreateCubeTexture(
	UINT uSize, 
	UINT uUsage, 
	CTexture::FORMAT Format, 
	UINT uMipLevel, 
	const void* pPositiveXData,					  
	const void* pNegativeXData,				   
	const void* pPositiveYData,				  
	const void* pNegativeYData,				  
	const void* pPositiveZData,			  
	const void* pNegativeZData)
{
	if(!uSize)
		return ALCHEMY_NULL;

	LPTEXTURE pOutput = static_cast<LPTEXTURE>( CreateResource(CGraphicsResource::TEXTURE) );
	if(pOutput)
	{
		pOutput->m_TextureType   = CTexture::CUBE;

		pOutput->m_uUsage        = uUsage;
		pOutput->m_Format        = Format;
		pOutput->m_uStride       = CTexture::GetFormatSize(Format);

		pOutput->m_uWidth        = uSize;
		pOutput->m_uHeight       = uSize;
		pOutput->m_uDepth        = 1;

		UINT uShift              = ALCHEMY_SHIFT(uSize);

		pOutput->m_uWidthShift   = uShift;
		pOutput->m_uHeightShift  = uShift;
		pOutput->m_uDepthShift   = 0;

		pOutput->m_uMipLevel     = uMipLevel ? uMipLevel : ALCHEMY_TOTAL_BITS(UINT);
		pOutput->m_uMipLevel     = ALCHEMY_MININUM(pOutput->m_uMipLevel, uShift + 1);

		UINT uCurrentSize;
		pOutput->m_uMemberCount = 0;
		for(UINT i = 0; i < pOutput->m_uMipLevel; ++ i)
		{
			uCurrentSize = ALCHEMY_FLAG(uShift - i);
			pOutput->m_uMemberCount += uCurrentSize * uCurrentSize * 6;
		}

		pOutput->m_uByteSize = pOutput->m_uMemberCount * pOutput->m_uStride;

		if( pOutput->_Create() )
		{
			uCurrentSize = ALCHEMY_FLAG(uShift);
			if(pOutput->m_uMipLevel == 1 && uCurrentSize == uSize)
			{
				UINT uBasicPitch = uCurrentSize * pOutput->m_uStride, uMipMapSize = uBasicPitch * uCurrentSize, uPitch, uOffset, i;
				void* pSurfaceData;
				const UINT8* puData;

				pSurfaceData = pOutput->Lock(CTexture::POSITIVE_X << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
				if(pSurfaceData)
				{
					puData = reinterpret_cast<const UINT8*>(pPositiveXData);
					if(uBasicPitch == uPitch)
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
					else
					{
						uOffset = 0;
						for(i = 0; i < uCurrentSize; ++ i)
						{
							ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uBasicPitch);

							uOffset += uPitch;
						}
					}

					pOutput->Unlock();
				}

				pSurfaceData = pOutput->Lock(CTexture::NEGATIVE_X << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
				if(pSurfaceData)
				{
					puData = reinterpret_cast<const UINT8*>(pNegativeXData);
					if(uBasicPitch == uPitch)
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
					else
					{
						uOffset = 0;
						for(i = 0; i < uCurrentSize; ++ i)
						{
							ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uBasicPitch);

							uOffset += uPitch;
						}
					}

					pOutput->Unlock();
				}

				pSurfaceData = pOutput->Lock(CTexture::POSITIVE_Y << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
				if(pSurfaceData)
				{
					puData = reinterpret_cast<const UINT8*>(pPositiveYData);
					if(uBasicPitch == uPitch)
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
					else
					{
						uOffset = 0;
						for(i = 0; i < uCurrentSize; ++ i)
						{
							ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uBasicPitch);

							uOffset += uPitch;
						}
					}

					pOutput->Unlock();
				}

				pSurfaceData = pOutput->Lock(CTexture::NEGATIVE_Y << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
				if(pSurfaceData)
				{
					puData = reinterpret_cast<const UINT8*>(pNegativeYData);
					if(uBasicPitch == uPitch)
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
					else
					{
						uOffset = 0;
						for(i = 0; i < uCurrentSize; ++ i)
						{
							ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uBasicPitch);

							uOffset += uPitch;
						}
					}

					pOutput->Unlock();
				}

				pSurfaceData = pOutput->Lock(CTexture::POSITIVE_Z << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
				if(pSurfaceData)
				{
					puData = reinterpret_cast<const UINT8*>(pPositiveZData);
					if(uBasicPitch == uPitch)
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
					else
					{
						uOffset = 0;
						for(i = 0; i < uCurrentSize; ++ i)
						{
							ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uBasicPitch);

							uOffset += uPitch;
						}
					}

					pOutput->Unlock();
				}

				pSurfaceData = pOutput->Lock(CTexture::NEGATIVE_Z << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
				if(pSurfaceData)
				{
					puData = reinterpret_cast<const UINT8*>(pNegativeZData);
					if(uBasicPitch == uPitch)
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
					else
					{
						uOffset = 0;
						for(i = 0; i < uCurrentSize; ++ i)
						{
							ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uBasicPitch);

							uOffset += uPitch;
						}
					}

					pOutput->Unlock();
				}
			}
			else 
			{
				UINT i, j, uSourceOffset, uOutputOffset, uOutputPitch, uOutputSize, uSourcePitch, uSourceSize, uSourceByteSize = uSize * uSize * pOutput->m_uStride;
				void* pOutputData;
				PVOID pSourceData;

				ALCHEMY_DEBUG_NEW(pSourceData, UINT8[uCurrentSize * uCurrentSize * pOutput->m_uStride]);
				ALCHEMY_MEMORY_COPY(pSourceData, pPositiveXData, uSourceByteSize);
				uOutputSize = uCurrentSize;
				uSourcePitch = uCurrentSize * pOutput->m_uStride;
				uSourceSize = uSize;
				for(i = 0; i < pOutput->m_uMipLevel; ++ i)
				{
					pOutputData = pOutput->Lock(CTexture::POSITIVE_X << CTexture::CUBE_TEXTURE_SURFACE_SHIFT | i, 0, &uOutputPitch, ALCHEMY_NULL);
					if(pOutputData)
					{
						uOutputSize = ALCHEMY_FLAG(uShift - i);
						CTexture::BilerpBlends(
							pOutputData, 
							uOutputPitch, 
							uOutputSize, 
							uOutputSize, 
							pSourceData, 
							uSourcePitch, 
							uSourceSize, 
							uSourceSize, 
							2,
							2, 
							1.0f,
							Format);

						uSourcePitch = uOutputSize * pOutput->m_uStride;
						if(uSourcePitch == uOutputPitch)
							ALCHEMY_MEMORY_COPY(pSourceData, pOutputData, uSourcePitch * uOutputSize);
						else
						{
							uSourceOffset = 0;
							uOutputOffset = 0;
							for(j = 0; j < uSourceSize; ++ j)
							{
								ALCHEMY_MEMORY_COPY(reinterpret_cast<PUINT8>(pSourceData) + uSourceOffset, reinterpret_cast<const UINT8*>(pOutputData) + uOutputOffset, uSourcePitch);

								uOutputOffset += uOutputPitch;
								uSourceOffset += uSourcePitch;
							}
						}

						uSourceSize  = uOutputSize;

						pOutput->Unlock();
					}
				}
				
				ALCHEMY_MEMORY_COPY(pSourceData, pNegativeXData, uSourceByteSize);
				uOutputSize = uCurrentSize;
				uSourcePitch = uCurrentSize * pOutput->m_uStride;
				uSourceSize = uSize;
				for(i = 0; i < pOutput->m_uMipLevel; ++ i)
				{
					pOutputData = pOutput->Lock(CTexture::NEGATIVE_X << CTexture::CUBE_TEXTURE_SURFACE_SHIFT | i, 0, &uOutputPitch, ALCHEMY_NULL);
					if(pOutputData)
					{
						uOutputSize = ALCHEMY_FLAG(uShift - i);
						CTexture::BilerpBlends(
							pOutputData, 
							uOutputPitch, 
							uOutputSize, 
							uOutputSize, 
							pSourceData, 
							uSourcePitch, 
							uSourceSize, 
							uSourceSize, 
							2,
							2, 
							1.0f,
							Format);

						uSourcePitch = uOutputSize * pOutput->m_uStride;
						if(uSourcePitch == uOutputPitch)
							ALCHEMY_MEMORY_COPY(pSourceData, pOutputData, uSourcePitch * uOutputSize);
						else
						{
							uSourceOffset = 0;
							uOutputOffset = 0;
							for(j = 0; j < uSourceSize; ++ j)
							{
								ALCHEMY_MEMORY_COPY(reinterpret_cast<PUINT8>(pSourceData) + uSourceOffset, reinterpret_cast<const UINT8*>(pOutputData) + uOutputOffset, uSourcePitch);

								uOutputOffset += uOutputPitch;
								uSourceOffset += uSourcePitch;
							}
						}

						uSourceSize  = uOutputSize;

						pOutput->Unlock();
					}
				}

				ALCHEMY_MEMORY_COPY(pSourceData, pPositiveYData, uSourceByteSize);
				uOutputSize = uCurrentSize;
				uSourcePitch = uCurrentSize * pOutput->m_uStride;
				uSourceSize = uSize;
				for(i = 0; i < pOutput->m_uMipLevel; ++ i)
				{
					pOutputData = pOutput->Lock(CTexture::POSITIVE_Y << CTexture::CUBE_TEXTURE_SURFACE_SHIFT | i, 0, &uOutputPitch, ALCHEMY_NULL);
					if(pOutputData)
					{
						uOutputSize = ALCHEMY_FLAG(uShift - i);
						CTexture::BilerpBlends(
							pOutputData, 
							uOutputPitch, 
							uOutputSize, 
							uOutputSize, 
							pSourceData, 
							uSourcePitch, 
							uSourceSize, 
							uSourceSize, 
							2,
							2, 
							1.0f,
							Format);

						uSourcePitch = uOutputSize * pOutput->m_uStride;
						if(uSourcePitch == uOutputPitch)
							ALCHEMY_MEMORY_COPY(pSourceData, pOutputData, uSourcePitch * uOutputSize);
						else
						{
							uSourceOffset = 0;
							uOutputOffset = 0;
							for(j = 0; j < uSourceSize; ++ j)
							{
								ALCHEMY_MEMORY_COPY(reinterpret_cast<PUINT8>(pSourceData) + uSourceOffset, reinterpret_cast<const UINT8*>(pOutputData) + uOutputOffset, uSourcePitch);

								uOutputOffset += uOutputPitch;
								uSourceOffset += uSourcePitch;
							}
						}

						uSourceSize  = uOutputSize;

						pOutput->Unlock();
					}
				}

				ALCHEMY_MEMORY_COPY(pSourceData, pNegativeYData, uSourceByteSize);
				uOutputSize = uCurrentSize;
				uSourcePitch = uCurrentSize * pOutput->m_uStride;
				uSourceSize = uSize;
				for(i = 0; i < pOutput->m_uMipLevel; ++ i)
				{
					pOutputData = pOutput->Lock(CTexture::NEGATIVE_Y << CTexture::CUBE_TEXTURE_SURFACE_SHIFT | i, 0, &uOutputPitch, ALCHEMY_NULL);
					if(pOutputData)
					{
						uOutputSize = ALCHEMY_FLAG(uShift - i);
						CTexture::BilerpBlends(
							pOutputData, 
							uOutputPitch, 
							uOutputSize, 
							uOutputSize, 
							pSourceData, 
							uSourcePitch, 
							uSourceSize, 
							uSourceSize, 
							2,
							2, 
							1.0f,
							Format);

						uSourcePitch = uOutputSize * pOutput->m_uStride;
						if(uSourcePitch == uOutputPitch)
							ALCHEMY_MEMORY_COPY(pSourceData, pOutputData, uSourcePitch * uOutputSize);
						else
						{
							uSourceOffset = 0;
							uOutputOffset = 0;
							for(j = 0; j < uSourceSize; ++ j)
							{
								ALCHEMY_MEMORY_COPY(reinterpret_cast<PUINT8>(pSourceData) + uSourceOffset, reinterpret_cast<const UINT8*>(pOutputData) + uOutputOffset, uSourcePitch);

								uOutputOffset += uOutputPitch;
								uSourceOffset += uSourcePitch;
							}
						}

						uSourceSize  = uOutputSize;

						pOutput->Unlock();
					}
				}

				ALCHEMY_MEMORY_COPY(pSourceData, pPositiveZData, uSourceByteSize);
				uOutputSize = uCurrentSize;
				uSourcePitch = uCurrentSize * pOutput->m_uStride;
				uSourceSize = uSize;
				for(i = 0; i < pOutput->m_uMipLevel; ++ i)
				{
					pOutputData = pOutput->Lock(CTexture::POSITIVE_Z << CTexture::CUBE_TEXTURE_SURFACE_SHIFT | i, 0, &uOutputPitch, ALCHEMY_NULL);
					if(pOutputData)
					{
						uOutputSize = ALCHEMY_FLAG(uShift - i);
						CTexture::BilerpBlends(
							pOutputData, 
							uOutputPitch, 
							uOutputSize, 
							uOutputSize, 
							pSourceData, 
							uSourcePitch, 
							uSourceSize, 
							uSourceSize, 
							2,
							2, 
							1.0f,
							Format);

						uSourcePitch = uOutputSize * pOutput->m_uStride;
						if(uSourcePitch == uOutputPitch)
							ALCHEMY_MEMORY_COPY(pSourceData, pOutputData, uSourcePitch * uOutputSize);
						else
						{
							uSourceOffset = 0;
							uOutputOffset = 0;
							for(j = 0; j < uSourceSize; ++ j)
							{
								ALCHEMY_MEMORY_COPY(reinterpret_cast<PUINT8>(pSourceData) + uSourceOffset, reinterpret_cast<const UINT8*>(pOutputData) + uOutputOffset, uSourcePitch);

								uOutputOffset += uOutputPitch;
								uSourceOffset += uSourcePitch;
							}
						}

						uSourceSize  = uOutputSize;

						pOutput->Unlock();
					}
				}

				ALCHEMY_MEMORY_COPY(pSourceData, pNegativeZData, uSourceByteSize);
				uOutputSize = uCurrentSize;
				uSourcePitch = uCurrentSize * pOutput->m_uStride;
				uSourceSize = uSize;
				for(i = 0; i < pOutput->m_uMipLevel; ++ i)
				{
					pOutputData = pOutput->Lock(CTexture::NEGATIVE_Z << CTexture::CUBE_TEXTURE_SURFACE_SHIFT | i, 0, &uOutputPitch, ALCHEMY_NULL);
					if(pOutputData)
					{
						uOutputSize = ALCHEMY_FLAG(uShift - i);
						CTexture::BilerpBlends(
							pOutputData, 
							uOutputPitch, 
							uOutputSize, 
							uOutputSize, 
							pSourceData, 
							uSourcePitch, 
							uSourceSize, 
							uSourceSize, 
							2,
							2, 
							1.0f,
							Format);

						uSourcePitch = uOutputSize * pOutput->m_uStride;
						if(uSourcePitch == uOutputPitch)
							ALCHEMY_MEMORY_COPY(pSourceData, pOutputData, uSourcePitch * uOutputSize);
						else
						{
							uSourceOffset = 0;
							uOutputOffset = 0;
							for(j = 0; j < uSourceSize; ++ j)
							{
								ALCHEMY_MEMORY_COPY(reinterpret_cast<PUINT8>(pSourceData) + uSourceOffset, reinterpret_cast<const UINT8*>(pOutputData) + uOutputOffset, uSourcePitch);

								uOutputOffset += uOutputPitch;
								uSourceOffset += uSourcePitch;
							}
						}

						uSourceSize  = uOutputSize;

						pOutput->Unlock();
					}
				}
				
				ALCHEMY_DEBUG_DELETE_ARRAY(pSourceData);
			}

			return pOutput;
		}

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

void CRenderSystem::__CreateRenderTargetView(CGraphicsBuffer<CTexture>& Texture, INT nShift)
{
	static_cast<CRenderSystem&>( Texture.GetGraphicsResourcePool() ).GetViewport(&Texture.m_uWidth, &Texture.m_uHeight);

	Texture.m_uWidthShift  = ALCHEMY_SHIFT(Texture.m_uWidth );
	Texture.m_uHeightShift = ALCHEMY_SHIFT(Texture.m_uHeight);

	if(nShift)
	{
		Texture.m_uWidthShift  += nShift;
		Texture.m_uHeightShift += nShift;

		Texture.m_uWidth  = ALCHEMY_FLAG(Texture.m_uWidthShift);
		Texture.m_uHeight = ALCHEMY_FLAG(Texture.m_uHeightShift);
	}

	Texture.m_uMemberCount = 0;
	for(UINT i = 0; i < Texture.m_uMipLevel; ++ i)
		Texture.m_uMemberCount += ALCHEMY_FLAG(Texture.m_uWidthShift - i) * ALCHEMY_FLAG(Texture.m_uHeightShift - i);

	Texture.m_uByteSize = Texture.m_uMemberCount * Texture.m_uStride;

	Texture.Destroy();
	Texture._Create();
}