#include "AlchemySimplePostProcessEffect.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CSimplePostProcessEffect::CSimplePostProcessEffect(void)
:m_pFullScreenDrawer(ALCHEMY_NULL)
{

}

CSimplePostProcessEffect::~CSimplePostProcessEffect(void)
{

}

bool CSimplePostProcessEffect::ApplyForRender()
{
	if( !m_pFullScreenDrawer || !m_pFullScreenDrawer->IsValuable() )
		return false;

	CSceneManager& SceneManager = m_pFullScreenDrawer->GetSceneManager();

	const CMesh* pMesh = m_pFullScreenDrawer->GetMesh();
	CRenderQueue::RENDERENTRY& RenderEntry = SceneManager.LockRenderEntry(*this);
	RenderEntry.uRenderTarget              = 0;
	RenderEntry.uEffect                    = GetEffectHandle();
	RenderEntry.uRenderPass                = 0;
	RenderEntry.uVertexDeclaration         = pMesh->GetVertex().GetVertexDeclaration().GetResourceHandle();
	RenderEntry.uModel                     = pMesh->GetResourceHandle();
	RenderEntry.uModelParameter            = 0;

	return SceneManager.UnlockRenderEntry(CSceneManager::EFFECT_2D);
}

void CSimplePostProcessEffect::Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag)
{
	if( !m_pFullScreenDrawer || !m_pFullScreenDrawer->IsValuable() )
		return;

	CSceneManager& SceneManager = m_pFullScreenDrawer->GetSceneManager();
	CGraphicsResource::HANDLE SceneRenderTarget = SceneManager.GetSceneRenderTarget();
	if(SceneRenderTarget == Entry.uRenderTarget)
		return;

	OnEffectUpdate(uFlag, SceneRenderTarget);
}