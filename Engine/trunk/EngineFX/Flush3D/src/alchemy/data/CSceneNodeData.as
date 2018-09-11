package alchemy.data
{
	import alchemy.animation.CAnimationController;
	import alchemy.animation.CSkinInfo;
	import alchemy.data.animation.CBoneData;
	
	import flash.geom.Matrix3D;

	public class CSceneNodeData extends CSafeData
	{
		static public const NODE_TYPE:int = 0;
		static public const OBJECT_TYPE:int = 1;
		static public const BONE_TYPE:int = 2;
		
		public var id:String;
		public var name:String;
		
		public var parent:int;
		public var type:int;
		
		public var meshData:int;
		public var subset:int;
		public var materialData:int;
		
		//public var skinInfo:CSkinInfo;
		//public var animController:CAnimationController;
		
		public var skinInfoHandle:uint;
		
		public var rootBone:uint;
		public var isUpdate:Boolean;
		
		protected var _transform:Matrix3D;
		
		public function CSceneNodeData()
		{
			super();
			
			parent = 0;
			type = NODE_TYPE;
			
			_transform = new Matrix3D;
		}
	
		public function get transform():Matrix3D
		{
			return _transform;
		}
	}
}