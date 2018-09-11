package alchemy.scene
{
	import alchemy.compilers.CAbstractCompiler;
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.effect.CEffectCode;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.tools.CDebug;
	
	import flash.geom.ColorTransform;
	import flash.geom.Vector3D;

	public class CSilhouetteProgram extends CPointer
	{
		public function CSilhouetteProgram(node:CSceneNode, mesh:uint, silhouetteColor:Vector3D, widthModulus:Number = 0.2, isUpdate:Boolean = false)
		{
			var pointers:Array = getPointers(node, mesh, widthModulus, silhouetteColor, isUpdate);
			
			_pointer = pointers[0];
			
			CDebug.assert(_pointer != 0, " Create SilhouetteProgram error! ");
		}
		
		private function getPointers(node:CSceneNode, mesh:uint, widthModulus:Number, silhouetteColor:Vector3D, isUpdate:Boolean):Array
		{
			var ptr:uint = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE * 4 );
			
			CLibrary.memory.position = ptr;
			CLibrary.memory.writeFloat( silhouetteColor.x );
			CLibrary.memory.writeFloat( silhouetteColor.y );
			CLibrary.memory.writeFloat( silhouetteColor.z );
			CLibrary.memory.writeFloat( 1.0 );
			
			var pointers:Array = CLibrary.method.CreateSilhoutteProgram(node.pointer, mesh, widthModulus, silhouetteColor, isUpdate);
			
			CLibrary.method.DestroyBuffer( ptr );
			
			return pointers;
		}
	}
}