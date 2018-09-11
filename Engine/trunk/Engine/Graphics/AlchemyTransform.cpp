#include "AlchemyTransform.h"

#include "../Math/AlchemyVector3.h"
#include "../Math/AlchemyMath.h"

#include "../Core/AlchemyNull.h"
#include "../Core/AlchemyBitFlag.h"

using namespace alchemy;

CTransform::CTransform(CMatrix4x4& Matrix) :
m_Matrix(Matrix),
m_uDirtyFlag(0),
m_bIsTransformDirty(false)
{
	Scale.x = 1.0f;
	Scale.y = 1.0f;
	Scale.z = 1.0f;

	Rotation.x = 0.0f;
	Rotation.y = 0.0f;
	Rotation.z = 0.0f;

	Translation.x = 0.0f;
	Translation.y = 0.0f;
	Translation.z = 0.0f;
}

CTransform::~CTransform(void)
{

}

void CTransform::SetMatrix(const MATRIX4X4& Matrix)
{
	m_bIsTransformDirty = true;

	ALCHEMY_SET_BIT(m_uDirtyFlag, SCALE);
	ALCHEMY_SET_BIT(m_uDirtyFlag, ROTATION);
	ALCHEMY_SET_BIT(m_uDirtyFlag, TRANSLATION);

	ALCHEMY_CLEAR_BIT(m_uDirtyFlag, MATRIX);

	m_Matrix = Matrix;
}

void CTransform::SetScale(const FLOAT3& Value)
{
	if( !ALCHEMY_TEST_BIT(m_uDirtyFlag, MATRIX) && ALCHEMY_TEST_BIT(m_uDirtyFlag, SCALE) )
	{
		m_Matrix.Decompose(ALCHEMY_NULL, &Rotation, ALCHEMY_TEST_BIT(m_uDirtyFlag, TRANSLATION) ? &Translation : ALCHEMY_NULL, &m_Quaternion);

		Rotation.x *= g_fRAD2DEG;
		Rotation.y *= g_fRAD2DEG;
		Rotation.z *= g_fRAD2DEG;

		ALCHEMY_CLEAR_MASK(m_uDirtyFlag, MATRIX);
	}

	Scale = Value;

	ALCHEMY_SET_BIT(m_uDirtyFlag, MATRIX);
	ALCHEMY_SET_BIT(m_uDirtyFlag, SCALE);
}

void CTransform::SetScale(FLOAT x, FLOAT y, FLOAT z)
{
	Scale.x = x;
	Scale.y = y;
	Scale.z = z;

	SetScale(Scale);
}

void CTransform::SetRotation(const FLOAT3& Value)
{
	if( !ALCHEMY_TEST_BIT(m_uDirtyFlag, MATRIX) && ALCHEMY_TEST_BIT(m_uDirtyFlag, ROTATION) )
	{
		m_Matrix.Decompose(ALCHEMY_TEST_BIT(m_uDirtyFlag, SCALE) ? &Scale : ALCHEMY_NULL, ALCHEMY_NULL, ALCHEMY_TEST_BIT(m_uDirtyFlag, TRANSLATION) ? &Translation : ALCHEMY_NULL, ALCHEMY_NULL);

		ALCHEMY_CLEAR_MASK(m_uDirtyFlag, MATRIX);
	}

	Rotation = Value;

	ALCHEMY_SET_BIT(m_uDirtyFlag, MATRIX);
	ALCHEMY_SET_BIT(m_uDirtyFlag, ROTATION);
}

void CTransform::SetRotation(FLOAT x, FLOAT y, FLOAT z)
{
	Rotation.x = x;
	Rotation.y = y;
	Rotation.z = z;
	
	SetRotation(Rotation);
}

void CTransform::SetTranslation(const FLOAT3& Value)
{
	if( !ALCHEMY_TEST_BIT(m_uDirtyFlag, MATRIX) && ALCHEMY_TEST_BIT(m_uDirtyFlag, TRANSLATION) )
	{
		m_Matrix._41 = Value.x;
		m_Matrix._42 = Value.y;
		m_Matrix._43 = Value.z;
		//m_Matrix._44 = Translation.w;

		ALCHEMY_CLEAR_BIT(m_uDirtyFlag, TRANSLATION);
	}

	ALCHEMY_SET_BIT(m_uDirtyFlag, MATRIX);
	ALCHEMY_SET_BIT(m_uDirtyFlag, TRANSLATION);

	Translation = Value;
}

void CTransform::SetTranslation(FLOAT x, FLOAT y, FLOAT z)
{
	Translation.x = x;
	Translation.y = y;
	Translation.z = z;
	
	SetTranslation(Translation);
}

void CTransform::SetDirection(const FLOAT3& Value)
{
	CVector3 Direction(Value), Rotation;

	if(Direction.x == 0 && Direction.z == 0 && Direction.y == 0)
	{
		Rotation.x = 0;
		Rotation.y = 0;
	}
	else if(Direction.x == 0 && Direction.z == 0)
	{
		Rotation.x = - asin( Direction.y / Direction.GetLength() );
		Rotation.y =   0;
	}
	else if(Direction.z < 0)
	{
		Rotation.x = - asin( Direction.y / Direction.GetLength() );
		Rotation.y =   g_fPI - asin( Direction.x / sqrt(Direction.x * Direction.x + Direction.z * Direction.z) );
	}
	else
	{
		Rotation.x = - asin( Direction.y / Direction.GetLength() );
		Rotation.y =   asin( Direction.x / sqrt(Direction.x * Direction.x + Direction.z * Direction.z) );
	}

	Rotation.x *= g_fRAD2DEG;
	Rotation.y *= g_fRAD2DEG;
	Rotation.z  = this->Rotation.z;

	SetRotation(Rotation);
}

void CTransform::SetDirection(FLOAT x, FLOAT y, FLOAT z)
{
	CFloat3 Value(x, y, z);
	SetDirection(Value);
}

const FLOAT3& CTransform::GetScale()
{
	if( !ALCHEMY_TEST_BIT(m_uDirtyFlag, MATRIX) && ALCHEMY_TEST_BIT(m_uDirtyFlag, SCALE) )
	{
		if( ALCHEMY_TEST_BIT(m_uDirtyFlag, ROTATION) )
		{
			m_Matrix.Decompose(&Scale, &Rotation   , ALCHEMY_TEST_BIT(m_uDirtyFlag, TRANSLATION) ? &Translation :ALCHEMY_NULL, &m_Quaternion);

			Rotation.x *= g_fRAD2DEG;
			Rotation.y *= g_fRAD2DEG;
			Rotation.z *= g_fRAD2DEG;
		}
		else
			m_Matrix.Decompose(&Scale, ALCHEMY_NULL, ALCHEMY_TEST_BIT(m_uDirtyFlag, TRANSLATION) ? &Translation :ALCHEMY_NULL, ALCHEMY_NULL );

		ALCHEMY_CLEAR_MASK(m_uDirtyFlag, MATRIX);
	}

	return Scale;
}

const FLOAT3& CTransform::GetRotation()
{
	if( !ALCHEMY_TEST_BIT(m_uDirtyFlag, MATRIX) && ALCHEMY_TEST_BIT(m_uDirtyFlag, ROTATION) )
	{
		m_Matrix.Decompose(ALCHEMY_TEST_BIT(m_uDirtyFlag, SCALE) ? &Scale : ALCHEMY_NULL, &Rotation, ALCHEMY_TEST_BIT(m_uDirtyFlag, TRANSLATION) ? &Translation :ALCHEMY_NULL, &m_Quaternion);

		Rotation.x *= g_fRAD2DEG;
		Rotation.y *= g_fRAD2DEG;
		Rotation.z *= g_fRAD2DEG;

		ALCHEMY_CLEAR_MASK(m_uDirtyFlag, MATRIX);
	}

	return Rotation;
}

const FLOAT3& CTransform::GetTranslation()
{
	if( !ALCHEMY_TEST_BIT(m_uDirtyFlag, MATRIX) && ALCHEMY_TEST_BIT(m_uDirtyFlag, TRANSLATION) )
	{
		Translation.x = m_Matrix._41;
		Translation.y = m_Matrix._42;
		Translation.z = m_Matrix._43;
		//Translation.w = m_Matrix._44;

		ALCHEMY_CLEAR_BIT(m_uDirtyFlag, TRANSLATION);
	}

	return Translation;
}

void CTransform::UpdateTransform()
{
}

void CTransform::Update()
{
	if( ALCHEMY_TEST_BIT(m_uDirtyFlag, MATRIX) )
	{
		if( ALCHEMY_TEST_BIT(m_uDirtyFlag, SCALE) || ALCHEMY_TEST_BIT(m_uDirtyFlag, ROTATION) )
		{
			m_Matrix.Identity();

			m_Matrix.Scaling(Scale.x, Scale.y, Scale.z);

			ALCHEMY_CLEAR_BIT(m_uDirtyFlag, SCALE);

			if( ALCHEMY_TEST_BIT(m_uDirtyFlag, ROTATION) )
			{
				m_Quaternion.RotationYawPitchRoll(Rotation.y * g_fDEG2RAD, Rotation.x * g_fDEG2RAD, Rotation.z * g_fDEG2RAD);
				
				CMatrix4x4 Matrix;

				Matrix.RotationQuaternion(m_Quaternion);

				m_Matrix *= Matrix;

				ALCHEMY_CLEAR_BIT(m_uDirtyFlag, ROTATION);
			}

			m_Matrix._41 = Translation.x;
			m_Matrix._42 = Translation.y;
			m_Matrix._43 = Translation.z;
			//m_Matrix._44 = Translation.w;

			ALCHEMY_CLEAR_BIT(m_uDirtyFlag, TRANSLATION);

			//ALCHEMY_SET_BIT(m_uDirtyFlag, RIGHT  );
			//ALCHEMY_SET_BIT(m_uDirtyFlag, UP     );
			//ALCHEMY_SET_BIT(m_uDirtyFlag, FORWARD);
		}
		else if( ALCHEMY_TEST_BIT(m_uDirtyFlag, TRANSLATION) )
		{
			m_Matrix._41 = Translation.x;
			m_Matrix._42 = Translation.y;
			m_Matrix._43 = Translation.z;
			//m_Matrix._44 = Translation.w;

			ALCHEMY_CLEAR_BIT(m_uDirtyFlag, TRANSLATION);
		}

		ALCHEMY_CLEAR_BIT(m_uDirtyFlag, MATRIX);

		m_bIsTransformDirty = true;
	}

	if(m_bIsTransformDirty)
		UpdateTransform();
}

void CTransform::Right(FLOAT fNumSteps)
{
	CVector3 Right(m_Matrix._11, m_Matrix._12, m_Matrix._13);

	if(m_Matrix._14)
		Right /= m_Matrix._14;

	Right.Normalize();

	Right *= fNumSteps;

	Right += Translation;

	SetTranslation(Right);
}

void CTransform::Up(FLOAT fNumSteps)
{
	CVector3 Up(m_Matrix._21, m_Matrix._22, m_Matrix._23);

	if(m_Matrix._24)
		Up /= m_Matrix._24;

	Up.Normalize();

	Up *= fNumSteps;

	Up += Translation;

	SetTranslation(Up);
}

void CTransform::Forward(FLOAT fNumSteps)
{
	CVector3 Forward(m_Matrix._31, m_Matrix._32, m_Matrix._33);

	if(m_Matrix._34)
		Forward /= m_Matrix._34;

	Forward.Normalize();

	Forward *= fNumSteps;

	Forward += Translation;

	SetTranslation(Forward);
}

void CTransform::Clone(TRANSFORM& Transform)const
{
	Copy(Transform.Scale      , Scale);
	Copy(Transform.Rotation   , Rotation);
	Copy(Transform.Translation, Translation);
}

void CTransform::Clone(CTransform& Transform)const
{
	if( ALCHEMY_TEST_BIT(m_uDirtyFlag, MATRIX) )
	{
		Transform.SetScale(Scale);
		Transform.SetRotation(Rotation);
		Transform.SetTranslation(Rotation);
	}
	else
		Transform.SetMatrix(m_Matrix);
}