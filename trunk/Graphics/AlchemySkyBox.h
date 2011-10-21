#pragma once

#include "AlchemySceneManager.h"

namespace alchemy
{
	class CSkyBox : 
		public CSceneNode
	{
	public:
		typedef enum
		{
			EFFECT_PARAMETER_WORLD_VIEW_PROJECTION,

			EFFECT_PARAMETER_TEXTURE,

			EFFECT_PARAMETER_COUNT
		}EFFECT_PARAMETER_TYPE;

		CSkyBox();
		virtual ~CSkyBox();

		bool Create(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, CGraphicsResource::HANDLE Texture);

		FLOAT GetWidth() const;
		FLOAT GetHeight() const;
		FLOAT GetDepth() const;

		virtual bool ApplyForRender();
		virtual void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);

		virtual void Destroy();

	private:
		bool __IsValuable() const;

		bool __CreateRenderMethod(CCompilerInterface& Compiler);
		bool __ComputeVertexes();
		bool __ComputeIndices();

	private:
		LPVERTEXBUFFER			m_pPositionBuffer;
		LPVERTEXBUFFER			m_pUVBuffer;
		LPINDEXBUFFER			m_pIndexBuffer;
		LPTEXTURE				m_pTexture;
		CSceneManager*			m_pSceneManager;
		CVertexDeclaration*		m_pVertexDeclartion;
		CEffect*				m_pEffect;

		UINT					m_uEffectParameters[EFFECT_PARAMETER_COUNT];

		FLOAT					m_fWidth;
		FLOAT					m_fHeight;
		FLOAT					m_fDepth;
	};

	ALCHEMY_INLINE FLOAT CSkyBox::GetWidth() const 
	{
		return m_fWidth;
	}

	ALCHEMY_INLINE FLOAT CSkyBox::GetHeight() const
	{
		return m_fHeight;
	}

	ALCHEMY_INLINE FLOAT CSkyBox::GetDepth() const
	{
		return m_fDepth;
	}

	ALCHEMY_INLINE bool CSkyBox::__IsValuable() const
	{
		return m_pSceneManager && m_pIndexBuffer && m_pPositionBuffer && m_pVertexDeclartion && m_pTexture && m_pEffect;
	}
}