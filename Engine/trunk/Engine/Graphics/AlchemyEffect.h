#pragma once

//#include "../Math/AlchemyMatrix4x4.h"
//#include "AlchemyTexture.h"
#include "AlchemyEffectInterface.h"
#include "AlchemyVertexDeclaration.h"
//#include "AlchemyEffectCode.h"
#include "AlchemyShader.h"

#include "../Core/AlchemyString.h"

namespace alchemy
{
	class CRenderSystem;
	class CEffect :
		/*public CEffectInterface, */public CGraphicsResource
	{
		friend class CRenderSystem;
	public:
		typedef struct
		{
			IEffect::EFFECTTYPE Type;
			UINT uIndex;
		}PARAMETERINFO, * LPPARAMETERINFO;

		typedef struct  
		{
			PARAMETERINFO* pParameterInfos;
			PARAMETERINFO* pTextureParameterInfos;
			CVertexDeclaration::ELEMENT* pVertexElements;
			const CHAR** ppcSamplerNames;
			const CHAR** ppcAttributeNames;
			UINT uParameterInfoCount;
			UINT uTextureCount;
			UINT uVertexElementCount;
		}PASSINFO, * LPPASSINFO;

		CEffect(void);
		~CEffect(void);

		static CBuffer& CreatePassInfoBuffer(const PASSINFO* pPassInfos, UINT uPassInfoCount);
		static void DestroyPassInfos(LPPASSINFO pPassInfos, UINT uPassInfoCount);


		UINT GetTotalPass()const;

		/*virtual bool CommitChange() = 0;

		virtual bool Begin() = 0;
		virtual bool BeginPass(UINT uIndex) = 0;
		virtual bool EndPass() = 0;
		virtual bool End() = 0;*/
		UINT GetPassInfoCount()const;

		UINT GetRenderTargetCount(UINT uPass)const;

		const PASSINFO* GetPassInfo(UINT uPass, UINT uRenderTargetIndex)const;

		void Destroy();

		virtual bool GetShader(UINT uPass, UINT uRenderTargetIndex, CShader** ppOutputShader, CShader** ppOutputAdditionalShader) = 0;

	protected:
		LPPASSINFO m_pPassInfos;
		CBuffer* m_pPassInfoBuffer;
		UINT m_uPassCount;

		PUINT m_puRenderTargetIndexCount;
		PUINT m_puRenderTargetIndexOffset;

		STRING* m_pCodes;
		STRING* m_pAdditionalCodes;
	};

	ALCHEMY_INLINE UINT CEffect::GetTotalPass()const
	{
		return m_uPassCount;
	}

	ALCHEMY_INLINE UINT CEffect::GetPassInfoCount()const
	{
		if(!m_puRenderTargetIndexCount || !m_puRenderTargetIndexOffset)
			return 0;

		UINT uFinalPassIndex = m_uPassCount - 1;
		return m_puRenderTargetIndexOffset[uFinalPassIndex] + m_puRenderTargetIndexCount[uFinalPassIndex];
	}

	ALCHEMY_INLINE UINT CEffect::GetRenderTargetCount(UINT uPass)const
	{
		if(m_puRenderTargetIndexCount && m_uPassCount > uPass)
			return m_puRenderTargetIndexCount[uPass];

		return 0;
	}

	ALCHEMY_INLINE const CEffect::PASSINFO* CEffect::GetPassInfo(UINT uPass, UINT uRenderTargetIndex)const
	{
		if(m_pPassInfos && m_puRenderTargetIndexCount && m_puRenderTargetIndexOffset && uPass < m_uPassCount && uRenderTargetIndex < m_puRenderTargetIndexCount[uPass])
			return &m_pPassInfos[m_puRenderTargetIndexOffset[uPass] + uRenderTargetIndex];

		return ALCHEMY_NULL;
	}
}