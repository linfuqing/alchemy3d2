package alchemy.effect
{
	import alchemy.core.CLibrary;

	public class CEmptyEffect extends CAbstractEffectInfo
	{
		public function CEmptyEffect()
		{
			var ptrs:Array = CLibrary.method.CreateEmptyEffect();
			
			super(ptrs[0]);
			
			_host = ptrs[1];
		}
		
		private var _host:uint;
	}
}