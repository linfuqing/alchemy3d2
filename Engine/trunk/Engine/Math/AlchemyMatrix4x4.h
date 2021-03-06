#pragma once

//#include "AlchemyVector3.h"
//#include "AlchemyVector4.h"

#include "AlchemyQuaternion.h"

namespace alchemy
{
	typedef struct MATRIX4X4
	{
//#pragma pack(1)
		union
		{
			/*struct
			{
				FLOAT _11, _12, _13, _14,
					  _21, _22, _23, _24,
					  _31, _32, _33, _34,
					  _41, _42, _43, _44;
			};*/
			struct
			{
				FLOAT _11, _21, _31, _41,
					_12, _22, _32, _42,
					_13, _23, _33, _43,
					_14, _24, _34, _44;
			};

			//FLOAT fPosition[4][4];
			FLOAT fValue[16];

			//FLOAT4 Row[4];
		};
//#pragma pack()
	}MATRIX4X4, * LPMATRIX4X4;

	MATRIX4X4& Scaling(MATRIX4X4& Output, FLOAT x, FLOAT y, FLOAT z);

	MATRIX4X4& Identity(MATRIX4X4& Output);
	MATRIX4X4& Copy(MATRIX4X4& Destination, const MATRIX4X4& Source);
	MATRIX4X4& Copy4x3(MATRIX4X4& Destination, const MATRIX4X4& Source);
	MATRIX4X4& Copy3x4(MATRIX4X4& Destination, const MATRIX4X4& Source);
	MATRIX4X4& CopyRow(MATRIX4X4& Destination, const MATRIX4X4& Source, UINT uRow);
	MATRIX4X4& Multiply4x3(MATRIX4X4& Output, const MATRIX4X4& Matrix1, const MATRIX4X4& Matrix2);
	MATRIX4X4& Multiply(MATRIX4X4& Output, const MATRIX4X4& Matrix1, const MATRIX4X4& Matrix2);
	MATRIX4X4& RotationX(MATRIX4X4& Output, FLOAT fAngle);
	MATRIX4X4& RotationY(MATRIX4X4& Output, FLOAT fAngle);
	MATRIX4X4& RotationZ(MATRIX4X4& Output, FLOAT fAngle);
	MATRIX4X4& RotationQuaternion(MATRIX4X4& Output, const FLOAT4& Quaternion);
	MATRIX4X4& Slerp(MATRIX4X4& Output, const MATRIX4X4& From, const MATRIX4X4& To, FLOAT fWeight);

	ALCHEMY_INLINE MATRIX4X4& Scaling(MATRIX4X4& Output, FLOAT x, FLOAT y, FLOAT z)
	{
		Output._11 *= x;
		Output._22 *= y;
		Output._33 *= z;

		return Output;
	}

	class CMatrix4x4 :
		public MATRIX4X4
	{
	public:
		//friend CMatrix4x4 operator*(const MATRIX4X4& Source, const MATRIX4X4& Destination);
		CMatrix4x4(void) {}
		CMatrix4x4(FLOAT _11, FLOAT _12, FLOAT _13, FLOAT _14, 
			       FLOAT _21, FLOAT _22, FLOAT _23, FLOAT _24,
				   FLOAT _31, FLOAT _32, FLOAT _33, FLOAT _34,
				   FLOAT _41, FLOAT _42, FLOAT _43, FLOAT _44);
		CMatrix4x4(const MATRIX4X4& Source);
		~CMatrix4x4(void);

		CMatrix4x4& operator=(const MATRIX4X4& Source); 
		CMatrix4x4& operator=(const CMatrix4x4& Source); 

		//CMatrix4x4 operator*(const MATRIX4X4& Matrix)const;
		CMatrix4x4& operator*=(const MATRIX4X4& Matrix);

		operator const FLOAT*()const;

		void SetPosition(FLOAT x, FLOAT y, FLOAT z);
		void SetPosition(FLOAT3& Position);
		void SetPosition(FLOAT4& Position);

		void GetPosition(FLOAT& x, FLOAT& y, FLOAT& z)const;
		void GetPosition(PFLOAT pOutputX, PFLOAT pOutputY, PFLOAT pOutputZ)const;
		void GetPosition(FLOAT3& Position)const;
		void GetPosition(FLOAT4& Position)const;

		void GetDirection(FLOAT& x, FLOAT& y, FLOAT& z)const;
		void GetDirection(PFLOAT pOutputX, PFLOAT pOutputY, PFLOAT pOutputZ)const;
		void GetDirection(FLOAT3& Direction)const;

		FLOAT GetDeterminant3x3()const;

		//void SetYawPitchRoll(FLOAT fYaw, FLOAT fPitch, FLOAT fRoll);
		void GetYawPitchRoll(PFLOAT pfYaw, PFLOAT pfPitch, PFLOAT pfRoll);

		void Scaling(FLOAT x, FLOAT y, FLOAT z);
		void RotationQuaternion(CQuaternion& Quaternion);

		void Decompose(LPFLOAT3 pScale, LPFLOAT3 pRotation, LPFLOAT3 pTranslation, CQuaternion* pQuaternion)const;

		void Identity();

		void Slerp(const MATRIX4X4& From, const MATRIX4X4& To, FLOAT fWeight);

		bool Invert3x4();

		bool Invert4x4();

		void Transpose();

		void PerspectiveFieldOfViewLeftHandHalfDepth(FLOAT fTop, FLOAT fBottom, FLOAT fLeft, FLOAT fRight, FLOAT fNear, FLOAT fFar);
		
		void PerspectiveFieldOfViewLeftHandFullDepth(FLOAT fTop, FLOAT fBottom, FLOAT fLeft, FLOAT fRight, FLOAT fNear, FLOAT fFar);
		
		void OrthoOffCenterLeftHandHalfDepth(FLOAT fTop, FLOAT fBottom, FLOAT fLeft, FLOAT fRight, FLOAT fNear, FLOAT fFar);
		
		void OrthoOffCenterLeftHandFullDepth(FLOAT fTop, FLOAT fBottom, FLOAT fLeft, FLOAT fRight, FLOAT fNear, FLOAT fFar);

		void Projection(FLOAT fFOV, FLOAT fAspect, FLOAT fNear, FLOAT fFar, bool bIsLeftHand, bool bIsHalfDepth);

		void LookAt(const FLOAT3& Eye, const FLOAT3& At, const FLOAT3& Up);

		void Reflect(const FLOAT4& Plane);
	};

	ALCHEMY_INLINE CMatrix4x4::operator const FLOAT*()const
	{
		return fValue;
	}

	ALCHEMY_INLINE void CMatrix4x4::SetPosition(FLOAT x, FLOAT y, FLOAT z)
	{
		_41 = x;
		_42 = y;
		_43 = z;
	}

	ALCHEMY_INLINE void CMatrix4x4::SetPosition(FLOAT3& Position)
	{
		_41 = Position.x;
		_42 = Position.y;
		_43 = Position.z;
	}

	ALCHEMY_INLINE void CMatrix4x4::SetPosition(FLOAT4& Position)
	{
		_41 = Position.x;
		_42 = Position.y;
		_43 = Position.z;
		_44 = Position.w;
	}

	ALCHEMY_INLINE void CMatrix4x4::GetPosition(FLOAT& x, FLOAT& y, FLOAT& z)const
	{
		x = _41;
		y = _42;
		z = _43;
	}

	ALCHEMY_INLINE void CMatrix4x4::GetPosition(PFLOAT pOutputX, PFLOAT pOutputY, PFLOAT pOutputZ)const
	{
		if(pOutputX)
			*pOutputX = _41;

		if(pOutputX)
			*pOutputY = _42;

		if(pOutputX)
			*pOutputZ = _43;
	}

	ALCHEMY_INLINE void CMatrix4x4::GetPosition(FLOAT3& Position)const
	{
		Position.x = _41;
		Position.y = _42;
		Position.z = _43;
	}

	ALCHEMY_INLINE void CMatrix4x4::GetPosition(FLOAT4& Position)const
	{
		GetPosition(Position.Float3);
		Position.w = _44;
	}

	ALCHEMY_INLINE void CMatrix4x4::GetDirection(FLOAT& x, FLOAT& y, FLOAT& z)const
	{
		x = _31;
		y = _32;
		z = _33;
	}

	ALCHEMY_INLINE void CMatrix4x4::GetDirection(PFLOAT pOutputX, PFLOAT pOutputY, PFLOAT pOutputZ)const
	{
		if(pOutputX)
			*pOutputX = _31;

		if(pOutputX)
			*pOutputY = _32;

		if(pOutputX)
			*pOutputZ = _33;
	}

	ALCHEMY_INLINE void CMatrix4x4::GetDirection(FLOAT3& Direction)const
	{
		Direction.x = _31;
		Direction.y = _32;
		Direction.z = _33;
	}

	ALCHEMY_INLINE FLOAT CMatrix4x4::GetDeterminant3x3()const
	{
		 return (_11 * _22 * _33 - _23 * _32) + _12 * (_23 * _31 - _21 * _33 ) + _13 * (_21 * _32 - _22 * _31);
	}

	ALCHEMY_INLINE void CMatrix4x4::Scaling(FLOAT x, FLOAT y, FLOAT z)
	{
		_11 *= x;
		_22 *= y;
		_33 *= z;
	}
}