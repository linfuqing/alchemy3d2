#pragma once

//#include "../Core/AlchemyDataType.h"
#include "AlchemyGraphicsResource.h"
#include "AlchemyEffectInterface.h"

#include "../Core/AlchemyBuffer.h"
//#include "../Math/AlchemyMatrix4x4.h"

namespace alchemy
{
	struct ICompiler;
	class CRenderSystem;
	class CShader :
		public CGraphicsResource
	{
		friend class CRenderSystem;
	public:

		CShader(void);
		~CShader(void);

		IEffect::EFFECTTYPE GetShaderType()const;

		void Destroy();

	protected:
		CBuffer* m_pCode;
		CBuffer* m_pAdditionalCode;

	private:
		IEffect::EFFECTTYPE m_Type;
	};

	ALCHEMY_INLINE IEffect::EFFECTTYPE CShader::GetShaderType()const
	{
		return m_Type;
	}
}
