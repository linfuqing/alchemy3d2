package alchemy.resource.extend
{
	import alchemy.manager.CSceneManager;
	import alchemy.resource.basic.CEffect;
	
	public class CRenderMethod extends CHighLevelResource
	{
		public function CRenderMethod(pointer:uint, handle:uint, interfacePointer:uint, sceneManager:CSceneManager)
		{
			super(pointer, handle, interfacePointer, sceneManager);
			
			_renderMethodInterfacePtr = super.getPointers()[0];
		}
		
		public function get renderMethodInterfacePointer():uint
		{
			return _renderMethodInterfacePtr;
		}
		
		private var _renderMethodInterfacePtr:uint;
	}
}