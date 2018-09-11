package alchemy.resource.extend 
{
	import alchemy.manager.CSceneManager;
	import alchemy.resource.CResource;
	import alchemy.tools.CDebug;

	public class CHighLevelResource extends CResource
	{
		public function get sceneManager():CSceneManager
		{
			return _sceneManager;
		}
		
		public function CHighLevelResource(pointer:uint, handle:uint, interfacePointer:uint, sceneManager:CSceneManager)
		{
			CDebug.assert(sceneManager != null                   , "Need sceneManager");
			CDebug.assert(sceneManager.canCreateHighLevelResource, "");
			
			super(pointer, handle, interfacePointer);
			
			_sceneManager = sceneManager;
		}
		
		public function onResourceCollect(handles:Vector.<uint>):void
		{
			
		}
		
		private var _sceneManager:CSceneManager;
	}
}