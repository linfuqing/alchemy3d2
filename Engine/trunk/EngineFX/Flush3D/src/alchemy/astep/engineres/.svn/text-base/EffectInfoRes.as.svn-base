package alchemy.astep.engineres
{
	import alchemy.effect.CAbstractEffectInfo;

	public class EffectInfoRes extends BasicRes
	{
		public function EffectInfoRes()
		{
			
		}
		
		public function build( effectType:String, parameters:Object, sourceEffectInfo:CAbstractEffectInfo = null ):void
		{ 
			_effectType = effectType;
			_parameters = parameters;
			_effectInfo = sourceEffectInfo;
		}
		
		public function set effectInfo( val:CAbstractEffectInfo ):void
		{
			_effectInfo = val;
		}
		
		public function get effectInfo():CAbstractEffectInfo
		{
			return _effectInfo;
		}
		
		override public function get type():String
		{
			return RES_TYPE_EFFECTINFO;
		}
		
		private var _effectType:String;
		private var _parameters:Object;
		private var _effectInfo:CAbstractEffectInfo;
	}
}