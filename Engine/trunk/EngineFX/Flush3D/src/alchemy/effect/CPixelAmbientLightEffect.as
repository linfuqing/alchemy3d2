package alchemy.effect
{
	import alchemy.core.CLibrary;

	public class CPixelAmbientLightEffect extends CAbstractEffectInfo
	{
		public function CPixelAmbientLightEffect()
		{
			var pointers:Array = CLibrary.method.CreatePixelAmbientLightEffect();
			
			super(pointers[0]);
			
			_sourcePointer = pointers[1];
		}
		
		private var _sourcePointer:uint;
	}
}