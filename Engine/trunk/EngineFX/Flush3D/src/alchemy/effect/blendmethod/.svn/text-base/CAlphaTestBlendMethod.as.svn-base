package alchemy.effect.blendmethod
{
	import alchemy.core.CLibrary;

	public class CAlphaTestBlendMethod extends CAbstractBlendMethod
	{
		public function CAlphaTestBlendMethod(  )
		{
			var ptrs:Array = CLibrary.method.CreateAlphaTestBlend();
			
			super(ptrs[0]);
			
			_host = ptrs[1];
		}
		
		private var _host:uint;
	}
}