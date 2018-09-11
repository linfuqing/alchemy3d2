#ifndef ALCHEMY_INTERFACE_H
#define ALCHEMY_INTERFACE_H

#include "AlchemyValue.h"
#include "AlchemyNull.h"

namespace alchemy
{
#define ALCHEMY_INTERFACE(Name) struct Name : public IInterface
#define ALCHEMY_INTERFACE_EXTENDS(Name) ALCHEMY_INTERFACE(Name),
#define ALCHEMY_IMPLEMENT_INTERFACE \
public: \
	UINT AddRef(void) \
	{ \
		UINT& uReferenceCount = static_cast<UINT&>(m_ReferenceCount); \
		return ++ uReferenceCount; \
	} \
	\
	UINT Release(void) \
	{ \
		UINT uReferenceCount = -- static_cast<UINT&>(m_ReferenceCount); \
		if(!uReferenceCount) \
			delete this; \
		\
		return uReferenceCount; \
	} \
	\
private: \
	CValue<UINT, 1> m_ReferenceCount;
	
#define ALCHEMY_IMPLEMENT_SUPER_INTERFACE(Class) \
	ALCHEMY_INLINE UINT AddRef(void) \
	{ \
		return Class::AddRef(); \
	} \
	\
	ALCHEMY_INLINE UINT Release(void) \
	{ \
		return Class::Release(); \
	}

#define ALCHEMY_INTERFACE_METHOD(Name, Type) virtual Type Name

	struct IInterface
	{
		virtual UINT AddRef(void) = 0;
		virtual UINT Release(void) = 0;
	};
}

#endif