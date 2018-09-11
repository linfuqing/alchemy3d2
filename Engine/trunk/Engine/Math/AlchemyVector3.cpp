#include "AlchemyVector3.h"
#include "AlchemyMatrix4x4.h"

using namespace alchemy;

FLOAT alchemy::GetLength(const FLOAT3& Value)
{
	return sqrtf(Value.x * Value.x + Value.y * Value.y + Value.z * Value.z);
}

FLOAT alchemy::Normalize(FLOAT3& Output, const FLOAT3 Value)
{
	FLOAT fLength = GetLength(Value);

	if(fLength)
	{
		FLOAT fInverceLength = 1.0f / fLength;
		
		Output.x = Value.x * fInverceLength;
		Output.y = Value.y * fInverceLength;
		Output.z = Value.z * fInverceLength;
	}
	else
		Copy(Output, Value);

	return fLength;
}

FLOAT CVector3::Distance(const FLOAT3& lhs, const FLOAT3 rhs)
{
	FLOAT fX = lhs.x - rhs.x;
	FLOAT fY = lhs.y - rhs.y;
	FLOAT fZ = lhs.z - rhs.z;
	
	return sqrtf( fX * fX  +  fY * fY + fZ * fZ );
}

CVector3::CVector3(FLOAT x, FLOAT y, FLOAT z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

CVector3::CVector3(const FLOAT3& Value)
{
	x = Value.x;
	y = Value.y;
	z = Value.z;
}

CVector3::~CVector3(void)
{
}

void CVector3::CrossProduct(const FLOAT3& v0, const FLOAT3& v1)
{
	FLOAT3* pDestination =  this;
	if(pDestination == &v0 || pDestination == &v1)
	{
		static FLOAT3 Temp;

		pDestination = &Temp;
	}

	pDestination->x = v0.y * v1.z - v0.z * v1.y;
	pDestination->y = v0.z * v1.x - v0.x * v1.z;
	pDestination->z = v0.x * v1.y - v0.y * v1.x;

	if(pDestination != this)
		*this = *pDestination;
}

CVector3& CVector3::Transform(const FLOAT4& Quaternion)
{
	CVector3 UV;
	UV.CrossProduct(Quaternion.Float3, *this);
	
	CVector3 UUV;
	UUV.CrossProduct(Quaternion.Float3, UV);
	UUV *= 2.0f;
	
	UV *= Quaternion.w * 2.0f;
	
	*this += UV;
	*this += UUV;
	
	return *this;
}

FLOAT CVector3::Normalize()
{
	FLOAT fLength = GetLength();

	if(fLength)
		(*this) /= fLength;

	return fLength;
}

void CVector3::Slerp(const FLOAT3& From, const FLOAT3& To, FLOAT fWeight)
{
	if(fWeight == 1.0f)
	{
		*this = To;

		return;
	}

	if(fWeight == 0.0f)
	{
		*this = From;

		return;
	}

	FLOAT fInverseWeight = 1 - fWeight;
	x = To.x * fWeight + From.x * fInverseWeight;
	y = To.y * fWeight + From.y * fInverseWeight;
	z = To.z * fWeight + From.z * fInverseWeight;
}