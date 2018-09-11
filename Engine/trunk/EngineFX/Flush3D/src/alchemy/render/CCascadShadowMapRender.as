package alchemy.render
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.scene.CSceneNode;
	import alchemy.tools.CDebug;

	public class CCascadShadowMapRender extends CAbstractProgramRender
	{
		public function CCascadShadowMapRender(lightIndex:uint, size:uint, splitNum:uint = 4)
		{
			var ptrs:Array = CLibrary.method.CreateCascadShadowMap(lightIndex, size, splitNum);
			
			CDebug.assert(ptrs != null, "CCascadShadowMapRender() - CreateCascadShadowMap() - Error");
			
			super(ptrs[2], ptrs[1], ptrs[0]);
			
			var splitTexutrePtr:uint = ptrs[3];
			_splitTexures = new Vector.<uint>(splitNum,true);
			
			CDebug.assert(splitTexutrePtr != 0, "CCascadShadowMapRender() - Error texture pointer - Error");
			
			if(splitTexutrePtr)
			{
				CLibrary.memory.position = splitTexutrePtr;
				for(var i:uint=0; i<splitNum; ++i)
					_splitTexures[i] = CLibrary.memory.readShort();
			}
		}
		
		public function addShadowCaster( sceneNode:CSceneNode, effect:uint, renderPass:uint, renderMethod:CRenderMethod ):void
		{
			if(_pointer && sceneNode && renderMethod)
				CLibrary.method.AddShadowCasterToCascadShadowMap(_pointer, sceneNode.pointer, effect, renderPass, renderMethod.pointer);
		}
		
		public function get splitTextures():Vector.<uint>
		{
			return _splitTexures;
		}
	
		private var _splitTexures:Vector.<uint>;
	}
}