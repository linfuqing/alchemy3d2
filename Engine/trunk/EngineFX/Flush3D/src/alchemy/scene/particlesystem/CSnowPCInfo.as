package alchemy.scene.particlesystem
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	
	import flash.geom.Vector3D;

	public class CSnowPCInfo extends CAbstractParticleComputeInfo
	{
		public function CSnowPCInfo(
			minEdge:Vector3D, maxEdge:Vector3D, 
			minLiveTime:Number, maxLiveTime:Number, 
			groundPos:Number)
		{	
			var minEdgePtr:uint = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE*3 );
			var maxEdgePtr:uint = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE*3 );
			
			CLibrary.memory.position = minEdgePtr;
			CLibrary.memory.writeFloat( minEdge.x );
			CLibrary.memory.writeFloat( minEdge.y );
			CLibrary.memory.writeFloat( minEdge.z );
			
			CLibrary.memory.position = maxEdgePtr;
			CLibrary.memory.writeFloat( maxEdge.x );
			CLibrary.memory.writeFloat( maxEdge.y );
			CLibrary.memory.writeFloat( maxEdge.z );
			
			var ptrs:Array = CLibrary.method.CreateSnowParticleCompute(minEdgePtr, maxEdgePtr, minLiveTime, maxLiveTime, groundPos);
			
			CLibrary.method.DestroyBuffer( minEdgePtr );
			CLibrary.method.DestroyBuffer( maxEdgePtr );
			
			super(ptrs[0], ptrs[1]);
		}
	}
}