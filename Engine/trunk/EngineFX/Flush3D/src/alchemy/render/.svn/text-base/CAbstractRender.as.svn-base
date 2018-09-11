package alchemy.render
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	
	public class CAbstractRender extends CPointer
	{
		public function CAbstractRender( renderPt:uint = 0, hostPt:uint = 0)
		{
			_pointer = hostPt;
			_renderPointer = renderPt;
		}
		
		public function set visible( val:Boolean ):void
		{
			if(_renderPointer)
				CLibrary.method.SetVisibleToRender(_renderPointer, val);
		}
		
		public function get renderPointer():uint
		{
			return _renderPointer;
		}
		
		protected var _renderPointer:uint;
	}
}