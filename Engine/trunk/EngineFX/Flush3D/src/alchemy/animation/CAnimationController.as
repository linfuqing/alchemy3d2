package alchemy.animation
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.scene.CBone;
	
	import flash.geom.Matrix3D;
	import flash.utils.Dictionary;

	public class CAnimationController extends CPointer
	{
		public function CAnimationController( sourcePointer:uint = 0 )
		{
			_pointer = sourcePointer;
		}
		
		public function reset(maxOutputCnt:int = 300, maxAnimSetCnt:int = 5):void
		{
			_boneDefaultMap = new Dictionary;
			_pointer = CLibrary.method.CreateAnimationController(maxOutputCnt,maxAnimSetCnt);
		}
		
		override public function destroy():void
		{
			CLibrary.method.DestroyAnimController(_pointer);
		}
		
		public function getAnimationSet( index:uint ):CAnimationSet
		{
			return _pointer ? new CAnimationSet( CLibrary.method.GetAnimationSetFromController(_pointer, index) ) : null;
		}
		
		public function registerAnimationSet(animationset:CAnimationSet,weight:Number,speed:Number):void
		{
			CLibrary.method.RegisterAniamtionSetToController(_pointer,animationset.pointer,weight,speed);
		}
		
		public function unregisterAniamtionSet(animationset:CAnimationSet):void
		{
			CLibrary.method.UnregisterAnimationSetFromController(_pointer,animationset.pointer);
		}
		
		public function resetTime():void
		{
			CLibrary.method.ResetAnimationController(_pointer);
		}
		
		public function setupBoneMatrix(rootBone:CBone):void
		{
			_travelBone(rootBone,true);
		}
		
		public function makeDefaultBoneMatrix(rootBone:CBone):void
		{
			_boneDefaultMap = new Dictionary;
			_travelBone(rootBone,false);
		}
		
		public function update( elapse:Number ):void
		{
			if(_pointer)
				CLibrary.method.UpdateAnimation(_pointer, elapse);
		}
		
		protected function _travelBone(curNode:CBone, restore:Boolean):void
		{
			if(curNode == null)
				return;
			
			if(restore == true)
				curNode.transform = _boneDefaultMap[curNode.name];
			else
				_boneDefaultMap[curNode.name] = curNode.transform;
			
			var len:int = curNode.childrenLen;
			var child:CBone;
			for(var i:int=0; i<len; ++i)
			{
				child = curNode.getChildByIndex(i) as CBone;
				_travelBone(child,restore);
			}
		}
		
		protected var _boneDefaultMap:Dictionary;
	}
}