package alchemy.render
{
	import alchemy.core.CLibrary;
	import alchemy.scene.CSceneNode;
	import alchemy.tools.CDebug;

	public class CEnvironmentRender extends CAbstractRender
	{
		public function CEnvironmentRender( uTextureSize:uint, sceneNode:CSceneNode )
		{
			var ptrs:Array = CLibrary.method.CreateEnvironmentRender(uTextureSize, sceneNode);
			
			CDebug.assert(ptrs != null, "CDebug::Create Error!");
			
			_pointer = ptrs[0];
			_renderPointer = ptrs[2];
		}
	}
}