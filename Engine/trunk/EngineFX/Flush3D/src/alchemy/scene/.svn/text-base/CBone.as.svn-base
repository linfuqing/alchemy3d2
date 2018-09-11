package alchemy.scene
{
	import alchemy.animation.CAnimationController;
	import alchemy.core.CLibrary;

	public class CBone extends CSceneNode
	{
		public function CBone(pointer:uint=0, interfacePointer:uint=0, sceneNodePointer:uint=0)
		{
			super(pointer, interfacePointer, sceneNodePointer);
		}
		
		override protected function getSceneNodePointers():Array
		{
			return CLibrary.method.CreateBone();
		}
		
		public function registerAnimationController( controller:CAnimationController ):void
		{
			if(_pointer && controller)
				CLibrary.method.RegisterBoneToAnimationController(controller.pointer, _pointer);
		}
		
//		override public function set name(val:String):void
//		{
//			super.name = val;
//			
//			CLibrary.method.SetBoneName(_pointer,val);
//		}
	}
}