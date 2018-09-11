package alchemy.effect.blendmethod
{
	import alchemy.core.CLibrary;

	public class CBidirectionalBlendMethod extends CAbstractBlendMethod
	{
		static public const POSITIVATE:uint = 0;
		static public const NEGATIVATE:uint = 1;
		
		public function CBidirectionalBlendMethod( method:uint )
		{
			var ptrs:Array = CLibrary.method.CreateBidirectionalBlendMethod(method);
			
			super(ptrs[0]);
			
			_sourcePtr = ptrs[1];
		}
		
		private var _sourcePtr:uint;
	}
}