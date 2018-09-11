#pragma once

#include "AlchemyStaticObject.h"

namespace alchemy
{
	struct IMeshProcessor;

	class CSceneObject :
		public CStaticObject
	{
		friend class CSceneManager;
	public:
		CSceneObject(void);
		~CSceneObject(void);

		CMesh* GetSourceMesh();

		void SetMeshProcessor(IMeshProcessor* pMeshProcessor);

		IMeshProcessor* GetMeshProcessor();

		CSceneNode& Clone(ISceneNodeFactory& SceneNodeFactory)const;

		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);
		
		void Destroy();

	protected:
		CMesh* _ConvertMesh(CMesh* pSourceMesh);
	private:
		CMesh*			m_pSourceMesh;
		IMeshProcessor* m_pMeshProcessor;
	};
	
	ALCHEMY_INLINE CMesh* CSceneObject::GetSourceMesh()
	{
		return m_pSourceMesh;
	}

	ALCHEMY_INLINE IMeshProcessor* CSceneObject::GetMeshProcessor()
	{
		return m_pMeshProcessor;
	}
}