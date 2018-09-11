#pragma once

#include "AlchemySceneObject.h"

namespace alchemy
{
	class CGeometryInstance :
		public CSceneObject
	{
	public:
		typedef enum
		{
			POSITION_X,
			POSITION_Y,
			POSITION_Z,

			SCALE_X, 
			SCALE_Y,
			SCALE_Z,

			ROTATION_X,
			ROTATION_Y,
			ROTATION_Z,

			QUATERNION, 

			MATRIX
		}TRANSFORMUSAGE;

		typedef struct  
		{
			TRANSFORMUSAGE Usage;
			UINT uOffset;
		}TRANSFORMINFO, * LPTRANSFORMINFO;

		CGeometryInstance(void);
		~CGeometryInstance(void);

		bool SetInstances(FLOAT fInstanceCount, UINT uTransformCount, const TRANSFORMINFO* pTransformInfos = ALCHEMY_NULL, UINT uTransformInfoCount = 0);
		bool SetTransform(UINT uTransformIndex, UINT uTransformInfoIndex, const FLOAT* pfValues, bool bIsKeep = false);

		bool ApplyForRender(UINT uRenderPass, UINT uRenderList);

		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

	protected:
		CMesh* _ConvertMesh(CMesh* pSourceMesh);

	private:
		void __BuildTransformOffsetTable();

		typedef struct
		{
			UINT uTransformIndex;
			UINT uTransformInfoIndex;
			const FLOAT* pfValues;
		}KEPTVALUE, * LPKEPTVALUE;

		FLOAT m_fInstanceCount;

		PFLOAT m_pfTransform;
		UINT m_uTransformStride;
		UINT m_uTransformCount;

		LPTRANSFORMINFO m_pTransformInfos;
		UINT m_uTransformInfoCount;

		FLOAT m_fHeadInstanceCount;
		FLOAT m_fTailInstanceCount;

		CMesh* m_pSourceMesh;

		CMesh* m_pHeadMesh;
		CMesh* m_pTailMesh;

		IMeshProcessor* m_pTailMeshProcessor;

		PUINT m_puTransformOffsetTable;
		CVector<KEPTVALUE> m_KeptValues;
	};
}