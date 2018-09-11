#include "AlchemyVector4.h"

using namespace alchemy;

CVector4::CVector4(FLOAT x, FLOAT y, FLOAT z, FLOAT w) :
CFloat4(x, y, z, w)
{
}

CVector4::CVector4(const CFloat4& Float) :
CFloat4(Float.x, Float.y, Float.z, Float.w)
{
}

CVector4::~CVector4(void)
{
}
