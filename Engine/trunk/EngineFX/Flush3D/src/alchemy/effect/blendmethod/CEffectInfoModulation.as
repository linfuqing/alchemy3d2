package alchemy.effect.blendmethod
{
	import alchemy.core.CLibrary;

	public class CEffectInfoModulation extends CAbstractBlendMethod
	{
		static public const SOURCE_ALPHA:uint 	= 0;
		static public const DEST_ALPHA:uint 		= 1;
		static public const BLEND_ALPHA:uint 		= 2;
		
		public function CEffectInfoModulation( alphaFlag:uint )
		{
			var pointers:Array = CLibrary.method.CreateEffectInfoModulation(alphaFlag);
			
			super(pointers[0]);
			
			_sourcePointer = pointers[1];
		}
		
		private var _sourcePointer:uint;
	}
}