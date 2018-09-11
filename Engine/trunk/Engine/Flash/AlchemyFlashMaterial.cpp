#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashMaterial.h"
#include "AlchemyFlashSceneManager.h"

using namespace alchemy;

CFlashMaterial::CFlashMaterial(void)
{

}

CFlashMaterial::~CFlashMaterial(void)
{

}

#ifdef AS3_H
AS3TYPE CFlashMaterial::Init()
{
	return AS3_Array("PtrType, PtrType, PtrType, PtrType, PtrType", &m_Ambient, &m_Diffuse, &m_Specular, &m_Emissive, &m_fPower);
}
#endif

bool CFlashMaterial::_Create()
{
	static_cast<CFlashSceneManager&>( GetGraphicsResourcePool() ).CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType(), GetResourceHandle() );

	return true;
}