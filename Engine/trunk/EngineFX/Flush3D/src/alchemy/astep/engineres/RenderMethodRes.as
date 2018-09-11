package alchemy.astep.engineres
{
	import alchemy.astep.engineres.data.EffectBlendData;
	import alchemy.effect.CAbstractEffectInfo;
	import alchemy.resource.basic.CEffect;
	import alchemy.resource.extend.CRenderMethod;
	

	public class RenderMethodRes extends BasicRes
	{
		public function RenderMethodRes()
		{
			
		}
		
		public function build( blendDatas:Vector.<EffectBlendData>, sourceEffect:CEffect = null, sourceRenderMethod:CRenderMethod = null ):void
		{
			if(!_effectBlendDatas)
				_effectBlendDatas = new Vector.<EffectBlendData>;
			
			_effectBlendDatas = _effectBlendDatas.concat(blendDatas);
			_effect = sourceEffect;
			_renderMethod = sourceRenderMethod;
		}
		
		public function setRenderInfo( sourceEffect:CEffect, sourceRenderMethod:CRenderMethod ):void
		{
			_effect = sourceEffect;
			_renderMethod = sourceRenderMethod;
		}
		
		public function get effect():CEffect
		{
			return _effect;
		}
		
		public function get renderMethod():CRenderMethod
		{
			return _renderMethod;
		}
		
		override public function get type():String
		{
			return RES_TYPE_RENDERMETHOD;
		}
		
		private var _effect:CEffect;
		private var _renderMethod:CRenderMethod;
		private var _effectBlendDatas:Vector.<EffectBlendData>;
	}
}