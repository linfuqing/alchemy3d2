package alchemy.data
{
	import alchemy.core.CLibrary;
	import alchemy.effect.CAbstractEffectInfo;
	import alchemy.effect.blendmethod.CAbstractBlendMethod;

	public class CEffectInfoData extends CSafeData
	{
		public static const SIZE:int = CLibrary.POINTER_SIZE + CLibrary.POINTER_SIZE + CLibrary.INT_SIZE + CLibrary.INT_SIZE + CLibrary.INT_SIZE;
		
		public var blendMethod:CAbstractBlendMethod;
		
		public function set effectInfo(info:CAbstractEffectInfo):void
		{
			_effectInfo = info;
		}
		
		public function get effectInfo():CAbstractEffectInfo
		{
			return _effectInfo;
		}
		
		public function CEffectInfoData(effectInfo:CAbstractEffectInfo, blendMethod:CAbstractBlendMethod = null)
		{
			super();
			
			this.effectInfo  = effectInfo;
			this.blendMethod = blendMethod;
		}
		
		private var _effectInfo:CAbstractEffectInfo;
	}
}