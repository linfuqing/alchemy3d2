package alchemy.effect.blendmethod
{
	import alchemy.core.CLibrary;
	
	public class CFresnelBlendMethod extends CAbstractBlendMethod
	{
		public function CFresnelBlendMethod()
		{
			var pointers:Array = CLibrary.method.CreateFresnelBlendMethod();
			
			super(pointers[0]);
			
			_sourcePointer = pointers[1];
		}
		
		private var _sourcePointer:uint;
	}
}