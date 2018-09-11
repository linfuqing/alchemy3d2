package alchemy.effect
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;

	public class CDepthEffect extends CAbstractEffectInfo
	{
		static public const PERPECTIVATE:uint = 0;
		static public const ORTHOGGONAL:uint  = 1;
		
		public function CDepthEffect(type:uint = 0)
		{
			var ptrs:Array = CLibrary.method.CreateDepthEffect(type);
			
			CDebug.assert(ptrs != null, "CDepthEffect::CreateDepthEffect() - failed");
			
			super(ptrs[0]);
			
			_interfacePtr = ptrs[1];
		}
		
		private var _interfacePtr:uint;
	}
}