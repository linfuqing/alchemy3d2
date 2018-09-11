#pragma once

#include "../Graphics/AlchemyEffect.h"
#include "AlchemyFlashProgram3D.h"

namespace alchemy
{
	class CFlashContext3D;
	class CFlashEffect :
		public CEffect, public CFlashObjectInterface
	{
		friend class CFlashContext3D;
	public:
		CFlashEffect(void);
		~CFlashEffect(void);

		void Destroy();

		/*bool SetMatrix(IEffect::EFFECTTYPE Type, UINT uIndex, const MATRIX4X4& Matrix);
		bool SetParameter(IEffect::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfData, INT nSize);
		bool SetTexture(IEffect::EFFECTTYPE Type, UINT uIndex, CGraphicsResource::HANDLE Texture);

		bool SetMatrixArray(IEffect::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount);
		bool SetMatrixPointerArray(IEffect::EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount);

		bool CommitChange();

		bool Begin();
		bool BeginPass(UINT uIndex);
		bool EndPass();
		bool End();*/

		//bool SetPass(CFlashProgram3D* pPass, UINT uIndex);

		bool GetShader(UINT uPass, UINT uRenderTargetIndex, CShader** ppOutputShader, CShader** ppOutputAdditionalShader);

#ifdef AS3_H
		virtual AS3TYPE Init();
#endif

	protected:
		bool _Create();

	private:
		CFlashProgram3D** m_ppPass;

		//CFlashProgram3D* m_pCurrentPass;

		//UINT m_uTextureFlag;
	};
}
