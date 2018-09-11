package alchemy.effect
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	
	public class COutLineEffect extends CAbstractEffectInfo
	{
		public function COutLineEffect()
		{
			var pointers:Array = CLibrary.method.CreateOutlineEffect();
			
			super(pointers[0]);
			
			_sourcePointer = pointers[1];
		}
		
		private var _sourcePointer:uint;
	}
}