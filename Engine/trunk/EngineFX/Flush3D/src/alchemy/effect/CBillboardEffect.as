package alchemy.effect
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;

	public class CBillboardEffect extends CAbstractEffectInfo
	{
		public function CBillboardEffect()
		{
			var ptrs:Array = CLibrary.method.CreateBillboardEffect();
			
			CDebug.assert( ptrs != null, "CBillboardEffect::CreateBillboardEffect() - Error!" );
			
			super( ptrs[0] );
			
			_hostPointer = ptrs[1];
		}
		
		private var _hostPointer:uint;
	}
}