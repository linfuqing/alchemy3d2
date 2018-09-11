package alchemy.scene.particlesystem
{
	import alchemy.core.CInterfacePointer;

	public class CAbstractParticleComputeInfo extends CInterfacePointer
	{
		public function CAbstractParticleComputeInfo( ptr:uint, interPtr:uint )
		{
			_pointer = ptr;
			_interfacePointer = interPtr;
		}
	}
}