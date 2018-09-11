package alchemy.effect
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;

	public class CNormalMapEffect extends CAbstractEffectInfo
	{
		public function CNormalMapEffect( normalMapSampler:uint, uvIndex:uint, len:uint, lightIndex:Vector.<uint> = null)
		{
			var lightPointer:uint = 0;
			if(lightIndex && lightIndex.length > 0)
			{
				lightPointer = CLibrary.method.CreateBuffer( CLibrary.INT_SIZE*lightIndex.length );
				CLibrary.memory.position = lightPointer;
				
				for(var i:int=0; i<lightIndex.length; ++i)
					CLibrary.memory.writeUnsignedInt( lightIndex[i] );
			}
			
			var ptrs:Array = CLibrary.method.CreateNormalMapEffect( uvIndex, normalMapSampler, lightPointer ? lightIndex.length : len,  lightPointer);
			CDebug.assert(ptrs != null, "CNormalMapEffect::CNormalMapEffect() - error!");
			
			super(ptrs[0]);
			
			_hostPointer = ptrs[1];
			
			if(lightPointer)
				CLibrary.method.DestroyBuffer( lightPointer );
		}
		
		private var _hostPointer:uint;
	}
}