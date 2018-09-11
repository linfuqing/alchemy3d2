#pragma once

#include "AlchemyRenderMethodInterface.h"
#include "AlchemyHighLevelGraphicsResource.h"
#include "AlchemyRenderStateInterface.h"

namespace alchemy
{
	class CSceneManager;
	//class IRenderState;
	class CRenderMethod :
		public CHighLevelGraphicsResource, public IRenderMethod
	{
		friend class CSceneManager;
	public:
		/*typedef enum
		{
			WORLD_MATRIX = 0,
			VIEW_MATRIX,
			PROJECTION_MATRIX,

			WORLD_VIEW_MATRIX,
			VIEW_PROJECTION_MATRIX,
			WORLD_VIEW_PROJECTION_MATRIX,

			INVERSE_WORLD_MATRIX,
			INVERSE_VIEW_MATRIX,
			INVERSE_PROJECTION_MATRIX,

			//INVERSE_WORLD_VIEW_MATRIX,
			//INVERSE_VIEW_PROJECTION_MATRIX,
			//INVERSE_WORLD_VIEW_PROJECTION_MATRIX,

			TOTAL_MATRIX_PARAMETERS
		}MATRIXPARAMETER;*/

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

			CAMERA_VIEW_MATRIX,
			CAMERA_PROJECTION_MATRIX,

			CAMERA_WORLD_VIEW_MATRIX,
			CAMERA_VIEW_PROJECTION_MATRIX,
			CAMERA_WORLD_VIEW_PROJECTION_MATRIX,

			//CAMERA_INVERSE_WORLD_MATRIX,
			CAMERA_INVERSE_VIEW_MATRIX,
			CAMERA_INVERSE_PROJECTION_MATRIX,

			TOTAL_CAMERA_PARAMETERS
		}CAMERAPARAMETER;

		typedef enum
		{
			MATERIAL_AMBIENT = 0,
			MATERIAL_DIFFUSE,
			MATERIAL_SPECULAR,
			MATERIAL_EMISSIVE,
			MATERIAL_POWER,

			MATERIAL_INVERSE_255_POWER,

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
			LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA,

			LIGHT_LOCAL_POSITION,
			LIGHT_LOCAL_DIRECTION,

			LIGHT_VIEW_POSITION,
			LIGHT_VIEW_DIRECTION,

			LIGHT_LOCAL_DIRECTION_NEGATE,

			LIGHT_VIEW_DIRECTION_NEGATE,

			//LIGHT_WORLD_VIEW_PROJECT_MATRIX,

			TOTAL_LIGHT_PARAMETERS
		}LIGHTPARAMETER;

		typedef enum
		{
			CONSTANT_ZERO = 0,
			CONSTANT_ONE,
			CONSTANT_HALF_ONE,
			CONSTANT_TWO,
			CONSTANT_FOUR,
			CONSTANT_255,
			CONSTANT_INVERSE_255,
			CONSTANT_FLOAT_TO_RGBA_CODE,
			CONSTANT_RGBA_TO_FLOAT_CODE,

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
			IEffect::EFFECTTYPE Type;
			UINT uAlphaIndex;
			UINT uBetaIndex;
			//UINT uOpcodeIndex;
		}PARAMETERINFO, * LPPARAMETERINFO;

		typedef struct
		{
			PARAMETERINFO Parameters[IRenderMethod::TOTAL_PARAMETERS];
			PARAMETERINFO ArrayParameters[IRenderMethod::TOTAL_ARRAY_PARAMETERS];

			PARAMETERINFO MaterialParameters[TOTAL_MATERIAL_PARAMETERS];
			PARAMETERINFO ConstantParameters[TOTAL_CONSTANT_PARAMETERS];

			PARAMETERINFO (*pCameraParameters)[TOTAL_CAMERA_PARAMETERS];
			PARAMETERINFO (*pLightParameters)[TOTAL_LIGHT_PARAMETERS];
			PARAMETERINFO (*pTextureParameters)[TOTAL_TEXTURE_PARAMETERS];

			PARAMETERINFO* pTextures;
			
			IEffect::SAMPLERDECLARATION* pSamplerDeclaration;

			UINT uParameterFlag;
			UINT uArrayParameterFlag;
			UINT uMaterialParameterFlag;
			UINT uConstantParameterFlag;

			PUINT puCameraParameterFlag;
			PUINT puLightParameterFlag;
			PUINT puTextureParameterFlag;

			UINT uCameraCount;
			UINT uLightCount;
			UINT uTextureCount;
		}PARAMETERLIST, * LPPARAMETERLIST;

		ALCHEMY_INTERFACE(IShaderParametersHandler)
		{
			virtual const CRenderMethod::PARAMETERLIST* GetParameterlist(UINT uIndex)const = 0;

			virtual bool SetParameters(IEffect& Effect, UINT uIndex, bool bIsActivate) = 0;
			virtual bool SetParameters(const void* pValue, UINT uBufferIndex, UINT uSize) = 0;
			virtual bool SetParameters(const FLOAT* pfValues, UINT uBufferIndex, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount) = 0;
			virtual bool SetParameters(const PFLOAT* ppfValues, UINT uBufferIndex, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount) = 0;
		};
		
		ALCHEMY_INTERFACE(IRenderStateHandler)
		{
			virtual bool SetRenderState(IRenderState& RenderState) = 0;
		};
		
		class CRenderStateHandler :
			public CSmartClass<IRenderStateHandler>
		{
		public:
			CRenderStateHandler();
			~CRenderStateHandler();
			
			bool Create(const IRenderState::RENDERSTATEINFO* pRenderStateInfo, UINT uCount);
			
			bool SetRenderState(IRenderState& RenderState);
			
		private:
			IRenderState::LPRENDERSTATEINFO m_pRenderStateInfos;
			UINT m_uRenderStateInfoCount;
		};

		CRenderMethod(void);
		~CRenderMethod(void);

		static CBuffer& CreateParameterListBuffer(const PARAMETERLIST* pParameterList, UINT uParameterListCount);
		static void DestroyParameterList(LPPARAMETERLIST pParameterList, UINT uParameterListCount);

		void Destroy();
		
		const PARAMETERLIST* GetParameterList()const;
		UINT GetParameterListCount()const;

		UINT GetTextureCount(UINT uPass, UINT uRenderTargetIndex)const;

		const UINT* GetTextureInfo(UINT uPass, UINT uRenderTargetIndex, UINT& uCount);

		bool SetParameter(IRenderMethod::PARAMETER Index, const FLOAT* pfData);

		bool SetArray(IRenderMethod::ARRAYPARAMETER Index, const void* pData, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount, bool bIsPointer);

		bool SetCamera(const CCamera& Camera, UINT uIndex, bool bIsSetWorldMatrixOnly);

		bool SetLight(const CLight& Light, UINT uIndex, bool bIsSetWorldMatrixOnly);

		bool Activate(UINT uPass, UINT uRenderTargetIndex);

		//bool SetParameters(const CMatrix4x4& WorldMatrix, const CCamera& Camera, const CMaterial* pMaterial);
		bool CommitChange(const MATRIX4X4* pWorldMatrix, const CMaterial* pMaterial);

		bool SetWorldMatrix(const MATRIX4X4& Matrix);

		bool SetMaterial(const CMaterial& Material);

		ALCHEMY_IMPLEMENT_SUPER_INTERFACE(CGraphicsResource)
	private:
		LPPARAMETERLIST m_pParameterList;
		CBuffer* m_pParameterListBuffer;
 
		PUINT m_puRenderTargetIndexCount;
		PUINT m_puRenderTargetIndexOffset;
		PUINT m_puTextureCount;

		UINT**	m_ppTextureAlphaIndexes;

		UINT m_uParameterListIndex;
		UINT m_uPassCount;

		IShaderParametersHandler* m_pShaderParametersHandler;
		IRenderStateHandler* m_pRenderStateHandler;

		CMatrix4x4 m_WorldMatrix;
		CMatrix4x4 m_InverceWorldMatrix;
		CMatrix4x4 m_TransposeWorldMatrix;
		bool m_bIsInverceWorldMatrix;
		bool m_bIsTransposeWorldMatrix;
		bool m_bIsActivate;
	};
	
	ALCHEMY_INLINE const CRenderMethod::PARAMETERLIST* CRenderMethod::GetParameterList()const
	{
		return m_pParameterList;
	}

	ALCHEMY_INLINE UINT CRenderMethod::GetParameterListCount()const
	{
		if(m_puRenderTargetIndexOffset && m_puRenderTargetIndexCount && m_uPassCount)
		{
			UINT uFinalPassIndex = m_uPassCount - 1;
			return m_puRenderTargetIndexOffset[uFinalPassIndex] + m_puRenderTargetIndexCount[uFinalPassIndex];
		}

		return 0;
	}

	ALCHEMY_INLINE UINT CRenderMethod::GetTextureCount(UINT uPass, UINT uRenderTargetIndex)const
	{
		if(m_puTextureCount && m_puRenderTargetIndexOffset && m_puRenderTargetIndexCount && uPass < m_uPassCount && m_puRenderTargetIndexCount[uPass] > uRenderTargetIndex)
			return m_puTextureCount[m_puRenderTargetIndexOffset[uPass] + uRenderTargetIndex];

		return 0;
	}
}
