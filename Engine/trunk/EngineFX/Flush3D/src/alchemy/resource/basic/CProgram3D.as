package alchemy.resource.basic
{
	import alchemy.core.CLibrary;
	import alchemy.manager.CRenderSystem;
	import alchemy.tools.CDebug;
	
	import flash.display3D.Program3D;
	import flash.utils.ByteArray;

	public class CProgram3D extends CLowLevelResource
	{
		public function get instance():Program3D
		{
			return _program;
		}
		
		public function get vertexCode():ByteArray
		{
			return _vertexCode;
		}
		
		public function get fragmentCode():ByteArray
		{
			return _fragmentCode;
		}
		
		public function CProgram3D(pointer:uint, handle:uint, interfacePointer:uint, renderSystem:CRenderSystem)
		{
			super(pointer, handle, interfacePointer, renderSystem);
			
			init();
		}
		
		private function init():void
		{
			var pointers:Array = getPointers();
			_vertexCode        = pointers[0];
			_fragmentCode      = pointers[1];
			
			try
			{
				_program       = renderSystem.context.createProgram();
				
				_program.upload(_vertexCode, _fragmentCode);
			}
			catch(e:Error)
			{
				CDebug.error(e.message);
			}
		}
		
		private var _program:Program3D;
		private var _vertexCode:ByteArray;
		private var _fragmentCode:ByteArray;
	}
}