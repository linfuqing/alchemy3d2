#include "AlchemyRenderSystem.h"
#include "AlchemyGraphicsResourcePool.h"

#include "../Core/AlchemyUtils.h"

using namespace alchemy;

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

			ALCHEMY_DEBUG_NEW(pOutput->m_puVertexElementOrderMap, UINT[pOutput->m_uElementCount]);

			CVertexDeclaration::LPVERTEXELEMENT pVertexElementTemp = &pOutput->m_pVertexElements[0];
			pOutput->m_puVertexElementOrderMap[0] = CVertexDeclaration::FindVertexElement(pVertexElementTemp->Usage, pVertexElementTemp->Type, pVertexElementTemp->uOffset, pVertexElementTemp->uStream, pVertexElements, uElementCount);

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

				pOutput->m_puVertexElementOrderMap[i] = CVertexDeclaration::FindVertexElement(pVertexElementTemp->Usage, pVertexElementTemp->Type, pVertexElementTemp->uOffset, pVertexElementTemp->uStream, pVertexElements, uElementCount);
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
	LPVERTEXBUFFER pOutput;
	if( pOutput = static_cast<LPVERTEXBUFFER>( CreateResource(CGraphicsResource::VERTEX_BUFFER) ) )
	{
		pOutput->m_uMemberCount = uCount;
		pOutput->m_uStride      = uStride;
		pOutput->m_uUsage       = uUsage;
		pOutput->m_uByteSize    = uCount * uStride;

		if( pOutput->_Create() )
		{
			if(pData)
			{
				PVOID pVertices = pOutput->Lock(0);
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
	LPINDEXBUFFER pOutput;
	if( pOutput = static_cast<LPINDEXBUFFER>( CreateResource(CGraphicsResource::INDEX_BUFFER) ) )
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
				PVOID pIndices = pOutput->Lock(0);
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


CShader* CRenderSystem::CreateShader(CEffectInterface::EFFECTTYPE Type, CCompilerInterface& Compiler, const CHAR* pCode, const CHAR* pAdditionalCode, CBuffer** ppCompilationErrors)
{
	CShader* pOutput;
	if( pOutput = static_cast<CShader*>( CreateResource(CGraphicsResource::SHADER) ) )
	{
		pOutput->m_Type  = Type;

		if(pAdditionalCode)
		{
			Compiler.ProgramString(&pOutput->m_pCode          , &pOutput->m_uCodeSize          , CEffectInterface::VERTEX_SHADER  , pCode          , ppCompilationErrors);
			Compiler.ProgramString(&pOutput->m_pAdditionalCode, &pOutput->m_uAdditionalCodeSize, CEffectInterface::FRAGMENT_SHADER, pAdditionalCode, ppCompilationErrors);
		}
		else
		{
			Compiler.ProgramString(&pOutput->m_pCode, &pOutput->m_uCodeSize, Type, pCode, ppCompilationErrors);

			pOutput->m_pAdditionalCode     = ALCHEMY_NULL;
			pOutput->m_uAdditionalCodeSize = 0;
		}

		pOutput->m_pCompiler = &Compiler;

		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CEffect* CRenderSystem::CreateEffect(UINT uPassCount, const CHAR* pCode, CBuffer** ppCompilationErrors)
{
	CEffect* pOutput;
	if( pOutput = static_cast<CEffect*>( CreateResource(CGraphicsResource::EFFECT) ) )
	{
		pOutput->m_uPassCount  = uPassCount;
		pOutput->m_pEffectCode = ALCHEMY_NULL;

		if(pCode)
		{
			ALCHEMY_DEBUG_NEW(pOutput->m_pCode, STRING);

			*(pOutput->m_pCode) = pCode;
		}
		else
			pOutput->m_pCode = ALCHEMY_NULL;

		pOutput->m_ppCompilationErrors = ppCompilationErrors;

		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CEffect* CRenderSystem::CreateEffect(CEffectCode& EffectCode, CBuffer** ppCompilationErrors)
{
	CEffect* pOutput;
	if( pOutput = static_cast<CEffect*>( CreateResource(CGraphicsResource::EFFECT) ) )
	{
		pOutput->m_uPassCount  = EffectCode.GetPassCount();
		pOutput->m_pEffectCode = &EffectCode;

		pOutput->m_ppCompilationErrors = ppCompilationErrors;

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

	LPTEXTURE pOutput = ALCHEMY_NULL;
	if( pOutput = static_cast<LPTEXTURE>( CreateResource(CGraphicsResource::TEXTURE) ) )
	{
		pOutput->m_Type          = CTexture::NORMAL;

		pOutput->m_uUsage        = uUsage;
		pOutput->m_Format        = Format;
		pOutput->m_uStride       = CTexture::GetFormatSize(Format);

		pOutput->m_uWidthShift = Shift(uWidth);
		pOutput->m_uHeightShift = Shift(uHeight);
		pOutput->m_uDepthShift = 0;

		pOutput->m_uMipLevel     = ALCHEMY_MININUM( uMipLevel ? uMipLevel : ALCHEMY_TOTAL_BITS(UINT), ALCHEMY_MININUM(pOutput->m_uWidthShift, pOutput->m_uHeightShift) );

		pOutput->m_uMemberCount = 0;
		for(UINT i = 0; i < pOutput->m_uMipLevel; ++ i)
			pOutput->m_uMemberCount += ( 1 << (pOutput->m_uWidthShift - i) ) * ( 1 << (pOutput->m_uHeightShift - i) );

		pOutput->m_uByteSize = pOutput->m_uMemberCount * pOutput->m_uStride;

		if( pOutput->_Create() )
		{
			if(pData)
			{
				UINT uMipMapSize, uWidth, uPitch;
				PVOID pSurfaceData;
				const UINT8* puData = static_cast<const UINT8*>(pData);

				uWidth = 1 << pOutput->m_uWidthShift;
				uMipMapSize = uWidth * (1 << pOutput->m_uHeightShift) * pOutput->m_uStride;

				pSurfaceData = pOutput->Lock(0, 0, &uPitch, ALCHEMY_NULL);
				if(pSurfaceData)
				{
					if(uWidth == uPitch)
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
					else
					{
						UINT uOffset = 0;
						for(UINT i = 0; i < uHeight; ++ i)
						{
							ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uWidth);

							uOffset += uPitch;
						}
					}

					pOutput->Unlock();
				}
			}

			return pOutput;
		}

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

LPTEXTURE CRenderSystem::CreateTexture(CTexture::CTextureInfoInterface& Info, UINT uUsage, CTexture::FORMAT Format, UINT uMipLevel, const void* pData)
{
	UINT uWidth, uHeight;

	Info.GetSize(&uWidth, &uHeight, ALCHEMY_NULL);

	if(!uWidth || !uHeight)
		return ALCHEMY_NULL;

	LPTEXTURE pOutput = ALCHEMY_NULL;
	if( pOutput = static_cast<LPTEXTURE>( CreateResource(CGraphicsResource::TEXTURE) ) )
	{
		pOutput->m_Type          = CTexture::NORMAL;

		pOutput->m_uUsage        = uUsage;
		pOutput->m_Format        = Format;
		pOutput->m_uStride       = CTexture::GetFormatSize(Format);

		pOutput->m_uWidthShift = Shift(uWidth);
		pOutput->m_uHeightShift = Shift(uHeight);
		pOutput->m_uDepthShift = 0;

		pOutput->m_uMipLevel     = ALCHEMY_MININUM( uMipLevel ? uMipLevel : ALCHEMY_TOTAL_BITS(UINT), ALCHEMY_MININUM(pOutput->m_uWidthShift, pOutput->m_uHeightShift) );

		pOutput->m_uMemberCount = 0;
		for(UINT i = 0; i < pOutput->m_uMipLevel; ++ i)
			pOutput->m_uMemberCount += ( 1 << (pOutput->m_uWidthShift - i) ) * ( 1 << (pOutput->m_uHeightShift - i) );

		pOutput->m_uByteSize = pOutput->m_uMemberCount * pOutput->m_uStride;

		pOutput->m_pTextureInfo = &Info;

		if( pOutput->_Create() )
		{
			if(pData)
			{
				UINT uMipMapSize, uWidth, uPitch;
				PVOID pSurfaceData;
				const UINT8* puData = static_cast<const UINT8*>(pData);

				uWidth = 1 << pOutput->m_uWidthShift;
				uMipMapSize = uWidth * (1 << pOutput->m_uHeightShift) * pOutput->m_uStride;

				pSurfaceData = pOutput->Lock(0, 0, &uPitch, ALCHEMY_NULL);
				if(pSurfaceData)
				{
					if(uWidth == uPitch)
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
					else
					{
						UINT uOffset = 0;
						for(UINT i = 0; i < uHeight; ++ i)
						{
							ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uWidth);

							uOffset += uPitch;
						}
					}

					pOutput->Unlock();
				}
			}

			return pOutput;
		}

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

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

	LPTEXTURE pOutput = ALCHEMY_NULL;
	if( pOutput = static_cast<LPTEXTURE>( CreateResource(CGraphicsResource::TEXTURE) ) )
	{
		pOutput->m_Type          = CTexture::CUBE;

		pOutput->m_uUsage        = uUsage;
		pOutput->m_Format        = Format;
		pOutput->m_uStride       = CTexture::GetFormatSize(Format);

		UINT uShift = Shift(uSize);

		pOutput->m_uWidthShift = uShift;
		pOutput->m_uHeightShift = uShift;
		pOutput->m_uDepthShift = 0;

		pOutput->m_uMipLevel     = ALCHEMY_MININUM(uMipLevel ? uMipLevel : ALCHEMY_TOTAL_BITS(UINT), uShift);

		UINT uCurrentSize;
		pOutput->m_uMemberCount = 0;
		for(UINT i = 0; i < pOutput->m_uMipLevel; ++ i)
		{
			uCurrentSize = 1 << (uShift - i);
			pOutput->m_uMemberCount += uCurrentSize * uCurrentSize * 6;
		}

		pOutput->m_uByteSize = pOutput->m_uMemberCount * pOutput->m_uStride;

		if( pOutput->_Create() )
		{
			uCurrentSize = 1 << uShift;
			UINT uMipMapSize = uCurrentSize * uCurrentSize * pOutput->m_uStride, uPitch, uOffset, i;
			void* pSurfaceData;
			const UINT8* puData;

			pSurfaceData = pOutput->Lock(CTexture::POSITIVE_X << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
			if(pSurfaceData)
			{
				puData = reinterpret_cast<const UINT8*>(pPositiveXData);
				if(uCurrentSize == uPitch)
					ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
				else
				{
					uOffset = 0;
					for(i = 0; i < uCurrentSize; ++ i)
					{
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uCurrentSize);

						uOffset += uPitch;
					}
				}

				pOutput->Unlock();
			}

			pSurfaceData = pOutput->Lock(CTexture::NEGATIVE_X << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
			if(pSurfaceData)
			{
				puData = reinterpret_cast<const UINT8*>(pNegativeXData);
				if(uCurrentSize == uPitch)
					ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
				else
				{
					uOffset = 0;
					for(i = 0; i < uCurrentSize; ++ i)
					{
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uCurrentSize);

						uOffset += uPitch;
					}
				}

				pOutput->Unlock();
			}

			pSurfaceData = pOutput->Lock(CTexture::POSITIVE_Y << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
			if(pSurfaceData)
			{
				puData = reinterpret_cast<const UINT8*>(pPositiveYData);
				if(uCurrentSize == uPitch)
					ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
				else
				{
					uOffset = 0;
					for(i = 0; i < uCurrentSize; ++ i)
					{
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uCurrentSize);

						uOffset += uPitch;
					}
				}

				pOutput->Unlock();
			}

			pSurfaceData = pOutput->Lock(CTexture::NEGATIVE_Y << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
			if(pSurfaceData)
			{
				puData = reinterpret_cast<const UINT8*>(pNegativeYData);
				if(uCurrentSize == uPitch)
					ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
				else
				{
					uOffset = 0;
					for(i = 0; i < uCurrentSize; ++ i)
					{
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uCurrentSize);

						uOffset += uPitch;
					}
				}

				pOutput->Unlock();
			}

			pSurfaceData = pOutput->Lock(CTexture::POSITIVE_Z << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
			if(pSurfaceData)
			{
				puData = reinterpret_cast<const UINT8*>(pPositiveZData);
				if(uCurrentSize == uPitch)
					ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
				else
				{
					uOffset = 0;
					for(i = 0; i < uCurrentSize; ++ i)
					{
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uCurrentSize);

						uOffset += uPitch;
					}
				}

				pOutput->Unlock();
			}

			pSurfaceData = pOutput->Lock(CTexture::NEGATIVE_Z << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0, &uPitch, ALCHEMY_NULL);
			if(pSurfaceData)
			{
				puData = reinterpret_cast<const UINT8*>(pNegativeZData);
				if(uCurrentSize == uPitch)
					ALCHEMY_MEMORY_COPY(pSurfaceData, puData, uMipMapSize);
				else
				{
					uOffset = 0;
					for(i = 0; i < uCurrentSize; ++ i)
					{
						ALCHEMY_MEMORY_COPY(pSurfaceData, puData + uOffset, uCurrentSize);

						uOffset += uPitch;
					}
				}

				pOutput->Unlock();
			}

			return pOutput;
		}

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}