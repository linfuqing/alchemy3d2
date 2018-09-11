#pragma once

#include "AlchemyFloat4.h"

namespace alchemy
{
	class CMatrix4x4;
	class CQuaternion :
		public CFloat4
	{
	public:
		CQuaternion(void);
		~CQuaternion(void);

		CQuaternion& operator=(const FLOAT4& Value);

		FLOAT GetDotProduct(const FLOAT4& Target)const;

		void RotationYawPitchRoll(FLOAT fYaw, FLOAT fPitch, FLOAT fRoll);
		void GetYawPitchRoll(PFLOAT pfYaw, PFLOAT pfPitch, PFLOAT pfRoll);

		void RotationMatrix(CMatrix4x4& Matrix);

		void Slerp(const FLOAT4& From, const FLOAT4& To, FLOAT fWeight);
	};

	ALCHEMY_INLINE CQuaternion& CQuaternion::operator=(const FLOAT4& Value)
	{
		x = Value.x;
		y = Value.y;
		z = Value.z;
		w = Value.w;

		return *this;
	}

	ALCHEMY_INLINE FLOAT CQuaternion::GetDotProduct(const FLOAT4& Target)const
	{
		return w * Target.w + x * Target.x + y * Target.y + z * Target.z;
	}
}