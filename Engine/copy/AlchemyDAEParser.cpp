#include "AlchemyDAEParser.h"

#include "../Core/AlchemyOverBuffer.h"

#include "tinyxml.h"

#include "../Core/AlchemyDebug.h"
#include "../Core/AlchemyUtils.h"

#include "../Math/AlchemyVector3.h"

#include "../Math/AlchemyFloat2.h"
#include "../Math/AlchemyFloat4.h"

#include "../Graphics/AlchemySkinInfo.h"
#include "../Graphics/AlchemyAnimationController.h"

#include "AlchemyLoaderUtils.h"


using namespace alchemy;

const alchemy::UINT			alchemy::CDAEParser::sm_ENCODE_FLAG[TOTAL_ENCODE]	= { ENCODE_UTF8 };
const BASICSTRING			alchemy::CDAEParser::sm_ENCODINGTYPE[TOTAL_ENCODE]	= { "utf-8" };

typedef struct  
{
	CVertexDeclaration::VERTEXELEMENT		Element;
	CVector<alchemy::FLOAT>					Data;
	CVector<alchemy::UINT>					IndicesData;
}STREAMDATA, *LPSTREAMDATA;

namespace alchemy
{
	class CZeorVector3 :
		public CVector3
	{
	public:
		CZeorVector3() {x = 0.0f; y = 0.0f; z = 0.0f;}
	};

	class CNullMap : 
		public CMap<UINT, UINT>
	{
	public:
		CNullMap()
			:isNULL(true)
		{

		}

		bool isNULL;
	};
}


alchemy::CDAEParser::CDAEParser(CSceneParser::TEXTUREDATARES* pTextureDataRes, UINT uTextureDataNum, const STREAMUSAGE* pStreamUsage, UINT uStreamUsageNum,UINT uIndexUsage)
:m_pCollada(ALCHEMY_NULL),
m_bIsSkinMesh(false),
m_pTextureDataRes(ALCHEMY_NULL),
m_uTextureDataNum(uTextureDataNum),
m_uCurrentEncode(ENCODE_UTF8),
m_bIsYUp(false),
m_pStreamUsage(ALCHEMY_NULL),
m_uStreamUsageNum(0),
m_uIndexUsage(uIndexUsage),
m_pAnimationController(ALCHEMY_NULL)
{
	m_SceneData.uSceneDataCount = 0;
	m_SceneData.pSceneNodeData = ALCHEMY_NULL;

	if(pTextureDataRes && uTextureDataNum)
	{
		UINT uLen;
		ALCHEMY_DEBUG_NEW(m_pTextureDataRes, CSceneParser::TEXTUREDATARES[uTextureDataNum]);
		for (UINT i=0; i<uTextureDataNum; ++i)
		{
			m_pTextureDataRes[i].hTextureDataHandle = pTextureDataRes[i].hTextureDataHandle;
			if( pTextureDataRes[i].pcTextureName )
			{
				uLen = ALCHEMY_STRING_LENGTH(pTextureDataRes[i].pcTextureName);
				ALCHEMY_DEBUG_NEW(m_pTextureDataRes[i].pcTextureName, CHAR[uLen+1]);
				for (UINT j=0; j<uLen; ++j)
					m_pTextureDataRes[i].pcTextureName[j] = pTextureDataRes[i].pcTextureName[j];
				m_pTextureDataRes[i].pcTextureName[uLen] = 0;
			}
		}
	}

	if(pStreamUsage && uStreamUsageNum)
	{
		ALCHEMY_DEBUG_NEW(m_pStreamUsage, STREAMUSAGE[uStreamUsageNum]);
		for (UINT i=0; i<uStreamUsageNum; ++i)
			m_pStreamUsage[i] = pStreamUsage[i];

		m_uStreamUsageNum = uStreamUsageNum;
	}
}

alchemy::CDAEParser::~CDAEParser()
{
	UINT i,uLen;

	uLen = m_SceneData.uSceneDataCount;
	for (i=0; i<uLen; ++i)
	{
		if( m_SceneData.pSceneNodeData[i].uNameLen )
		{
			m_SceneData.pSceneNodeData[i].uNameLen = 0;
			ALCHEMY_DEBUG_DELETE_ARRAY( m_SceneData.pSceneNodeData[i].pcName );
			/*if(m_SceneData.pSceneNodeData[i].pAnimationController)
			{
				m_SceneData.pSceneNodeData[i].pAnimationController->Release();
				m_SceneData.pSceneNodeData[i].pAnimationController = ALCHEMY_NULL;
			}

			if(m_SceneData.pSceneNodeData[i].pSkinInfo)
			{
				m_SceneData.pSceneNodeData[i].pSkinInfo->Release();
				m_SceneData.pSceneNodeData[i].pSkinInfo = ALCHEMY_NULL;
			}*/
		}
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_SceneData.pSceneNodeData);
	m_SceneData.uSceneDataCount = 0;

	uLen = m_uTextureDataNum;
	for (i=0; i<uLen; ++i)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pTextureDataRes[i].pcTextureName);
	}
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pTextureDataRes);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pStreamUsage);

	uLen = m_GemotryList.GetLength();
	for (i=0; i<uLen; ++i)
	{
		ALCHEMY_DEBUG_DELETE(m_GemotryList[i]);
	}
}

bool alchemy::CDAEParser::Parse( const void* pData, const char* pFileName )
{
	TiXmlDocument* pDocument;
	ALCHEMY_DEBUG_NEW(pDocument,TiXmlDocument() );

	if(pFileName)
		pDocument->LoadFile(pFileName);
	else if( ALCHEMY_NULL == pDocument->Parse( reinterpret_cast<const char*>(pData) ) )
		return false;

	if(pData)
	{
		const char* pTempData = reinterpret_cast<const char*>(pData);
		UINT uEqual = 0;
		while(pTempData)
		{
			if( *pTempData == '=')
				++uEqual;

			if(uEqual == 2)
				break;

			++pTempData;
		}

		pTempData += 2;

		const char* pStart = pTempData;
		uEqual = 0;

		while (pTempData)
		{
			if( *pTempData == '?')
				break;

			++uEqual;
			++pTempData;
		}

		char* pEncode;
		ALCHEMY_DEBUG_NEW(pEncode, char[uEqual]);
		ALCHEMY_MEMORY_COPY(pEncode, pStart, uEqual-1);
		pEncode[uEqual-1] = 0;


		for (UINT i=0; i<TOTAL_ENCODE; ++i)
		{
			if( sm_ENCODINGTYPE[i] == pEncode )
			{
				m_uCurrentEncode = i;
				break;
			}
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(pEncode);
	}

	STRING strTemp;
	BASICSTRING strEncode;
	for (UINT j=0; j<m_uTextureDataNum; ++j)
	{
		strTemp = m_pTextureDataRes[j].pcTextureName;
		ALCHEMY_STRING_TO_BASICSTRING(strEncode, m_uCurrentEncode, strTemp);
		m_TextureMap.Insert(strEncode, m_pTextureDataRes[j].hTextureDataHandle);
		
		//printf("%s", strTemp);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_pTextureDataRes[j].pcTextureName);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pTextureDataRes);
	m_uTextureDataNum = 0;
	
	m_pCollada = pDocument->RootElement();

	TiXmlElement* pXML = m_pCollada->FirstChildElement("asset")->FirstChildElement("up_axis");
	strEncode = pXML->GetText();
	if(strEncode == "Y_UP")
		m_bIsYUp = true;

	pXML = m_pCollada->FirstChildElement("library_visual_scenes");
	if(!pXML)
		return false;

	pXML = pXML->FirstChildElement("visual_scene");
	if(!pXML)
		return false;

	pXML = pXML->FirstChildElement("node");

	while (pXML)
	{
		__parseNode(*pXML, ALCHEMY_NULL);
		pXML = pXML->NextSiblingElement("node");
	}

	UINT uLen = m_GemotryList.GetLength();
	for (UINT i=0; i<uLen; ++i)
		__parseGemotry( *m_GemotryList[i]->pGemotryXML, *m_GemotryList[i]->pHostNode, m_GemotryList[i]->pControllerXML );

	__parseAnimationSet();
	
	ALCHEMY_DEBUG_DELETE(pDocument);

	uLen = m_SceneNodes.GetLength();
	m_SceneData.uSceneDataCount = uLen;
	if(uLen)
	{
		ALCHEMY_DEBUG_NEW(m_SceneData.pSceneNodeData, CMeshManager::SCENENODEDATA[uLen]);
		for (UINT i=0; i<uLen; ++i)
			m_SceneData.pSceneNodeData[i] = m_SceneNodes[i];
	}

	return true;
}

const CMeshManager::SCENEDATA& alchemy::CDAEParser::GetSceneData() const
{
	return m_SceneData;
}

void alchemy::CDAEParser::__parseNode( const TiXmlElement& Node, CMeshManager::SCENENODEDATA* pParent )
{
	CMeshManager::SCENENODEDATA* pNode = ALCHEMY_NULL;
	const TiXmlElement* pElement = Node.ToElement();
	CMeshManager::SCENETYPE eType;

	if( pElement->FirstChild("instance_controller") || pElement->FirstChild("instance_geometry") )
		eType = CMeshManager::OBJECT;
	else
		eType = m_bIsSkinMesh ? CMeshManager::BONE : CMeshManager::NODE;

	pNode = __createSceneNodeData( eType, pParent );

	if(pParent == ALCHEMY_NULL)
	{
		if(pNode->Type == CMeshManager::BONE)
			m_RootBones.Add(pNode);
	}

	BASICSTRING strName = pElement->Attribute("name");
	STRING strTemp;
	ALCHEMY_BASICSTRING_TO_STRING(strTemp, m_uCurrentEncode, strName);
	pNode->uNameLen = strTemp.GetLength();

	ALCHEMY_DEBUG_NEW(pNode->pcName, CHAR[pNode->uNameLen+1]);

	for (UINT i=0; i<pNode->uNameLen; ++i)
		pNode->pcName[i] = strTemp[i];

	pNode->pcName[pNode->uNameLen] = 0;

	CMatrix4x4 matTransform;
	GEMOTRYDATA* pGemotryData;
	TiXmlElement* pGemotryElement, *pControllerElement;

	matTransform.Identity();

	pElement = pElement->FirstChildElement();
	while (pElement)
	{
		strName = pElement->Value();
		if(strName == "matrix")
			__getNodeMatrix(*pElement, *pNode, pParent, &matTransform);
		else if(strName == "node")
			__parseNode(*pElement, pNode);
		else if(strName == "instance_geometry")
		{
			ALCHEMY_DEBUG_NEW(pGemotryData, GEMOTRYDATA);
			pGemotryData->pHostNode = pNode;
			pGemotryElement = m_pCollada->FirstChildElement("library_geometries");
			pGemotryElement = __findElementByID( *pGemotryElement->FirstChildElement("geometry"), &pElement->Attribute("url")[1] );
			pGemotryData->pGemotryXML = pGemotryElement;
			pGemotryData->pControllerXML = ALCHEMY_NULL;
			m_GemotryList.Add(pGemotryData);
		}
		else if(strName == "instance_controller")
		{
			ALCHEMY_DEBUG_NEW(pGemotryData, GEMOTRYDATA);
			pGemotryData->pHostNode = pNode;
			pControllerElement = m_pCollada->FirstChildElement("library_controllers");
			pControllerElement = __findElementByID( *pControllerElement->FirstChildElement("controller"), &pElement->Attribute("url")[1] );
			pGemotryData->pControllerXML = pControllerElement;
			pGemotryElement = m_pCollada->FirstChildElement("library_geometries");
			pGemotryElement = __findElementByID( *pGemotryElement->FirstChildElement("geometry"), &pControllerElement->FirstChildElement("skin")->Attribute("source")[1] );
			pGemotryData->pGemotryXML = pGemotryElement;
			m_GemotryList.Add(pGemotryData);
			m_bIsSkinMesh = true;
		}

		pElement = pElement->NextSiblingElement();
	}

	pNode->Transform = matTransform;
}

void alchemy::CDAEParser::__parseGemotry( const TiXmlElement& Gemotry, CMeshManager::SCENENODEDATA& NodeData, const TiXmlElement* pController )
{
	const TiXmlElement* pPolygons = Gemotry.FirstChildElement("mesh")->FirstChildElement("polygons");
	CString<char>   strSemantic;
	UINT uUVNum = 0;

	if(pPolygons)
	{
		const TiXmlElement* pInputList = pPolygons->FirstChildElement("input");

		while (pInputList)
		{
			strSemantic = pInputList->Attribute("semantic");
			if( strSemantic == "TEXCOORD" )
				++uUVNum;

			pInputList = pInputList->NextSiblingElement("input");
		}
	}

	if(uUVNum > 1)
		__parseGemotrySingleStream(Gemotry, NodeData, pController);
	else
		__parseGemotryOptimize(Gemotry, NodeData, pController);
}

void alchemy::CDAEParser::__parseGemotrySingleStream( const TiXmlElement& Gemotry, CMeshManager::SCENENODEDATA& NodeData, const TiXmlElement* pController )
{
	CMeshManager::MESHDATA	MeshData = {0};
	CVector<UINT>			subsetMaterial;
	CVector<CMesh::ATTRIBUTERANGE>	meshSegmentDatas;
	CString<char>   strSemantic;

	CVertexDeclaration::VERTEXELEMENT Element;
	CVector<CVertexDeclaration::VERTEXELEMENT>	VertexElements;

	CVector<FLOAT>				PositionData;
	CVector<FLOAT>				NormalData;
	CVector< CVector<FLOAT>	>	UVData;
	CVector<FLOAT>				ColorData;

	CVector<FLOAT>				EmptyVertexData;
	CVector<FLOAT>*				pUVData;

	const TiXmlElement* pPolygons = Gemotry.FirstChildElement("mesh")->FirstChildElement("polygons");
	const TiXmlElement* pInputList = ALCHEMY_NULL, *pPList = ALCHEMY_NULL;
	TiXmlElement* pVerticesXML = ALCHEMY_NULL;

	CMesh::ATTRIBUTERANGE MeshAttribute = {0};
	INT nTriangleNum = 0;
	UINT uStream = 0;
	UINT i,j, uLen;

	CVector<UINT> IndicesData;

	while (pPolygons)
	{
		pPolygons->Attribute("count", &nTriangleNum);
		if(nTriangleNum > 0)
		{
			MeshAttribute.uStartIndex = MeshData.uNumIndices;
			MeshAttribute.uIndexCount = nTriangleNum*3;
			subsetMaterial.Add( __parseMaterial( pPolygons->Attribute("material") ) );
			meshSegmentDatas.Add(MeshAttribute);
		}

		MeshData.uNumIndices += MeshAttribute.uIndexCount;

		if(VertexElements.GetLength() == 0)
		{
			pInputList = pPolygons->FirstChildElement("input");
			while (pInputList)
			{
				strSemantic = pInputList->Attribute("semantic");
				if( strSemantic == "VERTEX" )
				{
					Element.uStream = uStream++;
					Element.uIndex = 0;
					Element.uOffset = 0;
					Element.Type = CVertexDeclaration::FLOAT4;
					Element.Usage = CVertexDeclaration::POSITION;

					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("vertices"), &pInputList->Attribute("source")[1]);
					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("source"), &pVerticesXML->FirstChildElement("input")->Attribute("source")[1] );

					__getFloatArray( PositionData, *pVerticesXML->FirstChildElement("float_array") );

					if(!pController)
						__transformData(NodeData.Transform, PositionData);

					VertexElements.Add(Element);
				}
				else if( strSemantic == "NORMAL" )
				{
					Element.uStream = uStream++;
					Element.uIndex = 0;
					Element.uOffset = 0;
					Element.Type = CVertexDeclaration::FLOAT3;
					Element.Usage = CVertexDeclaration::NORMAL;

					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
					__getFloatArray( NormalData, *pVerticesXML->FirstChildElement("float_array") );

					if(!pController)
						__transformData(NodeData.Transform, NormalData, true);

					VertexElements.Add(Element);
				}
				else if( strSemantic == "TEXCOORD" )
				{
					Element.uStream = uStream++;
					Element.uIndex = 0;
					Element.uOffset = 0;
					Element.Type = CVertexDeclaration::FLOAT2;
					Element.Usage = CVertexDeclaration::TEXCOORD;

					pUVData = UVData.Get( UVData.Add( EmptyVertexData ) );

					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
					__getFloatArray( *pUVData, *pVerticesXML->FirstChildElement("float_array") );

					VertexElements.Add(Element);
				}
				else if( strSemantic == "COLOR" )
				{
					Element.uStream = uStream++;
					Element.uIndex = 0;
					Element.uOffset = 0;
					Element.Type = CVertexDeclaration::FLOAT4;
					Element.Usage = CVertexDeclaration::COLOR;

					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
					__getFloatArray( ColorData, *pVerticesXML->FirstChildElement("float_array") );

					VertexElements.Add(Element);
				}

				pInputList = pInputList->NextSiblingElement("input");
			}
		}

		pPolygons = pPolygons->NextSiblingElement("polygons");
	}

	MeshData.uNumVertices = MeshData.uNumIndices;
	MeshData.uIndexUsage = m_uIndexUsage;
	uLen = VertexElements.GetLength();

	UINT uUVIndex;
	PUINT8* ppVertexData = ALCHEMY_NULL;
	CVertexDeclaration::VERTEXELEMENT* pElement = ALCHEMY_NULL;

	if(uLen)
	{
		ALCHEMY_DEBUG_NEW( MeshData.VertexElementData.puUsage, UINT[uLen] );
		ALCHEMY_DEBUG_NEW( MeshData.VertexElementData.pVertexElement, CVertexDeclaration::VERTEXELEMENT[uLen] );
		MeshData.VertexElementData.uElementCount = uLen;

		ALCHEMY_DEBUG_NEW( ppVertexData, PUINT8[uLen] );
		for (i=0; i<uLen; ++i)
		{
			pElement = VertexElements.Get(i);
			ALCHEMY_DEBUG_NEW( ppVertexData[i], UINT8[pElement->Type*MeshData.uNumVertices]);

			MeshData.VertexElementData.pVertexElement[i] = *pElement;
			MeshData.VertexElementData.puUsage[i] = ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY);

			for (j=0; j<m_uStreamUsageNum; ++j)
			{
				if(MeshData.VertexElementData.pVertexElement[i].Type == m_pStreamUsage[j].Type && MeshData.VertexElementData.pVertexElement[i].Usage == m_pStreamUsage[j].ElementUsage
					&& MeshData.VertexElementData.pVertexElement[i].uIndex == m_pStreamUsage[j].Index)
				{
					MeshData.VertexElementData.puUsage[i] = m_pStreamUsage[j].Usage;
					break;
				}
			}
		}
	}

	FLOAT2* pUVValue;
	FLOAT3* pNormalValue;
	FLOAT4* pColorValue;
	FLOAT4* pPositionValue;
	UINT uTmpIndex, uVertexIndex = 0;

	CMap<UINT, CVector<UINT> > OtherIndicesMap;
	CVector<UINT>* pSourceIndex;
	CVector<UINT>  EmptyIndex;

	pPolygons = Gemotry.FirstChildElement("mesh")->FirstChildElement("polygons");
	while(pPolygons)
	{
		pPList = pPolygons->FirstChildElement("p");
		while(pPList)
		{
			IndicesData.Reset();
			__getUintArray(IndicesData, *pPList);

			for (j=0; j<3; ++j)
			{
				uUVIndex = 0;
				for (i=0; i<uLen; ++i)
				{
					pElement = VertexElements.Get(i);
					switch(pElement->Usage)
					{
					case CVertexDeclaration::POSITION:
						pPositionValue = reinterpret_cast<FLOAT4*>(ppVertexData[i] + uVertexIndex*pElement->Type);
						uTmpIndex = IndicesData[j*uLen + i];

						pSourceIndex = OtherIndicesMap.Get(uTmpIndex);
						if(!pSourceIndex)
						{
							EmptyIndex.Reset();
							EmptyIndex.Add(uVertexIndex);
							OtherIndicesMap.Insert(uTmpIndex, EmptyIndex);
						}
						else
							pSourceIndex->Add(uVertexIndex);

						uTmpIndex *= 3;
						pPositionValue->x = PositionData[uTmpIndex];
						pPositionValue->y = PositionData[uTmpIndex+1];
						pPositionValue->z = PositionData[uTmpIndex+2];
						pPositionValue->w = 1.0f;

						break;
					case CVertexDeclaration::NORMAL:
						pNormalValue = reinterpret_cast<FLOAT3*>(ppVertexData[i] + uVertexIndex*pElement->Type);
						uTmpIndex = IndicesData[j*uLen + i];
						uTmpIndex *= 3;
						pNormalValue->x = NormalData[uTmpIndex];
						pNormalValue->y = NormalData[uTmpIndex+1];
						pNormalValue->z = NormalData[uTmpIndex+2];
						break;
					case CVertexDeclaration::TEXCOORD:
						pUVValue = reinterpret_cast<FLOAT2*>(ppVertexData[i] + uVertexIndex*pElement->Type);
						uTmpIndex = IndicesData[j*uLen + i];
						uTmpIndex *= 2;
						pUVData = &UVData[uUVIndex];
						pUVValue->x = (*pUVData->Get(uTmpIndex));
						pUVValue->y = 1.0f - (*pUVData->Get(uTmpIndex+1));
						++uUVIndex;
						break;
					case CVertexDeclaration::COLOR:
						pColorValue = reinterpret_cast<FLOAT4*>(ppVertexData[i] + uVertexIndex*pElement->Type);
						uTmpIndex = IndicesData[j*uLen + i];
						uTmpIndex *= 4;
						pColorValue->x = ColorData[uTmpIndex];
						pColorValue->y = ColorData[uTmpIndex+1];
						pColorValue->z = ColorData[uTmpIndex+2];
						pColorValue->w = ColorData[uTmpIndex+3];
						break;
					}
				}

				++uVertexIndex;
			}

			pPList = pPList->NextSiblingElement("p");
		}

		pPolygons = pPolygons->NextSiblingElement("polygons");
	}

	MeshData.ppVertexData = reinterpret_cast<PVOID*>(ppVertexData);

	uLen = meshSegmentDatas.GetLength();
	if(uLen > 0)
	{
		ALCHEMY_DEBUG_NEW(MeshData.pAttributeRange, CMesh::ATTRIBUTERANGE[uLen]);
		for(i=0; i<uLen; ++i)
			MeshData.pAttributeRange[i] = meshSegmentDatas[i];

		MeshData.uAttributeRangeCount = uLen;
	}

	if(MeshData.uNumVertices > 0xffff)
	{
		MeshData.IndexFormat = INDEXBUFFER::BIT32;
		PUINT32 pIndexData;
		ALCHEMY_DEBUG_NEW(pIndexData, UINT32[MeshData.uNumIndices]);

		for (i=0; i<MeshData.uNumIndices; i+=3)
		{
			pIndexData[i] = i;
			pIndexData[i+1] = i+2;
			pIndexData[i+2] = i+1;
		}

		MeshData.pIndexData = reinterpret_cast<PVOID>(pIndexData);

		NodeData.uMeshData = ALCHEMY_MESH_MANAGER.AddMeshData(MeshData);

		ALCHEMY_DEBUG_DELETE_ARRAY(pIndexData);
	}
	else
	{
		MeshData.IndexFormat = INDEXBUFFER::BIT16;
		PUINT16 pIndexData;
		ALCHEMY_DEBUG_NEW(pIndexData, UINT16[MeshData.uNumIndices]);

		for (i=0; i<MeshData.uNumIndices; i+=3)
		{
			pIndexData[i] = i;
			pIndexData[i+1] = i+2;
			pIndexData[i+2] = i+1;
		}

		MeshData.pIndexData = reinterpret_cast<PVOID>(pIndexData);

		NodeData.uMeshData = ALCHEMY_MESH_MANAGER.AddMeshData(MeshData);

		ALCHEMY_DEBUG_DELETE_ARRAY(pIndexData);
	}

	if(!pController)
	{
		CMatrix4x4 IdentityMat;
		IdentityMat.Identity();
		Copy(NodeData.Transform, IdentityMat);
	}
	NodeData.uSubset = 0;
	NodeData.MaterialDataHandle = subsetMaterial[0];
	NodeData.isUpdate = true;

	CMeshManager::SCENENODEDATA* pSceneNodeData;
	CVector<CMeshManager::SCENENODEDATA*> SceneObjects;
	STRING strName;
	for (i=1; i<MeshData.uAttributeRangeCount; ++i)
	{
		pSceneNodeData = __createSceneNodeData(CMeshManager::OBJECT, ALCHEMY_NULL);
		pSceneNodeData->MaterialDataHandle = subsetMaterial[i];
		pSceneNodeData->uMeshData = NodeData.uMeshData;
		pSceneNodeData->uSubset = i;
		pSceneNodeData->Transform = NodeData.Transform;
		strName = NodeData.pcName;
		strName += i;
		ALCHEMY_DEBUG_NEW(pSceneNodeData->pcName, CHAR[strName.GetLength()+1]);
		for(j=0; j<strName.GetLength(); ++j)
			pSceneNodeData->pcName[j] = strName[j];
		pSceneNodeData->pcName[strName.GetLength()] = 0;
		pSceneNodeData->uNameLen = strName.GetLength();
		pSceneNodeData->uParent = NodeData.uParent;
		pSceneNodeData->isUpdate = false;
		SceneObjects.Add(pSceneNodeData);
	}

	if(pController)
		__parseSkinGemotry(*pController, OtherIndicesMap, NodeData, SceneObjects);

	uLen = VertexElements.GetLength();
	for(i=0; i<uLen; ++i)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY( ppVertexData[i] );
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.pAttributeRange);
	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.VertexElementData.puUsage);
	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.VertexElementData.pVertexElement);
	ALCHEMY_DEBUG_DELETE_ARRAY(ppVertexData);
}

void alchemy::CDAEParser::__parseGemotryOptimize( const TiXmlElement& Gemotry, CMeshManager::SCENENODEDATA& NodeData, const TiXmlElement* pController )
{
	CMeshManager::MESHDATA	MeshData = {0};
	CVector<UINT>			subsetMaterial;
	CVector<CMesh::ATTRIBUTERANGE>	meshSegmentDatas;
	CString<char>   strSemantic;
	CPool< STREAMDATA >	StreamDataPool;

	STREAMDATA EmptyStreamData;
	STREAMDATA*	pPositionStreamData = ALCHEMY_NULL;
	STREAMDATA* pNormalStreamData = ALCHEMY_NULL;
	STREAMDATA* pColorStreamData = ALCHEMY_NULL;
	STREAMDATA* pUVStreamData = ALCHEMY_NULL;
	STREAMDATA* pStreamData = ALCHEMY_NULL;

	const TiXmlElement* pPolygons = Gemotry.FirstChildElement("mesh")->FirstChildElement("polygons");
	const TiXmlElement* pInputList = ALCHEMY_NULL, *pPList = ALCHEMY_NULL;

	TiXmlElement* pVerticesXML = ALCHEMY_NULL;

	CMesh::ATTRIBUTERANGE MeshAttribute = {0};
	INT nTriangleNum = 0;
	UINT uStream = 0;
	UINT i,j, uLen;

	CVector<UINT> IndicesData;

	while (pPolygons)
	{
		pPolygons->Attribute("count", &nTriangleNum);
		if(nTriangleNum > 0)
		{
			MeshAttribute.uStartIndex = MeshAttribute.uStartIndex;
			MeshAttribute.uIndexCount = nTriangleNum*3;
			subsetMaterial.Add( __parseMaterial( pPolygons->Attribute("material") ) );
			meshSegmentDatas.Add(MeshAttribute);
			MeshAttribute.uStartIndex = MeshAttribute.uIndexCount;
		}

		if(StreamDataPool.GetLength() == 0)
		{
			pInputList = pPolygons->FirstChildElement("input");
			while (pInputList)
			{
				strSemantic = pInputList->Attribute("semantic");
				if( strSemantic == "VERTEX" )
				{
					pStreamData = StreamDataPool.Get( StreamDataPool.Add( EmptyStreamData ) );

					pStreamData->Element.uStream = uStream++;
					pStreamData->Element.uIndex = 0;
					pStreamData->Element.uOffset = 0;
					pStreamData->Element.Type = CVertexDeclaration::FLOAT4;
					pStreamData->Element.Usage = CVertexDeclaration::POSITION;

					pPositionStreamData = pStreamData;

					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("vertices"), &pInputList->Attribute("source")[1]);
					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("source"), &pVerticesXML->FirstChildElement("input")->Attribute("source")[1] );

					__getFloatArray( pStreamData->Data, *pVerticesXML->FirstChildElement("float_array") );

					if(!pController)
						__transformData(NodeData.Transform, pStreamData->Data);
				}
				else if( strSemantic == "NORMAL" )
				{
					pStreamData = StreamDataPool.Get( StreamDataPool.Add( EmptyStreamData ) );

					pStreamData->Element.uStream = uStream++;
					pStreamData->Element.uIndex = 0;
					pStreamData->Element.uOffset = 0;
					pStreamData->Element.Type = CVertexDeclaration::FLOAT3;
					pStreamData->Element.Usage = CVertexDeclaration::NORMAL;

					pNormalStreamData = pStreamData;

					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
					__getFloatArray( pStreamData->Data, *pVerticesXML->FirstChildElement("float_array") );

					if(!pController)
						__transformData(NodeData.Transform, pStreamData->Data, true);
				}
				else if( strSemantic == "TEXCOORD" )
				{
					pStreamData = StreamDataPool.Get( StreamDataPool.Add( EmptyStreamData ) );

					pStreamData->Element.uStream = uStream++;
					pStreamData->Element.uIndex = 0;
					pStreamData->Element.uOffset = 0;
					pStreamData->Element.Type = CVertexDeclaration::FLOAT2;
					pStreamData->Element.Usage = CVertexDeclaration::TEXCOORD;

					pUVStreamData = pStreamData;

					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
					__getFloatArray( pStreamData->Data, *pVerticesXML->FirstChildElement("float_array") );
				}
				else if( strSemantic == "COLOR" )
				{
					pStreamData = StreamDataPool.Get( StreamDataPool.Add( EmptyStreamData ) );

					pStreamData->Element.uStream = uStream++;
					pStreamData->Element.uIndex = 0;
					pStreamData->Element.uOffset = 0;
					pStreamData->Element.Type = CVertexDeclaration::FLOAT4;
					pStreamData->Element.Usage = CVertexDeclaration::COLOR;

					pColorStreamData = pStreamData;

					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
					__getFloatArray( pStreamData->Data, *pVerticesXML->FirstChildElement("float_array") );
				}

				pInputList = pInputList->NextSiblingElement("input");
			}
		}

		uLen = StreamDataPool.GetLength();
		pPList = pPolygons->FirstChildElement("p");

		while(pPList)
		{
			IndicesData.Reset();
			__getUintArray(IndicesData, *pPList);

			for (j=0; j<3; ++j)
				for (i=0; i<uLen; ++i)
					StreamDataPool[i].IndicesData.Add( IndicesData[j*uLen+i] );

			pPList = pPList->NextSiblingElement("p");
		}

		pPolygons = pPolygons->NextSiblingElement("polygons");
	}

	UINT uAddLength = pPositionStreamData->Data.GetLength()/( ALCHEMY_TOTAL_BITS(UINT) )+1;
	UINT uCurrentUVLength, uPositionLength;

	//compute normal
	CPool<CZeorVector3> NormalVectors;
	NormalVectors.AddArray(uAddLength, true);
	UINT uVertexIndex, uNormalIndex;
	if(pNormalStreamData)
	{
		uLen = pPositionStreamData->IndicesData.GetLength();
		for (i=0; i<uLen; ++i)
		{
			uVertexIndex = pPositionStreamData->IndicesData[i];
			uNormalIndex = pNormalStreamData->IndicesData[i];

			NormalVectors[uVertexIndex].x += pNormalStreamData->Data[uNormalIndex*3];
			NormalVectors[uVertexIndex].y += pNormalStreamData->Data[uNormalIndex*3+1];
			NormalVectors[uVertexIndex].z += pNormalStreamData->Data[uNormalIndex*3+2];
		}

		uLen = pPositionStreamData->Data.GetLength()/3;
		for (i=0; i<uLen; ++i)
			NormalVectors[i].Normalize();
	}

	//color
	CPool<FLOAT4>	ColorVectors;
	if(pColorStreamData)
	{
		ColorVectors.AddArray(uAddLength, true);
		uLen = pPositionStreamData->IndicesData.GetLength();
		for (i=0; i<uLen; ++i)
		{
			uVertexIndex = pPositionStreamData->IndicesData[i];
			uNormalIndex = pColorStreamData->IndicesData[i];

			ColorVectors[uVertexIndex].x = pColorStreamData->Data[uNormalIndex*4];
			ColorVectors[uVertexIndex].y = pColorStreamData->Data[uNormalIndex*4+1];
			ColorVectors[uVertexIndex].z = pColorStreamData->Data[uNormalIndex*4+2];
			ColorVectors[uVertexIndex].w = pColorStreamData->Data[uNormalIndex*4+3];
		}

		ColorVectors.Finish();
	}

	//compute uv
	CMap<UINT, CVector<UINT> >	OtherIndicesMap;
	CVector<UINT>				EmptyIndicesData;
	CVector<UINT>* pOtherIndices;
	CVector<FLOAT> UVData;
	UINT uUVIndex;
	IndicesData.Clear();
	if( pUVStreamData )
	{
		CPool< CNullMap >			UVIndexMap;
		UVIndexMap.AddArray(uAddLength, true);
		CNullMap					EmptyIndices;
		CNullMap*					pIndices;
		UINT* pIndex;

		uCurrentUVLength = UVData.GetLength()/2;
		uPositionLength = pPositionStreamData->Data.GetLength()/3-1;

		UVData.AddArray( ( uPositionLength - uCurrentUVLength )*2/( ALCHEMY_TOTAL_BITS(UINT) )+1, true );

		uLen = pPositionStreamData->IndicesData.GetLength();
		for (i=0; i<uLen; ++i)
		{
			uVertexIndex = pPositionStreamData->IndicesData[i];
			uUVIndex = pUVStreamData->IndicesData[i];

			pIndices = UVIndexMap.Get( uVertexIndex );
			if(!pIndices->isNULL)
			{
				pIndex = pIndices->Get( uUVIndex );
				if(pIndex)
					IndicesData.Add( *pIndex );
				else
				{
					//add Position
					pPositionStreamData->Data.Add( pPositionStreamData->Data[3*uVertexIndex] );
					pPositionStreamData->Data.Add( pPositionStreamData->Data[3*uVertexIndex+1] );
					pPositionStreamData->Data.Add( pPositionStreamData->Data[3*uVertexIndex+2] );

					if(pController)
					{
						pOtherIndices = OtherIndicesMap.Get(uVertexIndex);
						if(pOtherIndices)
							pOtherIndices->Add(  pPositionStreamData->Data.GetLength()/3 - 1 );
						else
						{
							EmptyIndicesData.Reset();
							EmptyIndicesData.Add( pPositionStreamData->Data.GetLength()/3 - 1 );
							OtherIndicesMap.Insert( uVertexIndex, EmptyIndicesData );
						}
					}

					uPositionLength = pPositionStreamData->Data.GetLength()/3-1;


					//add Normal
					if(pNormalStreamData)
					{
						uCurrentUVLength = NormalVectors.GetLength()/3;
						if( uCurrentUVLength <= uPositionLength )
							NormalVectors.AddArray( ( uPositionLength - uCurrentUVLength )*3/( ALCHEMY_TOTAL_BITS(UINT) )+1, true );

						NormalVectors[uPositionLength] = NormalVectors[uVertexIndex];
					}

					//add Color
					if(pColorStreamData)
					{
						uCurrentUVLength = ColorVectors.GetLength()/3;
						if( uCurrentUVLength <= uPositionLength )
							ColorVectors.AddArray( ( uPositionLength - uCurrentUVLength )*4/( ALCHEMY_TOTAL_BITS(UINT) )+1, true );

						ColorVectors[uPositionLength] = ColorVectors[uVertexIndex];
					}

					uCurrentUVLength = UVData.GetLength()/2;

					IndicesData.Add( uPositionLength );

					if( uCurrentUVLength <= uPositionLength )
						UVData.AddArray( ( uPositionLength - uCurrentUVLength )*2/( ALCHEMY_TOTAL_BITS(UINT) )+1, true );

					UVData[ uPositionLength*2   ]	= pUVStreamData->Data[ uUVIndex*2 ];
					UVData[ uPositionLength*2+1 ]	= 1.0f - pUVStreamData->Data[ uUVIndex*2+1 ];

					pIndices->Insert(uUVIndex, uPositionLength);
				}
			}
			else
			{
				pIndices->isNULL = false;
				pIndices->Insert(uUVIndex,uVertexIndex);

				IndicesData.Add( uVertexIndex );

				UVData[uVertexIndex*2] = pUVStreamData->Data[ uUVIndex*2 ];
				UVData[uVertexIndex*2+1] = 1.0f - pUVStreamData->Data[ uUVIndex*2+1 ];
			}
		}
	}
	else
		IndicesData = pPositionStreamData->IndicesData;

	MeshData.uIndexUsage = m_uIndexUsage;
	MeshData.uAttributeRangeCount = meshSegmentDatas.GetLength();
	MeshData.uNumIndices = meshSegmentDatas[ meshSegmentDatas.GetLength()-1 ].uIndexCount;
	ALCHEMY_DEBUG_NEW(MeshData.pAttributeRange, CMesh::ATTRIBUTERANGE[MeshData.uAttributeRangeCount]);
	for (i=0; i<MeshData.uAttributeRangeCount; ++i)
		MeshData.pAttributeRange[i] = meshSegmentDatas[i];

	UINT uTriangleNum = MeshData.uNumIndices/3;

	MeshData.uNumVertices = pPositionStreamData->Data.GetLength()/3;

	if(MeshData.uNumVertices >= 0xffff)
	{
		MeshData.IndexFormat = INDEXBUFFER::BIT32;
		PUINT32 pIndexData;
		ALCHEMY_DEBUG_NEW(pIndexData, UINT32[MeshData.uNumIndices]);
		MeshData.pIndexData = reinterpret_cast<PVOID>(pIndexData);

		for (i=0; i<uTriangleNum; ++i)
		{
			pIndexData[i*3] = static_cast<UINT32>( IndicesData[i*3] );
			pIndexData[i*3+1] = static_cast<UINT32>( IndicesData[i*3+2] );
			pIndexData[i*3+2] = static_cast<UINT32>( IndicesData[i*3+1] );
		}
	}
	else
	{
		MeshData.IndexFormat = INDEXBUFFER::BIT16;
		PUINT16 pIndexData;
		ALCHEMY_DEBUG_NEW(pIndexData, UINT16[MeshData.uNumIndices]);
		MeshData.pIndexData = reinterpret_cast<PVOID>(pIndexData);

		for (i=0; i<uTriangleNum; ++i)
		{
			pIndexData[i*3] = static_cast<UINT16>( IndicesData[i*3] );
			pIndexData[i*3+1] = static_cast<UINT16>( IndicesData[i*3+2] );
			pIndexData[i*3+2] = static_cast<UINT16>( IndicesData[i*3+1] );
		}
	}

	PUINT8 pVertexData;
	FLOAT4* pPosData, *pColorData;
	FLOAT2* pUVData;
	FLOAT3* pNormalData;
	ALCHEMY_DEBUG_NEW(MeshData.ppVertexData, PUINT8[uStream]);

	MeshData.VertexElementData.uElementCount = uStream;
	ALCHEMY_DEBUG_NEW(MeshData.VertexElementData.puUsage,		 UINT[uStream]);
	ALCHEMY_DEBUG_NEW(MeshData.VertexElementData.pVertexElement, CVertexDeclaration::VERTEXELEMENT[uStream]);
	for (i=0; i<uStream; ++i)
	{
		MeshData.VertexElementData.puUsage[i] = ALCHEMY_FLAG( VERTEXBUFFER::WRITE_ONLY );
		MeshData.VertexElementData.pVertexElement[i] = StreamDataPool[i].Element;

		for (j=0; j<m_uStreamUsageNum; ++j)
		{
			if(MeshData.VertexElementData.pVertexElement[i].Type == m_pStreamUsage[j].Type && MeshData.VertexElementData.pVertexElement[i].Usage == m_pStreamUsage[j].ElementUsage
				&& MeshData.VertexElementData.pVertexElement[i].uIndex == m_pStreamUsage[j].Index)
			{
				MeshData.VertexElementData.puUsage[i] = m_pStreamUsage[j].Usage;
				break;
			}
		}

		switch(MeshData.VertexElementData.pVertexElement[i].Usage)
		{
		case CVertexDeclaration::POSITION:
			{
				ALCHEMY_DEBUG_NEW(pVertexData, UINT8[MeshData.uNumVertices*sizeof(FLOAT)*4]);
				MeshData.ppVertexData[i] = reinterpret_cast<PVOID>(pVertexData);

				pPosData = reinterpret_cast<FLOAT4*>(pVertexData);

				for (j=0; j<MeshData.uNumVertices; ++j)
				{
					pPosData->x = pPositionStreamData->Data[j*3]; pPosData->y = pPositionStreamData->Data[j*3+1]; pPosData->z = pPositionStreamData->Data[j*3+2]; pPosData->w = 1.0f;
					++pPosData;
				}
			}

			break;
		case CVertexDeclaration::NORMAL:
			{
				ALCHEMY_DEBUG_NEW(pVertexData, UINT8[MeshData.uNumVertices*sizeof(FLOAT)*3]);
				MeshData.ppVertexData[i] = reinterpret_cast<PVOID>(pVertexData);

				pNormalData = reinterpret_cast<FLOAT3*>(pVertexData);

				for (j=0; j<MeshData.uNumVertices; ++j)
				{
					pNormalData->x = NormalVectors[j].x;	pNormalData->y = NormalVectors[j].y;	pNormalData->z = NormalVectors[j].z;
					++pNormalData;
				}
			}
			break;
		case CVertexDeclaration::TEXCOORD:
			{
				ALCHEMY_DEBUG_NEW(pVertexData, UINT8[MeshData.uNumVertices*sizeof(FLOAT)*2]);
				MeshData.ppVertexData[i] = reinterpret_cast<PVOID>(pVertexData);

				pUVData = reinterpret_cast<FLOAT2*>(pVertexData);

				for (j=0; j<MeshData.uNumVertices; ++j)
				{
					pUVData->x = UVData[j*2];	pUVData->y = UVData[j*2+1];
					++pUVData;
				}
			}

			break;
		case CVertexDeclaration::COLOR:
			{
				ALCHEMY_DEBUG_NEW(pVertexData, UINT8[MeshData.uNumVertices*sizeof(FLOAT)*4]);
				MeshData.ppVertexData[i] = reinterpret_cast<PVOID>(pVertexData);

				pColorData = reinterpret_cast<FLOAT4*>(pVertexData);

				for (j=0; j<MeshData.uNumVertices; ++j)
				{
					pColorData->x = ColorVectors[j].x;	pColorData->y = ColorVectors[j].y;	pColorData->z = ColorVectors[j].z;
					pColorData->w = ColorVectors[j].w;
					++pColorData;
				}
			}
			break;
		}
	}

	if(!pController)
	{
		CMatrix4x4 IdentityMat;
		IdentityMat.Identity();
		Copy(NodeData.Transform, IdentityMat);
	}

	NodeData.uMeshData = ALCHEMY_MESH_MANAGER.AddMeshData(MeshData);
	NodeData.uSubset = 0;
	NodeData.MaterialDataHandle = subsetMaterial[0];
	NodeData.isUpdate = true;

	CMeshManager::SCENENODEDATA* pSceneNodeData;
	CVector<CMeshManager::SCENENODEDATA*> SceneObjects;
	STRING strName;
	for (i=1; i<MeshData.uAttributeRangeCount; ++i)
	{
		pSceneNodeData = __createSceneNodeData(CMeshManager::OBJECT, ALCHEMY_NULL);
		pSceneNodeData->MaterialDataHandle = subsetMaterial[i];
		pSceneNodeData->uMeshData = NodeData.uMeshData;
		pSceneNodeData->uSubset = i;
		pSceneNodeData->Transform = NodeData.Transform;
		strName = NodeData.pcName;
		strName += i;
		ALCHEMY_DEBUG_NEW(pSceneNodeData->pcName, CHAR[strName.GetLength()+1]);
		for(j=0; j<strName.GetLength(); ++j)
			pSceneNodeData->pcName[j] = strName[j];
		pSceneNodeData->pcName[strName.GetLength()] = 0;
		pSceneNodeData->uNameLen = strName.GetLength();
		pSceneNodeData->uParent = NodeData.uParent;
		pSceneNodeData->isUpdate = false;
		SceneObjects.Add(pSceneNodeData);
	}

	if(pController)
		__parseSkinGemotry(*pController, OtherIndicesMap, NodeData, SceneObjects);

	uLen = StreamDataPool.GetLength();
	for(i=0; i<uLen; ++i)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY( MeshData.ppVertexData[i] );
	}

	ALCHEMY_DEBUG_DELETE_ARRAY( MeshData.pIndexData );
	ALCHEMY_DEBUG_DELETE_ARRAY( MeshData.ppVertexData );
	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.pAttributeRange);
	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.VertexElementData.puUsage);
	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.VertexElementData.pVertexElement);
}

void alchemy::CDAEParser::__parseSkinGemotry(const TiXmlElement& Controller, const CMap<UINT, CVector<UINT> >& ExternalIndices, CMeshManager::SCENENODEDATA& NodeData, CVector<CMeshManager::SCENENODEDATA*>& OtherNodeData )
{
	const TiXmlElement* pSkinXML = Controller.FirstChildElement("skin");
	CString<char> strSemantic;
	const TiXmlElement* pInputList = pSkinXML->FirstChildElement("joints")->FirstChildElement("input");
	UINT i,j;

	while (pInputList)
	{
		strSemantic = pInputList->Attribute("semantic");
		if(strSemantic == "JOINT")
			break;

		pInputList = pInputList->NextSiblingElement("input");
	}

	pInputList = __findElementByID(*pSkinXML->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
	CString<char> strJoints = pInputList->FirstChildElement("Name_array")->GetText();
	strJoints.Replace("\n"," ");
	CVector<CString<char> > nameArray;
	strJoints.Split(" ", nameArray);
	CMatrix4x4 bindShapeMatrix;
	CVector<FLOAT> ShapeData;
	__getFloatArray(ShapeData, *pSkinXML->FirstChildElement("bind_shape_matrix") );
	for (i=0; i<16; ++i)
		bindShapeMatrix.fValue[i] = ShapeData[i];

	__getAxisMatrix(bindShapeMatrix, &bindShapeMatrix);
	pInputList = pSkinXML->FirstChildElement("joints")->FirstChildElement("input");
	while (pInputList)
	{
		strSemantic = pInputList->Attribute("semantic");
		if(strSemantic == "INV_BIND_MATRIX")
			break;

		pInputList = pInputList->NextSiblingElement("input");
	}

	pInputList = __findElementByID(*pSkinXML->FirstChildElement("source"), &pInputList->Attribute("source")[1]);

	CVector<FLOAT> FloatArrayData;
	__getFloatArray(FloatArrayData, *pInputList->FirstChildElement("float_array"));
	CMeshManager::SCENENODEDATA* pBone;

	STRING strCompare;
	STRING strError;
	CVector<CSkinInfo::BONE>	BoneDataes;
	CSkinInfo::BONE* pBoneData;
	CSkinInfo::BONE EmptyBoneData;
	CMatrix4x4 matInvalid, matTemp;
	bool isSetRootBone = false;
	UINT uRootBone = 0;

	for (i=0; i<FloatArrayData.GetLength(); i+=16)
	{
		if(!uRootBone)
		{
			pBone = __findNodeByName(nameArray[i/16], &uRootBone);
			uRootBone = uRootBone+1;
		}
		else
			pBone = __findNodeByName(nameArray[i/16], ALCHEMY_NULL);

		if(pBone)
		{
			if(!isSetRootBone)
			{
				CMeshManager::SCENENODEDATA* pRootBone = pBone;
				while(pRootBone->uParent && pRootBone->Type == CMeshManager::BONE)
				{
					uRootBone = pRootBone->uParent;
					pRootBone = m_SceneNodes.Get(pRootBone->uParent-1);
				}

				isSetRootBone = true;
			}
			pBoneData = BoneDataes.Get( BoneDataes.Add(EmptyBoneData) );
			pBoneData->pcName = ALCHEMY_NULL;
			pBoneData->pfWeights = ALCHEMY_NULL;
			pBoneData->puIndices = ALCHEMY_NULL;
			__copyChar(&pBoneData->pcName, nameArray[i/16]);
			matTemp = bindShapeMatrix;
			for (j=0; j<16; ++j)
				matInvalid.fValue[j] = FloatArrayData[i+j];
			__getBoneInvalidMatrix(matInvalid, &matInvalid);
			matTemp *= matInvalid;
			for (j=0; j<16; ++j)
				pBoneData->OffsetMatrix.fValue[j] = matTemp.fValue[j];
		}
		else
		{
			strError += ALCHEMY_TEXT("not found");
			ALCHEMY_DEBUG_WARNING(strError);
		}
	}

	pInputList = pSkinXML->FirstChildElement("vertex_weights")->FirstChildElement("input");
	while (pInputList)
	{
		strSemantic = pInputList->Attribute("semantic");
		if(strSemantic == "WEIGHT")
			break;

		pInputList = pInputList->NextSiblingElement("input");
	}

	pInputList = __findElementByID(*pSkinXML->FirstChildElement("source"), &pInputList->Attribute("source")[1]);

	FloatArrayData.Clear();
	__getFloatArray(FloatArrayData, *pInputList->FirstChildElement("float_array"));
	CVector<UINT> vCount, v;
	__getUintArray(vCount, *pSkinXML->FirstChildElement("vertex_weights")->FirstChildElement("vcount") );
	__getUintArray(v, *pSkinXML->FirstChildElement("vertex_weights")->FirstChildElement("v") );

	UINT uLen = vCount.GetLength();
	UINT c;
	UINT uCount, uIndex = 0;
	FLOAT fWeight;
	CVector<UINT> EmptyIndices;
	CVector<FLOAT>	EmptyWeightes;
	CVector<UINT>* puIndices;
	CVector<FLOAT>* pfWeights;
	const CVector<UINT>* pExternIndices;
	CMap<CSkinInfo::BONE*, CVector<UINT> > uIndicesMap;
	CMap<CSkinInfo::BONE*, CVector<FLOAT> > fWeightsMap;
	for (i=0; i<uLen; ++i)
	{
		uCount = vCount[i];
		pExternIndices = ExternalIndices.Get(i);
		for (j=0; j<uCount; ++j)
		{
			pBoneData = &BoneDataes[ v[uIndex] ];
			puIndices = uIndicesMap.Get(pBoneData);
			if(!puIndices)
			{
				uIndicesMap.Insert(pBoneData, EmptyIndices);
				puIndices = uIndicesMap.Get(pBoneData);
			}

			++uIndex;
			fWeight = FloatArrayData[ v[uIndex] ];
			pfWeights = fWeightsMap.Get(pBoneData);
			if(!pfWeights)
			{
				fWeightsMap.Insert(pBoneData, EmptyWeightes);
				pfWeights = fWeightsMap.Get(pBoneData);
			}

			++uIndex;

			if(fWeight > 0.0f)
			{
				puIndices->Add( i );
				pfWeights->Add( fWeight );
				if(pExternIndices)
				{
					for (c=0; c<pExternIndices->GetLength(); ++c)
					{
						puIndices->Add( *pExternIndices->Get(c) );
						pfWeights->Add( fWeight );
					}
				}
			}
		}
	}

	uLen = BoneDataes.GetLength();
	for (i=0; i<uLen; ++i)
	{
		pBoneData = &BoneDataes[i];
		puIndices = uIndicesMap.Get(pBoneData);
		pfWeights = fWeightsMap.Get(pBoneData);

		if(puIndices && pfWeights)
		{
			pBoneData->uInfluenceCount = puIndices->GetLength();
			ALCHEMY_DEBUG_NEW(pBoneData->pfWeights, FLOAT[pBoneData->uInfluenceCount]);
			ALCHEMY_DEBUG_NEW(pBoneData->puIndices, UINT[pBoneData->uInfluenceCount]);

			for (c=0; c<pBoneData->uInfluenceCount; ++c)
			{
				pBoneData->pfWeights[c] = *pfWeights->Get(c);
				pBoneData->puIndices[c] = *puIndices->Get(c);
			}
		}
		else
		{
			pBoneData->uInfluenceCount = 0;
			pBoneData->pfWeights = ALCHEMY_NULL;
			pBoneData->puIndices = ALCHEMY_NULL;
		}
	}

	CMeshManager::SKINDATA SkinData = {0};
	SkinData.uSkinBoneDataNum = BoneDataes.GetLength();

	ALCHEMY_DEBUG_NEW(SkinData.pSkinBoneData, CSkinInfo::BONE[SkinData.uSkinBoneDataNum]);
	for (i=0; i<SkinData.uSkinBoneDataNum; ++i)
	{
		SkinData.pSkinBoneData[i].pcName = BoneDataes[i].pcName;
		SkinData.pSkinBoneData[i].pfWeights = BoneDataes[i].pfWeights;
		SkinData.pSkinBoneData[i].puIndices = BoneDataes[i].puIndices;
		SkinData.pSkinBoneData[i].OffsetMatrix = BoneDataes[i].OffsetMatrix;
		SkinData.pSkinBoneData[i].uInfluenceCount = BoneDataes[i].uInfluenceCount;
	}

	//pSkinInfo->Create( pBoneData, BoneDataes.GetLength() );

	CMeshManager::HANDLE SkinHandle = ALCHEMY_MESH_MANAGER.AddSkinData(SkinData);

	for (i=0; i<SkinData.uSkinBoneDataNum; ++i)
	{
		pBoneData = &BoneDataes[i];
		ALCHEMY_DEBUG_DELETE_ARRAY( pBoneData->pcName );
		ALCHEMY_DEBUG_DELETE_ARRAY( pBoneData->pfWeights );
		ALCHEMY_DEBUG_DELETE_ARRAY( pBoneData->puIndices );
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(SkinData.pSkinBoneData);

	NodeData.SkinDataHandle = SkinHandle;
	//NodeData.pSkinInfo->AddRef();
	NodeData.uRootBone = uRootBone;
	m_SkinNodes.Add(&NodeData);

	for (i=0; i<OtherNodeData.GetLength(); ++i)
	{
		OtherNodeData[i]->SkinDataHandle = SkinHandle;
		OtherNodeData[i]->uRootBone = uRootBone;
		m_SkinNodes.Add(OtherNodeData[i]);
	}
}

//void alchemy::CDAEParser::__parseMesh( const TiXmlElement& Gemotry, CMeshManager::SCENENODEDATA& NodeData, const TiXmlElement* pController )
//{
//	CMeshManager::MESHDATA MeshData = {0};
//	int nFirstSubset = -1;
//	int nTriangleNum;
//	UINT i,j,uCurSemanticIndex,uCurVertexIndex, uCurIndex, uPolyIndex = 0, uTotalStream = 0, uStream = 0;
//	CMap<UINT,CMeshManager::HANDLE>	subsetMaterialDic;
//	CVector<UINT>	pVals, indexArray;
//	const TiXmlElement* pXMLList;
//	CString<char> strSemantic;
//	COverBuffer DataBuffer[3];
//	COverBuffer IndexData;
//	CVector<CVertexDeclaration::VERTEXELEMENT> curElementData;
//
//	UINT uAddNormalIndex;
//	CMap<UINT,CVector<UINT> > PosIndexNormalMap;
//	CMap<UINT, CVector<UINT> >	ExterntPosIndex;
//	CPool< CMap<UINT,UINT> > UVPosIndexMapPool;
//	CMap<UINT, CMap<UINT,UINT>* > PosIndexUVMap;
//	CMap<UINT,UINT>* pSourceUVMap;
//	CMap<UINT,UINT>	EmptyUVIndexMap;
//	CVector<UINT>* pSourceArray;
//	CVector<UINT> EmptySourceArray;
//	CVector<FLOAT>	vecVertexDataes[3];
//	CVector<CMesh::ATTRIBUTERANGE>	Attributes;
//	CMesh::ATTRIBUTERANGE EmptyAttribute;
//	CMesh::ATTRIBUTERANGE* pAttribute;
//	UINT uVertexNumIndex;
//	bool isNeedAdd = false;
//
//	bool isUvAdd = false;
//
//	UINT uSubsetNum = 0;
//
//	const TiXmlElement* pPolygons = Gemotry.FirstChildElement("mesh")->FirstChildElement("polygons");
//	const TiXmlElement* pInputList;
//	while (pPolygons)
//	{
//		++uSubsetNum;
//		pAttribute = Attributes.Get( Attributes.Add( EmptyAttribute ) );
//		pAttribute->uStartIndex = MeshData.uNumIndices;
//		pPolygons->Attribute("count", &nTriangleNum);
//		pAttribute->uIndexCount = nTriangleNum*3;
//		MeshData.uNumIndices += pAttribute->uIndexCount;
//
//		if(nTriangleNum > 0)
//		{
//			if(nFirstSubset == -1)
//				nFirstSubset = uPolyIndex;
//			subsetMaterialDic.Insert(uPolyIndex, __parseMaterial( pPolygons->Attribute("material") ) );
//		}
//
//		if(MeshData.VertexElementData.uElementCount == 0)
//		{
//			pInputList = pPolygons->FirstChildElement("input");
//			CVertexDeclaration::VERTEXELEMENT EmptyElementData;
//			CVertexDeclaration::VERTEXELEMENT* pVertexElement;
//			TiXmlElement* pVerticesXML;
//
//			while(pInputList)
//			{
//				strSemantic = pInputList->Attribute("semantic");
//				if( strSemantic == "VERTEX" )
//				{
//					pVertexElement = curElementData.Get( curElementData.Add(EmptyElementData) );
//					pVertexElement->uStream = uStream++;
//					pVertexElement->uIndex = 0;
//					pVertexElement->uOffset = 0;
//					pVertexElement->Type = CVertexDeclaration::FLOAT4;
//					pVertexElement->Usage = CVertexDeclaration::POSITION;
//
//					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("vertices"), &pInputList->Attribute("source")[1]);
//					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("source"), &pVerticesXML->FirstChildElement("input")->Attribute("source")[1] );
//
//					__getFloatArray( vecVertexDataes[0], *pVerticesXML->FirstChildElement("float_array") );
//					for (i=0; i<vecVertexDataes[0].GetLength(); i+=3)
//					{
//						DataBuffer[0].WriteFloat( vecVertexDataes[0][i] );
//						DataBuffer[0].WriteFloat( vecVertexDataes[0][i+1] );
//						DataBuffer[0].WriteFloat( vecVertexDataes[0][i+2] );
//						DataBuffer[0].WriteFloat( 1.0f );
//					}
//
//					uVertexNumIndex = vecVertexDataes[0].GetLength()/3-1;
//				}
//				else if( strSemantic == "NORMAL" )
//				{
//					pVertexElement = curElementData.Get( curElementData.Add(EmptyElementData) );
//					pVertexElement->uStream = uStream++;
//					pVertexElement->uIndex = 0;
//					pVertexElement->uOffset = 0;
//					pVertexElement->Type = CVertexDeclaration::FLOAT3;
//					pVertexElement->Usage = CVertexDeclaration::NORMAL;
//
//					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
//					__getFloatArray( vecVertexDataes[1], *pVerticesXML->FirstChildElement("float_array") );
//				}
//				else if( strSemantic == "TEXCOORD" )
//				{
//					if(isUvAdd)
//					{
//						++uTotalStream;
//						pInputList = pInputList->NextSiblingElement("input");
//						continue;
//					}
//
//					isUvAdd = true;
//					pVertexElement = curElementData.Get( curElementData.Add(EmptyElementData) );
//					pVertexElement->uStream = uStream++;
//					pVertexElement->uIndex = 0;
//					pVertexElement->uOffset = 0;
//					pVertexElement->Type = CVertexDeclaration::FLOAT2;
//					pVertexElement->Usage = CVertexDeclaration::TEXCOORD;
//
//					pVerticesXML = __findElementByID( *Gemotry.FirstChildElement("mesh")->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
//					__getFloatArray( vecVertexDataes[2], *pVerticesXML->FirstChildElement("float_array") );
//				}
//
//				++uTotalStream;
//
//				pInputList = pInputList->NextSiblingElement("input");
//			}
//
//			ALCHEMY_DEBUG_NEW(MeshData.VertexElementData.pVertexElement, CVertexDeclaration::VERTEXELEMENT[uStream]);
//			ALCHEMY_DEBUG_NEW(MeshData.VertexElementData.puUsage, UINT[uStream]);
//
//			MeshData.VertexElementData.uElementCount = uStream;
//		}
//
//		pInputList = pPolygons->FirstChildElement("input");
//		pXMLList = pPolygons->FirstChildElement("p");
//		while (pXMLList)
//		{
//			pVals.Clear();
//			indexArray.Clear();
//			__getUintArray(pVals, *pXMLList);
//			for (j=0; j<3; ++j)
//			{
//				uCurSemanticIndex = j*uTotalStream;
//				isNeedAdd = false;
//				isUvAdd = false;
//				pInputList = pPolygons->FirstChildElement("input");
//				while (pInputList)
//				{
//					strSemantic = pInputList->Attribute("semantic");
//					if( strSemantic == "VERTEX" )
//					{
//						uCurVertexIndex = pVals[uCurSemanticIndex];
//						indexArray.Add( uCurVertexIndex );
//					}
//					else if( strSemantic == "NORMAL" )
//					{
//						uCurIndex = pVals[uCurSemanticIndex];
//						uAddNormalIndex = uCurIndex;
//						pSourceArray = PosIndexNormalMap.Get(uCurVertexIndex);
//						if(pSourceArray)
//						{
//							UINT uTemp;
//							if( !pSourceArray->GetIndexOf(uTemp, uCurIndex) )
//								pSourceArray->Add( uCurIndex );
//							else
//							{
//								EmptySourceArray.Clear();
//								EmptySourceArray.Add(uCurIndex);
//								PosIndexNormalMap.Insert(uCurVertexIndex, EmptySourceArray);
//							}
//						}
//						else
//						{
//							EmptySourceArray.Clear();
//							EmptySourceArray.Add(uCurIndex);
//							PosIndexNormalMap.Insert(uCurVertexIndex, EmptySourceArray);
//						}
//					}
//					else if( strSemantic == "TEXCOORD" )
//					{
//						if(isUvAdd)
//						{
//							++uCurSemanticIndex;
//							pInputList = pInputList->NextSiblingElement("input");
//							continue;
//						}
//
//						isUvAdd = true;
//
//						uCurIndex = pVals[uCurSemanticIndex];
//						CMap<UINT,UINT>** ppSourceTemp = PosIndexUVMap.Get( uCurVertexIndex );
//						if(ppSourceTemp == ALCHEMY_NULL)
//						{
//							pSourceUVMap = UVPosIndexMapPool.Get( UVPosIndexMapPool.Add(EmptyUVIndexMap) );
//							pSourceUVMap->Insert(uCurIndex, uCurVertexIndex);
//							PosIndexUVMap.Insert(uCurVertexIndex, pSourceUVMap);
//
//							DataBuffer[2].SetPosition( uCurVertexIndex*sizeof(FLOAT)*2 );
//							DataBuffer[2].WriteFloat( vecVertexDataes[2][uCurIndex*2] );
//							DataBuffer[2].WriteFloat( 1.0f - vecVertexDataes[2][uCurIndex*2+1] );
//						}
//						else
//						{
//							pSourceUVMap = *ppSourceTemp;
//							UINT* pVertexIndex = pSourceUVMap->Get(uCurIndex);
//							if( pVertexIndex )
//								indexArray[j] = *pVertexIndex;
//							else
//							{
//								DataBuffer[0].WriteFloat( vecVertexDataes[0][ uCurVertexIndex*3 ] );
//								DataBuffer[0].WriteFloat( vecVertexDataes[0][ uCurVertexIndex*3+1 ] );
//								DataBuffer[0].WriteFloat( vecVertexDataes[0][ uCurVertexIndex*3+2 ] );
//								DataBuffer[0].WriteFloat( 1.0f );
//								++uVertexNumIndex;
//
//								pSourceUVMap->Insert( uCurIndex, uVertexNumIndex );
//								indexArray[j] = uVertexNumIndex;
//
//								DataBuffer[2].SetPosition( uVertexNumIndex*sizeof(FLOAT)*2 );
//								DataBuffer[2].WriteFloat( vecVertexDataes[2][uCurIndex*2] );
//								DataBuffer[2].WriteFloat( 1.0f - vecVertexDataes[2][uCurIndex*2+1] );
//
//								pSourceArray = ExterntPosIndex.Get( uCurVertexIndex );
//								if(pSourceArray)
//									pSourceArray->Add(uVertexNumIndex);
//								else
//								{
//									EmptySourceArray.Clear();
//									EmptySourceArray.Add(uVertexNumIndex);
//									ExterntPosIndex.Insert(uCurVertexIndex, EmptySourceArray);
//								}
//
//								isNeedAdd = true;
//							}
//						}
//					}
//
//					++uCurSemanticIndex;
//
//					pInputList = pInputList->NextSiblingElement("input");
//				}
//
//				if(isNeedAdd)
//				{
//					EmptySourceArray.Clear();
//					EmptySourceArray.Add(uAddNormalIndex);
//					PosIndexNormalMap.Insert(uVertexNumIndex, EmptySourceArray);
//				}
//			}
//
//			IndexData.WriteData( indexArray[0] );
//			IndexData.WriteData( indexArray[2] );
//			IndexData.WriteData( indexArray[1] );
//
//			pXMLList = pXMLList->NextSiblingElement("p");
//		}
//		
//		pPolygons = pPolygons->NextSiblingElement("polygons");
//		++uPolyIndex;
//	}
//
//	MeshData.uIndexUsage = ALCHEMY_FLAG( INDEXBUFFER::WRITE_ONLY );
//	MeshData.uAttributeRangeCount = uPolyIndex;
//	ALCHEMY_DEBUG_NEW(MeshData.pAttributeRange, CMesh::ATTRIBUTERANGE[MeshData.uAttributeRangeCount]);
//	for (i=0; i<uPolyIndex; ++i)
//		MeshData.pAttributeRange[i] = Attributes[i];
//
//	if(MeshData.uNumIndices >= 0xffff)
//	{
//		MeshData.IndexFormat = INDEXBUFFER::BIT32;
//		PUINT pIndexData;
//		ALCHEMY_DEBUG_NEW(pIndexData, UINT[MeshData.uNumIndices]);
//		MeshData.pIndexData = reinterpret_cast<PVOID>(pIndexData);
//		IndexData.SetPosition(0);
//		ALCHEMY_MEMORY_COPY( MeshData.pIndexData, IndexData.GetBuffer(), sizeof(UINT32)*MeshData.uNumIndices );
//	}
//	else
//	{
//		MeshData.IndexFormat = INDEXBUFFER::BIT16;
//		PUINT16 pIndexData;
//		ALCHEMY_DEBUG_NEW(pIndexData, UINT16[MeshData.uNumIndices]);
//		MeshData.pIndexData = reinterpret_cast<PVOID>(pIndexData);
//		IndexData.SetPosition(0);
//		for (i=0; i<MeshData.uNumIndices; ++i)
//			*reinterpret_cast<PUINT16>(MeshData.pIndexData+i*sizeof(UINT16)) = static_cast<UINT16>( IndexData.ReadData<UINT>() );
//	}
//
//	CMap<UINT, CVector<UINT> >::CIterator iter = PosIndexNormalMap.GetBeginIterator();
//
//	CVector3 normal;
//	while( CMap<UINT, CVector<UINT> >::IsValuable(iter) )
//	{
//		normal.x = normal.y = normal.z = 0.0f;
//		const CVector<UINT>& normalArray = iter.GetValue();
//		for (i=0; i<normalArray.GetLength(); ++i)
//		{
//			normal.x += vecVertexDataes[1][ normalArray[i]*3 ];
//			normal.y += vecVertexDataes[1][ normalArray[i]*3+1 ];
//			normal.z += vecVertexDataes[1][ normalArray[i]*3+2 ];
//		}
//
//		normal.Normalize();
//
//		DataBuffer[1].SetPosition( iter.GetKey()*sizeof(FLOAT)*3 );
//		DataBuffer[1].WriteFloat(normal.x);
//		DataBuffer[1].WriteFloat(normal.y);
//		DataBuffer[1].WriteFloat(normal.z);
//
//		++iter;
//	}
//
//	MeshData.uNumVertices = uVertexNumIndex + 1;
//
//	PUINT8 pVertexData;
//	ALCHEMY_DEBUG_NEW(MeshData.ppVertexData, PUINT8[uStream]);
//	for (i=0; i<uStream; ++i)
//	{
//		MeshData.VertexElementData.pVertexElement[i] = *curElementData.Get(i);
//		if(MeshData.VertexElementData.pVertexElement[i].Type == CVertexDeclaration::FLOAT4 && MeshData.VertexElementData.pVertexElement[i].Usage == CVertexDeclaration::POSITION)
//			MeshData.VertexElementData.puUsage[i] = pController ? (ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY) | ALCHEMY_FLAG(VERTEXBUFFER::DYNAMIC) ) : ( ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY) );
//
//		switch(MeshData.VertexElementData.pVertexElement[i].Usage)
//		{
//		case CVertexDeclaration::POSITION:
//			ALCHEMY_DEBUG_NEW(pVertexData, UINT8[MeshData.uNumVertices*sizeof(FLOAT)*4]);
//			MeshData.ppVertexData[i] = reinterpret_cast<PVOID>(pVertexData);
//			DataBuffer[0].SetPosition(0);
//			ALCHEMY_MEMORY_COPY(MeshData.ppVertexData[i], DataBuffer[0].GetBuffer(), sizeof(FLOAT)*MeshData.uNumVertices*4);
//			break;
//		case CVertexDeclaration::NORMAL:
//			ALCHEMY_DEBUG_NEW(pVertexData, UINT8[MeshData.uNumVertices*sizeof(FLOAT)*3]);
//			MeshData.ppVertexData[i] = reinterpret_cast<PVOID>(pVertexData);
//			DataBuffer[1].SetPosition(0);
//			ALCHEMY_MEMORY_COPY(MeshData.ppVertexData[i], DataBuffer[1].GetBuffer(), sizeof(FLOAT)*MeshData.uNumVertices*3);
//			break;
//		case CVertexDeclaration::TEXCOORD:
//			ALCHEMY_DEBUG_NEW(pVertexData, UINT8[MeshData.uNumVertices*sizeof(FLOAT)*2]);
//			MeshData.ppVertexData[i] = reinterpret_cast<PVOID>(pVertexData);
//			DataBuffer[2].SetPosition(0);
//			ALCHEMY_MEMORY_COPY(MeshData.ppVertexData[i], DataBuffer[2].GetBuffer(), sizeof(FLOAT)*MeshData.uNumVertices*2);
//			break;
//		}
//	}
//
//	NodeData.uMeshData = ALCHEMY_MESH_MANAGER.AddMeshData(MeshData);
//	NodeData.uSubset = nFirstSubset;
//	NodeData.MaterialDataHandle = subsetMaterialDic[nFirstSubset];
//	NodeData.isUpdate = true;
//
//	CMeshManager::HANDLE hMaterialData;
//	CMeshManager::SCENENODEDATA* pSceneNodeData;
//	CVector<CMeshManager::SCENENODEDATA*> SceneObjects;
//	STRING strName;
//	for (i=nFirstSubset+1; i<uSubsetNum; ++i)
//	{
//		hMaterialData = subsetMaterialDic[i];
//		if(hMaterialData == 0)
//			continue;
//
//		pSceneNodeData = __createSceneNodeData(CMeshManager::OBJECT, ALCHEMY_NULL);
//		pSceneNodeData->MaterialDataHandle = hMaterialData;
//		pSceneNodeData->uMeshData = NodeData.uMeshData;
//		pSceneNodeData->uSubset = i;
//		strName = NodeData.pcName;
//		strName += i;
//		ALCHEMY_DEBUG_NEW(pSceneNodeData->pcName, CHAR[strName.GetLength()+1]);
//		for(j=0; j<strName.GetLength(); ++j)
//			pSceneNodeData->pcName[j] = strName[j];
//		pSceneNodeData->pcName[strName.GetLength()] = 0;
//		pSceneNodeData->uNameLen = strName.GetLength();
//		pSceneNodeData->uParent = NodeData.uParent;
//		pSceneNodeData->isUpdate = false;
//		SceneObjects.Add(pSceneNodeData);
//	}
//
//	if(pController)
//	{
//		const TiXmlElement* pSkinXML = pController->FirstChildElement("skin");
//		CString<char> strSemantic;
//		pInputList = pSkinXML->FirstChildElement("joints")->FirstChildElement("input");
//		while (pInputList)
//		{
//			strSemantic = pInputList->Attribute("semantic");
//			if(strSemantic == "JOINT")
//				break;
//
//			pInputList = pInputList->NextSiblingElement("input");
//		}
//
//		pInputList = __findElementByID(*pSkinXML->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
//		CString<char> strJoints = pInputList->FirstChildElement("Name_array")->GetText();
//		strJoints.Replace("\n"," ");
//		CVector<CString<char> > nameArray;
//		strJoints.Split(" ", nameArray);
//		CMatrix4x4 bindShapeMatrix;
//		CVector<FLOAT> ShapeData;
//		__getFloatArray(ShapeData, *pSkinXML->FirstChildElement("bind_shape_matrix") );
//		for (i=0; i<16; ++i)
//			bindShapeMatrix.fValue[i] = ShapeData[i];
//
//		__getAxisMatrix(bindShapeMatrix, &bindShapeMatrix);
//		pInputList = pSkinXML->FirstChildElement("joints")->FirstChildElement("input");
//		while (pInputList)
//		{
//			strSemantic = pInputList->Attribute("semantic");
//			if(strSemantic == "INV_BIND_MATRIX")
//				break;
//
//			pInputList = pInputList->NextSiblingElement("input");
//		}
//
//		pInputList = __findElementByID(*pSkinXML->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
//
//		CVector<FLOAT> FloatArrayData;
//		__getFloatArray(FloatArrayData, *pInputList->FirstChildElement("float_array"));
//		CMeshManager::SCENENODEDATA* pBone;
//
//		STRING strCompare;
//		STRING strError;
//		CVector<CSkinInfo::BONE>	BoneDataes;
//		CSkinInfo::BONE* pBoneData;
//		CSkinInfo::BONE EmptyBoneData;
//		CMatrix4x4 matInvalid, matTemp;
//		bool isSetRootBone = false;
//		UINT uRootBone = 0;
//
//		for (i=0; i<FloatArrayData.GetLength(); i+=16)
//		{
//			if(!uRootBone)
//			{
//				pBone = __findNodeByName(nameArray[i/16], &uRootBone);
//				uRootBone = uRootBone+1;
//			}
//			else
//				pBone = __findNodeByName(nameArray[i/16], ALCHEMY_NULL);
//
//			if(pBone)
//			{
//				if(!isSetRootBone)
//				{
//					CMeshManager::SCENENODEDATA* pRootBone = pBone;
//					while(pRootBone->uParent && pRootBone->Type == CMeshManager::BONE)
//					{
//						uRootBone = pRootBone->uParent;
//						pRootBone = m_SceneNodes.Get(pRootBone->uParent-1);
//					}
//
//					isSetRootBone = true;
//				}
//				pBoneData = BoneDataes.Get( BoneDataes.Add(EmptyBoneData) );
//				__copyChar(&pBoneData->pcName, nameArray[i/16]);
//				matTemp = bindShapeMatrix;
//				for (j=0; j<16; ++j)
//					matInvalid.fValue[j] = FloatArrayData[i+j];
//				__getBoneInvalidMatrix(matInvalid, &matInvalid);
//				matTemp *= matInvalid;
//				for (j=0; j<16; ++j)
//					pBoneData->OffsetMatrix.fValue[j] = matTemp.fValue[j];
//			}
//			else
//			{
//				//StringAppend(strError, nameArray[i/16], 0, nameArray[i/16].GetLength());
//				strError += ALCHEMY_TEXT("not found");
//				ALCHEMY_DEBUG_WARNING(strError);
//			}
//		}
//
//		pInputList = pSkinXML->FirstChildElement("vertex_weights")->FirstChildElement("input");
//		while (pInputList)
//		{
//			strSemantic = pInputList->Attribute("semantic");
//			if(strSemantic == "WEIGHT")
//				break;
//
//			pInputList = pInputList->NextSiblingElement("input");
//		}
//
//		pInputList = __findElementByID(*pSkinXML->FirstChildElement("source"), &pInputList->Attribute("source")[1]);
//
//		FloatArrayData.Clear();
//		__getFloatArray(FloatArrayData, *pInputList->FirstChildElement("float_array"));
//		CVector<UINT> vCount, v;
//		__getUintArray(vCount, *pSkinXML->FirstChildElement("vertex_weights")->FirstChildElement("vcount") );
//		__getUintArray(v, *pSkinXML->FirstChildElement("vertex_weights")->FirstChildElement("v") );
//
//		UINT c,uLen = vCount.GetLength();
//		UINT uCount, uIndex = 0;
//		FLOAT fWeight;
//		CVector<UINT> EmptyIndices;
//		CVector<FLOAT>	EmptyWeightes;
//		CVector<UINT>* puIndices;
//		CVector<FLOAT>* pfWeights;
//		CVector<UINT>* pExternIndices;
//		CMap<CSkinInfo::BONE*, CVector<UINT> > uIndicesMap;
//		CMap<CSkinInfo::BONE*, CVector<FLOAT> > fWeightsMap;
//		for (i=0; i<uLen; ++i)
//		{
//			uCount = vCount[i];
//			pExternIndices = ExterntPosIndex.Get(i);
//			for (j=0; j<uCount; ++j)
//			{
//				pBoneData = &BoneDataes[ v[uIndex] ];
//				puIndices = uIndicesMap.Get(pBoneData);
//				if(!puIndices)
//				{
//					uIndicesMap.Insert(pBoneData, EmptyIndices);
//					puIndices = uIndicesMap.Get(pBoneData);
//				}
//
//				++uIndex;
//				fWeight = FloatArrayData[ v[uIndex] ];
//				pfWeights = fWeightsMap.Get(pBoneData);
//				if(!pfWeights)
//				{
//					fWeightsMap.Insert(pBoneData, EmptyWeightes);
//					pfWeights = fWeightsMap.Get(pBoneData);
//				}
//
//				++uIndex;
//
//				if(fWeight > 0.0f)
//				{
//					puIndices->Add( i );
//					pfWeights->Add( fWeight );
//					if(pExternIndices)
//					{
//						for (c=0; c<pExternIndices->GetLength(); ++c)
//						{
//							puIndices->Add( *pExternIndices->Get(c) );
//							pfWeights->Add( fWeight );
//						}
//					}
//				}
//			}
//		}
//
//		uLen = BoneDataes.GetLength();
//		for (i=0; i<uLen; ++i)
//		{
//			pBoneData = &BoneDataes[i];
//			puIndices = uIndicesMap.Get(pBoneData);
//			pfWeights = fWeightsMap.Get(pBoneData);
//			pBoneData->uInfluenceCount = puIndices->GetLength();
//			ALCHEMY_DEBUG_NEW(pBoneData->pfWeights, FLOAT[pBoneData->uInfluenceCount]);
//			ALCHEMY_DEBUG_NEW(pBoneData->puIndices, UINT[pBoneData->uInfluenceCount]);
//
//			for (c=0; c<pBoneData->uInfluenceCount; ++c)
//			{
//				pBoneData->pfWeights[c] = *pfWeights->Get(c);
//				pBoneData->puIndices[c] = *puIndices->Get(c);
//			}
//		}
//
//		CSkinInfo* pSkinInfo;
//		ALCHEMY_DEBUG_NEW(pSkinInfo, CSkinInfo);
//		ALCHEMY_DEBUG_NEW(pBoneData, CSkinInfo::BONE[BoneDataes.GetLength()]);
//		for (i=0; i<BoneDataes.GetLength(); ++i)
//		{
//			pBoneData[i].pcName = BoneDataes[i].pcName;
//			pBoneData[i].pfWeights = BoneDataes[i].pfWeights;
//			pBoneData[i].puIndices = BoneDataes[i].puIndices;
//			pBoneData[i].OffsetMatrix = BoneDataes[i].OffsetMatrix;
//			pBoneData[i].uInfluenceCount = BoneDataes[i].uInfluenceCount;
//		}
//
//		pSkinInfo->Create(pBoneData, BoneDataes.GetLength() );
//
//		for (i=0; i<BoneDataes.GetLength(); ++i)
//		{
//			ALCHEMY_DEBUG_DELETE_ARRAY( pBoneData[i].pcName );
//			ALCHEMY_DEBUG_DELETE_ARRAY( pBoneData[i].pfWeights );
//			ALCHEMY_DEBUG_DELETE_ARRAY( pBoneData[i].puIndices );
//		}
//
//		ALCHEMY_DEBUG_DELETE_ARRAY(pBoneData);
//
//		NodeData.pSkinInfo = pSkinInfo;
//		NodeData.pSkinInfo->AddRef();
//		NodeData.uRootBone = uRootBone;
//		m_SkinNodes.Add(&NodeData);
//
//		for (i=0; i<SceneObjects.GetLength(); ++i)
//		{
//			SceneObjects[i]->pSkinInfo = pSkinInfo;
//			SceneObjects[i]->pSkinInfo->AddRef();
//			SceneObjects[i]->uRootBone = uRootBone;
//			m_SkinNodes.Add(SceneObjects[i]);
//		}
//
//		pSkinInfo->Release();
//	}
//}

void alchemy::CDAEParser::__getNodeMatrix( const TiXmlElement& Element, CMeshManager::SCENENODEDATA& Node, const CMeshManager::SCENENODEDATA* pParent, CMatrix4x4* pOutMatrix )
{
	if(pOutMatrix)
	{
		BASICSTRING strData = Element.GetText();
		STRING strTemp;
		CVector< BASICSTRING > MatrixData;
		strData.Split(" ", MatrixData);
		for (UINT i=0; i<16; ++i)
			pOutMatrix->fValue[i] = ALCHEMY_STRING_FLOAT( MatrixData[i] );

		if(!pParent)
			__getAxisMatrix( *pOutMatrix, pOutMatrix );
	}
}

void alchemy::CDAEParser::__getAxisMatrix( const CMatrix4x4& SourceMatrix, CMatrix4x4* pOutMatrix )
{
	if(pOutMatrix)
	{
		CMatrix4x4 IdentityMat;
		IdentityMat.Identity();
		if( m_bIsYUp )
		{
			*pOutMatrix = SourceMatrix;
			IdentityMat._33 = -1.0f;

			(*pOutMatrix) *= IdentityMat;
		}
		else
		{
			*pOutMatrix = SourceMatrix;
			IdentityMat._22 = 0.0f;
			IdentityMat._32 = 1.0f;
			IdentityMat._23 = 1.0f;
			IdentityMat._33 = 0.0f;

			(*pOutMatrix) *= IdentityMat;
		}
	}
}

void alchemy::CDAEParser::__getAnimMatrix( const char* pName, const CMatrix4x4& SourceMatrix, CMatrix4x4* pOutMatrix )
{
	UINT uLen = m_RootBones.GetLength();
	BASICSTRING strTempName = pName;
	STRING strTemp;
	ALCHEMY_BASICSTRING_TO_STRING(strTemp, m_uCurrentEncode, strTempName);
	for (UINT i=0; i<uLen; ++i)
	{
		if( strTemp == m_RootBones[i]->pcName )
			return __getAxisMatrix(SourceMatrix, pOutMatrix);
		else
			*pOutMatrix = SourceMatrix;
	}
}

void alchemy::CDAEParser::__getBoneInvalidMatrix(const CMatrix4x4& SourceMatrix, CMatrix4x4* pOutMatrix)
{
	if(pOutMatrix)
	{
		CMatrix4x4 IdentityMat;
		IdentityMat.Identity();
		if( m_bIsYUp )
		{
			IdentityMat._33 = -1.0f;

			Multiply(*pOutMatrix, SourceMatrix, IdentityMat);
		}
		else
		{
			IdentityMat._22 = 0.0f;
			IdentityMat._32 = 1.0f;
			IdentityMat._23 = 1.0f;
			IdentityMat._33 = 0.0f;

			Multiply(*pOutMatrix, SourceMatrix, IdentityMat);
		}
	}
}

void alchemy::CDAEParser::__transformData(const MATRIX4X4& MatTransform, CVector<FLOAT>& Data, bool isNormal)
{
	UINT i,uLen;
	CVector3 vecData;

	if(isNormal)
	{
		CMatrix4x4 matTransform = MatTransform;
		matTransform.Invert3x4();
		matTransform.Transpose();

		uLen = Data.GetLength()/3;

		for(i=0; i<uLen; ++i)
		{
			vecData.x = Data[i*3];
			vecData.y = Data[i*3+1];
			vecData.z = Data[i*3+2];

			vecData.TransformNormal(matTransform);
			
			Data[i*3]   = vecData.x;
			Data[i*3+1] = vecData.y;
			Data[i*3+2] = vecData.z;
		}
	}
	else
	{
		uLen = Data.GetLength()/3;

		for(i=0; i<uLen; ++i)
		{
			vecData.x = Data[i*3];
			vecData.y = Data[i*3+1];
			vecData.z = Data[i*3+2];

			vecData.TransformCoord(MatTransform);
			
			Data[i*3]   = vecData.x;
			Data[i*3+1] = vecData.y;
			Data[i*3+2] = vecData.z;
		}
	}
}

CMeshManager::SCENENODEDATA* alchemy::CDAEParser::__findNodeByName(const char* pName, UINT* pIndex)
{
	BASICSTRING strName = pName;
	STRING strTemp;
	for (UINT i=0; i<m_SceneNodes.GetLength(); ++i)
	{
		ALCHEMY_BASICSTRING_TO_STRING(strTemp, m_uCurrentEncode, strName);
		if( strTemp == m_SceneNodes[i].pcName )
		{
			if(pIndex)
				*pIndex = i;
			return &m_SceneNodes[i];
		}
	}

	return ALCHEMY_NULL;
}

void alchemy::CDAEParser::__parseAnimationSet()
{
	TiXmlElement* pXML = m_pCollada->FirstChildElement("library_animations");
	if(pXML)
	{
		CVector<CAnimationSet::ANIMATION>	AnimationDatas;
		CAnimationSet::ANIMATION EmptyAnimation;
		CAnimationSet::ANIMATION* pAnimation;
		BASICSTRING	strAnimName, strSemantic, strTarget;
		STRING strTemp;
		CVector< BASICSTRING > vecTargets;
		TiXmlElement* pChannel, *pSampler, *pInputs, *pSource;
		CVector<FLOAT>	AnimTimes, AnimMatrixes;
		UINT i,j,uLen;
		CMatrix4x4 matTemp,matOut;

		pXML = pXML->FirstChildElement("animation");

		while (pXML)
		{
			vecTargets.Clear();
			strAnimName = pXML->Attribute("name");
			pChannel = pXML->FirstChildElement("animation")->FirstChildElement("channel");
			strTarget = pChannel->Attribute("target");
			strTarget.Split("/", vecTargets);


			pAnimation = AnimationDatas.Get( AnimationDatas.Add(EmptyAnimation) );

			ALCHEMY_DEBUG_NEW( pAnimation->pcName, CHAR[strAnimName.GetLength()+1] );
			pAnimation->pcName[strAnimName.GetLength()] = 0;

			__copyChar(&pAnimation->pcName, strAnimName);

			pSampler = pXML->FirstChildElement("animation")->FirstChildElement("sampler");
			pInputs = pSampler->FirstChildElement("input");

			while (pInputs)
			{
				strSemantic = pInputs->Attribute("semantic");
				pSource = __findElementByID( *pXML->FirstChildElement("animation")->FirstChildElement("source"), &pInputs->Attribute("source")[1] );

				if(strSemantic == "INPUT")
				{
					AnimTimes.Clear();
					__getFloatArray(AnimTimes, *pSource->FirstChildElement("float_array"));
					ALCHEMY_DEBUG_NEW(pAnimation->pfTime, FLOAT[AnimTimes.GetLength()]);
					for (i=0; i<AnimTimes.GetLength(); ++i)
						pAnimation->pfTime[i] = AnimTimes[i];

					pAnimation->uFrameCount = AnimTimes.GetLength();
				}
				else if(strSemantic == "OUTPUT")
				{
					AnimMatrixes.Clear();
					__getFloatArray(AnimMatrixes, *pSource->FirstChildElement("float_array"));
					uLen = AnimMatrixes.GetLength()/16;
					ALCHEMY_DEBUG_NEW(pAnimation->pTransformMatrices, MATRIX4X4[ uLen ]);
					for (j=0; j<uLen; ++j)
					{
						for (i=0; i<16; ++i)
							matTemp.fValue[i] = AnimMatrixes[j*16+i];

						__getAnimMatrix(strAnimName, matTemp, &matOut);
						pAnimation->pTransformMatrices[j] = matOut;
					}
				}

				pInputs = pInputs->NextSiblingElement("input");
			}
			
			pXML = pXML->NextSiblingElement("animation");
		}

		CAnimationController* pAnimController;
		CAnimationSet* pAnimationSet;

		ALCHEMY_DEBUG_NEW(pAnimationSet, CAnimationSet);
		ALCHEMY_DEBUG_NEW(pAnimController, CAnimationController);

		CAnimationSet::ANIMATION* pAnimationData;
		ALCHEMY_DEBUG_NEW( pAnimationData, CAnimationSet::ANIMATION[AnimationDatas.GetLength()] );
		for (i=0; i<AnimationDatas.GetLength(); ++i)
		{
			pAnimationData[i].pcName = AnimationDatas[i].pcName;
			pAnimationData[i].pfTime = AnimationDatas[i].pfTime;
			pAnimationData[i].pTransformMatrices = AnimationDatas[i].pTransformMatrices;
			pAnimationData[i].uFrameCount = AnimationDatas[i].uFrameCount;
		}

		pAnimationSet->Create(pAnimationData, AnimationDatas.GetLength() );

		//pAnimController->Create(200, 10);
		pAnimController->RegisterAnimationSet(*pAnimationSet);

		m_pAnimationController = pAnimController;

		//for (i=0; i<m_SkinNodes.GetLength(); ++i)
		//{
			//m_SkinNodes[i]->pAnimationController = pAnimController;
			//pAnimController->AddRef();
		//}

		//pAnimController->Release();
		pAnimationSet->Release();

		for (i=0 ;i<AnimationDatas.GetLength(); ++i)
		{
			ALCHEMY_DEBUG_DELETE_ARRAY( AnimationDatas[i].pcName );
			ALCHEMY_DEBUG_DELETE_ARRAY( AnimationDatas[i].pfTime );
			ALCHEMY_DEBUG_DELETE_ARRAY( AnimationDatas[i].pTransformMatrices );
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(pAnimationData);
	}
}

CMeshManager::HANDLE alchemy::CDAEParser::__parseMaterial( const char* pMaterialID )
{
	if(!pMaterialID)
		return 0;

	CMeshManager::HANDLE* pHandle = m_MaterialLib.Get(pMaterialID);
	if( pHandle )
		return *pHandle;

	TiXmlElement* pElement = m_pCollada->FirstChildElement("library_materials")->FirstChildElement("material");
	pElement = __findElementByID(*pElement, pMaterialID);
	if(!pElement)
		return 0;

	pElement = __findElementByID(*m_pCollada->FirstChildElement("library_effects")->FirstChildElement("effect"), &pElement->FirstChildElement("instance_effect")->Attribute("url")[1]);
	if(!pElement)
		return 0;

	pElement = pElement->FirstChildElement()->FirstChildElement()->FirstChildElement()->FirstChildElement();

	CMeshManager::MATERIALDATA MaterialData = {0};
	BASICSTRING strTag;
	CVector< BASICSTRING > OutColorVal;
	CARGBColor Color;
	TiXmlElement* pTextureElement;
	CVector<CMeshManager::HANDLE>	TextureHandles;
	BASICSTRING	strName;

	while (pElement)
	{
		strTag = pElement->Value();
		OutColorVal.Clear();
		if(strTag == "emission")
		{
			strTag = pElement->FirstChildElement("color")->GetText();
			strTag.Split(" ", OutColorVal);
			Color.r = ALCHEMY_STRING_FLOAT( OutColorVal[0] );
			Color.g = ALCHEMY_STRING_FLOAT( OutColorVal[1] );
			Color.b = ALCHEMY_STRING_FLOAT( OutColorVal[2] );
			Color.a = ALCHEMY_STRING_FLOAT( OutColorVal[3] );
			MaterialData.Emissive = Color;
		}
		else if(strTag == "ambient")
		{
			strTag = pElement->FirstChildElement("color")->GetText();
			strTag.Split(" ", OutColorVal);
			Color.r = ALCHEMY_STRING_FLOAT( OutColorVal[0] );
			Color.g = ALCHEMY_STRING_FLOAT( OutColorVal[1] );
			Color.b = ALCHEMY_STRING_FLOAT( OutColorVal[2] );
			Color.a = ALCHEMY_STRING_FLOAT( OutColorVal[3] );
			MaterialData.Ambient = Color;
		}
		else if(strTag == "diffuse")
		{
			if(pElement->FirstChildElement("color"))
			{
				strTag = pElement->FirstChildElement("color")->GetText();
				strTag.Split(" ", OutColorVal);
				Color.r = ALCHEMY_STRING_FLOAT( OutColorVal[0] );
				Color.g = ALCHEMY_STRING_FLOAT( OutColorVal[1] );
				Color.b = ALCHEMY_STRING_FLOAT( OutColorVal[2] );
				Color.a = ALCHEMY_STRING_FLOAT( OutColorVal[3] );
				MaterialData.Diffuse = Color;
			}
			else
			{
				MaterialData.Diffuse = 0xffffffff;
				pTextureElement = pElement->FirstChildElement("texture");

				strName = pTextureElement->Attribute("texture");

				//∏˘æ›MAPƒ√µΩTextureData
				pHandle = m_TextureMap.Get( strName );
				if(pHandle)
					TextureHandles.Add( *pHandle );
			}
		}
		else if(strTag == "specular")
		{
			if(pElement->FirstChildElement("color"))
			{
				strTag = pElement->FirstChildElement("color")->GetText();
				strTag.Split(" ", OutColorVal);
				Color.r = ALCHEMY_STRING_FLOAT( OutColorVal[0] );
				Color.g = ALCHEMY_STRING_FLOAT( OutColorVal[1] );
				Color.b = ALCHEMY_STRING_FLOAT( OutColorVal[2] );
				Color.a = ALCHEMY_STRING_FLOAT( OutColorVal[3] );
				MaterialData.Specular = Color;
			}
			else
			{
				MaterialData.Specular = 0xffffffff;
				pTextureElement = pElement->FirstChildElement("texture");

				strName = pTextureElement->Attribute("texture");

				//∏˘æ›MAPƒ√µΩTextureData
				pHandle = m_TextureMap.Get( strName );
				if(pHandle)
					TextureHandles.Add( *pHandle );
			}
		}
		else if(strTag == "shininess")
		{
			strTag = pElement->FirstChildElement("float")->GetText();
			MaterialData.fPower = ALCHEMY_STRING_FLOAT(strTag);
		}

		pElement = pElement->NextSiblingElement();
	}

	MaterialData.uTextureCount = TextureHandles.GetLength();

	if(MaterialData.uTextureCount)
	{
		ALCHEMY_DEBUG_NEW(MaterialData.pTextureDataHandles, CMeshManager::HANDLE[MaterialData.uTextureCount]);
		for (UINT i=0; i<MaterialData.uTextureCount; ++i)
			MaterialData.pTextureDataHandles[i] = TextureHandles[i];
	}
	else
		MaterialData.pTextureDataHandles = ALCHEMY_NULL;

	CMeshManager::HANDLE hMaterial = ALCHEMY_MESH_MANAGER.AddMaterialData( MaterialData );

	m_MaterialLib.Insert( pMaterialID, hMaterial);

	ALCHEMY_DEBUG_DELETE_ARRAY(MaterialData.pTextureDataHandles);

	return hMaterial;
}

TiXmlElement* alchemy::CDAEParser::__findElementByID(const TiXmlElement& Node, const char* pID )
{
	CString<char> strName;
	TiXmlElement* pNode = const_cast<TiXmlElement*>(&Node);
	while (pNode)
	{
		strName = pNode->Attribute("id");
		if(strName == pID)
			return pNode;
		
		pNode = pNode->NextSiblingElement();
	}

	return ALCHEMY_NULL;
}

CMeshManager::SCENENODEDATA* alchemy::CDAEParser::__createSceneNodeData( CMeshManager::SCENETYPE eType, CMeshManager::SCENENODEDATA* pParent )
{
	UINT uParentIndex = m_SceneNodes.GetLength();

	if(pParent)
	{
		m_SceneNodes.GetIndexOf(uParentIndex, *pParent);
		if(uParentIndex == m_SceneNodes.GetLength())
			return ALCHEMY_NULL;
		++uParentIndex;
	}
	else
		uParentIndex = 0;

	CMeshManager::SCENENODEDATA EmptyNodeData;
	CMeshManager::SCENENODEDATA* pNodeData = m_SceneNodes.Get( m_SceneNodes.Add(EmptyNodeData) );
	pNodeData->Type = eType;
	pNodeData->pcName = ALCHEMY_NULL;
	pNodeData->uNameLen = 0;
	pNodeData->MaterialDataHandle = 0;
	pNodeData->SkinDataHandle = 0;
	pNodeData->uMeshData = 0;
	pNodeData->uParent = uParentIndex;
	pNodeData->uRootBone = 0;

	return pNodeData;
}

void alchemy::CDAEParser::__getUintArray( CVector<UINT>& Out, const TiXmlElement& Element )
{
	BASICSTRING strUnitArray = Element.GetText();
	CVector< BASICSTRING > splitArray;
	strUnitArray.Split("\r\n", splitArray);
	BASICSTRING strUINT;
	strUINT.Join(splitArray, " ");

	splitArray.Clear();
	strUINT.Split(" ", splitArray);

	for (UINT i=0; i<splitArray.GetLength(); ++i)
		Out.Add( ALCHEMY_STRING_UNSIGN_INTEGER( splitArray[i] ) );
}

void alchemy::CDAEParser::__getFloatArray( CVector<FLOAT>& Out, const TiXmlElement& Element )
{
	/*BASICSTRING strFloatArray = Element.GetText();
	CVector< BASICSTRING > splitArray;
	strFloatArray.Split("\r\n", splitArray);

	BASICSTRING strFloat;
	strFloat.Join(splitArray, " ");
	splitArray.Clear();

	strFloat.Split(" ", splitArray);
	for (UINT i=0; i<splitArray.GetLength(); ++i)
		Out.Add( ALCHEMY_STRING_FLOAT(splitArray[i]) );*/
	const char* pcFloatArray = Element.GetText();
	if(pcFloatArray)
	{
		while(*pcFloatArray)
			Out.Add( ALCHEMY_STRING_FLOAT(pcFloatArray, ALCHEMY_NULL, &pcFloatArray) );
	}
}

void alchemy::CDAEParser::__copyChar(PCHAR* pDestName, const char* pName)
{
	if(pDestName && pName)
	{
		BASICSTRING strName = pName;
		STRING strTemp;
		ALCHEMY_BASICSTRING_TO_STRING(strTemp, m_uCurrentEncode, strName);
		ALCHEMY_DEBUG_NEW(*pDestName, CHAR[strTemp.GetLength()+1]);
		strTemp.CopyTo(*pDestName);
	}
}