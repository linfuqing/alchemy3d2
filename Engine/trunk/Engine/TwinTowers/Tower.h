#pragma once

#include "../Graphics/AlchemySceneNode.h"
#include "../Graphics/AlchemySceneNodeFactory.h"

namespace alchemy
{
	class CFloor;
	class CTowerLoadAllListener;
	class CFloorLoadAllListener;
	class CTower : 
		public EVENTDISPATCHER
	{
		friend class CTowerLoadAllListener;
		friend class CFloorLoadAllListener;
	public:
		ALCHEMY_BEGIN_EVENT_ENUM
			TOWER_LOADED_COMPELTE, 
			FLOOR_LOADED_COMPELTE
		ALCHEMY_END_EVENT_ENUM

		typedef struct  
		{
			PCHAR pcTypePath;
			PCHAR* ppIndexPath;
			UINT uIndexCount;
		}FLOORPATH, * LPFLOORPATH;

		class CFloorEvent : 
			public IEventDispatcher::CEvent
		{
		public:
			CFloorEvent(UINT uFloorIndex, UINT uType, bool bIsBubbles = true);
			~CFloorEvent();

			UINT GetFloorIndex()const;
		private:
			UINT m_uFloorIndex;
		};

		class CFloorNodeFactory : 
			public CSceneNodeFactory
		{
		public:
			CFloorNodeFactory();
			~CFloorNodeFactory();

			void PopSceneObjects(CVector<CSceneObject*>& SceneObjects);

			CSceneObject& CreateSceneObject();

		private:
			CVector<CSceneObject*> m_SceneObjects;
		};

		CTower(void);
		~CTower(void);

		bool Load(FLOAT uFirstHeight, FLOAT uFloorHeight, const CHAR* pcFirstFloorPath, const CHAR* pcFoundationPath, const FLOORPATH* pFloorPaths, UINT uFloorCount);

		bool LoadFloor(UINT uType);

		void Destroy();

	private:
		typedef struct
		{
			FLOORPATH Path;
			CSceneNode** ppSceneNodes;
			CGraphicsResource::HANDLE* pTextures;
		}FLOOR, * LPFLOOR;

		FLOAT m_fHeight;
		FLOAT m_fFloorHeight;

		CSceneNode m_SceneNode;
		CSceneNode* m_pFirstFloor;
		CSceneNode* m_pFoundation;
		CSceneManager* m_pSceneManager;
		CGraphicsResource::HANDLE m_Effect;
		IRenderMethod* m_pRenderMethod;

		CFloorNodeFactory m_FloorNodeFactory;

		LPFLOOR m_pFloor;
		UINT m_uFloorCount;

		CVector<CFloor*> m_Floors;
	};

	ALCHEMY_INLINE UINT CTower::CFloorEvent::GetFloorIndex()const
	{
		return m_uFloorIndex;
	}
}