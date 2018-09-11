package alchemy.core
{
	public class CInterfacePointer extends CPointer
	{
		public function CInterfacePointer()
		{
			
		}
		
		public function get interfacePointer():uint
		{
			return _interfacePointer;
		}
		
		protected var _interfacePointer:uint;
	}
}