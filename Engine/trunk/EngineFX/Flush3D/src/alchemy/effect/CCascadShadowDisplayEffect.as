package alchemy.effect
{
	import alchemy.core.CLibrary;
	import alchemy.render.CCascadShadowMapRender;
	import alchemy.tools.CDebug;

	public class CCascadShadowDisplayEffect extends CAbstractEffectInfo
	{
		public function CCascadShadowDisplayEffect( csmNode:CCascadShadowMapRender, samplerIndices:Vector.<uint>, lightIndex:uint )
		{
			if(csmNode && samplerIndices)
			{
				var samplerPointer:uint = CLibrary.method.CreateBuffer( CLibrary.INT_SIZE*samplerIndices.length );
				CLibrary.memory.position = samplerPointer;
				
				for(var i:uint=0; i<samplerIndices.length; ++i)
					CLibrary.memory.writeUnsignedInt( samplerIndices[i] );
				
				var ptrs:Array = CLibrary.method.CreateCascadShadowDisplayEffect(csmNode.pointer, samplerPointer, lightIndex);
				
				CDebug.assert(ptrs != null, "CCascadShadowDisplayEffect::CreateCascadShadowDisplayEffect() - error!");
				
				super(ptrs[0]);
				
				_hostPointer = ptrs[1];
				
				CLibrary.method.DestroyBuffer( samplerPointer );
			}
		}
		
		private var _hostPointer:uint;
	}
}