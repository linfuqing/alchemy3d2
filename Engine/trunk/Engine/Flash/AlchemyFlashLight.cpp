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
	return AS3_Array("PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType,PtrType",&Type,&Ambient,&Diffuse,
		&Specular,&Position,&Direction,&fRange,&fFalloff,&fAttenuation0,&fAttenuation1,&fAttenuation2,&fTheta,&fPhi);
}
#endif

bool CFlashLight::_Create()
{
	static_cast<CFlashSceneManager&>( GetGraphicsResourcePool() ).CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType(), GetResourceHandle() );

	return true;
}