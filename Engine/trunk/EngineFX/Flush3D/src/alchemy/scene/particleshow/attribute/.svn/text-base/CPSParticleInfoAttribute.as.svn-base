package alchemy.scene.particleshow.attribute
{
	import alchemy.core.CLibrary;
	import alchemy.filler.CAbstractMeshAttribute;
	import alchemy.tools.CDebug;

	public class CPSParticleInfoAttribute extends CAbstractMeshAttribute
	{
		public function CPSParticleInfoAttribute( fParticleWidth:Number, fParticleHeight:Number )
		{
			var ptrs:Array = CLibrary.method.CreatePSParticleInfoAttribute(fParticleWidth, fParticleHeight);
			
			CDebug.assert(ptrs != null, "CPSParticleInfoAttribute::CreatePSParticleInfoAttribute() Create Error!" );
			
			super( ptrs[1] );
			
			_pointer = ptrs[0];
		}
	}
}