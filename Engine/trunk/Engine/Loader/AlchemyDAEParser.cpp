#include "AlchemyDAEParser.h"

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


alchemy::CDAEParser::CDAEParser(TEXTUREDATARES* pTextureDataRes, UINT uTextureDataNum, const STREAMUSAGE* pStreamUsage, UINT uStreamUsageNum,UINT uIndexUsage)
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
	m_SceneData.pSceneNodeDatas = ALCHEMY_NULL;

	if(pTextureDataRes && uTextureDataNum)
	{
		UINT uLen;
		ALCHEMY_DEBUG_NEW(m_pTextureDataRes, TEXTUREDATARES[uTextureDataNum]);
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

	CVector<CMeshManager::SCENENODEDATA>::CIterator Iterator = m_SceneNodes.GetIterator(0);
	while( CVector<CMeshManager::SCENENODEDATA>::IsValuable(Iterator) )
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(Iterator->pcName);

		++ Iterator;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_SceneData.pSceneNodeDatas);
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
		CMeshManager::SCENENODEDATA* pSceneNode;
		ALCHEMY_DEBUG_NEW(m_SceneData.pSceneNodeDatas, CMeshManager::HANDLE[uLen]);
		for (UINT i=0; i<uLen; ++i)
		{
			pSceneNode = m_SceneNodes.Get(i);
			m_SceneData.pSceneNodeDatas[i] = pSceneNode ? __AddSceneNodeData(*pSceneNode) : 0;
		}
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
	UINT uLength = strTemp.GetLength();

	ALCHEMY_DEBUG_NEW(pNode->pcName, CHAR[uLength + 1]);

	for (UINT i = 0; i < uLength; ++ i)
		pNode->pcName[i] = strTemp[i];

	pNode->pcName[uLength] = 0;

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
	UINT uStream = 0, uIndex = 0;
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
					Element.uIndex = uIndex ++;
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
					Element.uIndex = uIndex ++;
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
					Element.uIndex =  uIndex ++;
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
					Element.uIndex =  uIndex ++;
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

		NodeData.MeshData = ALCHEMY_MESH_MANAGER.AddMeshData(MeshData);

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

		NodeData.MeshData = ALCHEMY_MESH_MANAGER.AddMeshData(MeshData);

		ALCHEMY_DEBUG_DELETE_ARRAY(pIndexData);
	}

	if(!pController)
	{
		CMatrix4x4 IdentityMat;
		IdentityMat.Identity();
		Copy(NodeData.Transform, IdentityMat);
	}
	NodeData.uSubset = 0;
	NodeData.MaterialData = subsetMaterial[0];
	//NodeData.isUpdate = true;

	CMeshManager::SCENENODEDATA* pSceneNodeData;
	CVector<CMeshManager::SCENENODEDATA*> SceneObjects;
	STRING strName;
	UINT uLength;
	for (i=1; i<MeshData.uAttributeRangeCount; ++i)
	{
		pSceneNodeData = __createSceneNodeData(CMeshManager::OBJECT, ALCHEMY_NULL);
		pSceneNodeData->MaterialData = subsetMaterial[i];
		pSceneNodeData->MeshData = NodeData.MeshData;
		pSceneNodeData->uSubset = i;
		pSceneNodeData->Transform = NodeData.Transform;
		strName = NodeData.pcName;
		strName += i;
		uLength = strName.GetLength();
		ALCHEMY_DEBUG_NEW(pSceneNodeData->pcName, CHAR[uLength + 1]);
		for(j = 0; j < uLength; ++ j)
			pSceneNodeData->pcName[j] = strName[j];

		pSceneNodeData->pcName[uLength] = 0;
		pSceneNodeData->Parent = NodeData.Parent;
		//pSceneNodeData->isUpdate = false;
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

	NodeData.MeshData = ALCHEMY_MESH_MANAGER.AddMeshData(MeshData);
	NodeData.uSubset = 0;
	NodeData.MaterialData = subsetMaterial[0];
	//NodeData.isUpdate = true;

	CMeshManager::SCENENODEDATA* pSceneNodeData;
	CVector<CMeshManager::SCENENODEDATA*> SceneObjects;
	STRING strName;
	UINT uLength;
	for (i=1; i<MeshData.uAttributeRangeCount; ++i)
	{
		pSceneNodeData = __createSceneNodeData(CMeshManager::OBJECT, ALCHEMY_NULL);
		pSceneNodeData->MaterialData = subsetMaterial[i];
		pSceneNodeData->MeshData = NodeData.MeshData;
		pSceneNodeData->uSubset = i;
		pSceneNodeData->Transform = NodeData.Transform;
		strName = NodeData.pcName;
		strName += i;

		uLength = strName.GetLength();
		ALCHEMY_DEBUG_NEW(pSceneNodeData->pcName, CHAR[uLength + 1]);
		for(j = 0; j < uLength; ++ j)
			pSceneNodeData->pcName[j] = strName[j];
		pSceneNodeData->pcName[uLength] = 0;
		pSceneNodeData->Parent = NodeData.Parent;
		//pSceneNodeData->isUpdate = false;
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

			++ uRootBone;
		}
		else
			pBone = __findNodeByName(nameArray[i/16], ALCHEMY_NULL);

		if(pBone)
		{
			if(!isSetRootBone)
			{
				const CMeshManager::SCENENODEDATA* pRootBone = pBone;
				while(pRootBone && pRootBone->Parent && pRootBone->Type == CMeshManager::BONE)
				{
					uRootBone = pRootBone->Parent;
					pRootBone = m_SceneNodes.Get(pRootBone->Parent - 1);
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
	SkinData.uBoneDataCount = BoneDataes.GetLength();

	ALCHEMY_DEBUG_NEW(SkinData.pBoneData, CSkinInfo::BONE[SkinData.uBoneDataCount]);
	for (i=0; i<SkinData.uBoneDataCount; ++i)
	{
		SkinData.pBoneData[i].pcName = BoneDataes[i].pcName;
		SkinData.pBoneData[i].pfWeights = BoneDataes[i].pfWeights;
		SkinData.pBoneData[i].puIndices = BoneDataes[i].puIndices;
		SkinData.pBoneData[i].OffsetMatrix = BoneDataes[i].OffsetMatrix;
		SkinData.pBoneData[i].uInfluenceCount = BoneDataes[i].uInfluenceCount;
	}

	//pSkinInfo->Create( pBoneData, BoneDataes.GetLength() );

	CMeshManager::HANDLE SkinHandle = ALCHEMY_MESH_MANAGER.AddSkinData(SkinData);

	for (i=0; i<SkinData.uBoneDataCount; ++i)
	{
		pBoneData = &BoneDataes[i];
		ALCHEMY_DEBUG_DELETE_ARRAY( pBoneData->pcName );
		ALCHEMY_DEBUG_DELETE_ARRAY( pBoneData->pfWeights );
		ALCHEMY_DEBUG_DELETE_ARRAY( pBoneData->puIndices );
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(SkinData.pBoneData);

	NodeData.SkinData = SkinHandle;
	//NodeData.pSkinInfo->AddRef();
	NodeData.RootBone = uRootBone;
	m_SkinNodes.Add(&NodeData);

	for (i=0; i<OtherNodeData.GetLength(); ++i)
	{
		OtherNodeData[i]->SkinData = SkinHandle;
		OtherNodeData[i]->RootBone = uRootBone;
		m_SkinNodes.Add(OtherNodeData[i]);
	}
}

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

CMeshManager::SCENENODEDATA* alchemy::CDAEParser::__findNodeByName(const char* pName, PUINT puOutputIndex)
{
	BASICSTRING strName = pName;
	STRING strTemp;
	for (UINT i=0; i<m_SceneNodes.GetLength(); ++i)
	{
		ALCHEMY_BASICSTRING_TO_STRING(strTemp, m_uCurrentEncode, strName);
		if( strTemp == m_SceneNodes[i].pcName )
		{
			if(puOutputIndex)
				*puOutputIndex = i;

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
		ALCHEMY_DEBUG_NEW(MaterialData.pTextureData, CMeshManager::HANDLE[MaterialData.uTextureCount]);
		for (UINT i=0; i<MaterialData.uTextureCount; ++i)
			MaterialData.pTextureData[i] = TextureHandles[i];
	}
	else
		MaterialData.pTextureData = ALCHEMY_NULL;

	CMeshManager::HANDLE hMaterial = ALCHEMY_MESH_MANAGER.AddMaterialData( MaterialData );

	m_MaterialLib.Insert( pMaterialID, hMaterial);

	ALCHEMY_DEBUG_DELETE_ARRAY(MaterialData.pTextureData);

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
	UINT uParent = 0;

	if(pParent)
	{
		if( m_SceneNodes.GetIndexOf(uParent, *pParent) )
			++ uParent;
	}

	CMeshManager::SCENENODEDATA EmptyNodeData;
	CMeshManager::SCENENODEDATA* pNodeData = m_SceneNodes.Get( m_SceneNodes.Add(EmptyNodeData) );
	pNodeData->Type = eType;
	pNodeData->pcName = ALCHEMY_NULL;
	pNodeData->MaterialData = 0;
	pNodeData->SkinData = 0;
	pNodeData->MeshData = 0;
	pNodeData->Parent = uParent;
	pNodeData->RootBone = 0;

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

CMeshManager::HANDLE alchemy::CDAEParser::__AddSceneNodeData(CMeshManager::SCENENODEDATA& SceneNodeData)
{
	CMeshManager::HANDLE* pSceneNodeDataHandle = m_SceneNodeMap.Get(&SceneNodeData);
	if(pSceneNodeDataHandle)
		return *pSceneNodeDataHandle;

	m_SceneNodeMap.Insert(&SceneNodeData, 0);

	CMeshManager::SCENENODEDATA CurrentSceneNodeData = SceneNodeData;

	CMeshManager::SCENENODEDATA* pSceneNodeData;
	if(SceneNodeData.Parent)
	{
		pSceneNodeData = m_SceneNodes.Get(SceneNodeData.Parent - 1);
		if(pSceneNodeData)
			CurrentSceneNodeData.Parent = __AddSceneNodeData(*pSceneNodeData);
		else
			CurrentSceneNodeData.Parent = 0;
	}

	if(SceneNodeData.RootBone)
	{
		pSceneNodeData = m_SceneNodes.Get(SceneNodeData.RootBone - 1);
		if(pSceneNodeData)
			CurrentSceneNodeData.RootBone = __AddSceneNodeData(*pSceneNodeData);
		else
			CurrentSceneNodeData.RootBone = 0;
	}

	CMeshManager::HANDLE Result = ALCHEMY_MESH_MANAGER.AddSceneNodeData(CurrentSceneNodeData);
	m_SceneNodeMap.Insert(&SceneNodeData, Result);

	return Result;
}