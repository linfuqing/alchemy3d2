#include "AlchemySkinMeshController.h"
#include "AlchemyEffectCode.h"
#include "AlchemyVertexDeclaration.h"
#include "AlchemyHardwareSkinEffect.h"
#include "AlchemyBone.h"
#include "../Core/AlchemyUtils.h"

#include "AlchemySceneManager.h"

using namespace alchemy;

CSkinMeshController::CSkinMeshController() :
m_pRoot(ALCHEMY_NULL),
m_pSourceMesh(ALCHEMY_NULL),
m_pDestinationMesh(ALCHEMY_NULL),
m_ppBones(ALCHEMY_NULL),
m_pSkinInfo(ALCHEMY_NULL),
m_ppChannels(ALCHEMY_NULL),
m_puChannelsCount(ALCHEMY_NULL),
m_pBoneCombination(ALCHEMY_NULL),
m_uBoneCombinationCount(0),
m_pBoneMatrices(ALCHEMY_NULL),
m_SkinType(SKIN_NONE),
m_puSkinningStream(ALCHEMY_NULL),
m_uSkinningStreamCount(0),
m_pHardwareSkinEffect(ALCHEMY_NULL)
{

}

CSkinMeshController::~CSkinMeshController()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppBones);

	if(m_ppChannels)
	{
		for(UINT i = 0; i < m_uSkinningStreamCount; ++ i)
			ALCHEMY_DEBUG_DELETE_ARRAY(m_ppChannels[i]);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppChannels);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puChannelsCount);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puSkinningStream);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pBoneMatrices);

	ALCHEMY_DEBUG_DELETE(m_pHardwareSkinEffect);
}

bool CSkinMeshController::Create(CMesh& Mesh, CSkinMeshController::SKINTYPE SkinType, CSkinInfo& SkinInfo, CBone& Root, const CEffectCode::ELEMENTDECLARATION* pSkinElementParameters, UINT uElementCount, UINT uMaxBoneInfluence)
{
	Destroy();

	CSceneManager* pSceneManager = CSceneManager::GetInstance();
	if(!pSceneManager)
		return false;

	m_pSourceMesh = &Mesh;
	m_pSourceMesh->AddRef();

	m_SkinType = SkinType;

	if(!pSkinElementParameters || !uElementCount)
	{
		ALCHEMY_DEBUG_WARNING("No skin element.");

		return false;
	}

	m_pSkinInfo = &SkinInfo;
	m_pRoot     = &Root;

	UINT uBoneCount = SkinInfo.GetBoneCount();

	ALCHEMY_DEBUG_NEW(m_ppBones, CBone*[uBoneCount]);


	UINT i;
	STRING Name;
	for(i = 0; i < uBoneCount; ++ i)
	{
		Name = SkinInfo.GetName(i);
		if(Root.GetName() == Name)
			m_ppBones[i] = &Root;
		else
			m_ppBones[i] = Root.GetChildByName(Name);
	}

	ALCHEMY_DEBUG_NEW(m_pBoneMatrices, CMatrix4x4[uBoneCount]);

	CMesh* pMesh = ALCHEMY_NULL;

	if(SkinType == SKIN_HARDWARE)
	{
		UINT uMaxVertexInfluence = 0;
		UINT uMatrixIndexOffset  = 1;
		CEffectCode EffectCode( pSceneManager->GetRenderSystem().GetCompiler() );
		uMatrixIndexOffset = EffectCode.GetRegisterCount(CEffectCode::MATRIX4X3_VARIABLE) >> EffectCode.GetCompiler().GetCompilerInfo().uByteAlignmentShift;

		CBuffer* pBoneCombinationBuffer = ALCHEMY_NULL;
		UINT uBoneCombinationCount = 0;
		pMesh = m_pSourceMesh->ConvertToBlendedMesh(SkinInfo, uMatrixIndexOffset, uMaxBoneInfluence, &uMaxVertexInfluence, &pBoneCombinationBuffer, &uBoneCombinationCount);

		if(pMesh)
		{
			if(pBoneCombinationBuffer)
			{
				m_pBoneCombination      = pBoneCombinationBuffer;
				m_uBoneCombinationCount = uBoneCombinationCount;
			}

			/*if(pEffectCode)
			{*/
			if(!m_pHardwareSkinEffect)
				ALCHEMY_DEBUG_NEW(m_pHardwareSkinEffect, CHardwareSkinEffect);

			m_pHardwareSkinEffect->Create(uMaxBoneInfluence ? Mininum(uBoneCount, uMaxBoneInfluence) : uBoneCount, uMaxVertexInfluence, pSkinElementParameters, uElementCount, m_pBoneCombination ? ALCHEMY_NULL : m_pBoneMatrices);
			//}
		}
	}
	else if(SkinType == SKIN_SOFTWARE)
	{
		CVertexDeclaration::LPVERTEXELEMENT pSkinVertexElement;

		ALCHEMY_DEBUG_NEW(pSkinVertexElement, CVertexDeclaration::VERTEXELEMENT[uElementCount]);

		CVertexDeclaration& VertexDeclaration = m_pSourceMesh->GetVertex().GetVertexDeclaration();
		const CVertexDeclaration::VERTEXELEMENT* pSourceVertexElement = VertexDeclaration.GetVertexElements();

		UINT uSourceVertexElementCount = VertexDeclaration.GetVertexElementCount(), uCurrentElement;
		const CEffectCode::ELEMENTDECLARATION* pElementParameter;
		for(i = 0; i < uElementCount; ++ i)
		{
			pElementParameter = &pSkinElementParameters[i];
			uCurrentElement = CVertexDeclaration::FindVertexElement(pElementParameter->Usage, pElementParameter->Type, pElementParameter->uIndex, pSourceVertexElement, uSourceVertexElementCount);

			if(uCurrentElement < uSourceVertexElementCount)
				pSkinVertexElement[i] = pSourceVertexElement[uCurrentElement];
			else
			{
				ALCHEMY_DEBUG_WARNING("CSceneObject::SetSkinInfo: Skin vertex element lookup failed.");

				ALCHEMY_DEBUG_DELETE_ARRAY(pSkinVertexElement);

				return false;
			}
		}

		m_uSkinningStreamCount = CVertexDeclaration::GetVertexElementStreamCount(pSkinVertexElement, uElementCount);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_puSkinningStream);
		ALCHEMY_DEBUG_NEW(m_puSkinningStream, UINT[m_uSkinningStreamCount]);

		UINT uCurrentStream, j;
		const CVertexDeclaration::VERTEXELEMENT* pCurrentElement;

		m_uSkinningStreamCount = 0;
		for(i = 0; i < uElementCount; ++ i)
		{
			pCurrentElement = pSkinVertexElement + m_uSkinningStreamCount;

			++ m_uSkinningStreamCount;

			uCurrentStream = pSkinVertexElement[i].uStream;
			for(j = 0; j < i; ++ j)
			{
				if(uCurrentStream == pSkinVertexElement[j].uStream)
				{
					-- m_uSkinningStreamCount;

					pCurrentElement = ALCHEMY_NULL;

					break;
				}
			}

			if(pCurrentElement)
				m_puSkinningStream[m_uSkinningStreamCount - 1] = pCurrentElement->uStream;
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppChannels);
		ALCHEMY_DEBUG_NEW(m_ppChannels, CSkinInfo::LPCHANNEL[m_uSkinningStreamCount]);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_puChannelsCount);
		ALCHEMY_DEBUG_NEW(m_puChannelsCount, UINT[m_uSkinningStreamCount]);

		UINT uStream, uChannelCount;
		CSkinInfo::LPCHANNEL pChannels, pCurrentChannel;
		for(i = 0; i < m_uSkinningStreamCount; ++ i)
		{
			uStream = m_puSkinningStream[i];
			uChannelCount = 0;
			for(j = 0; j < uElementCount; ++ j)
			{
				if(pSkinVertexElement[j].uStream == uStream)
					++ uChannelCount;
			}

			m_puChannelsCount[i] = uChannelCount;
			ALCHEMY_DEBUG_NEW(m_ppChannels[i], CSkinInfo::CHANNEL[uChannelCount]);

			pChannels = m_ppChannels[i];
			uChannelCount = 0;
			for(j = 0; j < uElementCount; ++ j)
			{
				pCurrentElement = &pSkinVertexElement[j];
				if(pCurrentElement->uStream == uStream)
				{
					pCurrentChannel = &pChannels[uChannelCount ++];

					pCurrentChannel->uSourceOffset = pCurrentChannel->uDestinationOffset = pCurrentElement->uOffset;

					pCurrentChannel->bIsNormal = pCurrentElement->Usage == CVertexDeclaration::POSITION ? false : true;
				}
			}
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(pSkinVertexElement);

		UINT uUsage = ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY) | ALCHEMY_FLAG(VERTEXBUFFER::DYNAMIC);
		PUINT puUsage;

		ALCHEMY_DEBUG_NEW(puUsage, UINT[m_uSkinningStreamCount]);

		for(i = 0; i < m_uSkinningStreamCount; ++ i)
			puUsage[i] = uUsage;

		pMesh = m_pSourceMesh->Clone( m_puSkinningStream, m_uSkinningStreamCount, puUsage, false, ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY) );

		ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);
	}

	if(pMesh)
	{
		if(m_pDestinationMesh)
			m_pDestinationMesh->Release();

		m_pDestinationMesh = pMesh;

		pMesh->AddRef();
	}

	/*if(m_pRoot)
		m_pRoot->Update();*/

	return true;
}

bool CSkinMeshController::Create(CMesh& Mesh, CSkinMeshController::SKINTYPE SkinType, CSkinInfo& SkinInfo, CBone& Root, UINT uMaxBoneInfluence)
{
	CEffectCode::ELEMENTDECLARATION SkinElementParameters[4];
	CEffectCode::LPELEMENTDECLARATION pSkinElement;

	UINT uSkinElementParameterCount = 0;

	CVertexDeclaration& VertexDeclaration = Mesh.GetVertex().GetVertexDeclaration();
	const CVertexDeclaration::VERTEXELEMENT* pSourceVertexElement = VertexDeclaration.GetVertexElements();

	UINT uSourceVertexElementCount = VertexDeclaration.GetVertexElementCount(), uCurrentElement;

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pSkinElement         = &SkinElementParameters[uSkinElementParameterCount ++ ];
		pSkinElement->Type   = CVertexDeclaration::FLOAT4;
		pSkinElement->Usage  = CVertexDeclaration::POSITION;
		pSkinElement->uIndex = 0;
	}

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::NORMAL, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pSkinElement         = &SkinElementParameters[uSkinElementParameterCount ++ ];
		pSkinElement->Type   = CVertexDeclaration::FLOAT3;
		pSkinElement->Usage  = CVertexDeclaration::NORMAL;
		pSkinElement->uIndex = 0;
	}

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::TANGENT, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pSkinElement         = &SkinElementParameters[uSkinElementParameterCount ++ ];
		pSkinElement->Type   = CVertexDeclaration::FLOAT3;
		pSkinElement->Usage  = CVertexDeclaration::TANGENT;
		pSkinElement->uIndex = 0;
	}

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::BINORMAL, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pSkinElement         = &SkinElementParameters[uSkinElementParameterCount ++ ];
		pSkinElement->Type   = CVertexDeclaration::FLOAT3;
		pSkinElement->Usage  = CVertexDeclaration::BINORMAL;
		pSkinElement->uIndex = 0;
	}

	return Create(Mesh,SkinType, SkinInfo, Root, SkinElementParameters, uSkinElementParameterCount, uMaxBoneInfluence);
}

void CSkinMeshController::Update()
{
	if(m_pDestinationMesh == ALCHEMY_NULL)
		return;

	UINT uBoneCount = m_pSkinInfo->GetBoneCount();
	CBone* pBone;
	//m_pRoot->Update();
	//m_pRoot->UpdateChildren();

	CMatrix4x4* pBoneMatrix;
	const MATRIX4X4* pOffsetMatrix, * pTemp;
	for(UINT i = 0; i < uBoneCount; ++ i)
	{
		pBoneMatrix = &m_pBoneMatrices[i];

		pBone = m_ppBones[i];
		if(pBone)
			pTemp = &pBone->GetWorldMatrix();
		else
		{
			(*pBoneMatrix).Identity();

			pTemp = pBoneMatrix;
		}

		pOffsetMatrix = m_pSkinInfo->GetBoneOffsetMatrix(i);

		if(pOffsetMatrix)
			Multiply4x3(*pBoneMatrix, *pTemp, *pOffsetMatrix);
	}

	switch (m_SkinType)
	{
	case SKIN_HARDWARE:
		//..
		break;
	case SKIN_SOFTWARE:
		LPVERTEXBUFFER pSourceVertexBuffer, pDestinationVertexBuffer;
		void* pSourceVertices, * pDestinationVertices;
		UINT uStream;
		for(UINT i = 0; i < m_uSkinningStreamCount; ++ i)
		{
			uStream = m_puSkinningStream[i];
			pSourceVertexBuffer      = m_pSourceMesh->GetVertex().GetVertexBuffer(uStream);
			pDestinationVertexBuffer = m_pDestinationMesh->/*pMesh->*/GetVertex().GetVertexBuffer(uStream);
			if(pSourceVertexBuffer && pDestinationVertexBuffer)
			{
				pSourceVertices      = pSourceVertexBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) );
				pDestinationVertices = pDestinationVertexBuffer->Lock(0);

				if(pSourceVertices && pDestinationVertices)
				{
					m_pSkinInfo->Skinning(
						pSourceVertexBuffer->GetMemberCount(), 
						pSourceVertices, 
						pSourceVertexBuffer->GetStride(), 
						pDestinationVertices, 
						pDestinationVertexBuffer->GetStride(), 
						m_pBoneMatrices, 
						m_pBoneMatrices, 
						m_ppChannels[i],
						m_puChannelsCount[i]);

					pSourceVertexBuffer     ->Unlock();
					pDestinationVertexBuffer->Unlock();
				}
			}
		}

		break;
            
    default:
        break;
	}
}

void CSkinMeshController::Destroy()
{
	m_pRoot = ALCHEMY_NULL;

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppBones);

	if(m_pSourceMesh)
	{
		m_pSourceMesh->Release();
		m_pSourceMesh = ALCHEMY_NULL;
	}

	if(m_pDestinationMesh)
	{
		m_pDestinationMesh->Release();
		m_pDestinationMesh = ALCHEMY_NULL;
	}

	for(UINT i = 0; i < m_uSkinningStreamCount; ++ i)
		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppChannels[i]);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppChannels);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puChannelsCount);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puSkinningStream);

	m_uSkinningStreamCount = 0;

	if(m_pBoneCombination)
	{
		m_pBoneCombination->Release();
		m_pBoneCombination = ALCHEMY_NULL;
	}

	m_uBoneCombinationCount = 0;

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pBoneMatrices);

	ALCHEMY_DEBUG_DELETE(m_pHardwareSkinEffect);

	m_SkinType = SKIN_NONE;
}

void CSkinMeshController::GetBoneCombinationRange(PUINT puOutputStart, PUINT puOutputEnd, const CMesh::ATTRIBUTERANGE& AttributeRange)
{
	if(puOutputStart || puOutputEnd)
	{
		if(m_pBoneCombination)
		{
			CMesh::LPBONECOMBINATION pBoneCombination = reinterpret_cast<CMesh::LPBONECOMBINATION>( m_pBoneCombination->GetBufferPointer() );

			UINT i, uBoneCombinationEndIndex, uAttributeEndIndex = AttributeRange.uStartIndex + AttributeRange.uIndexCount;

			bool bIsBegin = true;
			for(i = 0; i < m_uBoneCombinationCount; ++ i)
			{
				uBoneCombinationEndIndex = pBoneCombination->uStartIndex + pBoneCombination->uIndexCount;

				if(bIsBegin)
				{
					if(uBoneCombinationEndIndex > AttributeRange.uStartIndex && pBoneCombination->uStartIndex < uAttributeEndIndex)
					{
						bIsBegin = false;

						if(puOutputStart)
							*puOutputStart = i;

						if(puOutputEnd)
						{
							if(uBoneCombinationEndIndex >= uAttributeEndIndex)
							{
								*puOutputEnd = i;

								break;
							}
						}
						else
							break;
					}
				}
				else if(uBoneCombinationEndIndex >= uAttributeEndIndex)
				{
					*puOutputEnd = i;

					break;
				}

				++ pBoneCombination;
			}

			if(bIsBegin)
			{
				if(puOutputStart)
					*puOutputStart = 1;

				if(puOutputEnd)
					*puOutputEnd = 0;
			}
			else if(puOutputEnd)
				*puOutputEnd = i;
		}
		else
		{
			if(puOutputStart)
				*puOutputStart = 1;

			if(puOutputEnd)
				*puOutputEnd = 0;
		}
	}
}