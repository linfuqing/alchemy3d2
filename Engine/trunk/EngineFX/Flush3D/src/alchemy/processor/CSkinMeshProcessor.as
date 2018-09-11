package alchemy.processor
{
	import alchemy.animation.CAnimationController;
	import alchemy.animation.CSkinInfo;
	import alchemy.animation.CSkinMeshController;
	import alchemy.core.CLibrary;
	import alchemy.effect.CAbstractEffectInfo;
	import alchemy.effect.CEffectCode;
	import alchemy.scene.CBone;
	import alchemy.tools.CDebug;

	public class CSkinMeshProcessor extends CAbstractMeshProcessor
	{
		public function CSkinMeshProcessor( sourcePointer:uint = 0, processorPointer:uint = 0 )
		{	
			super(sourcePointer, processorPointer);
			
			if(sourcePointer && processorPointer)
				init();
		}
		
		public function reset( skinType:uint, skinInfo:CSkinInfo, rootBone:CBone ):void
		{
			var ptrs:Array = CLibrary.method.CreateSkinMeshProcessor( skinType, skinInfo.pointer, rootBone.pointer );
			
			CDebug.assert( ptrs != null, "CSkinMehProcessor::CreateSkinMeshProcessor ERROR!");
			
			_pointer = ptrs[0];
			_processorPointer = ptrs[1];
		}
		
		protected function init():void
		{
			_skinMeshController = new CSkinMeshController( CLibrary.method.GetSkinProcessorInfo(_pointer) );
		}
		
		public function set animtionController( animController:CAnimationController ):void
		{
			if(!_skinMeshController)
				init();
			
			if(_skinMeshController.rootBone == null || _skinMeshController.skinInfo == null)
				return;
			
			if(animController && _pointer)
			{
				animController.makeDefaultBoneMatrix( _skinMeshController.rootBone );
				animController.setupBoneMatrix( _skinMeshController.rootBone );
				
				_skinMeshController.rootBone.registerAnimationController( animController );
				
				//CLibrary.method.SetAnimControllerToProcessor(_processorPointer, animController.pointer);
			}
		}
		
		public function get skinMeshController():CSkinMeshController
		{
			if(!_skinMeshController)
				init();
			
			return _skinMeshController;
		}
		
		//public function get animationController():CAnimationController
		//{
		//	return _pointer ? new CAnimationController( CLibrary.method.GetAnimControllerFromProcessor(_pointer) ) : null;
		//}
		
		protected var _skinMeshController:CSkinMeshController;
	}
}