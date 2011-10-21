#pragma once

#include "../Math/AlchemyMatrix4x4.h"

namespace alchemy
{
	typedef struct
	{
		FLOAT3 Scale;
		FLOAT3 Rotation;
		FLOAT3 Translation;
	}TRANSFORM;

	class CTransform : 
		protected TRANSFORM
	{
	public:
		typedef enum
		{
			SCALE = 0,
			ROTATION,
			TRANSLATION,

			MATRIX
		}DIRTYFLAG;

		CTransform(CMatrix4x4& Matrix);
		~CTransform(void);

		void SetDirtyFlag(UINT uFlag);
		UINT GetDirtyFlag()const;

		void SetMatrix(const MATRIX4X4& Matrix);

		void SetScale(const FLOAT3& Value);
		void SetRotation(const FLOAT3& Value);
		void SetTranslation(const FLOAT3& Value);

		void SetDirection(const FLOAT3& Value);

		const FLOAT3& GetScale();
		const FLOAT3& GetRotation();
		const FLOAT3& GetTranslation();

		void Right(FLOAT fNumSteps);
		void Up(FLOAT fNumSteps);
		void Forward(FLOAT fNumSteps);

		virtual void UpdateTransform();

		virtual void Update();

	protected:
		CMatrix4x4& m_Matrix;
		CQuaternion m_Quaternion;

		UINT m_uDirtyFlag;

		bool m_bIsTransformDirty;
	};

	ALCHEMY_INLINE UINT CTransform::GetDirtyFlag()const
	{
		return m_uDirtyFlag;
	}

	ALCHEMY_INLINE void CTransform::SetDirtyFlag(UINT uFlag)
	{
		m_uDirtyFlag = uFlag;
	}
}