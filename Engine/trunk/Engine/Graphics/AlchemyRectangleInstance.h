#pragma once

#include "AlchemyGeometryInstance.h"

#include "../Math/AlchemyFloat2.h"
#include "../Math/AlchemyFloat4.h"

namespace alchemy
{
	class CRectangleInstance : 
		public CGeometryInstance
	{
	public:
		typedef enum
		{
			NORMAL,
			TRANSFORM
		}FORMAT;

		class CSprite
		{
			friend class CRectangleInstance;
		public:
			CSprite(CRectangleInstance& RectangleInstance, UINT uTransformIndex);
			~CSprite();

			bool SetPosition(FLOAT x, FLOAT y);
			bool SetTransform(const FLOAT* pfValues);

			bool AddChild(CSprite& Sprite, UINT uCount = 1);
			bool RemoveChild(CSprite& Sprite);

		private:
			UINT __GetTransformIndex();
			bool __SetTransform();

			CRectangleInstance& m_RectangleInstance;
			UINT m_uTransformIndex;
			UINT m_uDepth;
			UINT m_uChildrenCount;

			FLOAT2 m_Position;
			FLOAT4 m_Transform;

			CSprite* m_pParent;
			CSprite* m_pChild;
			CSprite* m_pNext;
			CSprite* m_pPrevious;
		};

		CRectangleInstance(void);
		~CRectangleInstance(void);

		bool Create(
			FLOAT fWidth, 
			FLOAT fHeight,
			FORMAT Format, 
			CGraphicsResource::HANDLE Effect, 
			IRenderMethod* pRenderMethod, 
			CGraphicsResource::HANDLE Texture, 
			UINT uRenderPass = 0, 
			CSceneManager* pSceneManager = ALCHEMY_NULL);

		CSprite** SetInstances(UINT uInstanceCount, FORMAT Format);

		CSprite& CreateInstance();

		CSprite* Test(FLOAT x, FLOAT y)const;

		void Update();
	private:
		typedef struct  
		{
			UINT uTransformIndex;
			CSprite* pHead;
		}DEPTHTRANSFORMINDEX, * LPDEPTHTRANSFORMINDEX;

		FLOAT m_fWidth;
		FLOAT m_fHeight;

		CVector<CSprite*> m_Sprites;
		UINT m_uInstanceCount;
		//CSprite** m_ppSprites;
		//UINT m_uSpriteCount;
		CVector<DEPTHTRANSFORMINDEX> m_DepthTransformIndices;
		CSprite* m_pDirtySprite;
		FORMAT m_Format;
		UINT m_uPositionXInfoIndex;
		UINT m_uPositionYInfoIndex;
		UINT m_uScaleXInfoIndex;
		UINT m_uScaleYInfoIndex;
		UINT m_uTransformInfoIndex;
	};

	ALCHEMY_INLINE UINT CRectangleInstance::CSprite::__GetTransformIndex()
	{
		UINT uTransformIndex = m_uTransformIndex;
		if(m_uDepth) 
		{
			const DEPTHTRANSFORMINDEX* pDepthTransformIndex =  m_RectangleInstance.m_DepthTransformIndices.Get(m_uDepth - 1);
			if(!pDepthTransformIndex)
				return m_RectangleInstance.m_Sprites.GetLength();

			uTransformIndex += pDepthTransformIndex->uTransformIndex;
		}

		return uTransformIndex;
	}
}