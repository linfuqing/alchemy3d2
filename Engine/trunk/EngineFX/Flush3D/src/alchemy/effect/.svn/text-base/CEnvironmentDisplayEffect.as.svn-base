package alchemy.effect
{
	import alchemy.core.CLibrary;

	public class CEnvironmentDisplayEffect extends CAbstractEffectInfo
	{
		public function CEnvironmentDisplayEffect()
		{
			var pointers:Array = CLibrary.method.CreateEnvironmentDisplayEffect();
			
			super(pointers[0]); 
			
			_sourcePointer = pointers[1];
		}
		
		private var _sourcePointer:uint;
	}
}