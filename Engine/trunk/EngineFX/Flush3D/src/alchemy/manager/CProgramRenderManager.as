package alchemy.manager
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.render.CAbstractProgramRender;
	import alchemy.tools.CDebug;
	
	public class CProgramRenderManager extends CPointer
	{
		static private var _programRenderManager:CProgramRenderManager;
		
		static public function get instance():CProgramRenderManager
		{
			if(_programRenderManager == null)
				_programRenderManager = new CProgramRenderManager();
			
			return _programRenderManager;
		}
		
		public function CProgramRenderManager()
		{
			CDebug.assert(_programRenderManager == null, "CProgramRenderManager() - Single Instance");
			_pointer = CLibrary.method.CreateProgramRenderManager();
		}
		
		public function addProgramRender( programRender:CAbstractProgramRender ):void
		{
			if(_pointer && programRender)
				CLibrary.method.AddProgramRenderToManager(programRender.programPointer);
		}
		
		public function update():void
		{
			if(_pointer)
				CLibrary.method.UpdateProgramRenderManager();
		}
	}
}