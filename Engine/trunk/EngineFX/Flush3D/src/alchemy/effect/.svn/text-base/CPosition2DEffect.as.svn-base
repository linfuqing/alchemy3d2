package alchemy.effect
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;

	public class CPosition2DEffect extends CAbstractEffectInfo
	{
		public function CPosition2DEffect()
		{
			var ptrs:Array = CLibrary.method.CreatePosition2DEffect();
			CDebug.assert(ptrs != null, "CPosition2DEffect - CreatePosition2DEffect() - error!");
			
			super(ptrs[0]);
			
			_hostPtr = ptrs[1];
		}
		
		private var _hostPtr:uint;
	}
}