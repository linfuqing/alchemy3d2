#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashProgram3D.h"
#include "AlchemyFlashTexture.h"
#include "AlchemyFlashContext3D.h"

using namespace alchemy;

CFlashProgram3D::CFlashProgram3D(void)
{
}

CFlashProgram3D::~CFlashProgram3D(void)
{
}

bool CFlashProgram3D::_Create()
{
	static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType() );

	return true;
}

#ifdef AS3_H
AS3TYPE CFlashProgram3D::Init()
{
	return AS3_Array("AS3ValType, AS3ValType", m_pCode, m_pAdditionalCode);
}
#endif