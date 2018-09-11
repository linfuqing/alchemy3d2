#pragma once

#include "../Core/AlchemyPool.h"
#include "AlchemyGraphicsResource.h"

namespace alchemy
{
	class CRenderSystem;
	class CVertexDeclaration :
		public CGraphicsResource
	{ 
		friend class CRenderSystem;
	public:
		typedef enum
		{
			FLOAT1    = sizeof(FLOAT),  // 1D float expanded to (value, 0., 0., 1.)
			FLOAT2    = 2 * FLOAT1,  // 2D float expanded to (value, value, 0., 1.)
			FLOAT3    = 3 * FLOAT1,  // 3D float expanded to (value, value, value, 1.)
			FLOAT4    = 4 * FLOAT1,  // 4D float
		}ELEMENTTYPE;

		typedef enum
		{
			POSITION,
			BLENDWEIGHT,   // 1
			BLENDINDICES,  // 2
			INSTANCEINDEX,
			NORMAL,        // 3
			PSIZE,         // 4
			TEXCOORD,      // 5
			TANGENT,       // 6
			BINORMAL,      // 7
			//TESSFACTOR,    // 8
			POSITIONT,     // 9
			COLOR,         // 10
			FOG,           // 11
			DEPTH,         // 12
			DIFFUSE,
			FACING,
			LIGHTNESS,

			UNKNOWN
		}ELEMENTUSAGE;

		typedef struct
		{
			ELEMENTTYPE  Type;
			ELEMENTUSAGE Usage;
			UINT         uUsageIndex;
			UINT         uIndex;
		}ELEMENT, * LPELEMENT;

		typedef struct
		{
			UINT         uStream;
			UINT         uOffset;
			ELEMENTTYPE  Type;
			ELEMENTUSAGE Usage;
			UINT         uIndex;
		}VERTEXELEMENT, * LPVERTEXELEMENT;

		CVertexDeclaration(void);
		~CVertexDeclaration(void);

		static bool IsValuableMeshDeclaration(const VERTEXELEMENT* pVertexElements, UINT uElementCount);

		static UINT GetVertexElementMaxinumStream(const VERTEXELEMENT* pVertexElements, UINT uElementCount);
		static UINT GetVertexElementStreamCount(const VERTEXELEMENT* pVertexElements, UINT uElementCount);
		static UINT GetVertexElementStreamStrideOf(UINT uStream, const VERTEXELEMENT* pVertexElements, UINT uElementCount);

		static UINT FindVertexElement(ELEMENTUSAGE Usage, UINT uIndex, const VERTEXELEMENT* pVertexElements, UINT uElementCount);
		static UINT FindVertexElement(ELEMENTUSAGE Usage, ELEMENTTYPE Type, UINT uIndex, const VERTEXELEMENT* pVertexElements, UINT uElementCount);
		static UINT FindVertexElement(ELEMENTUSAGE Usage, ELEMENTTYPE Type, UINT uOffset, UINT uStream, const VERTEXELEMENT* pVertexElements, UINT uElementCount);

		static void SortVertexElement(LPVERTEXELEMENT pOutput, const VERTEXELEMENT* pInput, UINT uCount);

		const VERTEXELEMENT* GetVertexElements()const;
		UINT GetVertexElementCount()const;

		const UINT* GetElementEndOfStream()const;
		UINT GetStreamCount()const;

		CVertexDeclaration* Clone(const ELEMENT* pElements, UINT uElementCount);

		void Destroy();

	private:
		VERTEXELEMENT* m_pVertexElements;
		UINT m_uElementCount;

		UINT m_uStreamCount;
		PUINT m_puElementEndOfStream;
	};

	ALCHEMY_INLINE const CVertexDeclaration::VERTEXELEMENT* CVertexDeclaration::GetVertexElements()const
	{
		return m_pVertexElements;
	}

	ALCHEMY_INLINE UINT CVertexDeclaration::GetVertexElementCount()const
	{
		return m_uElementCount;
	}

	ALCHEMY_INLINE const UINT* CVertexDeclaration::GetElementEndOfStream()const
	{
		return m_puElementEndOfStream;
	}

	ALCHEMY_INLINE UINT CVertexDeclaration::GetStreamCount()const
	{
		return m_uStreamCount;
	}
}
