package alchemy.scene.particlesystem
{
	import alchemy.core.CInterfacePointer;
	
	public class CParticleController extends CInterfacePointer
	{
		public function CParticleController( pt:uint, interPt:uint )
		{
			_pointer = pt;
			_interfacePointer = interPt;
		}
	}
}