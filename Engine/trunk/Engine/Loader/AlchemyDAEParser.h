#pragma once

#include "../Core/AlchemyNull.h"
#include "../Core/AlchemyVector.h"
#include "../Core/AlchemyMap.h"
#include "../Core/AlchemyString.h"

#include "../Graphics/AlchemyMeshManager.h"

class TiXmlDocument;
class TiXmlElement;

namespace alchemy
{
	class CDAEParser
	{
		typedef struct  
		{
			TiXmlElement*					pGemotryXML;
			CMeshManager::SCENENODEDATA*	pHostNode;
			TiXmlElement*					pControllerXML;
		}GEMOTRYDATA;

		typedef enum
		{
			ENCODE_UTF8,

			TOTAL_ENCODE
		}ENCODING;

		static const UINT		 sm_ENCODE_FLAG[TOTAL_ENCODE];
		static const BASICSTRING sm_ENCODINGTYPE[TOTAL_ENCODE];

	public:
		typedef struct  
		{
			PCHAR					pcTextureName;
			CMeshManager::HANDLE	hTextureDataHandle;
		}TEXTUREDATARES,*LPTEXTUREDATARES;

		typedef struct  
		{
			CVertexDeclaration::ELEMENTTYPE		Type;
			CVertexDeclaration::ELEMENTUSAGE	ElementUsage;
			UINT								Index;
			UINT								Usage;
		}STREAMUSAGE,*LPSTREAMUSAGE;

		CDAEParser(TEXTUREDATARES* pTextureDataRes = ALCHEMY_NULL, UINT uTextureDataNum = 0, const STREAMUSAGE* pStreamUsage = ALCHEMY_NULL, UINT uStreamUsageNum = 0, UINT uIndexUsage = ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY));
		~CDAEParser();

		typedef struct CSTRINGLELESS
		{
			bool operator()(const BASICSTRING& Left, const BASICSTRING& Right)const;
		}CSTRINGLELESS;

		typedef struct STRINGLELESS
		{
			bool operator()(const STRING& Left, const STRING& Right)const;
		}STRINGLELESS;

		bool Parse(const void* pData, const char* pFileName = ALCHEMY_NULL);
		const CMeshManager::SCENEDATA& GetSceneData() const;

		CAnimationController* GetAnimationController();

	private:
		void __parseNode( const TiXmlElement& Node, CMeshManager::SCENENODEDATA* pParent );
		void __parseGemotry( const TiXmlElement& Gemotry, CMeshManager::SCENENODEDATA& NodeData, const TiXmlElement* pController = ALCHEMY_NULL );

		void __parseGemotrySingleStream( const TiXmlElement& Gemotry, CMeshManager::SCENENODEDATA& NodeData, const TiXmlElement* pController = ALCHEMY_NULL );
		void __parseGemotryOptimize( const TiXmlElement& Gemotry, CMeshManager::SCENENODEDATA& NodeData, const TiXmlElement* pController = ALCHEMY_NULL );

		void __parseSkinGemotry(const TiXmlElement& Controller, const CMap<UINT, CVector<UINT> >& ExternalIndices, CMeshManager::SCENENODEDATA& NodeData, CVector<CMeshManager::SCENENODEDATA*>& OtherNodeData );

		//void __parseMesh( const TiXmlElement& Gemotry, CMeshManager::SCENENODEDATA& NodeData, const TiXmlElement* pController = ALCHEMY_NULL );

		void __getNodeMatrix( const TiXmlElement& Element, CMeshManager::SCENENODEDATA& Node, const CMeshManager::SCENENODEDATA* pParent, CMatrix4x4* pOutMatrix );
		void __getAxisMatrix( const CMatrix4x4& SourceMatrix, CMatrix4x4* pOutMatrix );
		void __getAnimMatrix( const char* pName, const CMatrix4x4& SourceMatrix, CMatrix4x4* pOutMatrix );
		void __getBoneInvalidMatrix(const CMatrix4x4& SourceMatrix, CMatrix4x4* pOutMatrix);

		void __transformData(const MATRIX4X4& MatTransform, CVector<FLOAT>& Data, bool isNormal = false);

		CMeshManager::SCENENODEDATA* __findNodeByName(const char* pName, PUINT puOutputIndex);

		void __parseAnimationSet();
		CMeshManager::HANDLE __parseMaterial( const char* pMaterialID );

		TiXmlElement* __findElementByID(const TiXmlElement& Node, const char* pID );

		CMeshManager::SCENENODEDATA* __createSceneNodeData( CMeshManager::SCENETYPE eType, CMeshManager::SCENENODEDATA* pParent = ALCHEMY_NULL );

		void __getUintArray( CVector<UINT>& Out, const TiXmlElement& Element );
		void __getFloatArray( CVector<FLOAT>& Out, const TiXmlElement& Element );

		void __copyChar(PCHAR* pDestName, const char* pName);

		CMeshManager::HANDLE __AddSceneNodeData(CMeshManager::SCENENODEDATA& SceneNodeData);

		CAnimationController*					m_pAnimationController;

		TiXmlElement*							m_pCollada;

		bool									m_bIsYUp;
		bool									m_bIsSkinMesh;

		CVector<CMeshManager::SCENENODEDATA>	m_SceneNodes;
		CVector<CMeshManager::SCENENODEDATA*>	m_RootBones;
		CVector<CMeshManager::SCENENODEDATA*>	m_SkinNodes;
		CVector<CMeshManager::SCENENODEDATA*>	m_StaticNodes;
		CVector<GEMOTRYDATA*>					m_GemotryList;

		CMap<BASICSTRING, CMeshManager::HANDLE, CSTRINGLELESS>		m_MaterialLib;
		CMap<BASICSTRING, CMeshManager::HANDLE, CSTRINGLELESS>		m_TextureMap;

		CMap<CMeshManager::SCENENODEDATA*, CMeshManager::HANDLE> m_SceneNodeMap;

		CMeshManager::SCENEDATA					m_SceneData;

		STREAMUSAGE*							m_pStreamUsage;
		UINT									m_uStreamUsageNum;
		UINT									m_uIndexUsage;

		UINT									m_uCurrentEncode;
		UINT									m_uTextureDataNum;
		TEXTUREDATARES*			                m_pTextureDataRes;
	};

	ALCHEMY_INLINE bool CDAEParser::CSTRINGLELESS::operator()(const BASICSTRING& Left, const BASICSTRING& Right)const
	{
		if(Left.GetLength() > Right.GetLength())
			return true;
		else if( Left.GetLength() < Right.GetLength() )
			return false;
		else
		{
			UINT uLen = Left.GetLength();
			for (UINT i=0; i<uLen; ++i)
			{
				if( Left[i] < Right[i] )
					return true;
			}

			return false;
		}
	}

	ALCHEMY_INLINE bool CDAEParser::STRINGLELESS::operator()(const STRING& Left, const STRING& Right)const
	{
		if(Left.GetLength() > Right.GetLength())
			return true;
		else if( Left.GetLength() < Right.GetLength() )
			return false;
		else
		{
			UINT uLen = Left.GetLength();
			for (UINT i=0; i<uLen; ++i)
			{
				if( Left[i] < Right[i] )
					return true;
			}

			return false;
		}
	}

	ALCHEMY_INLINE CAnimationController* CDAEParser::GetAnimationController()
	{
		return m_pAnimationController;
	}
}