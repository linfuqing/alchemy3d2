package alchemy.processor
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;

	public class CNormalMeshProcessor extends CAbstractMeshProcessor
	{
		public function CNormalMeshProcessor( sourcePointer:uint = 0, processorPointer:uint = 0 )
		{
			if(!sourcePointer && !processorPointer)
			{
				var ptrs:Array = CLibrary.method.CreateNormalMeshProcessor();
				CDebug.assert( ptrs != null, "CNormalMeshProcessor::CreateNormalMeshProcessor ERROR!");
				sourcePointer = ptrs[0];
				processorPointer = ptrs[1];
			}
			
			super(sourcePointer, processorPointer);
		}
	}
}