package alchemy.render
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.tools.CDebug;
	
	public class CAbstractProgramRender extends CAbstractRender
	{
		public function CAbstractProgramRender( programPt:uint = 0, renderPt:uint = 0, hostPt:uint = 0 )
		{
			super(renderPt, hostPt);
			
			_programPointer = programPt;
		}
		
		public function get programPointer():uint
		{
			return _programPointer;
		}
		
		protected var _programPointer:uint;
	}
}