package alchemy.effect
{
	import alchemy.core.CLibrary;

	public class CGrassAnimationEffect extends CAbstractEffectInfo
	{
		public function set offset(offset:Number):void
		{
			CLibrary.memory.position = _offsetPtr;
			CLibrary.memory.writeFloat(offset);
		}
		
		public function CGrassAnimationEffect()
		{
			_offsetPtr = CLibrary.method.CreateBuffer(CLibrary.FLOAT_SIZE);
			
			var ptrs:Array = CLibrary.method.CreateGrassAnimationEffect(_offsetPtr);
			
			super(ptrs[0]);
			
			_hostPointer = ptrs[1];
		}
		
		private var _hostPointer:uint;
		private var _offsetPtr:uint;
	}
}