#pragma once

#include "AlchemyVertexDeclaration.h"
//#include "AlchemyVertexBuffer.h"
#include "AlchemyGraphicsBuffer.h"
#include "AlchemyHighLevelGraphicsResource.h"
#include "AlchemySkinInfo.h"

#include "../Core/AlchemyBuffer.h"

namespace alchemy
{
	class CSceneManager;
	class CVertex :
		public CHighLevelGraphicsResource
	{
		friend class CSceneManager;
	public:
		typedef struct
		{
			void* pData;
			CVertexDeclaration::ELEMENTTYPE ElementType;
			CVertexDeclaration::ELEMENTUSAGE ElementUsage;
			bool bIsElementData;
		}INSTANCEDATA, * LPINSTANCEDATA;
		
		CVertex(void);
		~CVertex(void);

		CVertexDeclaration& GetVertexDeclaration();

		//bool Create(UINT uNumVertices, const CVertexDeclaration::VERTEXELEMENT* pVertexElement, UINT uElementCount);

		LPVERTEXBUFFER GetVertexBuffer(UINT uIndex);

		CVertex* Clone(const CVertexDeclaration::VERTEXELEMENT* pCloneElements, UINT uCloneElementCount, const UINT* puUsage = ALCHEMY_NULL);
		CVertex* Clone(const UINT* puCloneStream, UINT uCloneStreamCount, const UINT* puUsage = ALCHEMY_NULL);
		CVertex* Clone(FLOAT fCloneInstanceCount, PUINT puNumVertices);

		CVertex* ConvertToInstancedVertex(const UINT* puInstanceIndexSegments, UINT uInstanceCount, const INSTANCEDATA* pInstanceData = ALCHEMY_NULL, UINT uInstanceDataCount = 0);
		CVertex* ConvertToInstancedVertex(FLOAT fInstanceCount, const INSTANCEDATA* pInstanceData = ALCHEMY_NULL, UINT uInstanceDataCount = 0, PUINT puNumVertices = ALCHEMY_NULL);
		
		CVertex* ConvertToBlendedVertex(
			const CSkinInfo& SkinInfo, 
			UINT uBlendWeightUsage, 
			UINT uBlendIndexUsage, 
			UINT uMatrixIndexOffset, 
			PUINT puOutputMaxVertexInfluenceCount = ALCHEMY_NULL);

		void Destroy();

		bool Activate(bool bIsActivateVertexDeclaration = true);

	private:
		CVertexDeclaration* m_pVertexDeclaration;

		//CVertexBuffer** m_ppVertexBuffer;
		LPVERTEXBUFFER* m_ppVertexBuffer;

		//UINT m_uVertexBufferCount;
	};

	ALCHEMY_INLINE CVertexDeclaration& CVertex::GetVertexDeclaration()
	{
		ALCHEMY_DEBUG_ASSERT(m_pVertexDeclaration, "Vertex declaration has not been created");

		return *m_pVertexDeclaration;
	}

	ALCHEMY_INLINE LPVERTEXBUFFER CVertex::GetVertexBuffer(UINT uIndex)
	{
		if(!m_pVertexDeclaration)
		{
			ALCHEMY_DEBUG_WARNING("Vertex declaration has not been created");

			return ALCHEMY_NULL;
		}

		if( m_pVertexDeclaration->GetStreamCount() > uIndex)
			return m_ppVertexBuffer[uIndex];

		ALCHEMY_DEBUG_WARNING("Index out of range.");

		return ALCHEMY_NULL;
	}
}