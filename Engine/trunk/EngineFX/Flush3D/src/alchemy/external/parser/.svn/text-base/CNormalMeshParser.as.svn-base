package alchemy.external.parser
{
//	import alchemy.data.CMeshData;
//	import alchemy.data.CSceneData;
//	import alchemy.data.CSceneNodeData;
//	import alchemy.data.CSceneObjectData;
//	import alchemy.data.CVertexElementData;
//	import alchemy.manager.CRenderSystem;
//	import alchemy.manager.CSceneManager;
//	import alchemy.processor.CAbstractMeshProcessor;
//	import alchemy.processor.CNormalMeshProcessor;
//	import alchemy.resource.basic.CTexture;
//	import alchemy.resource.extend.CMaterial;
//	import alchemy.resource.extend.CMesh;
//	import alchemy.resource.extend.CRenderMethod;
//	import alchemy.scene.CSceneNode;
//	import alchemy.scene.CSceneObject;
//	import alchemy.scene.ISceneContainer;
//	import alchemy.tools.CDebug;
//	import alchemy.tools.CMath;
//	
	public class CNormalMeshParser //implements ISceneDataParser
	{
//		public function CNormalMeshParser()
//		{
//			
//		}
//		
//		public function get dummyObjects():Vector.<CSceneObject>
//		{
//			return _dummyObjects;
//		}
//		
//		public function onParse(datas:CSceneData):Boolean
//		{
//			if(datas == null)
//				return false;
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
//			var material:CMaterial;
//			for(i=0; i<datas.materialDatas.length; ++i)
//			{
//				material = sceneManager.createMaterial(datas.materialDatas[i]);
//				
//				for(j=0; j<datas.materialDatas[i].textureDataHandles.length; ++j)
//					material.addTexture( textures[ datas.materialDatas[i].textureDataHandles[j] ] );
//				
//				materials.push( material );
//			}
//			
//			
//			//scene create
//			_dummyObjects = new Vector.<CSceneObject>;
//			len = datas.sceneNodesDatas.length;
//			
//			var nodeData:CSceneNodeData;
//			var ojectData:CSceneObjectData;
//			var meshData:CMeshData;
//			
//			var meshs:Vector.<CMesh> = new Vector.<CMesh>(datas.meshDatas.length, true);
//			
//			var sceneObj:CSceneObject;
//			
//			var processor:CAbstractMeshProcessor;
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
//					if( ! meshs[ ojectData.meshData ] )
//					{
//						meshData = datas.meshDatas[ ojectData.meshData ];
//						
//						for each(var element:CVertexElementData in  meshData.vertexDeclaration )
//						{
//							if(element.usage == CVertexElementData.ELEMENT_USAGE_POSITION && element.type == CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT4)
//								CMath.leftMulTransformByteArray(ojectData.transform, meshData.vertexData[ element.stream ].data, meshData.vertexData[ element.stream ].data, 4);
//							else if(element.usage == CVertexElementData.ELEMENT_USAGE_NORMAL && element.type == CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT3)
//								CMath.leftMulTransformByteArray(ojectData.transform, meshData.vertexData[ element.stream ].data, meshData.vertexData[ element.stream ].data, 3, true);
//						}
//						
//						meshs[ ojectData.meshData ] = sceneManager.createMesh( meshData );
//						
//						CDebug.assert(meshs[ ojectData.meshData ] != null, "create mesh error!");
//					}
//					
//					sceneObj = sceneManager.createSceneObject( meshs[ojectData.meshData].handle, ojectData.subset, materials[ ojectData.materialData ].handle, 0 );
//					
//					sceneObj.setMeshProcessor( processor = new CNormalMeshProcessor( meshs[ojectData.meshData].handle, ojectData.subset ) );
//					
//					_dummyObjects.push( sceneObj );
//				}
//			}
//			
//			return true;
//		}
//		
//		public function onAddToScene( sceneContainer:ISceneContainer , effectHandle:uint = 0, renderMethod:CRenderMethod = null):void
//		{
//			if(sceneContainer && _dummyObjects)
//			{
//				for each(var object:CSceneObject in _dummyObjects)
//				{
//					if(renderMethod)
//						object.setRenderMethod( renderMethod );
//					if(effectHandle)
//						object.setEffect(effectHandle);
//					sceneContainer.addChild( object );
//				}
//			}
//		}
//		
//		private var _dummyObjects:Vector.<CSceneObject>;
	}
}