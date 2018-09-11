package alchemy.scene.particleshow.attribute
{
	import alchemy.core.CLibrary;
	import alchemy.filler.CAbstractMeshAttribute;
	import alchemy.tools.CDebug;

	public class CPSVelocityAttribute extends CAbstractMeshAttribute
	{
		public function CPSVelocityAttribute( fMinVelocity:Number, fMaxVelocity:Number )
		{
			var ptrs:Array = CLibrary.method.CreatePSVelocityAttribute(fMinVelocity, fMaxVelocity);
			
			CDebug.assert(ptrs != null, "CPSVelocityAttribute::CreatePSVelocityAttribute() Create Error!" );
			
			super( ptrs[1] );
			
			_pointer = ptrs[0];
		}
	}
}