#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashRenderMethod.h"
#include "AlchemyFlashSceneManager.h"

using namespace alchemy;

CFlashRenderMethod::CFlashRenderMethod(void)
{
}

CFlashRenderMethod::~CFlashRenderMethod(void)
{
}

#ifdef AS3_H
AS3TYPE CFlashRenderMethod::Init()
{
	return AS3_Array("PtrType", static_cast<IRenderMethod*>(this));
}
#endif

bool CFlashRenderMethod::_Create()
{
	static_cast<CFlashSceneManager&>( GetGraphicsResourcePool() ).CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType(), GetResourceHandle() );

	return true;
}