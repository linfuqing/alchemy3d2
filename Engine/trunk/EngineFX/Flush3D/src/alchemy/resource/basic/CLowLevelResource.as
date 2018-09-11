package alchemy.resource.basic 
{
	import alchemy.manager.CRenderSystem;
	import alchemy.resource.CResource;
	import alchemy.tools.CDebug;

	public class CLowLevelResource extends CResource
	{
		public function get renderSystem():CRenderSystem
		{
			return _renderSystem;
		}
		
		public function CLowLevelResource(pointer:uint, handle:uint, interfacePointer:uint, renderSystem:CRenderSystem)
		{
			CDebug.assert(renderSystem != null                  , "Need renderSystem.");
			CDebug.assert(renderSystem.canCreateLowLevelResource, "");

			super(pointer, handle, interfacePointer);
			
			_renderSystem = renderSystem;
		}
		
		private var _renderSystem:CRenderSystem;
	}
}