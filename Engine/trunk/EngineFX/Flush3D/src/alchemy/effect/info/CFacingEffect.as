package alchemy.effect.info
{
	import alchemy.core.CLibrary;
	import alchemy.effect.CAbstractEffectInfo;
	
	public class CFacingEffect extends CAbstractEffectInfo
	{
		public function CFacingEffect(isVertexShader:Boolean = true)
		{
			var pointers:Array = CLibrary.method.CreateFacingEffect(isVertexShader ? 1 : 0);
			
			super(pointers[0]);
			
			_sourcePointer = pointers[1];
		}
		
		private var _sourcePointer:uint;
	}
}