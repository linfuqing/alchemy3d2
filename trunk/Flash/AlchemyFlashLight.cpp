#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashLight.h"
#include "AlchemyFlashSceneManager.h"

using namespace alchemy;
CFlashLight::CFlashLight(void)
{

}

CFlashLight::~CFlashLight()
{

}

#ifdef AS3_H
AS3TYPE CFlashLight::Init()
{
	return AS3_Array("PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType",&m_Type,&m_Ambient,&m_Diffuse,
		&m_Specular,&m_Position,&m_Direction,&m_fRange,&m_fFalloff,&m_fAttenuation0,&m_fAttenuation1,&m_fAttenuation2,&m_fTheta,&m_fPhi);
}
#endif

bool CFlashLight::_Create()
{
	static_cast<CFlashSceneManager&>( GetGraphicsResourcePool() ).CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType() );

	return true;
}