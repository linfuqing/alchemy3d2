package alchemy.effect.particleshow
{
	import alchemy.core.CLibrary;
	import alchemy.effect.CAbstractEffectInfo;
	import alchemy.tools.CDebug;
	
	public class CPSMoveEffect extends CAbstractEffectInfo
	{
		public function CPSMoveEffect()
		{
			var ptrs:Array = CLibrary.method.CreatePSMoveEffect();
			
			CDebug.assert( ptrs != null, "PSMoveEffect::CreatePSMoveEffect() - Error!" );
			
			super( ptrs[0] );
			
			_hostPointer = ptrs[1];
		}
		
		private var _hostPointer:uint;
	}
}