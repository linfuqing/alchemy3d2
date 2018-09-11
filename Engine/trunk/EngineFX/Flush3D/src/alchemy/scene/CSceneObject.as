package alchemy.scene 
{
	import alchemy.animation.CAnimationController;
	import alchemy.animation.CSkinInfo;
	import alchemy.animation.CSkinMeshController;
	import alchemy.compilers.CAbstractCompiler;
	import alchemy.core.CLibrary;
	import alchemy.data.CMeshSegmentData;
	import alchemy.data.rendermethod.CVertexElementParameterData;
	import alchemy.effect.CAbstractEffectInfo;
	import alchemy.effect.CEffectCode;
	import alchemy.manager.CSceneManager;
	import alchemy.processor.CAbstractMeshProcessor;
	import alchemy.processor.CNormalMeshProcessor;
	import alchemy.processor.CSkinMeshProcessor;
	import alchemy.resource.basic.CEffect;
	import alchemy.resource.basic.CVertexBuffer3D;
	import alchemy.resource.extend.CMaterial;
	import alchemy.resource.extend.CMesh;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.tools.CDebug;

	public class CSceneObject extends CSceneNode
	{
		public function CSceneObject(pointer:uint = 0, interfacePointer:uint = 0, sceneNodePointer:uint = 0)
		{
			super(pointer, interfacePointer, sceneNodePointer);
		}
		
		public function reset( meshHandle:uint, subset:uint, materialHandle:uint ):Boolean
		{
			return _pointer ? CLibrary.method.CreateSceneObject(meshHandle, subset, materialHandle, _pointer) > 0 : false;
		}
		
		override public function destroy():void
		{
			
		}
		
		override protected function getSceneNodePointers():Array
		{
			return CLibrary.method.CreateSceneObject(0, 0, 0, 0);
		}
		
		public function setMeshProcessor( processor:CAbstractMeshProcessor, isupdate:Boolean = false ):void
		{
			if(_pointer && processor)
				CLibrary.method.SetMeshProcessorToSceneObject(_pointer, processor.processorPointer, isupdate);
		}
		
		public function get meshProcessor():CAbstractMeshProcessor
		{
			var b:CSkinMeshProcessor;
			if(_pointer)
			{
				var ptrs:Array = CLibrary.method.GetMeshProcessorFromSceneObject(_pointer);
				CDebug.assert(ptrs != null, "CSceneObject::meshProcessor() -- GetMeshProcessorFromSceneObject - error");
				
				switch( ptrs[2] )
				{
					case 1:
						return new CNormalMeshProcessor(ptrs[0], ptrs[1]);
					case 2:
						return new CSkinMeshProcessor(ptrs[0], ptrs[1]);
				}
			}
			
			return null;
		}
		
		public function get material():uint
		{
			return _pointer ? CLibrary.method.GetMaterialFromSceneObject(_pointer) : 0;
		}
		
		public function set effect(handle:uint):void
		{
			if(_pointer && handle)
				CLibrary.method.SetEffectToSceneObject(_pointer, handle);
		}
		
		public function get effect():uint
		{
			return _pointer ? CLibrary.method.GetEffectFromSceneObject(_pointer) : 0;
		}
		
		public function setRenderMethod(renderMethod:CRenderMethod):void
		{
			CLibrary.method.SetRenderMethodToSceneObject(_pointer,renderMethod.pointer);
		}
		
		public function get renderMethod():uint
		{
			return _pointer ? CLibrary.method.GetRenderMethodFromSceneObject(_pointer) : 0;
		}
		
	}
}