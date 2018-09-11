package alchemy.scene
{
	import alchemy.core.CLibrary;
	import alchemy.resource.extend.CRenderMethod;

	public class CStaticMeshInstance extends CSceneNode
	{
		public function CStaticMeshInstance(effectHandle:uint, defRenderMethod:CRenderMethod)
		{
			_effect = effectHandle;
			_renderMethod = defRenderMethod;
			super(0, 0, 0);
		}
		
		public function set effect( val:uint ):void
		{
			if(_pointer)
				CLibrary.method.SetEffectToStaticMeshInstance(_pointer, val);
		}
		
		public function set renderMethod( val:CRenderMethod ):void
		{
			if(_pointer)
				CLibrary.method.SetRenderMethodToStaticMeshInstance(_pointer, val ? val.pointer : null);
		}
		
		public function get materialNum():uint
		{
			return _pointer ? CLibrary.method.GetMaterialNumFromStaticMeshInstance(_pointer) : 0;
		}
		
		public function getMaterialByIndex( index:uint ):uint
		{
			return _pointer ? CLibrary.method.GetMaterialFromStaticMeshInstance(_pointer, index) : 0;
		}
		
		public function setSceneData( handle:uint ):Boolean
		{
			return _pointer ? CLibrary.method.SetSceneDataToStaticMeshInstance(_pointer, handle) : false;
		}
		
		override protected function getSceneNodePointers():Array
		{
			return CLibrary.method.CreateStaticMeshInstance(_effect, _renderMethod.pointer);
		}
		
		private var _effect:uint;
		private var _renderMethod:CRenderMethod;
	}
}