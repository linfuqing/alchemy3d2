package alchemy.scene.particleshow.attribute
{
	import alchemy.core.CLibrary;
	import alchemy.filler.CAbstractMeshAttribute;
	import alchemy.tools.CDebug;
	
	import flash.geom.Vector3D;

	public class CPSPositionAttribute extends CAbstractMeshAttribute
	{
		public function CPSPositionAttribute( pos:Vector3D )
		{
			var ptrs:Array = CLibrary.method.CreatePSPositionAttribute(pos.x, pos.y, pos.z);
			
			CDebug.assert(ptrs != null, "CPSPositionAttribute::CreatePSPositionAttribute() Create Error!" );
			
			super( ptrs[1] );
			
			_pointer = ptrs[0];
		}
	}
}