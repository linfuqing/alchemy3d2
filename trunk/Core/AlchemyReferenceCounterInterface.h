#pragma once

#include "../Core/AlchemyDataType.h"

namespace alchemy
{
#define ALCHEMY_INTERFACE(Name) class Name : public CReferenceCounterInterface
#define ALCHEMY_INTERFACE_EXTENDS(Name) ALCHEMY_INTERFACE(Name),

	class CReferenceCounterInterface
	{
	public:

		CReferenceCounterInterface(void) {}
		virtual ~CReferenceCounterInterface(void) {}

		virtual INT AddRef() = 0;
		virtual INT Release() = 0;
	};
}