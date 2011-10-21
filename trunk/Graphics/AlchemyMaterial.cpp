#include "AlchemyMaterial.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CMaterial::CMaterial(void)
{
}

CMaterial::~CMaterial(void)
{
}

bool CMaterial::AddTexture(CGraphicsResource::HANDLE Texture)
{
	LPTEXTURE pTexture = static_cast<LPTEXTURE>( static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().GetResource(Texture, CGraphicsResource::TEXTURE) );

	if(pTexture)
	{
		_AddRef(*pTexture);

		m_Textures.Add(pTexture);

		return true;
	}

	return false;
}

bool CMaterial::RemoveTexture(UINT uIndex)
{
	LPTEXTURE* ppTexture = m_Textures.Get(uIndex);

	if(ppTexture)
	{
		if(*ppTexture)
			_Release(**ppTexture);
		
		return m_Textures.Remove(uIndex, 1);
	}

	return false;
}

void CMaterial::Destroy()
{
	LPTEXTURE pTexture;
	CVector<LPTEXTURE>::CConstantIterator Iterator = m_Textures.GetBeginIterator();
	while( CVector<LPTEXTURE>::IsValuable(Iterator) )
	{
		pTexture = *Iterator; 

		if(pTexture)
			_Release(*pTexture);

		++ Iterator;
	}

	m_Textures.Clear();
}