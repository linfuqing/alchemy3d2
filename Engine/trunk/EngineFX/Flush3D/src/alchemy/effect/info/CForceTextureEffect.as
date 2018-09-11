package alchemy.effect.info
{
	import alchemy.core.CLibrary;
	import alchemy.effect.CAbstractEffectInfo;

	public class CForceTextureEffect extends CAbstractEffectInfo
	{
		public function CForceTextureEffect(samplerIndex:uint = 0)
		{
			var pointers:Array = CLibrary.method.CreateForceTextureEffect(samplerIndex);
			
			super(pointers[0]);
			
			_sourcePointer = pointers[1];
		}
		
		private var _sourcePointer:uint;
	}
}