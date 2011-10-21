#ifndef ALCHEMY_DEBUG_H
#define ALCHEMY_DEBUG_H

#include "AlchemyDataType.h"

namespace alchemy
{
#ifndef ALCHEMY_DEBUG_ERROR
#define ALCHEMY_DEBUG_ERROR(sComment) 
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
#define ALCHEMY_DEBUG_WARNING(sComment) 
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

#ifndef ALCHEMY_DEBUG_FORCE_NEW
#define ALCHEMY_DEBUG_FORCE_NEW(Param, TypeName) \
	{\
		Param = new TypeName;\
		ALCHEMY_DEBUG_ASSERT(Param, "Memory full!");\
	}
#endif

#ifndef ALCHEMY_DEBUG__FORCE_DELETE
#define ALCHEMY_DEBUG__FORCE_DELETE(Param) \
	{\
		if(Param)\
		{\
			delete (Param);\
			Param = ALCHEMY_NULL;\
		}\
	}
#endif

#ifndef ALCHEMY_DEBUG_FORCE_DELETE_ARRAY
#define ALCHEMY_DEBUG_FORCE_DELETE_ARRAY(Param) \
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