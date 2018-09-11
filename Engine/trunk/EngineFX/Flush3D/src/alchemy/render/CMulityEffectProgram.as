package alchemy.render
{
	import alchemy.core.CLibrary;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.scene.CSceneNode;
	import alchemy.tools.CDebug;

	public class CMulityEffectProgram extends CAbstractRender
	{
		public function CMulityEffectProgram(color:uint, clearFlag:uint, renderTarget:uint, cameraIndex:uint = 0)
		{
			var ptrs:Array = CLibrary.method.CreateMulityEffectProgram(color, clearFlag, renderTarget, cameraIndex);
			CDebug.assert(ptrs != null, "CLibrary.method.CreateMulityEffectProgram() - error!");
			
			_pointer = ptrs[0];
			_renderPointer = ptrs[1];
		}
		
		public function addRenderObject(node:CSceneNode, effect:uint, renderMethod:CRenderMethod, renderPass:uint = 0, material:uint = 0):void
		{
			if(_pointer)
				CLibrary.method.AddRenderObjectToProgram(_pointer, node.pointer, effect, renderMethod.pointer, renderPass, material);
		}
		
		public function addRenderObjectSelf(node:CSceneNode, renderPass:uint = 0):void
		{
			if(_pointer)
				CLibrary.method.AddRenderObjectSelfToProgram(_pointer, node ? node.pointer : null, renderPass);
		}
	}
}