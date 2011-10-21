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

		bool SetMatrix(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const MATRIX4X4& Matrix);
		bool SetParameter(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfData, INT nSize);
		bool SetTexture(CEffectInterface::EFFECTTYPE Type, UINT uIndex, CGraphicsResource::HANDLE Texture);

		bool SetMatrixArray(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount);
		bool SetMatrixPointerArray(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount);

		bool CommitChange();

		bool Begin();
		bool BeginPass(UINT uIndex);
		bool EndPass();
		bool End();

		bool SetPass(CFlashProgram3D* pPass, UINT uIndex);

#ifdef AS3_H
		virtual AS3TYPE Init();
#endif

	protected:
		bool _Create();

	private:
		CEffectInterface::EFFECTTYPE __BuildShaderCode(STRING& Desination, UINT uShaderFunction, const CEffectCode& Code);

		CFlashProgram3D** m_ppPass;

		CFlashProgram3D* m_pCurrentPass;

		UINT m_uTextureFlag;
	};
}
