#include "AlchemyLoader.h"

using namespace alchemy;

namespace alchemy
{
	class CLoaderListener : 
		public CSmartClass<IEventDispatcher::IListener>
	{
	public:
		CLoaderListener(CLoader& Parent, const CHAR** pcPaths, UINT uCount) : 
		  m_Parent(Parent), 
			  m_puDataIndices(ALCHEMY_NULL), 
			  m_uDataCount(0), 
			  m_uLoadedCount(0)
		  {
			  m_uDataCount = uCount;

			  CVector<UINT> Indices;
			  Indices.Add(0);

			  m_IndexMap.Clear();
			  CVector<UINT>* pIndices;
			  for(UINT i = 0; i < uCount; ++ i)
			  {
				  pIndices = m_IndexMap.Get(pcPaths[i]);
				  if(pIndices)
					  pIndices->Add(i);
				  else
				  {
					  Indices.Set(i, 0);

					  m_IndexMap.Insert(pcPaths[i], Indices);
				  }
			  }

			  m_uLoadedCount = 0;
		  }

		~CLoaderListener()
		{
			ALCHEMY_DEBUG_DELETE_ARRAY(m_puDataIndices);
		}

		void Handler(const IEventDispatcher::CEvent& Event)
		{
			const CLoader::CLoadEvent& LoadEvent = static_cast<const CLoader::CLoadEvent&>(Event);
			const STRING& Path = LoadEvent.GetPath();
			CVector<UINT>* pIndices = m_IndexMap.Get( LoadEvent.GetPath() );
			if(pIndices)
			{
				UINT uIndex, uDestination = LoadEvent.GetIndex();
				CVector<UINT>::CIterator Iterator = pIndices->GetIterator(0);
				while( CVector<UINT>::IsValuable(Iterator) )
				{
					uIndex = *Iterator;
					if(uIndex < m_uDataCount)
					{
						if(!m_puDataIndices)
							ALCHEMY_DEBUG_NEW(m_puDataIndices, UINT[m_uDataCount]);

						m_puDataIndices[uIndex] = uDestination;

						++ m_uLoadedCount;
					}

					++ Iterator;
				}

				pIndices->Clear();
			}

			if(m_uLoadedCount == m_uDataCount)
			{
				IEventDispatcher* pEventDispatcher = Event.GetTarget();
				if(pEventDispatcher)
				{
					UINT uType;

					if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_ALL_COMPLETE, CLoader) )
					{
						CLoader::CLoadAllEvent LoadAllEvent(m_puDataIndices, m_uDataCount, uType);

						pEventDispatcher->DispatchEvent(LoadAllEvent);
					}

					if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_COMPLETE, CLoader) )
						pEventDispatcher->RemoveEventListener(uType, *this);
				}
			}
		}

	private:
		CLoader& m_Parent;

		PUINT m_puDataIndices;
		UINT m_uDataCount;

		CMap<STRING, CVector<UINT>, STRINGLESS> m_IndexMap;

		UINT m_uLoadedCount;
	};
}

CLoader::CLoadEvent::CLoadEvent(const CLoader::DATA& Data, const CHAR* pcPath, UINT uType, bool bIsBubbles) : 
CEvent(uType, bIsBubbles), 
m_Path(pcPath), 
m_uIndex(0)
{
	m_Data.uAlpha = Data.uAlpha;
	m_Data.uBeta = Data.uBeta;

	UINT uSize = Data.uBeta ? (Data.uAlpha * Data.uBeta * 4) : Data.uAlpha;
	ALCHEMY_DEBUG_NEW(m_Data.pData, UINT8[uSize]);
	ALCHEMY_MEMORY_COPY(m_Data.pData, Data.pData, uSize);
}

CLoader::CLoadEvent::~CLoadEvent()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_Data.pData);
}

void CLoader::CLoadEvent::_SetTarget(IEventDispatcher& EventDispatcher)
{
	m_uIndex = static_cast<CLoader&>(EventDispatcher).m_DataPool.Add(m_Data);

	m_Data.pData = ALCHEMY_NULL;

	CEvent::_SetTarget(EventDispatcher);
}

CLoader::CLoadAllEvent::CLoadAllEvent(const UINT* puDataIndices, UINT uDataCount, UINT uType, bool bIsBubbles) :
CEvent(uType, bIsBubbles), 
m_puDataIndices(puDataIndices), 
m_uDataCount(uDataCount)
{

}

CLoader::CLoadAllEvent::~CLoadAllEvent()
{

}

CLoader::CLoader(void) : 
CSingleton<CLoader>(*this)
{
}

CLoader::~CLoader(void)
{
	CPool<DATA>::CIterator DataIterator = m_DataPool.GetIterator(0);
	while( CPool<DATA>::IsValuable(DataIterator) )
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(DataIterator->pData);

		++ DataIterator;
	}
}

bool CLoader::GetPathInfo(const CHAR* pcPath, STRING* pOutputLocal, STRING* pOutputName, STRING* pOutputExtension)
{
	if(pOutputLocal || pOutputName || pOutputExtension)
	{
		UINT uLastIndex = 0, uIndex = uLastIndex;
		CHAR cCurrentChar;
		while( cCurrentChar = *( pcPath + uIndex) )
		{
			if( cCurrentChar == ALCHEMY_TEXT('\\') || cCurrentChar == ALCHEMY_TEXT('/') )
				uLastIndex = uIndex;

			++ uIndex;
		}

		if( *(pcPath + uLastIndex) )
		{
			UINT uLength = uIndex;

			if(pOutputLocal)
				pOutputLocal->Assign(pcPath, 0, uLastIndex);

			if(pOutputName || pOutputExtension)
			{
				uIndex = uLastIndex + 1;
				while( cCurrentChar = *( pcPath + uIndex) )
				{
					if( cCurrentChar == ALCHEMY_TEXT('.') )
						break;

					++ uIndex;
				}

				if(pOutputName)
					pOutputName->Assign(pcPath, uLastIndex + 1, uIndex - uLastIndex);

				if(pOutputExtension)
				{
					++ uIndex;

					pOutputExtension->Assign(pcPath, uIndex, uLength - uIndex);
				}
			}

			return true;
		}
	}

	return false;
}

void CLoader::LoadAll(const CHAR** pcPaths, UINT uCount)
{
	UINT uType;
	if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_COMPLETE, CLoader) )
	{
		CLoaderListener* pListener;
		ALCHEMY_DEBUG_NEW( pListener, CLoaderListener(*this, pcPaths, uCount) );

		AddEventListener(uType, *pListener);

		pListener->Release();

		CMap<STRING, bool, STRINGLESS> PathMap;
		const CHAR* pcPath;
		bool* pbIsCommonPath;
		for(UINT i = 0; i < uCount; ++ i)
		{
			pcPath = pcPaths[i];
			pbIsCommonPath = PathMap.Get(pcPath);
			if(!pbIsCommonPath)
			{
				PathMap.Insert(pcPath, true);

				Load(pcPath);
			}
		}
	}
}