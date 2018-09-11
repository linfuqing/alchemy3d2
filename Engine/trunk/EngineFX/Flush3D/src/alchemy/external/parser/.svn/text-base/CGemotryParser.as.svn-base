package alchemy.external.parser
{
//	import alchemy.compilers.CAGALAssembler;
//	import alchemy.data.CMeshData;
//	import alchemy.data.CMeshSegmentData;
//	import alchemy.data.CSceneData;
//	import alchemy.data.CSceneNodeData;
//	import alchemy.data.CSceneObjectData;
//	import alchemy.data.CVertexBufferData;
//	import alchemy.data.CVertexElementData;
//	import alchemy.manager.CRenderSystem;
//	import alchemy.manager.CSceneManager;
//	import alchemy.processor.CNormalMeshProcessor;
//	import alchemy.resource.basic.CTexture;
//	import alchemy.resource.extend.CMaterial;
//	import alchemy.resource.extend.CMesh;
//	import alchemy.resource.extend.CRenderMethod;
//	import alchemy.scene.CSceneObject;
//	import alchemy.scene.ISceneContainer;
//	import alchemy.tools.CDebug;
//	import alchemy.tools.CMath;
//	
//	import flash.utils.Dictionary;
//	
	public class CGemotryParser //implements ISceneDataParser
	{
//		public function CGemotryParser()
//		{
//			
//		}
//		
//		public function get dummyObjects():Vector.<CSceneObject>
//		{
//			return _dummyNodes;
//		}
//		
//		public function getMaterialByName( name:String ):CMaterial
//		{
//			return _mateiralLib[name];
//		}
//		
//		public function getObjectsByMaterialName( name:String ):Vector.<CSceneObject>
//		{
//			return _materialObjects[name];
//		}
//		
//		public function onParse(datas:CSceneData):Boolean
//		{
//			CDebug.assert(datas != null, "CGemotryParser::onParse sceneDatas is null!");
//			
//			
//			var sceneManager:CSceneManager = CSceneManager.getInstance();
//			var renderSystem:CRenderSystem = sceneManager.renderSystem;
//			
//			var i:int=0, j:int=0;
//			var len:int;
//			
//			//texture create
//			var textures:Vector.<CTexture> = new Vector.<CTexture>;
//			for(i=0 ; i<datas.textureDatas.length; ++i)
//				textures.push( renderSystem.createTextureWithData( datas.textureDatas[i] ) );
//			
//			//material create
//			var materials:Vector.<CMaterial> = new Vector.<CMaterial>;
//			_mateiralLib = new Dictionary;
//			_materialObjects = new Dictionary;
//			var material:CMaterial;
//			for(i=0; i<datas.materialDatas.length; ++i)
//			{
//				material = sceneManager.createMaterial(datas.materialDatas[i]);
//				
//				for(j=0; j<datas.materialDatas[i].textureDataHandles.length; ++j)
//					material.addTexture( textures[ datas.materialDatas[i].textureDataHandles[j] ] );
//				
//				materials.push( material );
//				
//				_mateiralLib[ datas.materialDatas[i].name ] = material;
//			}
//			
//			
//			//scene create
//			len = datas.sceneNodesDatas.length;
//			var nodeData:CSceneNodeData;
//			var ojectData:CSceneObjectData;
//			var meshFlag:uint = 0;
//			var meshData:CMeshData;
//			
//			var textureMap:Vector.< Vector.<CSceneObjectData> > = new Vector.< Vector.<CSceneObjectData> >(datas.materialDatas.length, true);
//			var meshFlags:Vector.<Boolean> = new Vector.<Boolean>( datas.meshDatas.length,  true);
//			
//			var staticMeshData:CMeshData ;
//			
//			for(i=0; i<len; ++i)
//			{
//				nodeData = datas.sceneNodesDatas[i];
//				if(nodeData.type == CSceneNodeData.OBJECT_TYPE)
//				{
//					ojectData = nodeData as CSceneObjectData;
//					
//					while(ojectData.parent != -1)
//					{
//						ojectData.transform.append( datas.sceneNodesDatas[ ojectData.parent ].transform );
//						ojectData.parent =  datas.sceneNodesDatas[ojectData.parent].parent;
//					}
//					
//					if( ! meshFlags[ ojectData.meshData ] )
//					{
//						meshData = datas.meshDatas[ ojectData.meshData ];
//						
//						if(staticMeshData == null)
//						{
//							staticMeshData = new CMeshData;
//							staticMeshData.indexUsage = meshData.indexUsage;
//							var elementData:CVertexElementData;
//							var vertexBufData:CVertexBufferData;
//							for(j=0; j<meshData.vertexDeclaration.length; ++j)
//							{
//								elementData = new CVertexElementData;
//								elementData.offset = meshData.vertexDeclaration[j].offset;
//								elementData.stream = meshData.vertexDeclaration[j].stream;
//								elementData.type = meshData.vertexDeclaration[j].type;
//								elementData.usage = meshData.vertexDeclaration[j].usage;
//								staticMeshData.vertexDeclaration.push(elementData);
//								
//								vertexBufData = new CVertexBufferData;
//								vertexBufData.usage = meshData.vertexData[j].usage;
//								
//								staticMeshData.vertexData.push( vertexBufData );
//							}
//							
//							var attribute:CMeshSegmentData = new CMeshSegmentData;
//							attribute.startIndex = attribute.indexCount = 0;
//							staticMeshData.attributeRange.push( attribute );
//						}
//						
//						for each(var element:CVertexElementData in  meshData.vertexDeclaration )
//						{
//							if(element.usage == CVertexElementData.ELEMENT_USAGE_POSITION && element.type == CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT4)
//								CMath.leftMulTransformByteArray(ojectData.transform, meshData.vertexData[ element.stream ].data, meshData.vertexData[ element.stream ].data, 4);
//							else if(element.usage == CVertexElementData.ELEMENT_USAGE_NORMAL && element.type == CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT3)
//								CMath.leftMulTransformByteArray(ojectData.transform, meshData.vertexData[ element.stream ].data, meshData.vertexData[ element.stream ].data, 3, true);
//						}
//						
//						meshFlags[ ojectData.meshData ] = true;
//					}
//					
//					if(textureMap[ojectData.materialData] == null)
//						textureMap[ojectData.materialData] = new Vector.<CSceneObjectData>;
//					
//					textureMap[ojectData.materialData].push( ojectData );
//				}
//			}
//			
//			CDebug.assert(staticMeshData != null, "CGemotryParser::onParse() - no mesh data?");
//			
//			//gemotryinstance combine
//			var vecTextureObjects:Vector.<CSceneObjectData>;
//			var curMeshData:CMeshData;
//			var lastMeshData:CMeshData;
//			var leaveVertices:int, leaveIndices:int;
//			var curObjectsLen:int;
//			
//			len = textureMap.length;
//			_dummyNodes = new Vector.<CSceneObject>;
//			var curObjects:Vector.<CSceneObject>;
//			
//			for (i=0; i<len; ++i)
//			{
//				if( !(vecTextureObjects = textureMap[i]) )
//					continue;
//				
//				_initMeshData( staticMeshData );
//				curObjects = new Vector.<CSceneObject>;
//				_materialObjects[ datas.materialDatas[i].name ] = curObjects;
//				curObjectsLen = vecTextureObjects.length;
//				for(j=0; j<curObjectsLen; ++j)
//				{
//					curMeshData =  datas.meshDatas[ vecTextureObjects[j].meshData ];
//					if(curMeshData != lastMeshData)
//					{
//						staticMeshData.numVertices += curMeshData.numVertices;
//						staticMeshData.numIndices += curMeshData.numIndices;
//						
//						leaveVertices = staticMeshData.numVertices - 65535;
//						leaveIndices = staticMeshData.numIndices - 65535;
//						
//						if(leaveVertices >= 0 || leaveIndices >= 0)
//						{
//							if(leaveVertices >= 0)
//							{
//								staticMeshData.numVertices = 65535;
//								_writeMeshData(curMeshData, staticMeshData, 0, 65535);
//							}
//							
//							if(leaveIndices >= 0)
//							{
//								staticMeshData.numIndices = 65535;
//								_writeIndexData(curMeshData, staticMeshData, 0, 65535);
//							}
//							
//							_onCompleteGemotry(staticMeshData, materials[i].handle, sceneManager, curObjects);
//							
//							_initMeshData(staticMeshData);
//							
//							if(leaveVertices >= 0)
//							{
//								staticMeshData.numVertices = leaveVertices;
//								_writeMeshData(curMeshData, staticMeshData, 65535, leaveVertices);
//							}
//							
//							if(leaveIndices >= 0)
//							{
//								staticMeshData.numIndices = leaveIndices;
//								_writeIndexData(curMeshData, staticMeshData,  65535, leaveIndices);
//							}
//							continue;
//						}
//						
//						_writeMeshData(curMeshData, staticMeshData, 0, curMeshData.numVertices);
//						_writeIndexData(curMeshData, staticMeshData, 0, curMeshData.numIndices);
//						
//						lastMeshData = curMeshData;
//					}
//				}
//				
//				_onCompleteGemotry(staticMeshData, materials[i].handle, sceneManager, curObjects);
//			}
//			
//			return true;
//		}
//		
//		public function onAddToScene( sceneContainer:ISceneContainer, effectHandle:uint = 0, renderMethod:CRenderMethod = null ):void
//		{
//			if(sceneContainer && _dummyNodes)
//			{
//				for each(var object:CSceneObject in _dummyNodes)
//				{
//					if(renderMethod)
//						object.setRenderMethod( renderMethod );
//					if(effectHandle)
//						object.setEffect( effectHandle );
//					sceneContainer.addChild( object );
//				}
//			}
//		}
//		
//		private function _initMeshData(data:CMeshData):void
//		{
//			data.numIndices = data.numVertices = 0;
//			data.indexData.clear();
//			data.attributeRange[0].startIndex = data.attributeRange[0].indexCount = 0;
//			
//			for each(var vertexData:CVertexBufferData in data.vertexData)
//				vertexData.data.clear();
//		}
//		
//		protected function _onCompleteGemotry(data:CMeshData, material:uint, sceneManager:CSceneManager, groupObjects:Vector.<CSceneObject> ):void
//		{
//			data.attributeRange[0].indexCount = data.numIndices;
//			var mesh:CMesh = sceneManager.createMesh(data);
//			var sceneObject:CSceneObject = sceneManager.createSceneObject(mesh.handle, 0, material, 0);
//			sceneObject.setMeshProcessor( new CNormalMeshProcessor(mesh.handle, 0) );
//			_dummyNodes.push( sceneObject );
//			groupObjects.push(sceneObject);
//		}
//		
//		protected function _writeIndexData(sourceData:CMeshData, destData:CMeshData, offset:uint, length:uint):void
//		{
//			var index:int;
//			var curOffset:uint = destData.numVertices - sourceData.numVertices;
//			sourceData.indexData.position = offset*2;
//			for(var i:int=0; i<length; ++i)
//			{
//				index = sourceData.indexData.readShort();
//				index += curOffset;
//				destData.indexData.writeShort( index );
//			}
//		}
//		
//		protected function _writeMeshData(sourceData:CMeshData, destData:CMeshData, offset:uint, length:uint):void
//		{
//			var i:int=0;
//			var srcVertexBufData:CVertexBufferData;
//			var dstVertexBufData:CVertexBufferData;
//			var vertexElement:CVertexElementData;
//			var len:int = sourceData.vertexDeclaration.length < destData.vertexDeclaration.length ? sourceData.vertexDeclaration.length : destData.vertexDeclaration.length;
//			
//			for( i=0; i<len; ++i)
//			{
//				vertexElement = sourceData.vertexDeclaration[i];
//				srcVertexBufData = sourceData.vertexData[ vertexElement.stream ];
//				
//				dstVertexBufData = destData.vertexData[ vertexElement.stream ];
//				
//				dstVertexBufData.data.writeBytes(srcVertexBufData.data, offset * vertexElement.type, length * vertexElement.type);
//			}
//		}
//		
//		private var _materialObjects:Dictionary;
//		private var _mateiralLib:Dictionary;
//		private var _dummyNodes:Vector.<CSceneObject>;
	}
}