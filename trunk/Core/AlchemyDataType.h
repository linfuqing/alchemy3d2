#ifndef ALCHEMY_DATAYPE_H
#define ALCHEMY_DATAYPE_H

///
// 类型定义
///
namespace alchemy
{

#ifdef _UNICODE
	typedef wchar_t CHAR, * PCHAR;

#ifndef ALCHEMY_TEXT
	#define ALCHEMY_TEXT(a) L ## a
#endif

#else
	typedef char CHAR, * PCHAR;

#ifndef ALCHEMY_TEXT
	#define ALCHEMY_TEXT(a) a
#endif

#endif

	class CNull
	{
	public:
		CNull() {}
		~CNull() {}

		template<class TYPE>
		operator TYPE*() const
		{ 
			return 0; 
		} 

		template<class CLASS, class TYPE>
		operator TYPE CLASS::*() const
		{ 
			return 0; 
		}

	private:
		void operator&() const;

	};

	const CNull g_NULL;

#define ALCHEMY_NULL g_NULL
#define ALCHEMY_TRUE  1
#define ALCHEMY_FALSE 0

#ifdef __VERSION__
#define ALCHEMY_INLINE inline __attribute__((always_inline))
#else
#ifdef _MSC_VER
#define ALCHEMY_INLINE __forceinline
#else
#define ALCHEMY_INLINE inline
#endif
#endif

	typedef char               INT8  ,   * PINT8;
	typedef short              INT16 ,   * PINT16;
	typedef long               INT32 ,   * PINT32;
	typedef long long          INT64 ,   * PINT64;

	typedef int                INT  ,    * PINT;
	typedef INT64              MAXINT,   * PMAXINT;

	typedef unsigned char      UINT8 ,   * PUINT8;
	typedef unsigned short     UINT16,   * PUINT16;
	typedef unsigned long      UINT32,   * PUINT32;
	typedef unsigned long long UINT64,   * PUINT64;

	typedef unsigned int       UINT,     * PUINT;
	typedef UINT64             MAXUINT,  * PMAXUINT;

	typedef float              FLOAT32,  * PFLOAT32;
	typedef double             FLOAT64,  * PFLOAT64;

	typedef float              FLOAT,    * PFLOAT;
	//typedef FLOAT64            MAXFLOAT, * PMAXFLOAT;

	typedef UINT8              VOID,     * PVOID;
}

#endif