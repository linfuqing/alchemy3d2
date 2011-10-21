#include "AlchemyVertexDeclaration.h"
#include "AlchemyRenderSystem.h"
#include "../Core/AlchemyUtils.h"

using namespace alchemy;

CVertexDeclaration::CVertexDeclaration(void) :
m_pVertexElements(ALCHEMY_NULL),
m_puElementEndOfStream(ALCHEMY_NULL),
m_puVertexElementOrderMap(ALCHEMY_NULL)
{
}

CVertexDeclaration::~CVertexDeclaration(void)
{
}

bool CVertexDeclaration::IsValuableMeshDeclaration(const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount)
{
	return (GetVertexElementMaxinumStream(pVertexElements, uElementCount) + 1) == GetVertexElementStreamCount(pVertexElements, uElementCount);
}

UINT CVertexDeclaration::GetVertexElementMaxinumStream(const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount)
{
	UINT uMaxStream = 0;

	const VERTEXELEMENT* pVertexElement;
	for(UINT i = 0; i < uElementCount; i ++)
	{
		pVertexElement = &pVertexElements[i];
		if(pVertexElement->uStream > uMaxStream)
			uMaxStream = pVertexElement->uStream;
	}

	return uMaxStream;
}

UINT CVertexDeclaration::GetVertexElementStreamCount(const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount)
{
	UINT uCurrentStream, uStreamCount = 0, i, j;

	for(i = 0; i < uElementCount; ++ i)
	{
		++ uStreamCount;

		uCurrentStream = pVertexElements[i].uStream;
		for(j = 0; j < i; ++ j)
		{
			if(uCurrentStream == pVertexElements[j].uStream)
			{
				-- uStreamCount;

				break;
			}
		}
	}

	return uStreamCount;
}

UINT CVertexDeclaration::GetVertexElementStreamStrideOf(UINT uStream, const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount)
{
	UINT uMaxOffsetElement = uElementCount, uMaxOffset = 0;

	const VERTEXELEMENT* pVertexElement;
	for(UINT i = 0; i < uElementCount; i ++)
	{
		pVertexElement = &pVertexElements[i];
		if(pVertexElement->uStream == uStream && pVertexElement->uOffset >= uMaxOffset)
		{
			uMaxOffset        = pVertexElement->uOffset;
			uMaxOffsetElement = i;
		}
	}

	if(uMaxOffsetElement == uElementCount)
		return 0;

	return uMaxOffset + pVertexElements[uMaxOffsetElement].Type;
}

UINT CVertexDeclaration::FindVertexElement(CVertexDeclaration::ELEMENTUSAGE Usage, UINT uIndex, const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount)
{
	const VERTEXELEMENT* pVertexElement;
	for(UINT i = 0; i < uElementCount; i ++)
	{
		pVertexElement = &pVertexElements[i];
		if(pVertexElement->Usage == Usage)
		{
			if(uIndex)
				-- uIndex;
			else
				return i;
		}
	}

	return uElementCount;
}

UINT CVertexDeclaration::FindVertexElement(CVertexDeclaration::ELEMENTUSAGE Usage, CVertexDeclaration::ELEMENTTYPE Type, UINT uIndex, const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount)
{
	const VERTEXELEMENT* pVertexElement;
	for(UINT i = 0; i < uElementCount; i ++)
	{
		pVertexElement = &pVertexElements[i];
		if(pVertexElement->Type == Type && pVertexElement->Usage == Usage)
		{
			if(uIndex)
				-- uIndex;
			else
				return i;
		}
	}

	return uElementCount;
}

UINT CVertexDeclaration::FindVertexElement(ELEMENTUSAGE Usage, ELEMENTTYPE Type, UINT uOffset, UINT uStream, const VERTEXELEMENT* pVertexElements, UINT uElementCount)
{
	const VERTEXELEMENT* pVertexElement;
	for(UINT i = 0; i < uElementCount; i ++)
	{
		pVertexElement = &pVertexElements[i];
		if(pVertexElement->Type == Type && pVertexElement->Usage == Usage && pVertexElement->uOffset == uOffset && pVertexElement->uStream == uStream)
			return i;
	}

	return uElementCount;
}

typedef struct
{
	bool operator()(const CVertexDeclaration::VERTEXELEMENT& Source, const CVertexDeclaration::VERTEXELEMENT& Target)const
	{
		return Source.uStream == Target.uStream ? (Source.uOffset < Target.uOffset) : (Source.uStream < Target.uStream);
	}

}VERTEXELEMENTLESS;

void CVertexDeclaration::SortVertexElement(CVertexDeclaration::LPVERTEXELEMENT pOutput, const CVertexDeclaration::VERTEXELEMENT* pInput, UINT uCount)
{
	Sort<VERTEXELEMENT, VERTEXELEMENTLESS>(pOutput, pInput, uCount);
}

void CVertexDeclaration::Destroy()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pVertexElements);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puElementEndOfStream);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puVertexElementOrderMap);
}