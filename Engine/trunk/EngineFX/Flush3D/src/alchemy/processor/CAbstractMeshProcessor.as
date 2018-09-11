package alchemy.processor
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	
	public class CAbstractMeshProcessor extends CPointer
	{	
		public function CAbstractMeshProcessor( hostPtr:uint, processorPtr:uint )
		{
			_pointer = hostPtr;
			_processorPointer = processorPtr;
		}
		
		public function get processorPointer():uint
		{
			return _processorPointer;
		}
		
		public function get mesh():uint
		{
			return _processorPointer ? CLibrary.method.GetMeshFromProcessor(_processorPointer) : 0;
		}
		
		protected var _processorPointer:int;
	}
}