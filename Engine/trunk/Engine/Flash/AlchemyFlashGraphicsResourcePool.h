#pragma once

#include "AlchemyFlashObject.h"
#include "../Graphics/AlchemyGraphicsResource.h"

namespace alchemy
{
	class CFlashGraphicsResourcePool :
		public CFlashObject
	{
	public:
		CFlashGraphicsResourcePool(void);
		~CFlashGraphicsResourcePool(void);

		void CallBackToCreate(const void* pObject, const void* pInterface, CGraphicsResource::TYPE Type, CGraphicsResource::HANDLE Handle);

#ifdef AS3_H
	protected:
		AS3TYPE m_CreateResourceMethod;
		AS3TYPE m_DestroyResourceMethod;
#endif
	};
}