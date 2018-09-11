package alchemy.scene.particlesystem
{
	import alchemy.core.CInterfacePointer;

	public class CAbstractParticleVariableComputeInfo extends CInterfacePointer
	{
		public function CAbstractParticleVariableComputeInfo( ptr:uint, interPtr:uint  )
		{
			_pointer = ptr;
			_interfacePointer = interPtr;
		}
	}
}