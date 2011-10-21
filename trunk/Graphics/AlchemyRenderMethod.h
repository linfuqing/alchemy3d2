#pragma once

#include "AlchemyRenderMethodInterface.h"

namespace alchemy
{
	class CSceneManager;
	class CRenderMethod :
		public CHighLevelGraphicsResource, public CRenderMethodInterface
	{
		friend class CSceneManager;
	public:
		typedef enum
		{
			WORLD_MATRIX = 0,
			VIEW_MATRIX,
			PROJECTION_MATRIX,

			WORLD_VIEW_MATRIX,
			VIEW_PROJECTION_MATRIX,
			WORLD_VIEW_PROJECTION_MATRIX,

			INVERSE_WORLD_MATRIX,

			//INVERSE_VIEW_MATRIX,
			//INVERSE_PROJECTION_MATRIX,

			//INVERSE_WORLD_VIEW_MATRIX,
			//INVERSE_VIEW_PROJECTION_MATRIX,
			//INVERSE_WORLD_VIEW_PROJECTION_MATRIX,

			TOTAL_MATRIX_PARAMETERS
		}MATRIXPARAMETER;

		typedef enum
		{
			CAMERA_POSITION = 0,
			CAMERA_DIRECTION,
			CAMERA_NEGATE_DIRECTION,

			CAMERA_NEAR,
			CAMERA_FAR,
			CAMERA_DISTANCE,

			CAMERA_LOCAL_POSITION,
			CAMERA_LOCAL_DIRECTION,

			CAMERA_LOCAL_DIRECTION_NEGATE,

			TOTAL_CAMERA_PARAMETERS
		}CAMERAPARAMETER;

		typedef enum
		{
			MATERIAL_AMBIENT = 0,
			MATERIAL_DIFFUSE,
			MATERIAL_SPECULAR,
			MATERIAL_EMISSIVE,
			MATERIAL_POWER,

			TOTAL_MATERIAL_PARAMETERS
		}MATERIALPARAMETER;

		typedef enum
		{
			LIGHT_AMBIENT = 0,
			LIGHT_DIFFUSE,
			LIGHT_SPECULAR,
			LIGHT_POSITION,
			LIGHT_DIRECTION,
			LIGHT_RANGE,
			LIGHT_FALLOFF,
			LIGHT_ATTENUATION0,
			LIGHT_ATTENUATION1,
			LIGHT_ATTENUATION2,
			LIGHT_THETA,
			LIGHT_PHI,

			LIGHT_COS_THETA,

			LIGHT_LOCAL_POSITION,
			LIGHT_LOCAL_DIRECTION,

			LIGHT_WORLD_VIEW_PROJECT_MATRIX,

			TOTAL_LIGHT_PARAMETERS
		}LIGHTPARAMETER;

		typedef enum
		{
			CONSTANT_ZERO = 0,
			CONSTANT_ONE,
			CONSTANT_HALF_ONE,
			CONSTANT_TWO,
			CONSTANT_FOUR,

			TOTAL_CONSTANT_PARAMETERS
		}CONSTANTPARAMETER;

		typedef enum
		{
			TEXTURE_WIDTH = 0,
			TEXTURE_HEIGHT,

			TOTAL_TEXTURE_PARAMETERS
		}TEXTUREPARAMETER;

		typedef struct
		{
			CEffectInterface::EFFECTTYPE Type;
			UINT uAlphaIndex;
			UINT uBetaIndex;
		}PARAMETERINFO, * LPPARAMETERINFO;

		typedef struct
		{
			PARAMETERINFO Parameters[CRenderMethodInterface::TOTAL_PARAMETERS];
			PARAMETERINFO ArrayParameters[CRenderMethodInterface::TOTAL_ARRAY_PARAMETERS];

			PARAMETERINFO MatrixParameters[TOTAL_MATRIX_PARAMETERS];
			PARAMETERINFO CameraParameters[TOTAL_CAMERA_PARAMETERS];
			PARAMETERINFO MaterialParameters[TOTAL_MATERIAL_PARAMETERS];
			PARAMETERINFO ConstantParameters[TOTAL_CONSTANT_PARAMETERS];

			PARAMETERINFO (*pLightParameters)[TOTAL_LIGHT_PARAMETERS];
			PARAMETERINFO (*pTextureParameters)[TOTAL_TEXTURE_PARAMETERS];

			PARAMETERINFO* pTextures;

			UINT uParameterFlag;
			UINT uArrayParameterFlag;
			UINT uMatrixParameterFlag;
			UINT uCameraParameterFlag;
			UINT uMaterialParameterFlag;
			UINT uConstantParameterFlag;

			PUINT puLightParameterFlag;
			PUINT puTextureParameterFlag;

			UINT uLightCount;
			UINT uTextureCount;

			CEffectCode::LPELEMENTPARAMETER pVertexElementParameters;
			UINT uVertexElementParameterCount;
		}PARAMETERLIST, * LPPARAMETERLIST;

		CRenderMethod(void);
		~CRenderMethod(void);

		static CBuffer& CreateParameterListBuffer(const PARAMETERLIST* pParameterList, UINT uParameterListCount);
		static void DestroyParameterList(LPPARAMETERLIST pParameterList, UINT uParameterListCount);

		void Destroy();

		bool SetParameter(CRenderMethodInterface::PARAMETER Index, const FLOAT* pfData);

		bool SetArray(CRenderMethodInterface::ARRAYPARAMETER Index, const void* pData, UINT uCount, bool bIsPointer);

		//bool SetWorldMatrix(const CMatrix4x4& Matrix);
		//bool SetCamera(const CCamera& Camera);
		//bool SetMaterial(const CMaterial& Material);
		bool SetLight(const CLight& Light, UINT uIndex);

		bool Activate(UINT uPass);
		//bool CommitTransform();

		bool SetParameters(const CMatrix4x4& WorldMatrix, const CCamera& Camera, const CMaterial& Material);

		CEffect& GetEffect();

		const CEffectCode::ELEMENTPARAMETER* GetVertexElementParameters(UINT uIndex, PUINT puOutCount = ALCHEMY_NULL)const;
	private:
		LPPARAMETERLIST m_pParameterList;

		CBuffer* m_pParameterListBuffer;

		CEffectCode::CEffectParameterHandlerInterface* m_pEffectParameterHandler;

		CEffect* m_pEffect;

		UINT m_uPass;
	};

	ALCHEMY_INLINE CEffect& CRenderMethod::GetEffect()
	{
		return *m_pEffect;
	}
}
