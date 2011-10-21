#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashSceneManager.h"
#include "AlchemyFlashMesh.h"
#include "AlchemyFlashMaterial.h"
#include "AlchemyFlashLight.h"
#include "AlchemyFlashRenderMethod.h"
#include "AlchemyFlashCamera.h"

using namespace alchemy;

CFlashSceneManager::CFlashSceneManager(CFlashContext3D& Context3D, UINT uMaxRenderEntries) :
CSceneManager(Context3D, ALCHEMY_FLASH_SCENE_NODE_FACTORY, uMaxRenderEntries)
{
}

CFlashSceneManager::~CFlashSceneManager(void)
{
}

void CFlashSceneManager::Render()
{
	CSceneManager::Render();

	static_cast<CFlashContext3D&>( GetRenderSystem() ).SubmitRenderDataQueueToRender();
}

#ifdef AS3_H
AS3TYPE CFlashSceneManager::Init()
{
	return AS3_Array("AS3ValType, PtrType, PtrType", static_cast<const AS3TYPE>( static_cast<const CFlashCamera&>( GetCamera() ) ), static_cast<const FLOAT*>(m_AmbientLight), &m_BackgroundColor);
}

void CFlashSceneManager::Fill(AS3TYPE Object, AS3TYPE MethodArray)
{
	m_Object = Object;

	AS3_ArrayValue(MethodArray, "AS3ValType, AS3ValType", &m_CreateResourceMethod, &m_DestroyResourceMethod);
}
#endif

CGraphicsResource* CFlashSceneManager::_Create(CGraphicsResource::TYPE Type)
{
	CGraphicsResource* pResource = ALCHEMY_NULL;

	switch(Type)
	{
	case CGraphicsResource::VERTEX:
		ALCHEMY_DEBUG_NEW(pResource, CVertex);
		break;
	case CGraphicsResource::MESH:
		ALCHEMY_DEBUG_NEW(pResource, CFlashMesh);
		break;
	case CGraphicsResource::MATERIAL:
		ALCHEMY_DEBUG_NEW(pResource, CFlashMaterial);
		break;
	case CGraphicsResource::RENDER_METHOD:
		ALCHEMY_DEBUG_NEW(pResource, CFlashRenderMethod);
		break;
	case CGraphicsResource::LIGHT:
		ALCHEMY_DEBUG_NEW(pResource, CFlashLight);
		break;
	}

	return pResource;
}

#ifdef AS3_H
bool CFlashSceneManager::_RemoveResource(CGraphicsResource::HANDLE Handle, CGraphicsResource::TYPE Type)
{
	int nHandle = Handle, nType = Type;

	AS3_CallT(m_DestroyResourceMethod, m_Object, "IntType, IntType", nHandle, nType);

	return CGraphicsResourcePool::_RemoveResource(Handle, Type);
}
#endif