package alchemy.astep.engineres
{
	import alchemy.effect.blendmethod.CAbstractBlendMethod;

	public class BlendMethodRes extends BasicRes
	{
		public function BlendMethodRes()
		{
			super();
		}
		
		public function build(type:String, parameterMap:Object, sourceBlendMethod:CAbstractBlendMethod = null):void
		{
			_blendType = type;
			_parametersMap = parameterMap;
			_blendMethod = sourceBlendMethod;
		}
		
		public function set blendMethod( val:CAbstractBlendMethod ):void
		{
			_blendMethod = val;
		}
		
		public function get blendMethod():CAbstractBlendMethod
		{
			return _blendMethod;
		}
		
		public function get blendType():String
		{
			return _blendType;
		}
		
		private var _parametersMap:Object;
		private var _blendType:String;
		private var _blendMethod:CAbstractBlendMethod;
	}
}