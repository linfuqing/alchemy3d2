#pragma once
//#include "alchemygraphicsbuffer.h"
#include "AlchemyGraphicsResource.h"

namespace alchemy
{
	class CRenderSystem;
	class CIndexBuffer :
		public CGraphicsResource//, public CGraphicsBuffer
	{
		friend class CRenderSystem;
	public:
		typedef enum
		{
			BIT16 = 1,
			BIT32 = 2
		}FORMAT;

		typedef enum
		{
			TRIANGLE_LIST = 0,
			TRIANGLE_STRIP,
			TRIANGLE_FAN
		}TRIANGLETYPE;

		CIndexBuffer(void);
		~CIndexBuffer(void);

		FORMAT GetFormat()const;

		static UINT GetTriangleCount(UINT uIndexCount, TRIANGLETYPE Type);

		virtual PVOID Lock(UINT uStart, UINT uEnd, UINT uFlag) = 0;
		virtual PVOID Lock(                        UINT uFlag) = 0;
		virtual bool  Unlock(                                ) = 0;

	private:
		FORMAT m_Format;
	};

	ALCHEMY_INLINE CIndexBuffer::FORMAT CIndexBuffer::GetFormat()const
	{
		return m_Format;
	}
}