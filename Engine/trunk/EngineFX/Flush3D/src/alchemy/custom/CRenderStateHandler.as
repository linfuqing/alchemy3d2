package alchemy.custom
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.data.CRenderStateData;
	
	public class CRenderStateHandler extends CPointer
	{
		public function CRenderStateHandler( renderStates:Vector.<CRenderStateData> )
		{
			if(renderStates && renderStates.length)
			{
				var dataPointer:uint = CLibrary.method.CreateBuffer( CLibrary.INT_SIZE*2*renderStates.length );
				
				CLibrary.memory.position = dataPointer;
				for(var i:uint=0; i<renderStates.length; ++i)
				{
					CLibrary.memory.writeUnsignedInt( renderStates[i].renderState );
					CLibrary.memory.writeUnsignedInt( renderStates[i].value );
				}
				
				_pointer = CLibrary.method.CreateRenderStateSet(dataPointer, renderStates.length);
				
				CLibrary.method.DestroyBuffer(dataPointer);
			}
		}
	}
}