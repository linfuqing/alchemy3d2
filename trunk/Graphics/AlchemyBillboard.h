#pragma once

#include "AlchemySceneManager.h"
#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	class CBillboard :
		public CSceneNode
	{
	public:
		CBillboard();
		virtual ~CBillboard();

		FLOAT GetSpriteWidth() const;
		FLOAT GetSpriteHeight() const;

		bool Create(FLOAT fSpriteWidth, FLOAT fSpriteHeight, UINT uNumSprite, CGraphicsResource::HANDLE Texture, bool bIsDynamic = false);

		virtual void Update();
		virtual bool ApplyForRender();
		virtual void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);

		virtual void Destroy();

	protected:
		bool __IsValuable() const;

		bool __InitSprites(FLOAT fSpriteWidth, FLOAT fSpriteHeight, UINT uNumSprite, bool bIsDynamic);
		bool __InitEffect(CCompilerInterface& Compiler);

	protected:
		FLOAT				m_fSpriteWidth;
		FLOAT				m_fSpriteHeight;

		CEffect*			m_pEffect;
		CVertexDeclaration*	m_pVertexDeclartion;
		CSceneManager*		m_pSceneManager;

		LPVERTEXBUFFER		m_pPositionBuffer;
		LPVERTEXBUFFER		m_pUVBuffer;
		LPINDEXBUFFER		m_pIndexBuffer;
		LPTEXTURE			m_pTexture;

		UINT				m_uEffectParameter;

		UINT				m_uDrawStart;
		UINT				m_uDrawTriangleCount;
	};

	ALCHEMY_INLINE bool CBillboard::__IsValuable() const
	{
		return m_pSceneManager && m_pIndexBuffer && m_pPositionBuffer && m_pVertexDeclartion && m_pTexture && m_pEffect;
	}

	ALCHEMY_INLINE FLOAT CBillboard::GetSpriteWidth() const
	{
		return m_fSpriteWidth;
	}

	ALCHEMY_INLINE FLOAT CBillboard::GetSpriteHeight() const
	{
		return m_fSpriteHeight;
	}
}