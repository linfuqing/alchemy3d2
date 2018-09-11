package alchemy.effect
{
	import alchemy.core.CLibrary;

	public class CCartoonEffect extends CAbstractEffectInfo
	{
		public function CCartoonEffect(samplerIndex:uint = 0)
		{
			var pointers:Array = CLibrary.method.CreateCartoonEffect(samplerIndex);
			
			super(pointers[0]);
			
			_sourcePointer = pointers[1];
		}
		
		private var _sourcePointer:uint;
	}
}