#include "AlchemyVertexBuffer.h"
#include "AlchemyRenderSystem.h"
#include "../Core/AlchemyUtils.h"

using namespace alchemy;

CVertexBuffer::CVertexBuffer(void)
{
}

CVertexBuffer::~CVertexBuffer(void)
{
}

//bool CVertexBuffer::Activate(UINT uStream, UINT uOffsetInBytes)
//{
//	return static_cast<CRenderSystem&>( GetGraphicsResourcePool() ).SetStreamSource(*this, uStream, uOffsetInBytes);
//}