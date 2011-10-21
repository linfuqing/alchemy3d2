#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashMesh.h"
#include "AlchemyFlashSceneManager.h"

using namespace alchemy;

CFlashMesh::CFlashMesh(void)
{

}

CFlashMesh::~CFlashMesh(void)
{

}

#ifdef AS3_H
AS3TYPE CFlashMesh::Init()
{
	return AS3_Array( "IntType",GetIndexBuffer().GetResourceHandle() );
}
#endif

bool CFlashMesh::_Create()
{
	static_cast<CFlashSceneManager&>( GetGraphicsResourcePool() ).CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType() );

	return true;
}