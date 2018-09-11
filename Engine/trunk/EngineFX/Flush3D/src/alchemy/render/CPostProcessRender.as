package alchemy.render
{
	import alchemy.constant.CRenderType;
	import alchemy.core.CLibrary;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.tools.CDebug;

	public class CPostProcessRender extends CAbstractRender
	{
		public function CPostProcessRender( renderPt:uint = 0, hostPt:uint = 0 )
		{
			super(renderPt, hostPt);
		}
		
		static public function createBeforeRender(material:int, renderMethod:CRenderMethod, effect:int, renderPass:uint = 0,
													outputRenderTarget:int = 0, screenMesh:int = 0, renderType:int = CRenderType.RENDER_BEFOREHAND):CPostProcessRender
		{
			if(!renderMethod)
				return null;
			
			var ptrs:Array = CLibrary.method.CreateBeforePostProcessRender(material, renderMethod.pointer, effect, renderPass, outputRenderTarget, screenMesh, renderType);
			
			CDebug.assert(ptrs != null, "createBeforeRender() - error!");
			if(!ptrs)
				return null;
			
			return new CPostProcessRender(ptrs[1], ptrs[0]);
		}
		
		static public function createSceneRender(material:int, renderIndex:uint, renderMethod:CRenderMethod, effect:int, renderPass:uint = 0,
												 outputRenderTarget:int = 0, screenMesh:int = 0, renderType:int = CRenderType.RENDER_BEFOREHAND):CPostProcessRender
		{
			if(!renderMethod)
				return null;
			
			var ptrs:Array = CLibrary.method.CreateScenePostProcessRender(material, renderIndex, renderMethod.pointer, effect, renderPass, outputRenderTarget, screenMesh, renderType);
			
			CDebug.assert(ptrs != null, "createSceneRender() - error!");
			if(!ptrs)
				return null;
			
			return new CPostProcessRender(ptrs[1], ptrs[0]);
		}
	}
}