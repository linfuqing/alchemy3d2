package alchemy.scene.particleshow.attribute
{
	import alchemy.core.CLibrary;
	import alchemy.filler.CAbstractMeshAttribute;
	import alchemy.tools.CDebug;

	public class CPSTextureAttribute extends CAbstractMeshAttribute
	{
		public function CPSTextureAttribute()
		{
			var ptrs:Array = CLibrary.method.CreatePSTextureAttribute();
			
			CDebug.assert(ptrs != null, "CPSTextureAttribute::CreatePSTextureAttribute() Create Error!" );
			
			super( ptrs[1] );
			
			_pointer = ptrs[0];
		}
	}
}