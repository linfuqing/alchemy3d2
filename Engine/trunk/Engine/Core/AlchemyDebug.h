#ifndef ALCHEMY_DEBUG_H
#define ALCHEMY_DEBUG_H

#include "AlchemyDataType.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#include <stdio.h>
#endif
#endif

namespace alchemy
{

#ifndef ALCHEMY_DEBUG_LOG
#ifdef _MSC_VER
#ifdef _DEBUG
#define ALCHEMY_DEBUG_LOG(sComment) printf("%s\n", sComment);
#else
#define ALCHEMY_DEBUG_LOG(sComment)
#endif
#else
#define ALCHEMY_DEBUG_LOG(sComment)
#endif
#endif

#ifndef ALCHEMY_DEBUG_ERROR
#ifdef _MSC_VER
#ifdef _DEBUG
#define ALCHEMY_DEBUG_ERROR(sComment) printf("%s\n", sComment);
#else
#define ALCHEMY_DEBUG_ERROR(sComment)
#endif
#else
#define ALCHEMY_DEBUG_ERROR(sComment)
#endif
#endif

#ifndef ALCHEMY_DEBUG_ASSERT
#define ALCHEMY_DEBUG_ASSERT(x, sComment) \
	{\
		if(!(x))\
		{\
			ALCHEMY_DEBUG_ERROR(sComment);\
		}\
	}
#endif

#ifndef ALCHEMY_DEBUG_WARNING
#ifdef _MSC_VER
#ifdef _DEBUG
#define ALCHEMY_DEBUG_WARNING(sComment) printf("%s\n", sComment);
#else
#define ALCHEMY_DEBUG_WARNING(sComment)
#endif
#else
#define ALCHEMY_DEBUG_WARNING(sComment)
#endif
#endif

#ifndef ALCHEMY_DEBUG_WARNING_ASSERT
#define ALCHEMY_DEBUG_WARNING_ASSERT(x, sComment) \
	{\
		if(!(x))\
			ALCHEMY_DEBUG_WARNING(sComment);\
	}
#endif

#ifndef ALCHEMY_DEBUG_NEW
#define ALCHEMY_DEBUG_NEW(Param, TypeName) \
	{\
		Param = new TypeName;\
		ALCHEMY_DEBUG_ASSERT(Param, "Memory full!");\
	}
#endif

#ifndef ALCHEMY_DEBUG_DELETE
#define ALCHEMY_DEBUG_DELETE(Param) \
	{\
		if(Param)\
		{\
			delete (Param);\
			Param = ALCHEMY_NULL;\
		}\
	}
#endif

#ifndef ALCHEMY_DEBUG_DELETE_ARRAY
#define ALCHEMY_DEBUG_DELETE_ARRAY(Param) \
	{\
		if(Param)\
		{\
			delete [] (Param);\
			Param = ALCHEMY_NULL;\
		}\
	}
#endif
}

#endif