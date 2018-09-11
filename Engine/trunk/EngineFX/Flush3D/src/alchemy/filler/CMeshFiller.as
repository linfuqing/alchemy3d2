package alchemy.filler
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.tools.CDebug;
	
	public class CMeshFiller extends CAbstractMeshFiller
	{
		public function CMeshFiller()
		{
			var ptrs:Array = CLibrary.method.CreateMeshFiller();
			
			CDebug.assert( ptrs != null, "CMeshFiller::CreateParticleShowBuilder() -- error!");
			
			super(ptrs[1]);
			
			_pointer = ptrs[0];
		}
		
		public function addMeshAttribute( val:CAbstractMeshAttribute ):void
		{
			if(val)
				CLibrary.method.AddAttributeToMeshFiller(pointer, val.interfacePointer);
		}
		
		public function fillMesh( mesh:uint ):Boolean
		{
			var bRet:Boolean = false;
			if(_pointer)
				bRet = CLibrary.method.FillMeshFromMeshFiller(_pointer, mesh);
			
			return bRet;
		}
	}
}