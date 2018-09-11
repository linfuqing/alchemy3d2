package alchemy.effect
{
	import alchemy.core.CLibrary;

	public class CPositionEffect extends CAbstractEffectInfo
	{
		public function CPositionEffect()
		{
			var pointers:Array = CLibrary.method.CreatePositionEffect();
			
			super(pointers[1]);
			
			_sourcePointer = pointers[0];
		}
		
		private var _sourcePointer:uint;
	}
}