#pragma once

#include "../Core/AlchemyDebug.h"
#include "../Core/AlchemyBitFlag.h"
#include "../Core/AlchemyUtils.h"
#include "../Graphics/AlchemyGraphicsBuffer.h"

namespace alchemy
{
	template<typename T>
	class CFlashGraphicsBuffer :
		public CGraphicsBuffer<T>
	{
	public:

		CFlashGraphicsBuffer(void) :
		  m_bIsActivate(false)
		{
		}

		~CFlashGraphicsBuffer(void) {}

		static bool IsNoData(UINT uUsage)
		{
			return ALCHEMY_TEST_BIT(uUsage, CGraphicsBuffer<T>::WRITE_ONLY) && !ALCHEMY_TEST_BIT(uUsage, CGraphicsBuffer<T>::DYNAMIC);
		}

		ALCHEMY_INLINE bool IsActivate()const
		{
			return m_bIsActivate;
		}

		ALCHEMY_INLINE void Activate()
		{
			m_bIsActivate = true;
		}

		ALCHEMY_INLINE void Reset()
		{
			m_bIsActivate = false;
		}
		
		void* Lock(UINT uStart, UINT uEnd, UINT uFlag)
		{
			return ALCHEMY_NULL;
		}
		
		void* Lock(                        UINT uFlag)
		{
			return ALCHEMY_NULL;
		}
		
		bool  Unlock(                                )
		{
			return false;
		}

	private:
		bool m_bIsActivate;
	}; 
}
