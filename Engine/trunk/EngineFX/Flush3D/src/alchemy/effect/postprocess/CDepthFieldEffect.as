package alchemy.effect.postprocess
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;

	public class CDepthFieldEffect extends CAbstractPostProcessEffect
	{
		public function CDepthFieldEffect( fDistance:Number = 2.5, fBlur:Number = 4.0)
		{
			var ptrs:Array = CLibrary.method.CreateDepthFieldEffect(fDistance, fBlur);
			
			CDebug.assert(ptrs != null,"CDepthFieldEffect::Create fail");
			super( ptrs[0], 0 );
			
			_hostPointer = ptrs[1];
		}
		
		private var _hostPointer:uint;
	}
}