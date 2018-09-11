package alchemy.core
{
	import alchemy.tools.CDebug;

	public class CObjectInterface extends CPointer
	{
		public function CObjectInterface(interfacePointer:uint)
		{
			super();
			
			_interfacePointer = interfacePointer;
		}
		
		public function getPointers():Array
		{
			return CLibrary.method.InitObject(_interfacePointer);
		}
		
		private var _interfacePointer:uint;
	}
}