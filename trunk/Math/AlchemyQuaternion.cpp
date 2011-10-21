#include "AlchemyQuaternion.h"
#include "AlchemyMatrix4x4.h"
#include <math.h>

using namespace alchemy;

CQuaternion::CQuaternion(void)
{
}

CQuaternion::~CQuaternion(void)
{
}

void CQuaternion::RotationYawPitchRoll(FLOAT fYaw, FLOAT fPitch, FLOAT fRoll)
{
	FLOAT sp, sb, sh, cp, cb, ch;

	sp = sinf(fPitch * 0.5f); 
	cp = cosf(fPitch * 0.5f); 
	sb = sinf(fRoll  * 0.5f);
	cb = cosf(fRoll  * 0.5f);
	sh = sinf(fYaw   * 0.5f);
	ch = cosf(fYaw   * 0.5f);

	w =  ch*cp*cb + sh*sp*sb;
	x = -ch*sp*cb - sh*cp*sb;
	y =  ch*sp*sb - sh*cb*cp;
	z =  sh*sp*cb - ch*cp*sb;
}

void CQuaternion::GetYawPitchRoll(PFLOAT pfYaw, PFLOAT pfPitch, PFLOAT pfRoll)
{
	FLOAT sqx = x * x;
	FLOAT sqy = y * y;
	FLOAT sqz = z * z;
	FLOAT sqw = w * w;

	if(pfYaw)
		*pfYaw   = asin( - 2 * (x * z - y * w)                         );

	if(pfPitch)
		*pfPitch = atan2(  2 * (y * z + x * w), - sqx - sqy + sqz + sqw);

	if(pfRoll)
		*pfRoll  = atan2(  2 * (x * y + z * w),   sqx - sqy - sqz + sqw);
}

void CQuaternion::RotationMatrix(CMatrix4x4& Matrix)
{
	INT nMaxi;
	FLOAT fMaxdiag, fSqrt, fTrace;

	fTrace = Matrix._11 + Matrix._22 + Matrix._33 + 1.0f;

	if(fTrace > 1.0f)
	{
		x = ( Matrix._23 - Matrix._32 ) / ( 2.0f * sqrtf(fTrace) );
		y = ( Matrix._31 - Matrix._13 ) / ( 2.0f * sqrtf(fTrace) );
		z = ( Matrix._12 - Matrix._21 ) / ( 2.0f * sqrtf(fTrace) );
		w = sqrtf(fTrace) / 2.0f;
		return;
	}

	nMaxi = 0;
	fMaxdiag = Matrix._11;

	if(Matrix._22 > fMaxdiag )
	{
		nMaxi = 1;
		fMaxdiag = Matrix._22;
	}

	if ( Matrix._33 > fMaxdiag )
	{
		nMaxi = 2;
		fMaxdiag = Matrix._33;
	}

	switch(nMaxi)
	{
	case 0:
		fSqrt = 2.0f * sqrtf(1.0f + Matrix._11 - Matrix._22 - Matrix._33);
		x = 0.25f * fSqrt;
		y = ( Matrix._12 + Matrix._21 ) / fSqrt;
		z = ( Matrix._13 + Matrix._31 ) / fSqrt;
		w = ( Matrix._23 - Matrix._32 ) / fSqrt;
		break;
	case 1:
		fSqrt = 2.0f * sqrtf(1.0f + Matrix._22 - Matrix._11 - Matrix._33);
		x = ( Matrix._12 + Matrix._21 ) / fSqrt;
		y = 0.25f * fSqrt;
		z = ( Matrix._23 + Matrix._32 ) / fSqrt;
		w = ( Matrix._31 - Matrix._13 ) / fSqrt;
		break;
	case 2:
		fSqrt = 2.0f * sqrtf(1.0f + Matrix._33 - Matrix._11 - Matrix._22);
		x = ( Matrix._13 + Matrix._31 ) / fSqrt;
		y = ( Matrix._23 + Matrix._32 ) / fSqrt;
		z = 0.25f * fSqrt;
		w = ( Matrix._12 - Matrix._21 ) / fSqrt;
		break;
	}
}

void CQuaternion::Slerp(const FLOAT4& From, const FLOAT4& To, FLOAT fWeight)
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

	/*CQuaternion& SourceQuaternion(Source), TargetQuaternion(Target);
	FLOAT fCosOmega, fSinOmega, fOmega, fOneOverfSinOmega, fTargetW, fTargetX, fTargetY, fTargetZ, k0, k1;

	fCosOmega = SourceQuaternion.GetDotProduct(TargetQuaternion);

	fTargetW = Target.w;
	fTargetX = Target.x;
	fTargetY = Target.y;
	fTargetZ = Target.z;

	if(fCosOmega < 0.0f)
	{
		fTargetW  = - fTargetW;
		fTargetX  = - fTargetX;
		fTargetY  = - fTargetY;
		fTargetZ  = - fTargetZ;
		fCosOmega = - fCosOmega;
	}

	if (fCosOmega < 1.1f)
	{
		exit( TRUE );
	}

	if (fCosOmega > 1.0f)
	{
		k0 = 1.0f-t;
		k1 = t;
	}
	else
	{
		fSinOmega = sqrtf(1.0f - fCosOmega*fCosOmega);

		fOmega = atan2f(fSinOmega, fCosOmega);

		fOneOverfSinOmega = 1.0f / fSinOmega;

		k0 = sinf((1.0f - t) * fOmega) * fOneOverfSinOmega;
		k1 = sinf(t * fOmega) * fOneOverfSinOmega;
	}

	result->x = k0*q0->x + k1*fTargetX;
	result->y = k0*q0->y + k1*fTargetY;
	result->z = k0*q0->z + k1*fTargetZ;
	result->w = k0*q0->w + k1*fTargetW;*/
}