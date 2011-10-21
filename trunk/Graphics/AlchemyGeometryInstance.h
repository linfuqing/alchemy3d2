#pragma once

#include "AlchemySceneNode.h"
#include "AlchemySceneManager.h"
#include "AlchemyProgramInstance.h"

namespace alchemy
{
	class CGeometryInstance :
		public CSceneNode
	{
		typedef enum
		{
			MAXINUM_INDEX_COUNT = 65536,
		}CONSTANTS;

		typedef struct  
		{
			CMesh* pMesh;
			CMesh::ATTRIBUTERANGE AttributeRange;
		}GEOMETRY, * LPGEOMETRY;

		typedef struct  
		{
			CVertexDeclaration::ELEMENTTYPE  Type;
			CVertexDeclaration::ELEMENTUSAGE Usage;
		}VERTEXELEMENTKEY;

		typedef struct VERTEXELEMENTKEYLESS
		{
			bool operator()(const VERTEXELEMENTKEY& Left, const VERTEXELEMENTKEY& Right)const;
		}VERTEXELEMENTKEYLESS;

		typedef struct VERTEXELEMENTKEYEQUAL
		{
			bool operator()(const VERTEXELEMENTKEY& Left, const VERTEXELEMENTKEY& Right)const;
		}VERTEXELEMENTKEYEQUAL;
	public:

		CGeometryInstance(void);
		~CGeometryInstance(void);

		bool Create(CRenderMethodInterface& RenderMethod, CGraphicsResource::HANDLE Material = 0, CGraphicsResource::HANDLE VertexDeclaration = 0);
		void Destroy();

		bool AddGeometry(
			CGraphicsResource::HANDLE Mesh, 
			const CMesh::ATTRIBUTERANGE* pAttributeRange = ALCHEMY_NULL, 
			LPMATRIX4X4 pTransform = ALCHEMY_NULL,
			const CEffectCode::ELEMENTPARAMETER* pTransformElementParameters = ALCHEMY_NULL,
			UINT uTransformElementParameterCount = 0);

		bool ApplyForRender();

		void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);
	private:

		bool __IsValuable()const;

		CVector<CMesh*> m_GeometryInstances;
		CVector<CProgramInstance*>	m_ProgramInstances;
		CRenderMethodInterface* m_pRenderMethod;
		CMaterial* m_pMaterial;
		CVertexDeclaration* m_pVertexDeclaration;
		CSceneManager* m_pSceneManager;

		CMesh* m_pCurrentInstance;
		CMesh::LPATTRIBUTERANGE m_pAttributeRange;
	};

	ALCHEMY_INLINE bool CGeometryInstance::__IsValuable()const
	{
		return m_pVertexDeclaration && m_pRenderMethod && m_pSceneManager;
	}

	ALCHEMY_INLINE bool CGeometryInstance::VERTEXELEMENTKEYLESS::operator()(const VERTEXELEMENTKEY& Left, const VERTEXELEMENTKEY& Right)const
	{
		if(Left.Type < Right.Type)
			return true;
		else if(Left.Type == Right.Type && Left.Usage < Right.Usage)
			return true;

		return false;
	}

	ALCHEMY_INLINE bool CGeometryInstance::VERTEXELEMENTKEYEQUAL::operator()(const VERTEXELEMENTKEY& Left, const VERTEXELEMENTKEY& Right)const
	{
		return Left.Type == Right.Type && Left.Usage == Right.Usage;
	}
}