package alchemy.effect.postprocess
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;

	public class CContortEffect extends CAbstractPostProcessEffect
	{
		public function CContortEffect(radius:Number = 0.01, scale:Number = 10.0)
		{
			var ptrs:Array = CLibrary.method.CreateContorEffect(radius, scale);
			
			CDebug.assert( ptrs != null, "CContortEffect::Create Error!" );
			
			super( ptrs[0], 0 );
			
			_hostPointer = ptrs[1];
		}
		
		private var _hostPointer:uint;
	}
}