package alchemy.effect.postprocess
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	
	public class CAbstractPostProcessEffect extends CPointer
	{
		public function CAbstractPostProcessEffect( sourcePointer:uint, renderPointer:uint )
		{
			_pointer = sourcePointer;
			_renderPointer = renderPointer;
		}
		
		public function set visible( val:Boolean ):void
		{
			_visible = val;
			CLibrary.method.SetVisibleToRender(_renderPointer, _visible);
		}
		
		public function get visible():Boolean
		{
			return _visible;
		}
		
		protected var _visible:Boolean;
		protected var _renderPointer:uint;
	}
}