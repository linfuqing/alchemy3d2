package alchemy.scene.particlesystem
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;
	
	import flash.geom.Vector3D;

	public class CAccelerationVC extends CAbstractParticleVariableComputeInfo
	{
		public function CAccelerationVC( acceVelocity:Vector3D, attenPhi:Number = 1.0 )
		{
			var acceVelocityPtr:uint = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE*3 );
			CLibrary.memory.position = acceVelocityPtr;
			CLibrary.memory.writeFloat(acceVelocity.x);
			CLibrary.memory.writeFloat(acceVelocity.y);
			CLibrary.memory.writeFloat(acceVelocity.z);
			
			var ptrs:Array = CLibrary.method.CreateAccelerationVelocityVC(acceVelocityPtr, attenPhi);
			
			CDebug.assert(ptrs != null, "CAccelerationVC::CreateAccelerationVelocityVC() - error!");
			
			super(ptrs[0],ptrs[1]);
			
			CLibrary.method.DestroyBuffer( acceVelocityPtr );
		}
	}
}