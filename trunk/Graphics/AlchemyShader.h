#pragma once

//#include "../Core/AlchemyDataType.h"
#include "AlchemyGraphicsResource.h"
#include "AlchemyEffectInterface.h"
#include "../Core/AlchemyString.h"
#include "../Math/AlchemyMatrix4x4.h"

namespace alchemy
{
	class CCompilerInterface;
	class CRenderSystem;
	class CShader :
		public CGraphicsResource
	{
		friend class CRenderSystem;
	public:

		CShader(void);
		~CShader(void);

		CEffectInterface::EFFECTTYPE GetShaderType()const;

		void Destroy();

	protected:
		PVOID m_pCode;
		UINT  m_uCodeSize;
		PVOID m_pAdditionalCode;
		UINT  m_uAdditionalCodeSize;

	private:
		CCompilerInterface* m_pCompiler;

		CEffectInterface::EFFECTTYPE m_Type;
	};

	ALCHEMY_INLINE CEffectInterface::EFFECTTYPE CShader::GetShaderType()const
	{
		return m_Type;
	}
}
