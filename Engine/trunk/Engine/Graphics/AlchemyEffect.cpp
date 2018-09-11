#include "AlchemyEffect.h"

using namespace alchemy;

CEffect::CEffect(void) :
m_pPassInfos(ALCHEMY_NULL),
m_pPassInfoBuffer(ALCHEMY_NULL),
m_uPassCount(0),
m_puRenderTargetIndexCount(ALCHEMY_NULL),
m_puRenderTargetIndexOffset(ALCHEMY_NULL),
m_pCodes(ALCHEMY_NULL),
m_pAdditionalCodes(ALCHEMY_NULL)
{
}

CEffect::~CEffect(void)
{
	if(!m_pPassInfoBuffer)
		DestroyPassInfos( m_pPassInfos, GetPassInfoCount() );

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puRenderTargetIndexCount);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puRenderTargetIndexOffset);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pCodes);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAdditionalCodes);
}

CBuffer& CEffect::CreatePassInfoBuffer(const PASSINFO* pPassInfos, UINT uPassInfoCount)
{
	UINT i, 
	uPassInfoBufferSize = 0, 
	uParameterBufferSize = 0, 
	uTextureParameterBufferSize = 0, 
	uVertexElementBufferSize = 0;
	const PASSINFO* pPassInfo;
	for(i = 0; i < uPassInfoCount; ++ i)
	{
		pPassInfo = &pPassInfos[i];
		uParameterBufferSize        += pPassInfo->uParameterInfoCount;
		uTextureParameterBufferSize += pPassInfo->uTextureCount      ;
		uVertexElementBufferSize    += pPassInfo->uVertexElementCount;
		uPassInfoBufferSize         += sizeof(PASSINFO);
	}
	
	UINT j, 
	uSamplerCount = uTextureParameterBufferSize, 
	uSamplerNameIndex = 0, 
	uSamplerNameBufferSize = 0, 
	uAttributeCount = uVertexElementBufferSize, 
	uAttributeNameIndex = 0, 
	uAttributeNameBufferSize = 0;
	PUINT puSamplerNameLength, puAttributeNameLength;
	ALCHEMY_DEBUG_NEW(puSamplerNameLength  , UINT[uSamplerCount  ]);
	ALCHEMY_DEBUG_NEW(puAttributeNameLength, UINT[uAttributeCount]);
	for(i = 0; i < uPassInfoCount; ++ i)
	{
		pPassInfo = &pPassInfos[i];
		
		for(j = 0; j < pPassInfo->uTextureCount      ; ++ j)
			uSamplerNameBufferSize   += puSamplerNameLength[uSamplerNameIndex     ++] = StringLength(pPassInfo->ppcSamplerNames  [j]) + 1;
		
		for(j = 0; j < pPassInfo->uVertexElementCount; ++ j)
			uAttributeNameBufferSize += puAttributeNameLength[uAttributeNameIndex ++] = StringLength(pPassInfo->ppcAttributeNames[j]) + 1;
	}
	
	uParameterBufferSize        *= sizeof(PARAMETERINFO              );
	uTextureParameterBufferSize *= sizeof(PARAMETERINFO              );
	uVertexElementBufferSize    *= sizeof(CVertexDeclaration::ELEMENT);
	uSamplerNameBufferSize      *= sizeof(CHAR                       );
	uAttributeNameBufferSize    *= sizeof(CHAR                       );

	UINT uSamplerNamesBufferSize = uSamplerCount * sizeof(const CHAR*), uAttributeNamesBufferSize = uAttributeCount * sizeof(const CHAR*);
	uSamplerNameIndex   = 0;
	uAttributeNameIndex = 0;
	
	CBuffer& Buffer = CreateBuffer(uParameterBufferSize        + 
								   uTextureParameterBufferSize + 
								   uVertexElementBufferSize    + 
								   uSamplerNameBufferSize      +
								   uSamplerNamesBufferSize     + 
								   uAttributeNameBufferSize    +
								   uAttributeNamesBufferSize   + 
								   uPassInfoBufferSize);
	PUINT8 puBuffer = reinterpret_cast<PUINT8>( Buffer.GetBufferPointer() );
	LPPASSINFO pCurrentPassInfo = reinterpret_cast<LPPASSINFO>(puBuffer);
	LPPARAMETERINFO pParameterInfo = reinterpret_cast<LPPARAMETERINFO>(puBuffer += uPassInfoBufferSize), pTextureParameterInfo = reinterpret_cast<LPPARAMETERINFO>(puBuffer += uParameterBufferSize);
	CVertexDeclaration::LPELEMENT pVertexElement = reinterpret_cast<CVertexDeclaration::LPELEMENT>(puBuffer += uTextureParameterBufferSize);
	const CHAR** ppcSamplerNames = reinterpret_cast<const CHAR**>(puBuffer += uVertexElementBufferSize), ** ppcAttributeNames = reinterpret_cast<const CHAR**>(puBuffer += uSamplerNamesBufferSize);
	CHAR* pcSamplerName = reinterpret_cast<CHAR*> (puBuffer += uAttributeNamesBufferSize), * pcAttributeName = reinterpret_cast<CHAR*> (puBuffer += uSamplerNameBufferSize);
	UINT uSamplerNameLength, uAttributeNameLength;
	for(i = 0; i < uPassInfoCount; ++ i)
	{
		pCurrentPassInfo->pParameterInfos        = pParameterInfo;
		pCurrentPassInfo->pTextureParameterInfos = pTextureParameterInfo;
		pCurrentPassInfo->pVertexElements        = pVertexElement;
		pCurrentPassInfo->ppcSamplerNames        = ppcSamplerNames;
		pCurrentPassInfo->ppcAttributeNames      = ppcAttributeNames;

		pPassInfo = &pPassInfos[i];
		for(j = 0; j < pPassInfo->uParameterInfoCount; ++ j)
			*(pParameterInfo        ++) = pPassInfo->pParameterInfos[j];

		for(j = 0; j < pPassInfo->uTextureCount; ++ j)
		{
			*(pTextureParameterInfo ++) = pPassInfo->pTextureParameterInfos[j];
			
			*(ppcSamplerNames     ++)   = pcSamplerName;
			
			uSamplerNameLength          = puSamplerNameLength[uSamplerNameIndex ++];
			
			ALCHEMY_MEMORY_COPY( pcSamplerName, pPassInfo->ppcSamplerNames[j], uSamplerNameLength * sizeof(CHAR) );
			
			pcSamplerName += uSamplerNameLength;
		}

		for(j = 0; j < pPassInfo->uVertexElementCount; ++ j)
		{
			*(pVertexElement        ++) = pPassInfo->pVertexElements[j];
			
			*(ppcAttributeNames     ++) = pcAttributeName;
			
			uAttributeNameLength        = puAttributeNameLength[uAttributeNameIndex ++];
			
			ALCHEMY_MEMORY_COPY( pcAttributeName, pPassInfo->ppcAttributeNames[j], uAttributeNameLength * sizeof(CHAR) );
				
			pcAttributeName += uAttributeNameLength;
		}

		pCurrentPassInfo->uParameterInfoCount    = pPassInfo->uParameterInfoCount;
		pCurrentPassInfo->uTextureCount          = pPassInfo->uTextureCount;
		pCurrentPassInfo->uVertexElementCount    = pPassInfo->uVertexElementCount;

		++ pCurrentPassInfo;
	}
	
	ALCHEMY_DEBUG_DELETE_ARRAY(puSamplerNameLength);
	ALCHEMY_DEBUG_DELETE_ARRAY(puAttributeNameLength);

	return Buffer;
}

void CEffect::DestroyPassInfos(LPPASSINFO pPassInfos, UINT uPassInfoCount)
{
	LPPASSINFO pPassInfo;
	UINT i, j;
	for(i = 0; i < uPassInfoCount; ++ i)
	{
		pPassInfo = &pPassInfos[i];
		ALCHEMY_DEBUG_DELETE_ARRAY(pPassInfo->pParameterInfos);
		ALCHEMY_DEBUG_DELETE_ARRAY(pPassInfo->pTextureParameterInfos);
		ALCHEMY_DEBUG_DELETE_ARRAY(pPassInfo->pVertexElements);
		
		if(pPassInfo->ppcSamplerNames)
		{
			for(j = 0; j < pPassInfo->uTextureCount; ++ j)
				ALCHEMY_DEBUG_DELETE_ARRAY(pPassInfo->ppcSamplerNames[j]);
			
			ALCHEMY_DEBUG_DELETE_ARRAY(pPassInfo->ppcSamplerNames);
		}
		
		if(pPassInfo->ppcAttributeNames)
		{
			for(j = 0; j < pPassInfo->uVertexElementCount; ++ j)
				ALCHEMY_DEBUG_DELETE_ARRAY(pPassInfo->ppcAttributeNames[j]);
			
			ALCHEMY_DEBUG_DELETE_ARRAY(pPassInfo->ppcAttributeNames);
		}
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(pPassInfos);
}

void CEffect::Destroy()
{
	if(m_pPassInfoBuffer)
	{
		m_pPassInfoBuffer->Release();

		m_pPassInfoBuffer = ALCHEMY_NULL;

		m_pPassInfos = ALCHEMY_NULL;
	}

	if(m_pPassInfos)
	{
		DestroyPassInfos( m_pPassInfos, GetPassInfoCount() );

		m_pPassInfos = ALCHEMY_NULL;
	}
	
	m_uPassCount = 0;

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puRenderTargetIndexCount);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puRenderTargetIndexOffset);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pCodes);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAdditionalCodes);
}
