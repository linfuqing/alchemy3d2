package alchemy.scene.particlesystem
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;

	public class CVelocityVCInfo extends CAbstractParticleVariableComputeInfo
	{
		public function CVelocityVCInfo( minVelocity:Number, maxVelocity:Number )
		{
			var ptrs:Array = CLibrary.method.CreateVelocityVC(minVelocity,maxVelocity);
			
			CDebug.assert(ptrs != null, "CVelocityVCInfo::CreateVelocityVC() - error!");
			
			super(ptrs[0], ptrs[1]);
		}
	}
}