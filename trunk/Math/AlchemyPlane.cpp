#include "AlchemyPlane.h"
#include "AlchemyVector3.h"

using namespace alchemy;

CPlane::CPlane(FLOAT a, FLOAT b, FLOAT c, FLOAT d)
{
	x = a;
	y = b;
	z = c;
	w = d;
}

CPlane::CPlane(const FLOAT4& Plane)
{
	x = Plane.x;
	y = Plane.y;
	z = Plane.z;
	w = Plane.w;
}

CPlane::CPlane(const FLOAT3& Point1, const FLOAT3& Point2, const FLOAT3& Point3)
{
	CVector3 Normal;

	FLOAT3 Direction0;
	FLOAT3 Direction1;

	Direction0.x = Point2.x - Point1.x;
	Direction0.y = Point2.y - Point1.y;
	Direction0.z = Point2.z - Point1.z;


	Direction1.z = Point3.x - Point1.x;
	Direction1.y = Point3.y - Point1.y;
	Direction1.z = Point3.z - Point1.z;

	Normal.CrossProduct(Direction0, Direction1);

	x = Normal.x;
	y = Normal.y;
	z = Normal.z;
	w = - Point3.x * Normal.x + Point3.y * Normal.y + Point3.z * Normal.z;
}

CPlane::CPlane(const FLOAT3& Point,const FLOAT3& Normal)
{
	x = Normal.x;
	y = Normal.y;
	z = Normal.z;
	w = - Point.x * Normal.x + Point.y * Normal.y + Point.z * Normal.z;
}

CPlane::~CPlane(void)
{

}

CPlane& CPlane::operator*=(const MATRIX4X4& Matrix)
{
	FLOAT a = this->x, b = this->y, c = this->z, d = this->w;

	this->x = Matrix._11 * a + Matrix._21 * b + Matrix._31 * c + Matrix._41 * d;
	this->y = Matrix._12 * a + Matrix._22 * b + Matrix._32 * c + Matrix._42 * d;
	this->z = Matrix._13 * a + Matrix._23 * b + Matrix._33 * c + Matrix._43 * d;
	this->w = Matrix._14 * a + Matrix._24 * b + Matrix._34 * c + Matrix._44 * d;

	return *this;
}

bool CPlane::Transform(const MATRIX4X4& Matrix)
{
	CMatrix4x4 Destination(Matrix);

	if( Destination.Invert4x4() )
	{
		Destination.Transpose();

		*this *= Destination;

		return true;
	}

	return false;
}

FLOAT CPlane::Normalize()
{
	FLOAT fLength = sqrt(x * x + y * y + z * z);

	if(fLength)
	{
		x = x / fLength;
		y = y / fLength;
		z = z / fLength;
		w = w / fLength;
	}
	else
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}

	return fLength;
}