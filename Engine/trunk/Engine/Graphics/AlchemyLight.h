#pragma once

#include "AlchemyGraphicsBuffer.h"
#include "AlchemyHighLevelGraphicsResource.h"
#include "AlchemyRGBColor.h"

#include "../Math/AlchemyFloat3.h"
#include "../Math/AlchemyFloat4.h"

namespace alchemy
{
	typedef enum
	{
		POINT_LIGHT = 0,
		SPOT_LIGHT,
		DIRECTIONAL_LIGHT
	}LIGHTTYPE;

	typedef struct  
	{
		LIGHTTYPE Type;   
		FLOAT4    Ambient; 
		FLOAT4    Diffuse; 
		FLOAT4    Specular; 
		FLOAT4    Position;
		FLOAT3    Direction; 
		FLOAT     fRange;
		FLOAT     fFalloff;
		FLOAT     fAttenuation0; 
		FLOAT     fAttenuation1;
		FLOAT     fAttenuation2;
		FLOAT     fTheta;
		FLOAT     fPhi;
	}LIGHT;

	class CSceneManager;
	class CLight :
		public CHighLevelGraphicsResource, public LIGHT
	{
		friend class CSceneManager;

	public:

		CLight(void);
		~CLight(void);

		//LPTEXTURE        GetShadowMap()const;

		//void GetShadowMapInfo(FLOAT& fFOV, FLOAT& fNear, FLOAT& fFar) const;
		
		///bool BuildShadowMap(UINT uSize, FLOAT fFOV, FLOAT fNear, FLOAT fFar);

		//void Destroy();

	/*private:
		FLOAT	m_fFOV;
		FLOAT	m_fNear;
		FLOAT	m_fFar;

		LPTEXTURE m_pShadowMap;*/
	};

	/*ALCHEMY_INLINE void CLight::GetShadowMapInfo(FLOAT& fFOV, FLOAT& fNear, FLOAT& fFar) const
	{
		fFOV = m_fFOV;		fNear = m_fNear;		fFar = m_fFar;
	}*/
}
