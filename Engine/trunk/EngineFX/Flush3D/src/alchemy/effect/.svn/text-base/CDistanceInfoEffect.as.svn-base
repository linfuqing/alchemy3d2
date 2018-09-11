package alchemy.effect
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;

	public class CDistanceInfoEffect extends CAbstractEffectInfo
	{
		public function CDistanceInfoEffect( focuesDis:Number )
		{
			var ptrs:Array = CLibrary.method.CreateDistanceInfoEffect( focuesDis );
			
			CDebug.assert(ptrs != null, "CDistanceInfoEffect::CreateDistanceInfoEffect() - failed");
			
			super(ptrs[0]);
			
			_interfacePtr = ptrs[1];
		}
		
		private var _interfacePtr:uint;
	}
}