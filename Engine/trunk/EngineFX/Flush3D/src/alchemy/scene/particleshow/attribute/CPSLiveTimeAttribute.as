package alchemy.scene.particleshow.attribute
{
	import alchemy.core.CLibrary;
	import alchemy.filler.CAbstractMeshAttribute;
	import alchemy.tools.CDebug;

	public class CPSLiveTimeAttribute extends CAbstractMeshAttribute
	{
		public function CPSLiveTimeAttribute( minLiveTime:Number, maxLiveTime:Number )
		{
			var ptrs:Array = CLibrary.method.CreatePSLiveTimeAttribute( minLiveTime, maxLiveTime );
			
			CDebug.assert(ptrs != null, "CPSLiveTimeAttribute::CreatePSLiveTimeAttribute() Create Error!" );
			
			super( ptrs[1] );
			
			_pointer = ptrs[0];
		}
	}
}