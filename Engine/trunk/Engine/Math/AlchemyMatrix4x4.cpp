#include "AlchemyMatrix4x4.h"
#include "AlchemyMatrix2x2.h"
#include "AlchemyVector3.h"
#include "AlchemyVector4.h"
#include "AlchemyMath.h"
#include "AlchemyPlane.h"

#include "../Core/AlchemyNull.h"
#include "../Core/AlchemyUtils.h"
#include "../Core/AlchemyBitFlag.h"

using namespace alchemy;

MATRIX4X4& alchemy::Identity(MATRIX4X4& Output)
{
	Output._11 = 1.0f;
	Output._12 = 0.0f;
	Output._13 = 0.0f;
	Output._14 = 0.0f;

	Output._21 = 0.0f;
	Output._22 = 1.0f;
	Output._23 = 0.0f;
	Output._24 = 0.0f;

	Output._31 = 0.0f;
	Output._32 = 0.0f;
	Output._33 = 1.0f;
	Output._34 = 0.0f;

	Output._41 = 0.0f;
	Output._42 = 0.0f;
	Output._43 = 0.0f;
	Output._44 = 1.0f;

	return Output;
}

MATRIX4X4& alchemy::Copy(MATRIX4X4& Destination, const MATRIX4X4& Source)
{
	Destination._11 = Source._11;
	Destination._12 = Source._12;
	Destination._13 = Source._13;
	Destination._14 = Source._14;

	Destination._21 = Source._21;
	Destination._22 = Source._22;
	Destination._23 = Source._23;
	Destination._24 = Source._24;

	Destination._31 = Source._31;
	Destination._32 = Source._32;
	Destination._33 = Source._33;
	Destination._34 = Source._34;

	Destination._41 = Source._41;
	Destination._42 = Source._42;
	Destination._43 = Source._43;
	Destination._44 = Source._44;

	return Destination;
}

MATRIX4X4& alchemy::Copy4x3(MATRIX4X4& Destination, const MATRIX4X4& Source)
{
	Destination._11 = Source._11;
	Destination._12 = Source._12;
	Destination._13 = Source._13;

	Destination._21 = Source._21;
	Destination._22 = Source._22;
	Destination._23 = Source._23;

	Destination._31 = Source._31;
	Destination._32 = Source._32;
	Destination._33 = Source._33;

	Destination._41 = Source._41;
	Destination._42 = Source._42;
	Destination._43 = Source._43;

	return Destination;
}

MATRIX4X4& alchemy::Copy3x4(MATRIX4X4& Destination, const MATRIX4X4& Source)
{
	Destination._11 = Source._11;
	Destination._12 = Source._12;
	Destination._13 = Source._13;
	Destination._14 = Source._14;

	Destination._21 = Source._21;
	Destination._22 = Source._22;
	Destination._23 = Source._23;
	Destination._24 = Source._24;

	Destination._31 = Source._31;
	Destination._32 = Source._32;
	Destination._33 = Source._33;
	Destination._34 = Source._34;

	return Destination;
}

MATRIX4X4& alchemy::CopyRow(MATRIX4X4& Destination, const MATRIX4X4& Source, UINT uRow)
{
	switch(uRow)
	{
	case 0:
		Destination._11 = Source._11;
		Destination._12 = Source._12;
		Destination._13 = Source._13;
		Destination._14 = Source._14;
		break;

	case 1:
		Destination._21 = Source._21;
		Destination._22 = Source._22;
		Destination._23 = Source._23;
		Destination._24 = Source._24;
		break;

	case 2:
		Destination._31 = Source._31;
		Destination._32 = Source._32;
		Destination._33 = Source._33;
		Destination._34 = Source._34;
		break;

	case 3:
		Destination._41 = Source._41;
		Destination._42 = Source._42;
		Destination._43 = Source._43;
		Destination._44 = Source._44;
		break;

	default:
		break;
	}

	return Destination;
}

MATRIX4X4& alchemy::Multiply4x3(MATRIX4X4& Output, const MATRIX4X4& Matrix1, const MATRIX4X4& Matrix2)
{
	if(&Output == &Matrix1 || &Output == &Matrix2)
	{
		MATRIX4X4 Temp;

		Temp._11 = Matrix2._11 * Matrix1._11 + Matrix2._12 * Matrix1._21 + Matrix2._13 * Matrix1._31;
		Temp._12 = Matrix2._11 * Matrix1._12 + Matrix2._12 * Matrix1._22 + Matrix2._13 * Matrix1._32;
		Temp._13 = Matrix2._11 * Matrix1._13 + Matrix2._12 * Matrix1._23 + Matrix2._13 * Matrix1._33;
		Temp._14 = 0.0f;

		Temp._21 = Matrix2._21 * Matrix1._11 + Matrix2._22 * Matrix1._21 + Matrix2._23 * Matrix1._31;
		Temp._22 = Matrix2._21 * Matrix1._12 + Matrix2._22 * Matrix1._22 + Matrix2._23 * Matrix1._32;
		Temp._23 = Matrix2._21 * Matrix1._13 + Matrix2._22 * Matrix1._23 + Matrix2._23 * Matrix1._33;
		Temp._24 = 0.0f;

		Temp._31 = Matrix2._31 * Matrix1._11 + Matrix2._32 * Matrix1._21 + Matrix2._33 * Matrix1._31;
		Temp._32 = Matrix2._31 * Matrix1._12 + Matrix2._32 * Matrix1._22 + Matrix2._33 * Matrix1._32;
		Temp._33 = Matrix2._31 * Matrix1._13 + Matrix2._32 * Matrix1._23 + Matrix2._33 * Matrix1._33;
		Temp._34 = 0.0f;

		Temp._41 = Matrix2._41 * Matrix1._11 + Matrix2._42 * Matrix1._21 + Matrix2._43 * Matrix1._31 + Matrix1._41;
		Temp._42 = Matrix2._41 * Matrix1._12 + Matrix2._42 * Matrix1._22 + Matrix2._43 * Matrix1._32 + Matrix1._42;
		Temp._43 = Matrix2._41 * Matrix1._13 + Matrix2._42 * Matrix1._23 + Matrix2._43 * Matrix1._33 + Matrix1._43;
		Temp._44 = 1.0f;

		Copy(Output, Temp);
	}
	else
	{
		Output._11 = Matrix2._11 * Matrix1._11 + Matrix2._12 * Matrix1._21 + Matrix2._13 * Matrix1._31;
		Output._12 = Matrix2._11 * Matrix1._12 + Matrix2._12 * Matrix1._22 + Matrix2._13 * Matrix1._32;
		Output._13 = Matrix2._11 * Matrix1._13 + Matrix2._12 * Matrix1._23 + Matrix2._13 * Matrix1._33;
		Output._14 = 0.0f;

		Output._21 = Matrix2._21 * Matrix1._11 + Matrix2._22 * Matrix1._21 + Matrix2._23 * Matrix1._31;
		Output._22 = Matrix2._21 * Matrix1._12 + Matrix2._22 * Matrix1._22 + Matrix2._23 * Matrix1._32;
		Output._23 = Matrix2._21 * Matrix1._13 + Matrix2._22 * Matrix1._23 + Matrix2._23 * Matrix1._33;
		Output._24 = 0.0f;

		Output._31 = Matrix2._31 * Matrix1._11 + Matrix2._32 * Matrix1._21 + Matrix2._33 * Matrix1._31;
		Output._32 = Matrix2._31 * Matrix1._12 + Matrix2._32 * Matrix1._22 + Matrix2._33 * Matrix1._32;
		Output._33 = Matrix2._31 * Matrix1._13 + Matrix2._32 * Matrix1._23 + Matrix2._33 * Matrix1._33;
		Output._34 = 0.0f;

		Output._41 = Matrix2._41 * Matrix1._11 + Matrix2._42 * Matrix1._21 + Matrix2._43 * Matrix1._31 + Matrix1._41;
		Output._42 = Matrix2._41 * Matrix1._12 + Matrix2._42 * Matrix1._22 + Matrix2._43 * Matrix1._32 + Matrix1._42;
		Output._43 = Matrix2._41 * Matrix1._13 + Matrix2._42 * Matrix1._23 + Matrix2._43 * Matrix1._33 + Matrix1._43;
		Output._44 = 1.0f;
	}

	return Output;
}

MATRIX4X4& alchemy::Multiply(MATRIX4X4& Output, const MATRIX4X4& Matrix1, const MATRIX4X4& Matrix2)
{
	if(&Output == &Matrix1 || &Output == &Matrix2)
	{
		MATRIX4X4 Temp;

		Temp._11 = Matrix2._11 * Matrix1._11 + Matrix2._12 * Matrix1._21 + Matrix2._13 * Matrix1._31 + Matrix2._14 * Matrix1._41;
		Temp._12 = Matrix2._11 * Matrix1._12 + Matrix2._12 * Matrix1._22 + Matrix2._13 * Matrix1._32 + Matrix2._14 * Matrix1._42;
		Temp._13 = Matrix2._11 * Matrix1._13 + Matrix2._12 * Matrix1._23 + Matrix2._13 * Matrix1._33 + Matrix2._14 * Matrix1._43;
		Temp._14 = Matrix2._11 * Matrix1._14 + Matrix2._12 * Matrix1._24 + Matrix2._13 * Matrix1._34 + Matrix2._14 * Matrix1._44;

		Temp._21 = Matrix2._21 * Matrix1._11 + Matrix2._22 * Matrix1._21 + Matrix2._23 * Matrix1._31 + Matrix2._24 * Matrix1._41;
		Temp._22 = Matrix2._21 * Matrix1._12 + Matrix2._22 * Matrix1._22 + Matrix2._23 * Matrix1._32 + Matrix2._24 * Matrix1._42;
		Temp._23 = Matrix2._21 * Matrix1._13 + Matrix2._22 * Matrix1._23 + Matrix2._23 * Matrix1._33 + Matrix2._24 * Matrix1._43;
		Temp._24 = Matrix2._21 * Matrix1._14 + Matrix2._22 * Matrix1._24 + Matrix2._23 * Matrix1._34 + Matrix2._24 * Matrix1._44;

		Temp._31 = Matrix2._31 * Matrix1._11 + Matrix2._32 * Matrix1._21 + Matrix2._33 * Matrix1._31 + Matrix2._34 * Matrix1._41;
		Temp._32 = Matrix2._31 * Matrix1._12 + Matrix2._32 * Matrix1._22 + Matrix2._33 * Matrix1._32 + Matrix2._34 * Matrix1._42;
		Temp._33 = Matrix2._31 * Matrix1._13 + Matrix2._32 * Matrix1._23 + Matrix2._33 * Matrix1._33 + Matrix2._34 * Matrix1._43;
		Temp._34 = Matrix2._31 * Matrix1._14 + Matrix2._32 * Matrix1._24 + Matrix2._33 * Matrix1._34 + Matrix2._34 * Matrix1._44;

		Temp._41 = Matrix2._41 * Matrix1._11 + Matrix2._42 * Matrix1._21 + Matrix2._43 * Matrix1._31 + Matrix2._44 * Matrix1._41;
		Temp._42 = Matrix2._41 * Matrix1._12 + Matrix2._42 * Matrix1._22 + Matrix2._43 * Matrix1._32 + Matrix2._44 * Matrix1._42;
		Temp._43 = Matrix2._41 * Matrix1._13 + Matrix2._42 * Matrix1._23 + Matrix2._43 * Matrix1._33 + Matrix2._44 * Matrix1._43;
		Temp._44 = Matrix2._41 * Matrix1._14 + Matrix2._42 * Matrix1._24 + Matrix2._43 * Matrix1._34 + Matrix2._44 * Matrix1._44;

		Copy(Output, Temp);
	}
	else
	{
		Output._11 = Matrix2._11 * Matrix1._11 + Matrix2._12 * Matrix1._21 + Matrix2._13 * Matrix1._31 + Matrix2._14 * Matrix1._41;
		Output._12 = Matrix2._11 * Matrix1._12 + Matrix2._12 * Matrix1._22 + Matrix2._13 * Matrix1._32 + Matrix2._14 * Matrix1._42;
		Output._13 = Matrix2._11 * Matrix1._13 + Matrix2._12 * Matrix1._23 + Matrix2._13 * Matrix1._33 + Matrix2._14 * Matrix1._43;
		Output._14 = Matrix2._11 * Matrix1._14 + Matrix2._12 * Matrix1._24 + Matrix2._13 * Matrix1._34 + Matrix2._14 * Matrix1._44;

		Output._21 = Matrix2._21 * Matrix1._11 + Matrix2._22 * Matrix1._21 + Matrix2._23 * Matrix1._31 + Matrix2._24 * Matrix1._41;
		Output._22 = Matrix2._21 * Matrix1._12 + Matrix2._22 * Matrix1._22 + Matrix2._23 * Matrix1._32 + Matrix2._24 * Matrix1._42;
		Output._23 = Matrix2._21 * Matrix1._13 + Matrix2._22 * Matrix1._23 + Matrix2._23 * Matrix1._33 + Matrix2._24 * Matrix1._43;
		Output._24 = Matrix2._21 * Matrix1._14 + Matrix2._22 * Matrix1._24 + Matrix2._23 * Matrix1._34 + Matrix2._24 * Matrix1._44;

		Output._31 = Matrix2._31 * Matrix1._11 + Matrix2._32 * Matrix1._21 + Matrix2._33 * Matrix1._31 + Matrix2._34 * Matrix1._41;
		Output._32 = Matrix2._31 * Matrix1._12 + Matrix2._32 * Matrix1._22 + Matrix2._33 * Matrix1._32 + Matrix2._34 * Matrix1._42;
		Output._33 = Matrix2._31 * Matrix1._13 + Matrix2._32 * Matrix1._23 + Matrix2._33 * Matrix1._33 + Matrix2._34 * Matrix1._43;
		Output._34 = Matrix2._31 * Matrix1._14 + Matrix2._32 * Matrix1._24 + Matrix2._33 * Matrix1._34 + Matrix2._34 * Matrix1._44;

		Output._41 = Matrix2._41 * Matrix1._11 + Matrix2._42 * Matrix1._21 + Matrix2._43 * Matrix1._31 + Matrix2._44 * Matrix1._41;
		Output._42 = Matrix2._41 * Matrix1._12 + Matrix2._42 * Matrix1._22 + Matrix2._43 * Matrix1._32 + Matrix2._44 * Matrix1._42;
		Output._43 = Matrix2._41 * Matrix1._13 + Matrix2._42 * Matrix1._23 + Matrix2._43 * Matrix1._33 + Matrix2._44 * Matrix1._43;
		Output._44 = Matrix2._41 * Matrix1._14 + Matrix2._42 * Matrix1._24 + Matrix2._43 * Matrix1._34 + Matrix2._44 * Matrix1._44;
	}

	return Output;
}

MATRIX4X4& alchemy::RotationX(MATRIX4X4& Output, FLOAT fAngle)
{
	FLOAT fCos = cosf(fAngle);
	FLOAT fSin = sinf(fAngle);
	
	Output._11 =   1.0f;
	Output._12 =   0.0f;
	Output._13 =   0.0f;
	Output._14 =   0.0f;
	
	Output._21 =   0.0f;
	Output._22 =   fCos;
	Output._23 =   fSin;
	Output._24 =   0.0f;
	
	Output._31 =   0.0f;
	Output._32 = - fSin;
	Output._33 =   fCos;
	Output._34 =   0.0f;
	
	Output._41 =   0.0f;
	Output._42 =   0.0f;
	Output._43 =   0.0f;
	Output._44 =   1.0f;
	
	return Output;
}

MATRIX4X4& alchemy::RotationY(MATRIX4X4& Output, FLOAT fAngle)
{
	FLOAT fCos = cosf(fAngle);
	FLOAT fSin = sinf(fAngle);
	
	Output._11 =   fCos;
	Output._12 =   0.0f;
	Output._13 =   fSin;
	Output._14 =   0.0f;
	
	Output._21 =   0.0f;
	Output._22 =   1.0f;
	Output._23 =   0.0f;
	Output._24 =   0.0f;
	
	Output._31 = - fSin;
	Output._32 =   0.0f;
	Output._33 =   fCos;
	Output._34 =   0.0f;
	
	Output._41 =   0.0f;
	Output._42 =   0.0f;
	Output._43 =   0.0f;
	Output._44 =   1.0f;
	
	return Output;
}

MATRIX4X4& alchemy::RotationZ(MATRIX4X4& Output, FLOAT fAngle)
{
	FLOAT fCos = cosf(fAngle);
	FLOAT fSin = sinf(fAngle);
	
	Output._11 =   fCos;
	Output._12 =   fSin;
	Output._13 =   0.0f;
	Output._14 =   0.0f;
	
	Output._21 = - fSin;
	Output._22 =   fCos;
	Output._23 =   0.0f;
	Output._24 =   0.0f;
	
	Output._31 =   0.0f;
	Output._32 =   0.0f;
	Output._33 =   1.0f;
	Output._34 =   0.0f;
	
	Output._41 =   0.0f;
	Output._42 =   0.0f;
	Output._43 =   0.0f;
	Output._44 =   1.0f;
	
	return Output;
}

MATRIX4X4& alchemy::RotationQuaternion(MATRIX4X4& Output, const FLOAT4& Quaternion)
{
	FLOAT x, y, z, w, xx, xy, xz, xw, yy, yz, yw, zz, zw;

	w = Quaternion.w;
	x = Quaternion.x;
	y = Quaternion.y;
	z = Quaternion.z;

	xx = x * x;
	xy = x * y;
	xz = x * z;
	xw = x * w;
	yy = y * y;
	yz = y * z;
	yw = y * w;
	zz = z * z;
	zw = z * w;

	Output._11 = 1.0f - 2.0f * ( yy + zz );
	Output._12 = 2.0f * ( xy - zw );
	Output._13 = 2.0f * ( xz + yw );
	Output._14 = 0.0f;

	Output._21 = 2.0f * ( xy + zw );
	Output._22 = 1.0f - 2 * ( xx + zz );
	Output._23 = 2.0f * ( yz - xw );
	Output._24 = 0.0f;

	Output._31 = 2.0f * ( xz - yw );
	Output._32 = 2.0f * ( yz + xw );
	Output._33 = 1.0f - 2.0f * ( xx + yy );
	Output._34 = 0.0f;

	Output._41 = 0.0f;
	Output._42 = 0.0f;
	Output._43 = 0.0f;
	Output._44 = 1.0f;

	return Output;
}

MATRIX4X4& alchemy::Slerp(MATRIX4X4& Output, const MATRIX4X4& From, const MATRIX4X4& To, FLOAT fWeight)
{
	if(fWeight == 1.0f)
	{
		Output = To;

		return Output;
	}
	else if(fWeight == 0.0f)
	{
		Output = From;

		return Output;
	}

	CMatrix4x4 FromMatrix(From), ToMatrix(To);
	CVector3 Scale, FromScale, ToScale, Translation, FromTranslation, ToTranslation;
	CQuaternion Rotation, FromRotation, ToRotation;

	FromMatrix.Decompose(&FromScale, ALCHEMY_NULL, &FromTranslation, &FromRotation);
	ToMatrix.Decompose(&ToScale, ALCHEMY_NULL, &ToTranslation, &ToRotation);

	Scale.Slerp(FromScale, ToScale, fWeight);
	Rotation.Slerp(FromRotation, ToRotation, fWeight);
	Translation.Slerp(FromTranslation, ToTranslation, fWeight);

	Identity(Output);

	Scaling(Output, Scale.x, Scale.y, Scale.z);
	RotationQuaternion(Output, Rotation);

	Output._41 = Translation.x;
	Output._42 = Translation.y;
	Output._43 = Translation.z;

	Output._14 = 0.0f;
	Output._24 = 0.0f;
	Output._34 = 0.0f;
	Output._44 = Translation.w;

	return Output;
}

CMatrix4x4::CMatrix4x4(FLOAT _11, FLOAT _12, FLOAT _13, FLOAT _14, 
				       FLOAT _21, FLOAT _22, FLOAT _23, FLOAT _24,
					   FLOAT _31, FLOAT _32, FLOAT _33, FLOAT _34,
					   FLOAT _41, FLOAT _42, FLOAT _43, FLOAT _44)
{
	this->_11 = _11;
	this->_12 = _12;
	this->_13 = _13;
	this->_14 = _14;

	this->_21 = _21;
	this->_22 = _22;
	this->_23 = _23;
	this->_24 = _24;

	this->_31 = _31;
	this->_32 = _32;
	this->_33 = _33;
	this->_34 = _34;

	this->_41 = _41;
	this->_42 = _42;
	this->_43 = _43;
	this->_44 = _44;
}

CMatrix4x4::CMatrix4x4(const MATRIX4X4& Source)
{
	_11 = Source._11;
	_12 = Source._12;
	_13 = Source._13;
	_14 = Source._14;

	_21 = Source._21;
	_22 = Source._22;
	_23 = Source._23;
	_24 = Source._24;

	_31 = Source._31;
	_32 = Source._32;
	_33 = Source._33;
	_34 = Source._34;

	_41 = Source._41;
	_42 = Source._42;
	_43 = Source._43;
	_44 = Source._44;
}

CMatrix4x4::~CMatrix4x4(void)
{
}

CMatrix4x4& CMatrix4x4::operator=(const MATRIX4X4& Source)
{
	_11 = Source._11;
	_12 = Source._12;
	_13 = Source._13;
	_14 = Source._14;

	_21 = Source._21;
	_22 = Source._22;
	_23 = Source._23;
	_24 = Source._24;

	_31 = Source._31;
	_32 = Source._32;
	_33 = Source._33;
	_34 = Source._34;

	_41 = Source._41;
	_42 = Source._42;
	_43 = Source._43;
	_44 = Source._44;

	return *this;
}

CMatrix4x4& CMatrix4x4::operator=(const CMatrix4x4& Source)
{
	_11 = Source._11;
	_12 = Source._12;
	_13 = Source._13;
	_14 = Source._14;

	_21 = Source._21;
	_22 = Source._22;
	_23 = Source._23;
	_24 = Source._24;

	_31 = Source._31;
	_32 = Source._32;
	_33 = Source._33;
	_34 = Source._34;

	_41 = Source._41;
	_42 = Source._42;
	_43 = Source._43;
	_44 = Source._44;

	return *this;
}

/*CMatrix4x4 CMatrix4x4::operator*(const MATRIX4X4& Matrix)const
{
	CMatrix4x4 Temp;

	Temp._11 = _11 * Matrix._11 + _12 * Matrix._21 + _13 * Matrix._31 + _14 * Matrix._41;
	Temp._12 = _11 * Matrix._12 + _12 * Matrix._22 + _13 * Matrix._32 + _14 * Matrix._42;
	Temp._13 = _11 * Matrix._13 + _12 * Matrix._23 + _13 * Matrix._33 + _14 * Matrix._43;
	Temp._14 = _11 * Matrix._14 + _12 * Matrix._24 + _13 * Matrix._34 + _14 * Matrix._44;

	Temp._21 = _21 * Matrix._11 + _22 * Matrix._21 + _23 * Matrix._31 + _24 * Matrix._41;
	Temp._22 = _21 * Matrix._12 + _22 * Matrix._22 + _23 * Matrix._32 + _24 * Matrix._42;
	Temp._23 = _21 * Matrix._13 + _22 * Matrix._23 + _23 * Matrix._33 + _24 * Matrix._43;
	Temp._24 = _21 * Matrix._14 + _22 * Matrix._24 + _23 * Matrix._34 + _24 * Matrix._44;

	Temp._31 = _31 * Matrix._11 + _32 * Matrix._21 + _33 * Matrix._31 + _34 * Matrix._41;
	Temp._32 = _31 * Matrix._12 + _32 * Matrix._22 + _33 * Matrix._32 + _34 * Matrix._42;
	Temp._33 = _31 * Matrix._13 + _32 * Matrix._23 + _33 * Matrix._33 + _34 * Matrix._43;
	Temp._34 = _31 * Matrix._14 + _32 * Matrix._24 + _33 * Matrix._34 + _34 * Matrix._44;

	Temp._41 = _41 * Matrix._11 + _42 * Matrix._21 + _43 * Matrix._31 + _44 * Matrix._41;
	Temp._42 = _41 * Matrix._12 + _42 * Matrix._22 + _43 * Matrix._32 + _44 * Matrix._42;
	Temp._43 = _41 * Matrix._13 + _42 * Matrix._23 + _43 * Matrix._33 + _44 * Matrix._43;
	Temp._44 = _41 * Matrix._14 + _42 * Matrix._24 + _43 * Matrix._34 + _44 * Matrix._44;

	return Temp;
}*/

CMatrix4x4& CMatrix4x4::operator*=(const MATRIX4X4& Matrix)
{
	CMatrix4x4 Temp;

	Temp._11 = _11 * Matrix._11 + _12 * Matrix._21 + _13 * Matrix._31 + _14 * Matrix._41;
	Temp._12 = _11 * Matrix._12 + _12 * Matrix._22 + _13 * Matrix._32 + _14 * Matrix._42;
	Temp._13 = _11 * Matrix._13 + _12 * Matrix._23 + _13 * Matrix._33 + _14 * Matrix._43;
	Temp._14 = _11 * Matrix._14 + _12 * Matrix._24 + _13 * Matrix._34 + _14 * Matrix._44;

	Temp._21 = _21 * Matrix._11 + _22 * Matrix._21 + _23 * Matrix._31 + _24 * Matrix._41;
	Temp._22 = _21 * Matrix._12 + _22 * Matrix._22 + _23 * Matrix._32 + _24 * Matrix._42;
	Temp._23 = _21 * Matrix._13 + _22 * Matrix._23 + _23 * Matrix._33 + _24 * Matrix._43;
	Temp._24 = _21 * Matrix._14 + _22 * Matrix._24 + _23 * Matrix._34 + _24 * Matrix._44;

	Temp._31 = _31 * Matrix._11 + _32 * Matrix._21 + _33 * Matrix._31 + _34 * Matrix._41;
	Temp._32 = _31 * Matrix._12 + _32 * Matrix._22 + _33 * Matrix._32 + _34 * Matrix._42;
	Temp._33 = _31 * Matrix._13 + _32 * Matrix._23 + _33 * Matrix._33 + _34 * Matrix._43;
	Temp._34 = _31 * Matrix._14 + _32 * Matrix._24 + _33 * Matrix._34 + _34 * Matrix._44;

	Temp._41 = _41 * Matrix._11 + _42 * Matrix._21 + _43 * Matrix._31 + _44 * Matrix._41;
	Temp._42 = _41 * Matrix._12 + _42 * Matrix._22 + _43 * Matrix._32 + _44 * Matrix._42;
	Temp._43 = _41 * Matrix._13 + _42 * Matrix._23 + _43 * Matrix._33 + _44 * Matrix._43;
	Temp._44 = _41 * Matrix._14 + _42 * Matrix._24 + _43 * Matrix._34 + _44 * Matrix._44;

	*this = Temp;

	return *this;
}

void CMatrix4x4::GetYawPitchRoll(PFLOAT pfYaw, PFLOAT pfPitch, PFLOAT pfRoll)
{
	if(pfYaw)
		*pfYaw = g_fRAD2DEG * atan2f(_33, _31);

	if(pfPitch)
		*pfPitch = g_fRAD2DEG * asinf(- _32);

	if(pfRoll)
		*pfRoll = g_fRAD2DEG * atan2f(_22, _12); 
}

void CMatrix4x4::RotationQuaternion(CQuaternion& Quaternion)
{
	FLOAT x, y, z, w, xx, xy, xz, xw, yy, yz, yw, zz, zw;

	w = Quaternion.w;
	x = Quaternion.x;
	y = Quaternion.y;
	z = Quaternion.z;

	xx = x * x;
	xy = x * y;
	xz = x * z;
	xw = x * w;
	yy = y * y;
	yz = y * z;
	yw = y * w;
	zz = z * z;
	zw = z * w;

	_11 = 1.0f - 2.0f * ( yy + zz );
	_12 = 2.0f * ( xy - zw );
	_13 = 2.0f * ( xz + yw );
	_14 = 0.0f;

	_21 = 2.0f * ( xy + zw );
	_22 = 1.0f - 2.0f * ( xx + zz );
	_23 = 2.0f * ( yz - xw );
	_24 = 0.0f;

	_31 = 2.0f * ( xz - yw );
	_32 = 2.0f * ( yz + xw );
	_33 = 1.0f - 2.0f * ( xx + yy );
	_34 = 0.0f;

	_41 = 0.0f;
	_42 = 0.0f;
	_43 = 0.0f;
	_44 = 1.0f;
}

void CMatrix4x4::Decompose(LPFLOAT3 pScale, LPFLOAT3 pRotation, LPFLOAT3 pTranslation, CQuaternion* pQuaternion)const
{
	CMatrix4x4 Normalized;

	FLOAT fScaleX, fScaleY, fScaleZ;

	if(pTranslation)
	{
		pTranslation->x = _41;
		pTranslation->y = _42;
		pTranslation->z = _43;
	}

	fScaleX = sqrtf(_11 * _11 + _12 * _12 + _13 * _13);
	fScaleY = sqrtf(_21 * _21 + _22 * _22 + _23 * _23);
	fScaleZ = sqrtf(_31 * _31 + _32 * _32 + _33 * _33);

	if(pScale)
	{
		pScale->x = fScaleX;
		pScale->y = fScaleY;
		pScale->z = fScaleZ;
	}

	if(fScaleX == 0 || fScaleY == 0 || fScaleZ == 0)
		return;

	Normalized._11 = _11 / fScaleX;
	Normalized._12 = _12 / fScaleX;
	Normalized._13 = _13 / fScaleX;
	Normalized._21 = _21 / fScaleY;
	Normalized._22 = _22 / fScaleY;
	Normalized._23 = _23 / fScaleY;
	Normalized._31 = _31 / fScaleZ;
	Normalized._32 = _32 / fScaleZ;
	Normalized._33 = _33 / fScaleZ;

	if(pRotation)
		Normalized.GetYawPitchRoll(&pRotation->y, &pRotation->x, &pRotation->z);

	if(pQuaternion)
		pQuaternion->RotationMatrix(Normalized);
}

void CMatrix4x4::Identity()
{
	_11 = 1.0f;
	_12 = 0.0f;
	_13 = 0.0f;
	_14 = 0.0f;

	_21 = 0.0f;
	_22 = 1.0f;
	_23 = 0.0f;
	_24 = 0.0f;

	_31 = 0.0f;
	_32 = 0.0f;
	_33 = 1.0f;
	_34 = 0.0f;

	_41 = 0.0f;
	_42 = 0.0f;
	_43 = 0.0f;
	_44 = 1.0f;
}

bool CMatrix4x4::Invert3x4()
{
	FLOAT fDeterminant, InvertDeterminant, 
		fDeterminant00, fDeterminant10, fDeterminant20, 
		fDeterminant01, fDeterminant02, fDeterminant11, 
		fDeterminant12, fDeterminant21, fDeterminant22;

	FLOAT _11, _12, _13, _21, _22, _23, _31, _32, _33, _41, _42, _43;

	_11 = this->_11; _21 = this->_21; _31 = this->_31; _41 = this->_41;
	_12 = this->_12; _22 = this->_22; _32 = this->_32; _42 = this->_42;
	_13 = this->_13; _23 = this->_23; _33 = this->_33; _43 = this->_43;

	fDeterminant00 = _22 * _33 - _23 * _32;
	fDeterminant10 = _23 * _31 - _21 * _33;
	fDeterminant20 = _21 * _32 - _22 * _31;

	fDeterminant = _11 * fDeterminant00 + _12 * fDeterminant10 + _13 * fDeterminant20;

	if(!fDeterminant)
		return false;

	InvertDeterminant = 1.0f / fDeterminant;

	fDeterminant01 = _13 * _32 - _12 * _33;
	fDeterminant02 = _12 * _23 - _13 * _22;
	fDeterminant11 = _11 * _33 - _13 * _31;
	fDeterminant12 = _13 * _21 - _11 * _23;
	fDeterminant21 = _12 * _31 - _11 * _32;
	fDeterminant22 = _11 * _22 - _12 * _21;

	this->_11 = fDeterminant00 * InvertDeterminant;
	this->_12 = fDeterminant01 * InvertDeterminant;
	this->_13 = fDeterminant02 * InvertDeterminant;

	this->_21 = fDeterminant10 * InvertDeterminant;
	this->_22 = fDeterminant11 * InvertDeterminant;
	this->_23 = fDeterminant12 * InvertDeterminant;

	this->_31 = fDeterminant20 * InvertDeterminant;
	this->_32 = fDeterminant21 * InvertDeterminant;
	this->_33 = fDeterminant22 * InvertDeterminant;
	
	this->_41 = -(_41 * this->_11 + _42 * this->_21 + _43 * this->_31);
	this->_42 = -(_41 * this->_12 + _42 * this->_22 + _43 * this->_32);
	this->_43 = -(_41 * this->_13 + _42 * this->_23 + _43 * this->_33);

	return true;
}

bool CMatrix4x4::Invert4x4()
{
	MATRIX2X2 Temp1, Temp2, Temp3, Temp4;
	FLOAT fTemp, fDeterminant, InvertDeterminant;

	fDeterminant = _11 * _22 - _12 * _21;

	if(!fDeterminant)
		return false;

	InvertDeterminant = 1.0f / fDeterminant;

	Temp1._11 =   _22 * InvertDeterminant;
	Temp1._12 = - _12 * InvertDeterminant;
	Temp1._21 = - _21 * InvertDeterminant;
	Temp1._22 =   _11 * InvertDeterminant;

	Temp2._11 = Temp1._11 * _13 + Temp1._12 * _23;
	Temp2._12 = Temp1._11 * _14 + Temp1._12 * _24;
	Temp2._21 = Temp1._21 * _13 + Temp1._22 * _23;
	Temp2._22 = Temp1._21 * _14 + Temp1._22 * _24;

	Temp3._11 = _31 * Temp2._11 + _32 * Temp2._21;
	Temp3._12 = _31 * Temp2._12 + _32 * Temp2._22;
	Temp3._21 = _41 * Temp2._11 + _42 * Temp2._21;
	Temp3._22 = _41 * Temp2._12 + _42 * Temp2._22;

	Temp4._11 = Temp3._11 - _33;
	Temp4._12 = Temp3._12 - _34;
	Temp4._21 = Temp3._21 - _43;
	Temp4._22 = Temp3._22 - _44;

	fDeterminant = Temp4._11 * Temp4._22 - Temp4._12 * Temp4._21;

	if(!fDeterminant)
		return false;

	InvertDeterminant = 1.0f / fDeterminant;

	fTemp = Temp4._11;
	Temp4._11 =   Temp4._22 * InvertDeterminant;
	Temp4._12 = - Temp4._12 * InvertDeterminant;
	Temp4._21 = - Temp4._21 * InvertDeterminant;
	Temp4._22 =   fTemp * InvertDeterminant;

	Temp3._11 = _31 * Temp1._11 + _32 * Temp1._21;
	Temp3._12 = _31 * Temp1._12 + _32 * Temp1._22;
	Temp3._21 = _41 * Temp1._11 + _42 * Temp1._21;
	Temp3._22 = _41 * Temp1._12 + _42 * Temp1._22;

	_31 = Temp4._11 * Temp3._11 + Temp4._12 * Temp3._21;
	_32 = Temp4._11 * Temp3._12 + Temp4._12 * Temp3._22;
	_41 = Temp4._21 * Temp3._11 + Temp4._22 * Temp3._21;
	_42 = Temp4._21 * Temp3._12 + Temp4._22 * Temp3._22;

	_11 = Temp1._11 - Temp2._11 * _31 - Temp2._12 * _41;
	_12 = Temp1._12 - Temp2._11 * _32 - Temp2._12 * _42;
	_21 = Temp1._21 - Temp2._21 * _31 - Temp2._22 * _41;
	_22 = Temp1._22 - Temp2._21 * _32 - Temp2._22 * _42;

	_13 = Temp2._11 * Temp4._11 + Temp2._12 * Temp4._21;
	_14 = Temp2._11 * Temp4._12 + Temp2._12 * Temp4._22;
	_23 = Temp2._21 * Temp4._11 + Temp2._22 * Temp4._21;
	_24 = Temp2._21 * Temp4._12 + Temp2._22 * Temp4._22;

	_33 = - Temp4._11;
	_34 = - Temp4._12;
	_43 = - Temp4._21;
	_44 = - Temp4._22;

	return true;
}

void CMatrix4x4::Transpose()
{
	ALCHEMY_SWAP(_12, _21);
	ALCHEMY_SWAP(_13, _31);
	ALCHEMY_SWAP(_14, _41);

	ALCHEMY_SWAP(_23, _32);
	ALCHEMY_SWAP(_24, _42);

	ALCHEMY_SWAP(_34, _43);
}

void CMatrix4x4::Slerp(const MATRIX4X4& From, const MATRIX4X4& To, FLOAT fWeight)
{
	if(fWeight == 1.0f)
	{
		*this = To;

		return;
	}
	else if(fWeight == 0.0f)
	{
		*this = From;

		return;
	}

	CMatrix4x4 FromMatrix(From), ToMatrix(To);
	CVector3 Scale, FromScale, ToScale, Translation, FromTranslation, ToTranslation;
	CQuaternion Rotation, FromRotation, ToRotation;

	FromMatrix.Decompose(&FromScale, ALCHEMY_NULL, &FromTranslation, &FromRotation);
	ToMatrix.Decompose(&ToScale, ALCHEMY_NULL, &ToTranslation, &ToRotation);

	Scale.Slerp(FromScale, ToScale, fWeight);
	Rotation.Slerp(FromRotation, ToRotation, fWeight);
	Translation.Slerp(FromTranslation, ToTranslation, fWeight);

	Identity();

	Scaling(Scale.x, Scale.y, Scale.z);
	RotationQuaternion(Rotation);

	_41 = Translation.x;
	_42 = Translation.y;
	_43 = Translation.z;

	_14 = 0;
	_24 = 0;
	_34 = 0;
	_44 = Translation.w;
}

void CMatrix4x4::PerspectiveFieldOfViewLeftHandHalfDepth(FLOAT fTop, FLOAT fBottom, FLOAT fLeft, FLOAT fRight, FLOAT fNear, FLOAT fFar)
{
	FLOAT fWidth = fRight - fLeft, fHeight = fTop - fBottom;
	_11 = 2.0f * fNear / fWidth;
	_12 = 0.0f;
	_13 = 0.0f;
	_14 = 0.0f;

	_21 = 0.0f;
	_22 = 2.0f * fNear / fHeight;
	_23 = 0.0f;
	_24 = 0.0f;

	_31 = (fLeft   + fRight) / - fWidth ;
	_32 = (fBottom + fTop  ) / - fHeight;
	_33 = fFar / (fFar - fNear);
	_34 = 1.0f;

	_41 = 0.0f;
	_42 = 0.0f;
	_43 = - fNear * _33;
	_44 = 0.0f;
}

void CMatrix4x4::PerspectiveFieldOfViewLeftHandFullDepth(FLOAT fTop, FLOAT fBottom, FLOAT fLeft, FLOAT fRight, FLOAT fNear, FLOAT fFar)
{
	PerspectiveFieldOfViewLeftHandHalfDepth(fTop, fBottom, fLeft, fRight, fNear, fFar);
	
	_33 += fNear / (fFar - fNear);
	_43 *= 2.0f;
}

void CMatrix4x4::OrthoOffCenterLeftHandHalfDepth(FLOAT fTop, FLOAT fBottom, FLOAT fLeft, FLOAT fRight, FLOAT fNear, FLOAT fFar)
{
	FLOAT fWidth = fRight - fLeft, fHeight = fTop - fBottom, fLength = fFar - fNear;
	_11 = 2.0f / fWidth;
	_12 = 0.0f;
	_13 = 0.0f;
	_14 = 0.0f;
	
	_21 = 0.0f;
	_22 = 2.0f / fHeight;
	_23 = 0.0f;
	_24 = 0.0f;
	
	_31 = 0.0f;
	_32 = 0.0f;
	_33 = 1.0f / fLength;
	_34 = 0.0f;
	
	_41 = - (fRight + fLeft) / fWidth;
	_42 = - (fTop + fBottom) / fHeight;
	_43 = - fNear * _33;
	_44 = 1.0f;
}

void CMatrix4x4::OrthoOffCenterLeftHandFullDepth(FLOAT fTop, FLOAT fBottom, FLOAT fLeft, FLOAT fRight, FLOAT fNear, FLOAT fFar)
{
	OrthoOffCenterLeftHandHalfDepth(fTop, fBottom, fLeft, fRight, fNear, fFar);
	
	_33 *= 2.0f;
	_43 -= fFar / (fFar - fNear);
}

void CMatrix4x4::Projection(FLOAT fFOV, FLOAT fAspect, FLOAT fNear, FLOAT fFar, bool bIsLeftHand, bool bIsHalfDepth)
{
	FLOAT fTop = tanf(fFOV * 0.5f), fRight = fTop * fAspect;

	if(bIsHalfDepth)
		PerspectiveFieldOfViewLeftHandHalfDepth(fTop, - fTop, - fRight, fRight, fNear, fFar);
	else 
		PerspectiveFieldOfViewLeftHandFullDepth(fTop, - fTop, - fRight, fRight, fNear, fFar);
}

void CMatrix4x4::LookAt(const FLOAT3& Eye, const FLOAT3& At, const FLOAT3& Up)
{
	CVector3 CurrentForward(At);

	CurrentForward -= Eye;

	CurrentForward.Normalize();

	CVector3 CurrentRight;

	CurrentRight.CrossProduct(Up, CurrentForward);

	CVector3 CurrentUp;

	CurrentUp.CrossProduct(CurrentRight, CurrentForward);

	CurrentRight.Normalize();

	CurrentUp.Normalize();

	_11 =   CurrentRight.x;
	_21 =   CurrentRight.y;
	_31 =   CurrentRight.z;
	_41 = - CurrentRight.DotProduct(Eye);

	_12 =   CurrentUp.x;
	_22 =   CurrentUp.y;
	_32 =   CurrentUp.z;
	_42 = - CurrentUp.DotProduct(Eye);

	_13 =   CurrentForward.x;
	_23 =   CurrentForward.y;
	_33 =   CurrentForward.z;
	_43 = - CurrentForward.DotProduct(Eye);

	_14 = 0.0f;
	_24 = 0.0f;
	_34 = 0.0f;
	_44 = 1.0f;
}

void CMatrix4x4::Reflect(const FLOAT4& Plane)
{
	CPlane ReflectPlane(Plane);

	ReflectPlane.Normalize();

	_11 = - 2 * ReflectPlane.x * ReflectPlane.x + 1.0f;
	_12 = - 2 * ReflectPlane.y * ReflectPlane.x;
	_13 = - 2 * ReflectPlane.z * ReflectPlane.x;
	_14 = 0.0f;

	_21 = - 2 * ReflectPlane.x * ReflectPlane.y;
	_22 = - 2 * ReflectPlane.y * ReflectPlane.y + 1.0f;
	_23 = - 2 * ReflectPlane.z * ReflectPlane.y;
	_24 = 0.0f;

	_31 = - 2 * ReflectPlane.x * ReflectPlane.z;
	_32 = - 2 * ReflectPlane.y * ReflectPlane.z;
	_33 = - 2 * ReflectPlane.z * ReflectPlane.z + 1.0f;
	_34 = 0.0f;

	_41 = - 2 * ReflectPlane.x * ReflectPlane.w;
	_42 = - 2 * ReflectPlane.y * ReflectPlane.w;
	_43 = - 2 * ReflectPlane.z * ReflectPlane.w;
	_44 = 1.0f;
}