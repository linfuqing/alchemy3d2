package alchemy.astep.engineres
{
	import alchemy.resource.extend.CLight;

	public class LightRes extends BasicRes
	{
		public var shadeMode:uint;
		
		public function LightRes( sourceLight:CLight = null )
		{
			_light = sourceLight;
		}
		
		public function get light():CLight
		{
			return _light;
		}
		
		private var _light:CLight;
	}
}