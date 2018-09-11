package alchemy.deferred
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.tools.CDebug;
	
	public class CDeferredLightRenderer extends CPointer
	{
		public function CDeferredLightRenderer(AORandomMap:uint = 0, AONumRoatationMap:uint = 0, AOStep:uint = 0, outputRenderTarget:uint = 0)
		{
			_pointer = CLibrary.method.CreateDeferredLightRenderer(AORandomMap, AONumRoatationMap, AOStep, outputRenderTarget);
			
			CDebug.assert(_pointer != 0, "CDeferredLightRenderer has been created error! ");
			
			super();
		}
		
		public function applyForRender():void
		{
			CLibrary.method.ApplyForRenderFromDeferredLightRenderer(_pointer);
		}
	}
}