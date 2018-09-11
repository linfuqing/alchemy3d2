#include "Floor.h"

using namespace alchemy;

CFloor::CFloor(CTower& Tower, UINT uType, UINT uIndex) :
m_Tower(Tower), 
m_uType(uType), 
m_uIndex(uIndex)
{
}

CFloor::~CFloor(void)
{
}