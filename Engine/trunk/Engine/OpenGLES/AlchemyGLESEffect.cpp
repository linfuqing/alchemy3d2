#include "AlchemyGLESEffect.h"
#include "AlchemyGLESContext.h"

using namespace alchemy;

CGLESEffect::CGLESEffect() :
m_ppPass(ALCHEMY_NULL)
{
}

CGLESEffect::~CGLESEffect()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPass);
}

bool CGLESEffect::GetShader(UINT uPass, UINT uRenderTargetIndex, CShader** ppOutputShader, CShader** ppOutputAdditionalShader)
{
	if(ppOutputAdditionalShader)
		*ppOutputAdditionalShader = ALCHEMY_NULL;
	
	if(ppOutputShader)
	{
		if(m_ppPass && CEffect::m_pPassInfos && m_puRenderTargetIndexCount && m_puRenderTargetIndexOffset && uPass < m_uPassCount && uRenderTargetIndex < m_puRenderTargetIndexCount[uPass])
		{
			*ppOutputShader =  m_ppPass[m_puRenderTargetIndexOffset[uPass] + uRenderTargetIndex];
			
			return true;
		}
		
		return false;
	}
	
	return true;
}

void CGLESEffect::Destroy()
{
	if(m_ppPass)
	{
		UINT uPassInfoCount = GetPassInfoCount();
		CGLESShader* pProgram;
		for(UINT i = 0; i < uPassInfoCount; ++ i)
		{
			pProgram = m_ppPass[i];
			if(pProgram)
				_Release(*pProgram);
		}
		
		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPass);
	}
	
	CEffect::Destroy();
}
#include<stdio.h>
bool CGLESEffect::_Create()
{
	UINT uPassCount = CEffect::GetTotalPass();
	if(!uPassCount)
	{
		ALCHEMY_DEBUG_WARNING("CGLESEffect::_Create: No pass.");
		
		return false;
	}
	
	if(m_ppPass)
	{
		UINT uPassInfoCount = GetPassInfoCount();
		CGLESShader* pProgram;
		for(UINT i = 0; i < uPassInfoCount; ++ i)
		{
			pProgram = m_ppPass[i];
			if(pProgram)
				_Release(*pProgram);
		}
		
		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPass);
	}
	
	CGLESContext& Context = static_cast<CGLESContext&>( GetGraphicsResourcePool() );
	
	ALCHEMY_DEBUG_NEW(m_ppPass, CGLESShader*[GetPassInfoCount()]);
	CGLESShader* pProgram;
	UINT i, j, k, uRenderTargetCount, uPassInfoIndex = 0;
	GLuint uProgramHandle, uSampler;
	const CEffect::PASSINFO* pPassInfo;
	
	for(i = 0; i < uPassCount; ++ i)
	{
		uRenderTargetCount = CEffect::GetRenderTargetCount(i);
		for(j = 0; j < uRenderTargetCount; ++ j)
		{
			if( ( pProgram = m_ppPass[uPassInfoIndex] = static_cast<CGLESShader*>( Context.CreateShader(IEffect::PROGRAM, 
																								Context.GetCompiler(), 
																								static_cast<const CHAR*>(CEffect::m_pCodes[uPassInfoIndex]          ),
																								static_cast<const CHAR*>(CEffect::m_pAdditionalCodes[uPassInfoIndex]) ) ) ) )
			{
				_AddRef(*pProgram);
				
				pPassInfo = CEffect::GetPassInfo(i, j);
				if(pPassInfo)
				{
					uProgramHandle = pProgram->GetShaderHandle();
					
					/*if(pPassInfo->pTextureParameterInfos && pPassInfo->ppcSamplerNames)
					{
						glUseProgram(uProgramHandle);
						for(k = 0; k < pPassInfo->uTextureCount; ++ k)
						{
							uSampler = glGetUniformLocation(uProgramHandle, pPassInfo->ppcSamplerNames[k]);
							glUniform1i(uSampler, pPassInfo->pTextureParameterInfos[k].uIndex);
						}
					}*/
					
					if(pPassInfo->pVertexElements && pPassInfo->ppcAttributeNames)
					{
						for(k = 0; k < pPassInfo->uVertexElementCount; ++ k)
							glBindAttribLocation(uProgramHandle, 
												 pPassInfo->pVertexElements[k].uIndex, 
												 pPassInfo->ppcAttributeNames[k]);
					}
					
					glLinkProgram(uProgramHandle);
					glUseProgram(uProgramHandle);
					if(pPassInfo->pTextureParameterInfos && pPassInfo->ppcSamplerNames)
					{
						glUseProgram(uProgramHandle);
						for(k = 0; k < pPassInfo->uTextureCount; ++ k)
						{
							uSampler = glGetUniformLocation(uProgramHandle, pPassInfo->ppcSamplerNames[k]);
							glUniform1i(uSampler, pPassInfo->pTextureParameterInfos[k].uIndex);
						}
					}
				}
			}
			else 
			{
				ALCHEMY_DEBUG_WARNING("CGLESEffect::_Create: Program has been created fail.");
				
				return false;
			}
			
			++ uPassInfoIndex;
		}
	}
	
	return true;
}