#pragma once

namespace alchemy
{
	class CLoader
	{
	public:
		ALCHEMY_BEGIN_EVENT_ENUM
			LOAD_COMPLETE, 
			LOAD_ALL_COMPLETE
		ALCHEMY_END_EVENT_ENUM

		typedef struct  
		{
			UINT uAlpha;
			UINT uBeta;
			PVOID pData;
		}DATA, * LPDATA;

		class CLoadEvent : 
			public IEventDispatcher::CEvent
		{
		public:
			CLoadEvent(const DATA& Data, const CHAR* pcPath, UINT uType, bool bIsBubbles = true);
			~CLoadEvent();

			UINT GetIndex()const;
			const STRING& GetPath()const;

		protected:
			void _SetTarget(IEventDispatcher& EventDispatcher);

		private:
			DATA m_Data;
			STRING m_Path;
			UINT m_uIndex;
		};

		class CLoadAllEvent : 
			public IEventDispatcher::CEvent
		{
		public:
			CLoadAllEvent(const UINT* puDataIndices, UINT uDataCount, UINT uType, bool bIsBubbles = true);
			~CLoadAllEvent();

			const UINT* GetDataIndices()const;
			UINT GetDataCount()const;

		private:
			const UINT* m_puDataIndices;
			UINT m_uDataCount;
		};

		CLoader(void);
		~CLoader(void);

		static bool GetPathInfo(const CHAR* pcPath, STRING* pOutputLocal, STRING* pOutputName, STRING* pOutputExtension);

		const DATA* GetData(UINT uIndex)const;

		void LoadAll(const CHAR** pcPath, UINT uCount);

		virtual void Load(const CHAR* pcPath) = 0;

	private:
		CPool<DATA> m_DataPool;
	};
}