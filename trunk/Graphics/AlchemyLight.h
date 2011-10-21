#pragma once

#include "AlchemyHighLevelGraphicsResource.h"
#include "AlchemyRGBColor.h"
#include "AlchemySceneObject.h"

#include "../Core/AlchemyVector.h"

#include "../Math/AlchemyVector3.h"
#include "../Math/AlchemyVector4.h"

namespace alchemy
{
	class CSceneManager;
	class CLight :
		public CHighLevelGraphicsResource
	{
		friend class CSceneManager;

		typedef struct
		{
			CSceneObject* pSceneObject;
			CSceneObject::SHADOWTYPE ShadowType;
		}SHADOWOBJECT;
	public:
		typedef enum
		{
			POINT = 0,
			SPOT,
			DIRECTIONAL
		}TYPE;

		CLight(void);
		~CLight(void);

		TYPE             GetLightType()const;

		const CRGBColor& GetAmbient()const;
		const CRGBColor& GetDiffuse()const;
		const CRGBColor& GetSpecular()const;

		const CVector4&  GetPosition()const;
		const CVector3&  GetDirection()const;

		FLOAT            GetRange()const;
		FLOAT            GetFalloff()const;
		FLOAT            GetAttenuation0()const;
		FLOAT            GetAttenuation1()const;
		FLOAT            GetAttenuation2()const;
		FLOAT            GetTheta()const;
		FLOAT            GetPhi()const;

		LPTEXTURE        GetShadowMap()const;

		const CCamera*   GetCamera()const;
		
		bool BuildShadowMap(UINT uSize, FLOAT fFOV, FLOAT fNear, FLOAT fFar);

		bool AddShadowObject(CSceneObject& Object, CSceneObject::SHADOWTYPE Type);

		bool Render();

		void Destroy();

	protected:
		TYPE      m_Type;   
		CRGBColor m_Ambient; 
		CRGBColor m_Diffuse; 
		CRGBColor m_Specular; 
		CVector4  m_Position;
		CVector3  m_Direction; 
		FLOAT     m_fRange;
		FLOAT     m_fFalloff;
		FLOAT     m_fAttenuation0; 
		FLOAT     m_fAttenuation1;
		FLOAT     m_fAttenuation2;
		FLOAT     m_fTheta;
		FLOAT     m_fPhi;

	private:
		LPTEXTURE m_pShadowMap;
		CCamera* m_pCamera;
		CVector<SHADOWOBJECT> m_ShadowObjects;
	};

	ALCHEMY_INLINE CLight::TYPE      CLight::GetLightType()const
	{
		return m_Type;
	}

	ALCHEMY_INLINE const CRGBColor&  CLight::GetAmbient()const
	{
		return m_Ambient;
	}

	ALCHEMY_INLINE const CRGBColor&  CLight::GetDiffuse()const
	{
		return m_Diffuse;
	}

	ALCHEMY_INLINE const CRGBColor&  CLight::GetSpecular()const
	{
		return m_Specular;
	}

	ALCHEMY_INLINE const CVector4&   CLight::GetPosition()const
	{
		return m_Position;
	}

	ALCHEMY_INLINE const CVector3&   CLight::GetDirection()const
	{
		return m_Direction;
	}


	ALCHEMY_INLINE FLOAT             CLight::GetRange()const
	{
		return m_fRange;
	}

	ALCHEMY_INLINE FLOAT             CLight::GetFalloff()const
	{
		return m_fFalloff;
	}

	ALCHEMY_INLINE FLOAT             CLight::GetAttenuation0()const
	{
		return m_fAttenuation0;
	}

	ALCHEMY_INLINE FLOAT             CLight::GetAttenuation1()const
	{
		return m_fAttenuation1;
	}

	ALCHEMY_INLINE FLOAT             CLight::GetAttenuation2()const
	{
		return m_fAttenuation2;
	}

	ALCHEMY_INLINE FLOAT             CLight::GetTheta()const
	{
		return m_fTheta;
	}

	ALCHEMY_INLINE FLOAT             CLight::GetPhi()const
	{
		return m_fPhi;
	}

	ALCHEMY_INLINE LPTEXTURE        CLight::GetShadowMap()const
	{
		return m_pShadowMap;
	}

	/*ALCHEMY_INLINE const CCamera*   CLight::GetCamera()const
	{
		return m_pCamera;
	}

	ALCHEMY_INLINE CCamera*   CLight::GetCamera()
	{
		return m_pCamera;
	}*/
}
