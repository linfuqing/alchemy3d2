package alchemy.manager
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.data.CElementUsageData;
	import alchemy.data.CMaterialData;
	import alchemy.data.CMeshData;
	import alchemy.data.CMeshSegmentData;
	import alchemy.data.CSceneData;
	import alchemy.data.CSceneNodeData;
	import alchemy.data.CSkinData;
	import alchemy.data.CTextureData;
	import alchemy.data.CVertexElementData;
	import alchemy.data.animation.CBoneData;
	import alchemy.resource.basic.CTexture;
	import alchemy.resource.extend.CMesh;
	import alchemy.tools.CDebug;
	
	import flash.utils.Dictionary;
	
	public class CMeshManager extends CPointer
	{
		private var _meshDataMap:Dictionary;
		private var _sceneDataMap:Dictionary;
		private var _textureDataMap:Dictionary;
		private var _skinDataMap:Dictionary;
		private var _materialDataMap:Dictionary;
		
		static public function get instance():CMeshManager
		{
			return _meshManager;
		}
		
		public function CMeshManager()
		{
			CDebug.assert(_canMeshManagerCreated, "Mesh manager is singleton.");
			
			_canMeshManagerCreated = false;
			
			super();
			
			init();
		}
		
		public function getSkinData( handle:uint ):CSkinData
		{
			return _skinDataMap[handle];
		}
		
		public function addSkinData( skinData:CSkinData ):uint
		{
			if(!skinData || skinData.boneData.length == 0)
				return 0;
			
			if( _skinDataMap[skinData] != null )
				return _skinDataMap[skinData];
			
			var skinPtr:uint = CLibrary.method.CreateBuffer( CLibrary.POINTER_SIZE * 2 );
			
			var len:int = skinData.boneData.length;
			var boneAryPos:uint = CLibrary.method.CreateSkinInfoData(len);
			
			CLibrary.memory.position = skinPtr;
			CLibrary.memory.writeUnsignedInt(boneAryPos);
			CLibrary.memory.writeUnsignedInt(len);
			
			var boneInfoPtrs:Array;
			var boneData:CBoneData;
			
			var influence:uint;
			for(var i:int=0; i<len; ++i)
			{
				boneData = skinData.boneData[i];
				influence = boneData.weights.length;
				
				boneInfoPtrs = CLibrary.method.CreateBoneData( boneAryPos + i*CBoneData.BONE_DATA_OFFSET, influence , boneData.name.length+1  );
				
				CLibrary.memory.position = boneInfoPtrs[0];
				CLibrary.memory.writeUTFBytes( boneData.name );
				CLibrary.memory.writeByte(0);
				
				CLibrary.memory.position = boneInfoPtrs[1];
				for(var j:int=0; j<influence; ++j)
					CLibrary.memory.writeFloat( boneData.weights[j] );
				
				CLibrary.memory.position = boneInfoPtrs[2];
				for(j=0; j<influence; ++j)
					CLibrary.memory.writeUnsignedInt( boneData.indices[j] );
				
				CLibrary.memory.position = boneInfoPtrs[3];
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[0] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[4] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[8] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[12] );
				
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[1] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[5] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[9] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[13] );
				
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[2] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[6] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[10] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[14] );
				
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[3] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[7] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[11] );
				CLibrary.memory.writeFloat( boneData.offsetMatrix.rawData[15] );
			}
			
			var handle:uint = CLibrary.method.AddSkinDataToMeshManager(skinPtr);
			
			for(i=0; i<len; ++i)
				CLibrary.method.DestroyBoneData( boneAryPos + i*CBoneData.BONE_DATA_OFFSET );
			
			CLibrary.method.DestroySkinInfoData(boneAryPos);
			
			CLibrary.method.DestroyBuffer( skinPtr );
			
			_skinDataMap[handle] = skinData;
			
			return handle;
		}
		
		public function getTextureData( handle:uint ):CTextureData
		{
			return _textureDataMap[handle];
		}
		
		public function addTextureData( textureData:CTextureData ):uint
		{
			if(!textureData || !textureData.data)
				return 0;
			
			if( _textureDataMap[textureData] != null )
				return _textureDataMap[textureData];
			
			var length:uint = textureData.data.length;
			var ptrs:Array = CLibrary.method.CreateTextureData(length);
			CLibrary.memory.position = ptrs[0];
			CLibrary.memory.writeUnsignedInt(ptrs[1]);
			CLibrary.memory.writeUnsignedInt(textureData.width);
			CLibrary.memory.writeUnsignedInt(textureData.height);
			CLibrary.memory.writeUnsignedInt(CTexture.FORMAT_ARGB);
			CLibrary.memory.writeUnsignedInt(textureData.usage);
			CLibrary.memory.writeUnsignedInt(textureData.mipLevel);
			
			if(ptrs[1])
			{
				CLibrary.memory.position = ptrs[1];
				CLibrary.memory.writeBytes(textureData.data, 0, textureData.data.length);
			}
			
			var handle:uint =  CLibrary.method.AddTextureDataToMeshManager( ptrs[0] );
			
			CLibrary.method.DestroyTextureData(ptrs[0]);
			
			if(handle)
				_textureDataMap[handle] = textureData;
			
			return handle;
		}
		
		public function getMaterialData( handle:uint ):CMaterialData
		{
			return _materialDataMap[handle];
		}
		
		public function addMaterialData( materialData:CMaterialData ):uint
		{
			if( _meshDataMap[materialData] != null )
				return _meshDataMap[materialData];
			
			var ptrs:Array = CLibrary.method.CreateMaterialData( materialData.textureDataHandles.length );
			CLibrary.memory.position = ptrs[0];
			CLibrary.memory.writeUnsignedInt(materialData.ambient);
			CLibrary.memory.writeUnsignedInt(materialData.diffuse);
			CLibrary.memory.writeUnsignedInt(materialData.specular);
			CLibrary.memory.writeUnsignedInt(materialData.emissive);
			CLibrary.memory.writeFloat(materialData.power);
			
			if(ptrs[1])
			{
				CLibrary.memory.position = ptrs[1];
				for(var i:uint=0; i<materialData.textureDataHandles.length; ++i)
					CLibrary.memory.writeUnsignedInt( materialData.textureDataHandles[i] );
			}
			
			var handle:uint = CLibrary.method.AddMaterialDataToMeshManager( ptrs[0] );
			
			CLibrary.method.DestroyMaterialData( ptrs[0] );
			
			if(handle)
				_materialDataMap[handle] = materialData;
			
			return handle;
		}
		
		public function getMeshData( handle:uint ):CMeshData
		{
			return _meshDataMap[handle];
		}
		
		public function addMeshData( meshData:CMeshData ):uint
		{
			if( _meshDataMap[meshData] != null )
				return _meshDataMap[meshData];
			
			var streamCount:uint = meshData.streamCount;
			
			if(streamCount > meshData.vertexData.length)
			{
				CDebug.warning("Error mesh data");
				
				return null;
			}
			
			var meshDataPointer:uint = CLibrary.method.CreateMeshData(0);
			var vertexElementPointer:uint;
			var vertexUsagePointer:uint;
			var attributeRangePointer:uint;
			var vertexDataPointer:uint;
			var indexDataPointer:uint;
			
			var vertexElementLength:uint;
			var attributeRangeLength:uint;
			
			CLibrary.memory.position = meshDataPointer;
			CLibrary.memory.writeUnsignedInt(meshData.numVertices);
			CLibrary.memory.writeUnsignedInt(meshData.numIndices);
			CLibrary.memory.writeUnsignedInt(meshData.indexUsage);
			CLibrary.memory.writeInt(CMeshData.INDEX_FORMAT_16BIT);
			
			vertexElementPointer = CLibrary.memory.position;
			CLibrary.memory.writeUnsignedInt(0);
			CLibrary.memory.writeUnsignedInt(vertexElementLength = meshData.vertexDeclaration.length);
			
			vertexUsagePointer = CLibrary.memory.position;
			CLibrary.memory.writeUnsignedInt(0);
			
			attributeRangePointer = CLibrary.memory.position;
			CLibrary.memory.writeUnsignedInt(0);
			CLibrary.memory.writeUnsignedInt(attributeRangeLength = meshData.attributeRange.length);
			
			vertexDataPointer = CLibrary.memory.position;
			CLibrary.memory.writeUnsignedInt(0);
			
			indexDataPointer = CLibrary.memory.position;
			CLibrary.memory.writeUnsignedInt(0);
			
			CLibrary.method.CreateMeshData(meshDataPointer);
			
			var i:uint;
			
			var vertexElement:CVertexElementData;
			CLibrary.memory.position = vertexElementPointer;
			CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
			for(i = 0; i < vertexElementLength; i ++)
			{
				vertexElement = meshData.vertexDeclaration[i];
				CLibrary.memory.writeUnsignedInt(vertexElement.stream);
				CLibrary.memory.writeUnsignedInt(vertexElement.offset);
				CLibrary.memory.writeInt(vertexElement.type);
				CLibrary.memory.writeInt(vertexElement.usage);
				CLibrary.memory.writeUnsignedInt(i);
				//CLibrary.memory.writeUnsignedInt(vertexElement.vertexUsage);
			}
			
			CLibrary.memory.position = vertexUsagePointer;
			CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
			for(i = 0; i < streamCount; i ++)
				CLibrary.memory.writeUnsignedInt(meshData.vertexData[i].usage);
			
			var attributeRange:CMeshSegmentData;
			CLibrary.memory.position = attributeRangePointer;
			CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
			for(i = 0; i < attributeRangeLength; ++ i)
			{
				attributeRange = meshData.attributeRange[i];
				CLibrary.memory.writeUnsignedInt(attributeRange.startIndex);
				CLibrary.memory.writeUnsignedInt(attributeRange.indexCount);
			}
			
			CLibrary.memory.position = indexDataPointer;
			CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
			CLibrary.memory.writeBytes( meshData.indexData, 0, meshData.numIndices * (1 << CMeshData.INDEX_FORMAT_16BIT) );
			
			CLibrary.method.CreateMeshData(meshDataPointer);
			
			var vertexDataOffset:uint = 0;
			for(i = 0; i < streamCount; ++ i)
			{
				CLibrary.memory.position = vertexDataPointer;
				CLibrary.memory.position = CLibrary.memory.readUnsignedInt() + vertexDataOffset;
				CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
				CLibrary.memory.writeBytes(meshData.vertexData[i].data);
				
				vertexDataOffset += CLibrary.POINTER_SIZE;
			}
			
			var handle:uint = CLibrary.method.AddMeshDataToMeshManager(meshDataPointer);
			
			CLibrary.method.DestroyMeshData(meshDataPointer);
			
			if(handle)
				_meshDataMap[handle] = meshData;
			
			return handle;
		}
		
		public function getSceneData( handle:uint ):CSceneData
		{
			return _sceneDataMap[handle];
		}
		
		public function addSceneData( sceneData:CSceneData ):uint
		{
			if( _sceneDataMap[sceneData] != null )
				return _sceneDataMap[sceneData];
			
			var nodeData:CSceneNodeData;
			var i:uint, uLen:uint = sceneData.sceneNodesDatas.length;
			var ptrs:Array = CLibrary.method.CreateSceneData( uLen );
			var namePtr:uint, sourcePtr:uint;
			var namePtrs:Array = new Array;
			
			if(ptrs[1])
			{
				CLibrary.memory.position = ptrs[1];
				for(i=0; i<uLen; ++i)
				{
					nodeData = sceneData.sceneNodesDatas[i];
					CLibrary.memory.writeUnsignedInt( nodeData.parent );
					if(nodeData.name != null && nodeData.name.length)
					{
						sourcePtr = CLibrary.memory.position;
						namePtr = CLibrary.method.CreateBuffer( nodeData.name.length + 1 );
						CLibrary.memory.position = namePtr;
						CLibrary.memory.writeUTFBytes(nodeData.name);
						CLibrary.memory.writeByte(0);
						CLibrary.memory.position = sourcePtr;
						CLibrary.memory.writeUnsignedInt(namePtr);
					}
					else
						CLibrary.memory.writeUnsignedInt(0);
					
					CLibrary.memory.writeUnsignedInt( nodeData.name == null ? 0 : nodeData.name.length);
					
					CLibrary.memory.writeUnsignedInt( nodeData.type );
					
					CLibrary.memory.writeFloat( nodeData.transform.rawData[0] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[4] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[8] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[12] );
					
					CLibrary.memory.writeFloat( nodeData.transform.rawData[1] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[5] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[9] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[13] );
					
					CLibrary.memory.writeFloat( nodeData.transform.rawData[2] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[6] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[10] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[14] );
					
					CLibrary.memory.writeFloat( nodeData.transform.rawData[3] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[7] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[11] );
					CLibrary.memory.writeFloat( nodeData.transform.rawData[15] );
					
					CLibrary.memory.writeUnsignedInt( nodeData.meshData );
					CLibrary.memory.writeUnsignedInt( nodeData.subset );
					CLibrary.memory.writeUnsignedInt( nodeData.materialData );
					
					CLibrary.memory.writeUnsignedInt( nodeData.skinInfoHandle );
					//CLibrary.memory.writeUnsignedInt( nodeData.animController == null ? 0 : nodeData.animController.pointer );
					CLibrary.memory.writeUnsignedInt( nodeData.rootBone );
					
					
					CLibrary.memory.writeBoolean( nodeData.isUpdate );
					CLibrary.memory.position = CLibrary.memory.position + 3;
				}
			}
			
			var handle:uint = CLibrary.method.AddSceneDataToMeshManager( ptrs[0] );
			
			CLibrary.method.DestroySceneData( ptrs[0] );
			
			if(handle)
				_sceneDataMap[handle] = sceneData;
			
			return handle;
		}
		
		public function createPlaneMesh(
			sceneManager:CSceneManager, 
			width:Number, 
			height:Number,
			widthSegment:uint = 1,
			heightSegment:uint = 1,
			widthStep:uint = 1,
			heightStep:uint = 1):CMesh
		{
			CDebug.assert(sceneManager != null, "Scene manager is null.");
			
			var handle:uint = CLibrary.method.CreatePlaneFromMeshManager(sceneManager.pointer, width, height, widthSegment, heightSegment, widthStep, heightStep);
			
			if(handle)
				return sceneManager.getMesh(handle);
			
			return null;
		}
		
		public function createBillboardMeshData(
			width:Number,
			height:Number,
			billboardNum:uint, 
			vertexElements:Vector.<CVertexElementData>,
			vertexUsages:Vector.<uint>,
			indexUsage:uint
			):uint
		{
			if(vertexElements && vertexUsages && vertexUsages.length > 0 && vertexUsages.length == vertexElements.length)
			{
				var elementPtr:uint = CLibrary.method.CreateBuffer( vertexElements.length * CLibrary.INT_SIZE * 5 );
				var usagePtr:uint = CLibrary.method.CreateBuffer( vertexUsages.length * CLibrary.INT_SIZE );
				
				CLibrary.memory.position = elementPtr;
				var element:CVertexElementData;
				var i:uint, len:uint = vertexElements.length;
				for(i=0; i<len; ++i)
				{
					element = vertexElements[i];
					CLibrary.memory.writeUnsignedInt( element.stream );
					CLibrary.memory.writeUnsignedInt( element.offset );
					CLibrary.memory.writeInt( element.type );
					CLibrary.memory.writeInt( element.usage );
					CLibrary.memory.writeUnsignedInt(i);
				}
				
				CLibrary.memory.position = usagePtr;
				for(i=0; i<len; ++i)
					CLibrary.memory.writeUnsignedInt( vertexUsages[i] );
				
				var handle:uint = CLibrary.method.CreateBillboardMeshData(width, height, billboardNum, elementPtr, len, indexUsage, usagePtr );
				
				CLibrary.method.DestroyBuffer(elementPtr);
				CLibrary.method.DestroyBuffer(usagePtr);
				
				return handle;
			}
			
			return 0;
		}
		
		public function createMeshFromEffect( effect:uint, numVertices:uint, numIndices:uint, elementUsages:Vector.<CElementUsageData> = null, indexUsage:uint = 0):uint
		{
			var elementDataPtr:uint = 0;
			if(elementUsages)
			{
				elementDataPtr = CLibrary.method.CreateBuffer( CLibrary.INT_SIZE*4*elementUsages.length );
				
				CLibrary.memory.position = elementDataPtr;
				for(var i:int=0; i<elementUsages.length; ++i)
				{
					CLibrary.memory.writeUnsignedInt( elementUsages[i].elementType );
					CLibrary.memory.writeUnsignedInt( elementUsages[i].elementUsage );
					CLibrary.memory.writeUnsignedInt( elementUsages[i].uIndex );
					CLibrary.memory.writeUnsignedInt( elementUsages[i].uBufferUsage );
				}			
			}
			
			var handle:uint = CLibrary.method.CreateMeshFromEffect(effect, numVertices, numIndices, elementDataPtr, elementUsages ? elementUsages.length : 0, indexUsage);
			
			if(elementDataPtr)
				CLibrary.method.DestroyBuffer( elementDataPtr );
			
			return handle;
		}
		
		private function init():void
		{
			_pointer = CLibrary.method.CreateMeshManager();
			
			_meshDataMap = new Dictionary;
			_materialDataMap = new Dictionary;
			_sceneDataMap = new Dictionary;
			_textureDataMap = new Dictionary;
			_skinDataMap = new Dictionary;
		}
		
		private static var _canMeshManagerCreated:Boolean = true;
		private static var _meshManager:CMeshManager      = new CMeshManager();
	}
}