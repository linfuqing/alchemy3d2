package alchemy.effect
{
	import alchemy.core.CLibrary;
	import alchemy.manager.CRenderSystem;
	import alchemy.resource.basic.CTexture;
	import alchemy.resource.extend.CMaterial;
	import alchemy.tools.CDebug;

	public class CShadowMapDisplayEffect extends CAbstractEffectInfo
	{
		static public const SPOT_LIGHT:uint = 0;
		static public const DIRECTIONAL_LIGHT:uint = 1;
		
		public function CShadowMapDisplayEffect( samplerIndex:uint = 0, lightIndex:uint = 0, cameraIndex:uint = 0, type:uint = 0)
		{
			_samplerIndex = samplerIndex;
			var ptrs:Array;
			
			if(type == SPOT_LIGHT)
				ptrs = CLibrary.method.CreateShadowMapDisplayEffect(samplerIndex, lightIndex, cameraIndex);
			else
				ptrs = CLibrary.method.CreateDirectionalShdowDisplayEffect(samplerIndex, lightIndex, cameraIndex);
			
			CDebug.assert(ptrs != null,"CShadowMapDisplayEffect::Create Error!");
			
			super(ptrs[0]);
			
			_host = ptrs[1];
			_shadowMap = ptrs[2];
		}
		
//		public function applyMaterial( renderSys:CRenderSystem, material:CMaterial ):Boolean
//		{
//			var texture:CTexture;
//			if(renderSys && material && material.textureNumber() == _samplerIndex)
//			{
//				texture = renderSys.getTexture(_shadowMap);
//				if(texture)
//				{
//					material.addTexture(texture);
//					return true;
//				}
//			}
//			
//			return false;
//		}
		
		private var _samplerIndex:uint;
		private var _shadowMap:uint;
		private var _host:uint;
	}
}