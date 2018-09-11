#pragma once

#include "../Core/AlchemyEventDispatcher.h"

namespace alchemy
{
	class CTower;
	class CFloor : 
		public alchemy::EVENTDISPATCHER
	{
	public:
		CFloor(CTower& Tower, UINT uType, UINT uIndex);
		~CFloor(void);

	private:
		CTower& m_Tower;
		UINT m_uType;
		UINT m_uIndex;
	};
}