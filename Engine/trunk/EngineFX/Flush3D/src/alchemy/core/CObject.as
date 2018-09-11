package alchemy.core
{
	public class CObject extends CObjectInterface
	{
		public function CObject(interfacePointer:uint)
		{
			super(interfacePointer);
			
			_objectPointer = interfacePointer;
		}
		
		protected function fill(...Method):void
		{
			CLibrary.method.FillObject(_objectPointer, this, Method);
		}
		
		private var _objectPointer:uint;
	}
}