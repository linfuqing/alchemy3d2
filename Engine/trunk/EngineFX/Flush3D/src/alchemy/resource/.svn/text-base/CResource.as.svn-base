package alchemy.resource 
{
	import alchemy.core.CLibrary;
	import alchemy.core.CObjectInterface;
	import alchemy.core.CPointer;

	public class CResource extends CObjectInterface
	{
		public function get handle():uint
		{
			return _handle;
		}
		
		public function CResource(pointer:uint, handle:uint, interfacePointer:uint)
		{
			_pointer = pointer;
			_handle  = handle;
			
			super(interfacePointer);
		}
		
		public function rebuild():void
		{
			
		}
		
		private var _handle:uint;
	}
}