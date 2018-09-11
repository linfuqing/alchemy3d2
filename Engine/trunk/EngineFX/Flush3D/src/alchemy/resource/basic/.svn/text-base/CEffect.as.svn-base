package alchemy.resource.basic
{
	import alchemy.core.CLibrary;
	import alchemy.manager.CRenderSystem;
	import alchemy.manager.CSceneManager;

	public class CEffect extends CLowLevelResource
	{
		public function CEffect(pointer:uint, handle:uint, interfacePointer:uint, renderSystem:CRenderSystem)
		{
			super(pointer, handle, interfacePointer, renderSystem);
			
			init();
		}
		
		public function setPass(pass:CProgram3D, index:uint):void
		{
			program[index] = pass;
			
			CLibrary.method.SetPassToEffect(_pointer, pass.pointer, index);
		}
		
		private function init():void
		{
			var pointers:Array = getPointers();
			
			program = new Vector.<CProgram3D>(pointers[0], true);
		}
		
		private var program:Vector.<CProgram3D>;
	}
}