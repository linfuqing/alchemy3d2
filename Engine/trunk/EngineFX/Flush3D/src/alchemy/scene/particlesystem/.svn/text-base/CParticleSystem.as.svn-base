package alchemy.scene.particlesystem
{
	import alchemy.core.CLibrary;
	import alchemy.filler.CAbstractMeshFiller;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.scene.CSceneNode;
	
	
	public class CParticleSystem extends CSceneNode
	{
		public function CParticleSystem( 
			particleController:CParticleController,
			maxParticleCount:uint, meshFiller:CAbstractMeshFiller, effect:uint,
			renderMethod:CRenderMethod, material:uint
			)
		{
			_particlePointer = particleController.pointer;
			_maxParticleCount = maxParticleCount;
			_meshFiller = meshFiller;
			_effect = effect;
			_renderMethod = renderMethod;
			_material = material;
			
			super(0, 0, 0);
		}
		
		override protected function getSceneNodePointers():Array
		{	
			return CLibrary.method.CreateParticleSystem(_particlePointer, _maxParticleCount, _meshFiller.pointer, _effect, _renderMethod.pointer, _material);
		}
		
		public function setFrameEmitted( num:uint ):void
		{
			CLibrary.method.SetFrameEmitToParticleSystem(_pointer, num);
		}
		
		private var _particlePointer:uint;
		private var _maxParticleCount:uint;
		private var _effect:uint;
		private var _renderMethod:CRenderMethod;
		private var _material:uint;
		private var _meshFiller:CAbstractMeshFiller;
	}
}