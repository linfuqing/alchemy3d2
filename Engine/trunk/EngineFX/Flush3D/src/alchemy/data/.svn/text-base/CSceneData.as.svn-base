package alchemy.data
{
	import alchemy.data.animation.CAnimationSetData;
	import alchemy.data.animation.CSkinMeshData;
	import alchemy.tools.CDebug;
	
	import flash.utils.Dictionary;

	public class CSceneData extends CSafeData
	{
		//public var animationSetData:CAnimationSetData;
		
		public function CSceneData()
		{
			super();
			
//			_meshDatas = new Vector.<CMeshData>;
//			_textureDatas = new Vector.<CTextureData>;
//			_materialDatas = new Vector.<CMaterialData>;
			_sceneNodesDatas = new Vector.<CSceneNodeData>;
//			_skinMeshDataes = new Vector.<CSkinMeshData>;
		}
		
//		public function get skinMeshDataes():Vector.<CSkinMeshData>
//		{
//			return _skinMeshDataes;
//		}
//		
		public function get sceneNodesDatas():Vector.<CSceneNodeData>
		{
			return _sceneNodesDatas;
		}
//		
//		public function get materialDatas():Vector.<CMaterialData>
//		{
//			return _materialDatas;
//		}
//		
//		public function get meshDatas():Vector.<CMeshData>
//		{
//			return _meshDatas;
//		}
//		
//		public function get textureDatas():Vector.<CTextureData>
//		{
//			return _textureDatas;
//		}
		
		public function findChildByName(name:String):CSceneNodeData
		{
			for each(var child:CSceneNodeData in _sceneNodesDatas)
			{
				if(child.name == name)
					return child;
			}
			
			return null;
		}
		
		public function addChild(parent:CSceneNodeData, child:CSceneNodeData):Boolean
		{
			var index:int;
			if(parent && (index = _sceneNodesDatas.indexOf(parent)) != -1)
				child.parent = index;
			else
				return false;
			
			return true;
		}
		
		public function createNodeData(type:uint, parent:CSceneNodeData = null, isAdd:Boolean = true):CSceneNodeData
		{
			var sceneNode:CSceneNodeData = new CSceneNodeData;
//			switch(type)
//			{
//				case CSceneNodeData.NODE_TYPE:
//				case CSceneNodeData.BONE_TYPE:
//					sceneNode = new CSceneNodeData;
//					break;
//				case CSceneNodeData.OBJECT_TYPE:
//					sceneNode = new CSceneObjectData;
//					break;
//				default:
//					CDebug.assert(false,"CSceneData::createNodeData() -- unknon type");
//					return null;
//			}
		
			sceneNode.type = type;
			var index:int;
			if(parent && (index = _sceneNodesDatas.indexOf(parent)) != -1)
				sceneNode.parent = index+1;
			
			if(isAdd)
				_sceneNodesDatas.push( sceneNode );
			
			return sceneNode;
		}
	
		//protected var _meshDatas:Vector.<CMeshData>;
		//protected var _textureDatas:Vector.<CTextureData>;
		//protected var _materialDatas:Vector.<CMaterialData>;
		protected var _sceneNodesDatas:Vector.<CSceneNodeData>;
		//protected var _skinMeshDataes:Vector.<CSkinMeshData>;
	}
}