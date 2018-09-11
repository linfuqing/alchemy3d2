package alchemy.scene.particleshow
{
	import alchemy.core.CLibrary;
	import alchemy.filler.CAbstractMeshFiller;
	import alchemy.manager.CMeshManager;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.scene.CSceneNode;

	public class CParticleShowSystem extends CSceneNode
	{
		public function CParticleShowSystem()
		{
			super();
		}
		
		public function initPSSystem( particleCount:uint, psbuilder:CAbstractMeshFiller, effect:uint, renderMethod:CRenderMethod, material:uint ):Boolean
		{
			CMeshManager.instance;
			return CLibrary.method.InitParticleShowSystem( pointer, particleCount, psbuilder.interfacePointer, effect, renderMethod.pointer, material );
		}
		
		override protected function getSceneNodePointers():Array
		{
			return CLibrary.method.CreateParticleShowSystem();
		}
	}
}