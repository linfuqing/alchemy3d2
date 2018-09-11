package alchemy.render
{
	import alchemy.core.CLibrary;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.scene.CSceneNode;
	import alchemy.tools.CDebug;

	public class CShadowMapRender extends CAbstractProgramRender
	{
		static public const POINT_LIGHT_SHADOW:uint 		= 0;
		static public const DIRECTIONAL_LIGHT_SHADOW:uint = 1;
		static public const SPOT_LIGHT_SHADOW:uint 		= 2;
		
		public function CShadowMapRender( uLightIndex:uint, shadowType:uint = 1 )
		{
			var ptrs:Array = CLibrary.method.CreateShadowMapRender(uLightIndex, shadowType);
			
			CDebug.assert(ptrs != null, "CShadowMapRender() - CreateShadowMapRender() - Error");
			
			super(ptrs[1], ptrs[2], ptrs[0]);
			
			_cameraIndex = ptrs[3];
		}
		
		public function addShadowCaster( node:CSceneNode, effect:uint, renderPass:uint, renderMethod:CRenderMethod ):void
		{
			if(_pointer && node && renderMethod)
				CLibrary.method.AddShadowCasterToShadowMap(_pointer, node.pointer, effect, renderPass, renderMethod.pointer);
		}
		
		public function get shadowCameraIndex():uint
		{
			return _cameraIndex;
		}
		
		private var _cameraIndex:uint;
	}
}