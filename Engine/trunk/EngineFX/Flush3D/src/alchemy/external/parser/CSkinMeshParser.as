package alchemy.external.parser
{
//	import alchemy.animation.CAnimationSet;
//	import alchemy.animation.CSkinInfo;
//	import alchemy.animation.CSkinMeshController;
//	import alchemy.compilers.CAGALAssembler;
//	import alchemy.data.CSceneData;
//	import alchemy.data.CSceneNodeData;
//	import alchemy.data.CSceneObjectData;
//	import alchemy.data.animation.CSkinMeshData;
//	import alchemy.effect.CEffectCode;
//	import alchemy.manager.CRenderSystem;
//	import alchemy.manager.CSceneManager;
//	import alchemy.processor.CAbstractMeshProcessor;
//	import alchemy.processor.CNormalMeshProcessor;
//	import alchemy.processor.CSkinMehProcessor;
//	import alchemy.resource.basic.CEffect;
//	import alchemy.resource.basic.CTexture;
//	import alchemy.resource.extend.CMaterial;
//	import alchemy.resource.extend.CMesh;
//	import alchemy.resource.extend.CRenderMethod;
//	import alchemy.scene.CBone;
//	import alchemy.scene.CSceneNode;
//	import alchemy.scene.CSceneObject;
//	import alchemy.scene.ISceneContainer;
//	import alchemy.tools.CDebug;
//	
//	import flash.utils.Dictionary;
//	
//	
	public class CSkinMeshParser //implements ISceneDataParser
	{
		
//		static public const SKIN_SOFTWARE:uint = 1;
//		static public const SKIN_HARDWARE:uint = 2;
//		
//		public function CSkinMeshParser( skinType:uint = SKIN_HARDWARE )
//		{
//			_skinType = skinType;
//		}
//		
//		public function get rootNode():CSceneNode
//		{
//			return _rootNode;
//		}
//		
//		public function get dummyObjects():Vector.<CSceneObject>
//		{
//			return _roles;
//		}
//		
//		public function get animationSet():CAnimationSet
//		{
//			return _animSet;
//		}
//		
//		public function get staticNodes():Vector.<CSceneObject>
//		{
//			return _staticNodes;
//		}
//		
//		public function get skinNodes():Vector.<CSceneObject>
//		{
//			return _skinNodes;
//		}
//		
//		public function get skinMeshControlleres():Vector.<CSkinMeshController>
//		{
//			return _skinMeshControlleres;
//		}
//		
//		public function onParse(datas:CSceneData):Boolean
//		{
//			if(datas == null)
//				return false;	
//			
//			CDebug.assert(datas.skinMeshDataes.length > 0, "CSkinMeshParser::onParse(); error skin mesh format");
//			
//			var sceneManager:CSceneManager = CSceneManager.getInstance();
//			var renderSystem:CRenderSystem = sceneManager.renderSystem;
//			
//			var compiler:CAGALAssembler = new CAGALAssembler();
//			var effectCode:CEffectCode = compiler.createEffectCode();
//			var i:int=0, j:int=0;
//			var len:int;
//			
//			//mesh create
//			var meshs:Vector.<CMesh> = new Vector.<CMesh>;
//			var mesh:CMesh;
//			for(i=0; i<datas.meshDatas.length; ++i)
//			{
//				mesh = sceneManager.createMesh( datas.meshDatas[i] );
//				meshs.push( mesh );
//			}
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
//			//animationset create
//			if(datas.animationSetData != null)
//			{
//				_animSet = new CAnimationSet;
//				_animSet.name = datas.animationSetData.name;
//				_animSet.animationTracks = datas.animationSetData.animationTrackerDatas;
//			}
//			
//			var objDic:Dictionary = new Dictionary;
//			
//			//scene create
//			len = datas.sceneNodesDatas.length;
//			_roles = new Vector.<CSceneObject>;
//			//var skinFlag:uint = 0;
//			var sceneNodes:Vector.<CSceneNode> = new Vector.<CSceneNode>(len,true);
//			var sceneNode:CSceneNode;
//			var objectData:CSceneObjectData, nodeData:CSceneNodeData;
//			//var needUpdate:CSceneObject;
//			for(i=0; i<len; ++i)
//			{
//				nodeData = datas.sceneNodesDatas[i];
//				switch(nodeData.type)
//				{
//					case CSceneNodeData.NODE_TYPE:
//						sceneNode = new CSceneNode;
//						break;
//					case CSceneNodeData.BONE_TYPE:
//						sceneNode = new CBone;
//						break;
//					case CSceneNodeData.OBJECT_TYPE:
//						objectData = nodeData as CSceneObjectData;
//						sceneNode = sceneManager.createSceneObject( meshs[ objectData.meshData ].handle, objectData.subset, materials[ objectData.materialData ].handle, 0);
//						objDic[ sceneNode as CSceneObject ] = objectData;
//						j = _roles.push( sceneNode as CSceneObject );
////						if(objectData.needUpdate)
////							needUpdate = sceneNode as CSceneObject;
////						if( objectData.skinIndex != -1 )
////							skinFlag |= (1 << ( j-1 ));
//						break;
//					default:CDebug.assert(false,"unknow scenedata type!");
//				}
//				
//				sceneNode.id = nodeData.id;
//				sceneNode.name = nodeData.name;
//				sceneNode.transform = nodeData.transform;
//				sceneNodes[i] = sceneNode;
//			}
//			
//			for(i=0; i<len; ++i)
//			{
//				sceneNode = sceneNodes[i];
//				if(sceneNode && (j = datas.sceneNodesDatas[i].parent) != -1)
//					sceneNodes[j].addChild( sceneNode );
//				else if( datas.sceneNodesDatas[i].parent == -1 )
//					_rootNode = sceneNode;
//			}
//			
//			//skininfo create
//			//skinmeshController create
//			len = datas.skinMeshDataes.length;
//			_skinMeshControlleres = new Vector.<CSkinMeshController>;
//			var skinInfo:CSkinInfo;
//			var rootBone:CBone = sceneNodes[CSkinMeshData.s_rootBone] as CBone;
//			var skinmeshController:CSkinMeshController;
//			for(i=0; i<len; ++i)
//			{
//				skinInfo = new CSkinInfo;
//				skinInfo.bonesData = datas.skinMeshDataes[i].skinInfoDatas;
//				skinmeshController = new CSkinMeshController(meshs[ datas.skinMeshDataes[i].meshData ], _skinType, skinInfo, rootBone, effectCode);
//				_skinMeshControlleres.push(skinmeshController);
//			}
//			
//			_skinNodes = new Vector.<CSceneObject>;
//			_staticNodes = new Vector.<CSceneObject>;
//			
//			var processor:CAbstractMeshProcessor;
//			
//			len = _roles.length;
//			for(i=0; i<len; ++i)
//			{
//				objectData = objDic[ _roles[i] ];
//				if( objectData.skinIndex != -1 )
//				{
//					processor = new CSkinMehProcessor( _skinMeshControlleres[objectData.skinIndex], objectData.subset );
//					_roles[i].setMeshProcessor(processor, true);
//					_skinNodes.push( _roles[i] );
//				}
//				else
//				{
//					processor = new CNormalMeshProcessor( meshs[ objectData.meshData ].handle, objectData.subset );
//					_roles[i].setMeshProcessor(processor);
//					_staticNodes.push(_roles[i]);
//				}
//			}
//			
//			return true;
//		}
//		
//		public function onStaticObjectsRenderSet(renderMethod:CRenderMethod, effectHandle:uint):void
//		{
//			if(renderMethod || effectHandle)
//			{
//				for each(var obj:CSceneObject in _staticNodes)
//				{
//					if(renderMethod)
//						obj.setRenderMethod( renderMethod );
//					
//					if(effectHandle)
//						obj.setEffect(effectHandle);
//				}
//			}
//		}
//		
//		public function onSkinObjectsRenderSet(effectHandle:uint, renderMethod:CRenderMethod ):void
//		{
//			if(renderMethod || effectHandle)
//			{
//				for each(var obj:CSceneObject in _skinNodes)
//				{
//					if(renderMethod)
//						obj.setRenderMethod( renderMethod );
//					
//					if(effectHandle)
//						obj.setEffect(effectHandle);
//				}
//			}
//		}
//		
//		public function onAddToScene( sceneContainer:ISceneContainer, effectHandle:uint = 0, renderMethod:CRenderMethod = null):void
//		{
//			if(renderMethod || effectHandle)
//			{
//				for each(var obj:CSceneObject in _roles)
//				{
//					if(renderMethod)
//						obj.setRenderMethod( renderMethod );
//					
//					if(effectHandle)
//						obj.setEffect(effectHandle);
//				}
//			}
//			
//			if(sceneContainer)
//				sceneContainer.addChild( _rootNode );
//		}
//		
//		private var _skinType:uint;
//		private var _rootNode:CSceneNode;
//		private var _skinNodes:Vector.<CSceneObject>;
//		private var _staticNodes:Vector.<CSceneObject>;
//		private var _skinMeshControlleres:Vector.<CSkinMeshController>;
//		private var _roles:Vector.<CSceneObject>;
//		private var _animSet:CAnimationSet;
	}
}