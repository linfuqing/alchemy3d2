#include "AlchemyIndexBuffer.h"
#include "AlchemyRenderSystem.h"
#include "../Core/AlchemyUtils.h"

using namespace alchemy;

CIndexBuffer::CIndexBuffer(void)
{
}

CIndexBuffer::~CIndexBuffer(void)
{
}

UINT CIndexBuffer::GetTriangleCount(UINT uIndexCount, TRIANGLETYPE Type)
{
	UINT uFaceCount = 0;

	switch(Type)
	{
	case TRIANGLE_LIST:
		uFaceCount = uIndexCount / 3;
		break;
	case TRIANGLE_STRIP:
		uFaceCount = uIndexCount - 2;
		break;
	case TRIANGLE_FAN:
		uFaceCount = (uIndexCount - 1) / 2;
		break;
	}

	return uFaceCount;
}