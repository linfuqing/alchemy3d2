#include "AlchemySilhouetteProgram.h"
#include "AlchemySceneObject.h"
#include "AlchemyAttributeViewSpaceEffect.h"
#include "AlchemySilhouetteEffect.h"

using namespace alchemy;

CSilhouetteProgram::CSilhouetteProgram()
:CProgramInstance( CSceneManager::EFFECT_NORMAL ),
m_bIsUpdate(false),
m_pSourceMesh(ALCHEMY_NULL),
m_fWidthModulus(0.0f),
m_SilhouetteColor(0.0f, 0.0f, 0.0f, 1.0f),
m_pSilhoutteMesh(ALCHEMY_NULL),
m_pRenderMethod(ALCHEMY_NULL),
m_pSihoutetteEffect(ALCHEMY_NULL),
m_pAdjBuffer(ALCHEMY_NULL),
m_pSourcePosBuffer(ALCHEMY_NULL),
m_pSourceNormalBuffer(ALCHEMY_NULL),
m_pDestPosBuffer(ALCHEMY_NULL),
m_pSourceIndexBuffer(ALCHEMY_NULL),
m_pDestIndexBuffer(ALCHEMY_NULL),
m_uSourceNumFaces(0)
{
	for (UINT i=0; i<4; ++i)
		m_pViewEffect[i] = ALCHEMY_NULL;

	for (UINT i=0; i<3; ++i)
		m_pDestNormalBuffer[i] = ALCHEMY_NULL;

	for (UINT i=0; i<2; ++i)
		m_uOffsets[i] = 0;
}

CSilhouetteProgram::~CSilhouetteProgram()
{
	Destroy();
}

bool CSilhouetteProgram::Create(CGraphicsResource::HANDLE Mesh, FLOAT WidthModulus, CARGBColor& SilhouetteColor, bool isUpdate)
{
	Destroy();

	m_bIsUpdate = isUpdate;
	m_pSceneManager = CSceneManager::GetInstance();

	CMesh* pMesh = static_cast<CMesh*>( m_pSceneManager->GetResource(Mesh, CGraphicsResource::MESH) );

	if(!pMesh)
		return false;

	m_pSourceMesh = pMesh;
	m_pSourceMesh->AddRef();

	CVertex& Vertex = m_pSourceMesh->GetVertex();
	CVertexDeclaration& VertexDeclaration = Vertex.GetVertexDeclaration();
	const CVertexDeclaration::VERTEXELEMENT* pSourceVertexElement = VertexDeclaration.GetVertexElements();
	UINT uSourceVertexElementCount = VertexDeclaration.GetVertexElementCount(), uCurrentElement;

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
		m_pSourcePosBuffer = Vertex.GetVertexBuffer( pSourceVertexElement[ uCurrentElement ].uStream );
	else
		return false;

	m_uOffsets[0] = pSourceVertexElement[ uCurrentElement ].uOffset;

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::NORMAL, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
		m_pSourceNormalBuffer = Vertex.GetVertexBuffer( pSourceVertexElement[ uCurrentElement ].uStream );
	else
		return false;

	m_uOffsets[1] = pSourceVertexElement[ uCurrentElement ].uOffset;

	m_pSourceIndexBuffer = &m_pSourceMesh->GetIndexBuffer();

	m_uSourceNumFaces = m_pSourceIndexBuffer->GetMemberCount() / 3;
	UINT uNumIndices = m_pSourceIndexBuffer->GetMemberCount() * 6, uNumVertices =  m_uSourceNumFaces * 3 * 4 ;

	UINT uIndexFormat = CIndexBuffer::BIT16;

	if(uNumIndices >= 0xffff)
		uIndexFormat = CIndexBuffer::BIT32;

	CVertexDeclaration::VERTEXELEMENT Elements[4] = { 
		{0, 0, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION}, 
		{1, 0, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL}, 
		{2, 0, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL}, 
		{3, 0, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL}, 
	};

	UINT uBufferFlag = ( isUpdate == true ? ALCHEMY_FLAG( VERTEXBUFFER::WRITE_ONLY ) | ALCHEMY_FLAG( VERTEXBUFFER::DYNAMIC ) : ALCHEMY_FLAG( VERTEXBUFFER::WRITE_ONLY ) );

	UINT uUsage[4] = { uBufferFlag , uBufferFlag, uBufferFlag, uBufferFlag  };

	m_pSilhoutteMesh = m_pSceneManager->CreateMesh( uNumVertices, uNumIndices, uBufferFlag, (INDEXBUFFER::FORMAT)uIndexFormat, Elements, 4, uUsage);

	if(!m_pSilhoutteMesh)
		return false;

	CVertex& SilhouetteVertex = m_pSilhoutteMesh->GetVertex();
	CVertexDeclaration& SihouetteVertexDeclartion = SilhouetteVertex.GetVertexDeclaration();
	pSourceVertexElement = SihouetteVertexDeclartion.GetVertexElements();

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pSourceVertexElement, 4);
	if(uCurrentElement < 4)
		m_pDestPosBuffer = SilhouetteVertex.GetVertexBuffer( pSourceVertexElement[ uCurrentElement ].uStream );
	else
		return false;

	for (UINT i=0; i<3; ++i)
	{
		uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::NORMAL, CVertexDeclaration::FLOAT3, i, pSourceVertexElement, 4);
		if(uCurrentElement < 4)
			m_pDestNormalBuffer[i] = SilhouetteVertex.GetVertexBuffer( pSourceVertexElement[ uCurrentElement ].uStream );
		else
			return false;
	}

	m_pDestIndexBuffer = &m_pSilhoutteMesh->GetIndexBuffer();

	ALCHEMY_DEBUG_NEW(m_pAdjBuffer, UINT[ m_pSourceIndexBuffer->GetMemberCount() ]);
	if(false == pMesh->GenerateAdjacency( 0.0001f, m_pAdjBuffer ) )
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pAdjBuffer);
		return false;
	}

	__Update();


	PUINT8 pIndexData = (PUINT8)m_pDestIndexBuffer->Lock(0);

	PUINT16 p16IndexData = (PUINT16)pIndexData;
	PUINT32 p32IndexData = (PUINT32)pIndexData;

	UINT uNumEdges = m_uSourceNumFaces * 3;
	for (UINT i=0; i<uNumEdges; ++i)
	{
		if(uIndexFormat == CIndexBuffer::BIT16)
		{
			p16IndexData[ i*6 ] = i*4;
			p16IndexData[ i*6+1 ] = i*4 + 2;
			p16IndexData[ i*6+2 ] = i*4 + 1;

			p16IndexData[ i*6+3 ] = i*4 + 1;
			p16IndexData[ i*6+4 ] = i*4 + 2;
			p16IndexData[ i*6+5 ] = i*4 + 3;
		}
		else
		{
			p32IndexData[ i*6 ] = i*4;
			p32IndexData[ i*6+1 ] = i*4 + 2;
			p32IndexData[ i*6+2 ] = i*4 + 1;

			p32IndexData[ i*6+3 ] = i*4 + 1;
			p32IndexData[ i*6+4 ] = i*4 + 2;
			p32IndexData[ i*6+5 ] = i*4 + 3;
		}
	}

	m_pDestIndexBuffer->Unlock();

	CPool< CMesh::ATTRIBUTERANGE>& SourceAttribute = m_pSourceMesh->GetAttributeTable();
	UINT uLen = SourceAttribute.GetLength();
	CMesh::ATTRIBUTERANGE Attribute;
	for (UINT i=0; i<uLen; ++i)
	{
		Attribute.uStartIndex = SourceAttribute[i].uStartIndex * 6;
		Attribute.uIndexCount = SourceAttribute[i].uIndexCount * 6;

		m_pSilhoutteMesh->GetAttributeTable().Add(Attribute);
	}

	m_fWidthModulus = WidthModulus;
	m_SilhouetteColor = SilhouetteColor;

	CEffectCode EffectCode( m_pSceneManager->GetRenderSystem().GetComplier() );
	CEffectCode::CPassDefiner pPassDefiner(EffectCode);

	if(m_pSihoutetteEffect == ALCHEMY_NULL)
	{
		ALCHEMY_DEBUG_NEW( m_pSihoutetteEffect, CSilhouetteEffect(&m_SilhouetteColor, &m_fWidthModulus) );

		ALCHEMY_DEBUG_NEW( m_pViewEffect[0], CAttributeViewSpaceEffect(CVertexDeclaration::POSITION, false) );
		ALCHEMY_DEBUG_NEW( m_pViewEffect[1], CAttributeViewSpaceEffect(CVertexDeclaration::NORMAL, true) );
		ALCHEMY_DEBUG_NEW( m_pViewEffect[2], CAttributeViewSpaceEffect(CVertexDeclaration::NORMAL, true, 1) );
		ALCHEMY_DEBUG_NEW( m_pViewEffect[3], CAttributeViewSpaceEffect(CVertexDeclaration::NORMAL, true, 2) );
	}

	for (UINT i=0; i<4; ++i)
		pPassDefiner.AddInfo(*m_pViewEffect[i],ALCHEMY_NULL);

	pPassDefiner.AddInfo(*m_pSihoutetteEffect,ALCHEMY_NULL);

	UINT uPass = pPassDefiner.EndDefine();
	if(uPass == 0)
		return false;

	m_pRenderMethod = m_pSceneManager->CreateRenderMethod( EffectCode );

	return m_pRenderMethod == ALCHEMY_NULL ? false : true;
}

bool CSilhouetteProgram::OnEndRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag)
{
	if(m_pSilhoutteMesh && pSceneObject)
	{
		if(m_bIsUpdate)
			__Update();

		CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
		RenderSystem.RegisterRenderState();
		RenderSystem.SetRenderState(CRenderSystem::CULL_MODE, CRenderSystem::NONE);

		CEffect& Effect = m_pRenderMethod->GetEffect();
		Effect.Begin();
		Effect.BeginPass(0);
		m_pRenderMethod->Activate(0);
		//m_pRenderMethod->SetWorldMatrix( pSceneObject->GetWorldMatrix() );
		//m_pRenderMethod->SetCamera( m_pSceneManager->GetCamera() );
		//m_pRenderMethod->SetMaterial( *pSceneObject->GetMaterial() );
		//m_pRenderMethod->CommitTransform();
		m_pRenderMethod->SetParameters( pSceneObject->GetWorldMatrix(), m_pSceneManager->GetCamera(), *pSceneObject->GetMaterial() );
		Effect.CommitChange();
		m_pSilhoutteMesh->DrawSubset( pSceneObject->GetSubset() , true, true);

		RenderSystem.RestoreRenderState();
	}

	return true;
}

void CSilhouetteProgram::OnAddedToSceneObject()
{
	ALCHEMY_DEBUG_DELETE(m_pSihoutetteEffect);

	for(UINT i=0; i<4; ++i)
		ALCHEMY_DEBUG_DELETE(m_pViewEffect[i]);
}

void CSilhouetteProgram::Destroy()
{
	OnAddedToSceneObject();

	m_uSourceNumFaces = 0;
	m_bIsUpdate = false;

	if(m_pSourceMesh != ALCHEMY_NULL)
		m_pSourceMesh->Release();

	if(m_pSilhoutteMesh != ALCHEMY_NULL)
		m_pSilhoutteMesh->Release();

	if(m_pRenderMethod != ALCHEMY_NULL)
		m_pRenderMethod->Release();

	for (UINT i=0; i<3; ++i)
		m_pDestNormalBuffer[i] = ALCHEMY_NULL;

	m_pSourceIndexBuffer = m_pDestIndexBuffer = ALCHEMY_NULL;
	m_pSourceNormalBuffer = m_pSourcePosBuffer = m_pDestPosBuffer = ALCHEMY_NULL;

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAdjBuffer);

	m_pRenderMethod = ALCHEMY_NULL;
	m_pSilhoutteMesh = ALCHEMY_NULL;
	m_pSceneManager = ALCHEMY_NULL;
	m_pSourceMesh = ALCHEMY_NULL;
}

void CSilhouetteProgram::__GetFaceNormal(const CVector4& v0, const CVector4& v1, const CVector4& v2, CVector3& Normal)
{
	static CVector3 TempEdge1, TempEdge2;
	TempEdge1.x = v1.x - v0.x;	TempEdge1.y = v1.y - v0.y;	TempEdge1.z = v1.z - v0.z;
	TempEdge2.x = v2.x - v0.x;	TempEdge2.y = v2.y - v0.y;	TempEdge2.z = v2.z - v0.z;

	TempEdge1.Normalize();
	TempEdge2.Normalize();

	Normal.CrossProduct(TempEdge2,TempEdge1);
}

void CSilhouetteProgram::__Update()
{
	if( !__IsReady() )
		return;

	UINT uPosStride = m_pSourcePosBuffer->GetStride() , uNormalStride = m_pSourceNormalBuffer->GetStride();

	PUINT8 pSourcePosData = m_pSourcePosBuffer->Lock( ALCHEMY_FLAG( VERTEXBUFFER::READ_ONLY ) );
	PUINT8 pSourceNormalData = m_pSourceNormalBuffer->Lock( ALCHEMY_FLAG( VERTEXBUFFER::READ_ONLY ) );
	PUINT8 pSourceIndexData = m_pSourceIndexBuffer->Lock( ALCHEMY_FLAG( INDEXBUFFER::READ_ONLY ) );
	UINT uSourceIndexStride = m_pSourceIndexBuffer->GetFormat() == INDEXBUFFER::BIT16 ? 2 : 4;

	if(!pSourceNormalData || !pSourcePosData || !pSourceIndexData)
		return;

	CVector4* pPosData = (CVector4*)m_pDestPosBuffer->Lock(0);
	CVector3* pNormalData[3] = {0};
	for (UINT i=0; i<3; ++i)
		pNormalData[i] = (CVector3*)m_pDestNormalBuffer[i]->Lock(0);

	static CVector3 CurrentFaceNormal;
	static CVector3 AdjFaceNormal[3];
	static FLOAT3	ZeroVec = {0.0f, 0.0f, 0.0f };

	CVector4* pDstPos[3];
	CVector4* pSourcePos[3];
	CVector3* pSourceNormal[3];
	UINT uCurrentIndex;
	UINT uFaces[3];

	for (UINT i=0; i<m_uSourceNumFaces; ++i)
	{
		for (UINT j=0; j<3; ++j)
		{
			uCurrentIndex = uSourceIndexStride == 2 ? static_cast<UINT>(* reinterpret_cast<PUINT16>( pSourceIndexData + (i*3 + j)*uSourceIndexStride ) ) : static_cast<UINT>(* reinterpret_cast<PUINT>( pSourceIndexData + (i*3 + j)*uSourceIndexStride ) );
			pSourcePos[j] = reinterpret_cast<CVector4*>( pSourcePosData + uCurrentIndex*uPosStride + m_uOffsets[0] );
			pSourceNormal[j] = reinterpret_cast<CVector3*>( pSourceNormalData + uCurrentIndex*uNormalStride + m_uOffsets[1] );
		}

		__GetFaceNormal(*pSourcePos[0], *pSourcePos[1], *pSourcePos[2], CurrentFaceNormal);

		uFaces[0] = m_pAdjBuffer[i*3];
		uFaces[1] = m_pAdjBuffer[i*3+1];
		uFaces[2] = m_pAdjBuffer[i*3+2];

		for (UINT j=0; j<3; ++j)
		{
			if(uFaces[j] != -1)
			{
				for (UINT c=0; c<3; ++c)
				{
					uCurrentIndex = uSourceIndexStride == 2 ? static_cast<UINT>(* reinterpret_cast<PUINT16>( pSourceIndexData + (uFaces[j]*3 + c)*uSourceIndexStride ) ) : static_cast<UINT>(* reinterpret_cast<PUINT>( pSourceIndexData + (uFaces[j]*3 + c)*uSourceIndexStride ) );
					pDstPos[c] = reinterpret_cast<CVector4*>( pSourcePosData + uCurrentIndex*uPosStride + m_uOffsets[0] );
				}

				__GetFaceNormal(*pDstPos[0], *pDstPos[1], *pDstPos[2], AdjFaceNormal[j] );
			}
			else
			{
				AdjFaceNormal[j].x = -CurrentFaceNormal.x;
				AdjFaceNormal[j].y = -CurrentFaceNormal.y;
				AdjFaceNormal[j].z = -CurrentFaceNormal.z;
			}
		}

		//v0 -> v1
		*pPosData = *pSourcePos[0];					++pPosData;
		*pNormalData[0] = ZeroVec;					++pNormalData[0];
		*pNormalData[1] = CurrentFaceNormal;		++pNormalData[1];
		*pNormalData[2] = AdjFaceNormal[0];			++pNormalData[2];

		*pPosData = *pSourcePos[1];					++pPosData;
		*pNormalData[0] = ZeroVec;					++pNormalData[0];
		*pNormalData[1] = CurrentFaceNormal;		++pNormalData[1];
		*pNormalData[2] = AdjFaceNormal[0];			++pNormalData[2];

		*pPosData = *pSourcePos[0];					++pPosData;
		*pNormalData[0] = *pSourceNormal[0];		++pNormalData[0];
		*pNormalData[1] = CurrentFaceNormal;		++pNormalData[1];
		*pNormalData[2] = AdjFaceNormal[0];			++pNormalData[2];

		*pPosData = *pSourcePos[1];					++pPosData;
		*pNormalData[0] = *pSourceNormal[1];		++pNormalData[0];
		*pNormalData[1] = CurrentFaceNormal;		++pNormalData[1];
		*pNormalData[2] = AdjFaceNormal[0];			++pNormalData[2];


		//v1 -> v2
		*pPosData = *pSourcePos[1];					++pPosData;
		*pNormalData[0] = ZeroVec;					++pNormalData[0];
		*pNormalData[1] = CurrentFaceNormal;		++pNormalData[1];
		*pNormalData[2] = AdjFaceNormal[1];			++pNormalData[2];

		*pPosData = *pSourcePos[2];					++pPosData;
		*pNormalData[0] = ZeroVec;					++pNormalData[0];
		*pNormalData[1] = CurrentFaceNormal;		++pNormalData[1];
		*pNormalData[2] = AdjFaceNormal[1];			++pNormalData[2];

		*pPosData = *pSourcePos[1];					++pPosData;
		*pNormalData[0] = *pSourceNormal[1];		++pNormalData[0];
		*pNormalData[1] = CurrentFaceNormal;		++pNormalData[1];
		*pNormalData[2] = AdjFaceNormal[1];			++pNormalData[2];

		*pPosData = *pSourcePos[2];					++pPosData;
		*pNormalData[0] = *pSourceNormal[2];		++pNormalData[0];
		*pNormalData[1] = CurrentFaceNormal;		++pNormalData[1];
		*pNormalData[2] = AdjFaceNormal[1];			++pNormalData[2];


		//v2 -> v0
		*pPosData = *pSourcePos[2];					++pPosData;
		*pNormalData[0] = ZeroVec;					++pNormalData[0];
		*pNormalData[1] = CurrentFaceNormal;		++pNormalData[1];
		*pNormalData[2] = AdjFaceNormal[2];			++pNormalData[2];

		*pPosData = *pSourcePos[0];					++pPosData;
		*pNormalData[0] = ZeroVec;					++pNormalData[0];
		*pNormalData[1] = CurrentFaceNormal;		++pNormalData[1];
		*pNormalData[2] = AdjFaceNormal[2];			++pNormalData[2];

		*pPosData = *pSourcePos[2];					++pPosData;
		*pNormalData[0] = *pSourceNormal[2];		++pNormalData[0];
		*pNormalData[1] = CurrentFaceNormal;		++pNormalData[1];
		*pNormalData[2] = AdjFaceNormal[2];			++pNormalData[2];

		*pPosData = *pSourcePos[0];
		*pNormalData[0] = *pSourceNormal[0];
		*pNormalData[1] = CurrentFaceNormal;
		*pNormalData[2] = AdjFaceNormal[2];

		if(i < m_uSourceNumFaces-1)
		{
			++pPosData;
			++pNormalData[0];
			++pNormalData[1];
			++pNormalData[2];
		}
	}

	m_pSourcePosBuffer->Unlock();
	m_pSourceNormalBuffer->Unlock();
	m_pSourceIndexBuffer->Unlock();

	m_pDestPosBuffer->Unlock();

	for (UINT i=0; i<3; ++i)
		m_pDestNormalBuffer[i]->Unlock();
}