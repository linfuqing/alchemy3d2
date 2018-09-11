package alchemy.effect
{
	import alchemy.core.CLibrary;

	public class CVertexColorEffect extends CAbstractEffectInfo
	{
		public function CVertexColorEffect( index:uint )
		{
			var ptrs:Array = CLibrary.method.CreateVertexColorEffect(index);
			
			super(ptrs[0]);
			
			_hostPointer = ptrs[1];
		}
		
		private var _hostPointer:uint;
	}
}