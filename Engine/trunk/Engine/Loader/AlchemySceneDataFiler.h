#pragma once

#include "AlchemyFileWriterInterface.h"
#include "AlchemyFileReadInterface.h"

#include "../Graphics/AlchemyMeshManager.h"
#include "../Core/AlchemyEventDispatcher.h"

namespace alchemy
{
	class CSceneNodeDataListener;
	class CSceneDataListener;
	class CSceneDataFiler : 
		public EVENTDISPATCHER
	{
		friend class CSceneNodeDataListener;
		friend class CSceneDataListener;
	public:
		typedef enum
		{
			MESH_DATA, 
			TEXTURE_DATA, 
			MATERIAL_DATA,
			SKIN_DATA,
			SCENE_NODE_DATA, 

			TOTAL_DATA_FORMAT
		}FORMAT;

		typedef struct
		{
			FORMAT Format;
			CMeshManager::HANDLE* pSource;
			UINT uIndex;
		}DATARESOURCE;

		CSceneDataFiler(void);
		~CSceneDataFiler(void);

		void ClearIndexMap(UINT uFlag = ~0);

		bool WriteMeshData(const CMeshManager::MESHDATA& Data, IFileWriter& Writer, const CHAR* pcPath = ALCHEMY_NULL);
		bool WriteTextureData(const CMeshManager::TEXTUREDATA& Data, IFileWriter& Writer, const CHAR* pcPath = ALCHEMY_NULL);
		bool WriteMaterialData(const CMeshManager::MATERIALDATA& Data, IFileWriter& Writer, const CHAR* pcPath = ALCHEMY_NULL, UINT uIndex = 0, const CHAR* pcTexturePath = ALCHEMY_NULL, PUINT puOutputIndex = ALCHEMY_NULL);
		bool WriteSkinData(const CMeshManager::SKINDATA& Data, IFileWriter& Writer, const CHAR* pcPath = ALCHEMY_NULL);
		bool WriteSceneNodeData(const CMeshManager::SCENENODEDATA& Data, IFileWriter& Writer, const CHAR* pcPath = ALCHEMY_NULL, UINT uIndex = 0, const CHAR* pcDataPath[TOTAL_DATA_FORMAT] = ALCHEMY_NULL, PUINT puOutputIndex = ALCHEMY_NULL);
		bool WriteSceneData(const CMeshManager::SCENEDATA& Data, IFileWriter& Writer, const CHAR* pcPath = ALCHEMY_NULL, UINT uIndex = 0, const CHAR* pcDataPath[TOTAL_DATA_FORMAT] = ALCHEMY_NULL, PUINT puOutputIndex = ALCHEMY_NULL);

		CBuffer& ReadMeshData(IFileReader& Reader);
		CBuffer& ReadTextureData(IFileReader& Reader);
		CBuffer& ReadMaterialData(IFileReader& Reader);
		CBuffer& ReadSkinData(IFileReader& Reader);
		CBuffer& ReadSceneNodeData(IFileReader& Reader);
		CBuffer& ReadSceneData(IFileReader& Reader);

		void LoadSceneData(IFileReader& FileReader, const CHAR* pcSceneDataPath, const CHAR* pcDataPath[TOTAL_DATA_FORMAT] = ALCHEMY_NULL);
	private:
		CMap<CMeshManager::HANDLE, UINT> m_IndexMap[TOTAL_DATA_FORMAT];

		CMap<CMeshManager::HANDLE, CBuffer*> m_DataMap;
	};
}