#include "AlchemyMeshInstance.h"

#include "AlchemyMeshProcessorInterface.h"
#include "AlchemyRenderMethodInterface.h"

#include "AlchemySceneManager.h"

using namespace alchemy;

CMeshInstance::CMeshInstance()
:m_bIsDirty(false),
m_pEffect(ALCHEMY_NULL),
m_pRenderMethod(ALCHEMY_NULL),
m_pSceneManager(ALCHEMY_NULL)
{

}

CMeshInstance::~CMeshInstance()
{

}

bool CMeshInstance::Create(CGraphicsResource::HANDLE Effect, IRenderMethod& RenderMethod)
{
	Destroy();

	m_pSceneManager = CSceneManager::GetInstance();
	if(!m_pSceneManager)
		return false;

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
	m_pEffect = static_cast<CEffect*>( RenderSystem.GetResource(Effect, CGraphicsResource::EFFECT) );
	if(!m_pEffect)
		return false;

	m_pEffect->AddRef();

	m_pRenderMethod = &RenderMethod;
	m_pRenderMethod->AddRef();

	return true;
}

UINT CMeshInstance::AddSubMesh(const MATRIX4X4* pMatrix, IMeshProcessor& MeshProcessor, UINT16 uSubset, CGraphicsResource::HANDLE Material)
{
	if(pMatrix)
	{
		m_bIsDirty = true;

		SUBMESH SubMesh;
		SubMesh.pMatrix			= pMatrix;
		SubMesh.pMeshProcessor	= &MeshProcessor;
		SubMesh.uSubset			= uSubset;
		SubMesh.Material		= Material;
		MeshProcessor.AddRef();

		return m_SubMeshes.Add(SubMesh) + 1;
	}

	return 0;
}

void CMeshInstance::RemoveSubMesh(UINT uHandle)
{

}

void CMeshInstance::Destroy()
{

}

bool CMeshInstance::ApplyForRender()
{
	return true;
}

void CMeshInstance::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{

}