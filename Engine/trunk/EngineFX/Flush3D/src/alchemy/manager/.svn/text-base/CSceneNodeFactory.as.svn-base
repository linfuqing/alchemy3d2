package alchemy.manager
{
	import alchemy.core.CLibrary;
	import alchemy.core.CObject;
	import alchemy.scene.CBone;
	import alchemy.scene.CCamera;
	import alchemy.scene.CSceneNode;
	import alchemy.scene.CSceneObject;
	import alchemy.tools.CDebug;

	
	public class CSceneNodeFactory extends CAbstractResourcePool
	{
		public static function get instance():CSceneNodeFactory
		{
			return _sceneNodeFactory;
		}
		
		public function CSceneNodeFactory()
		{
			CDebug.assert(_canSceneNodeFactoryCreated, "Scene node factory is singleton.");
			
			var pointers:Array = CLibrary.method.CreateSceneNodeFactory();
			
			_pointer = pointers[0];

			super(pointers[1]);
			
			init();
			
			_canSceneNodeFactoryCreated = false;
		}
		
		public function getSceneNode(pointer:uint):CSceneNode
		{
			var length:uint = _sceneNodePool.length;
			for(var i:uint = 0; i < length; i ++)
				if(_sceneNodePool[i].pointer == pointer)
					return _sceneNodePool[i];
			
			return null;
		}
		
		private function init():void
		{
			fill(createSceneNode, destroySceneNode);
			
			_sceneNodePool = new Vector.<CSceneNode>();
		}
		
		private function createSceneNode(pointer:uint, interfacePointer:uint, sceneNodePointer:uint, type:int):void
		{
			lock(_sceneNodePool);
			switch(type)
			{
				case SCENE_NODE_TYPE_CAMERA:
					unlock( new CCamera(pointer, interfacePointer, sceneNodePointer) );
					break;
				
				case SCENE_NODE_TYPE_SCENE_OBJECT:
					unlock( new CSceneObject(pointer, interfacePointer, sceneNodePointer) );
					break;
				
				case SCENE_NODE_TYPE_BONE:
					unlock( new CBone(pointer, interfacePointer, sceneNodePointer) );
					break;
				
				default:
					unlock( new CSceneNode(pointer, interfacePointer, sceneNodePointer) );
			}
		}
		
		private function destroySceneNode(index:uint):void
		{
			_sceneNodePool[index] = null;
		}
		
		private var _sceneNodePool:Vector.<CSceneNode>;
		
		private static var _canSceneNodeFactoryCreated:Boolean = true;
		private static var _sceneNodeFactory:CSceneNodeFactory = new CSceneNodeFactory();
		
		private static const SCENE_NODE_TYPE_SCENE_NODE  :int = 0;
		private static const SCENE_NODE_TYPE_CAMERA      :int = 1;
		private static const SCENE_NODE_TYPE_SCENE_OBJECT:int = 2;
		private static const SCENE_NODE_TYPE_BONE        :int = 3;
	}
}