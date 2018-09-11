package alchemy.data.animation
{
	import alchemy.core.CLibrary;
	import alchemy.data.CSafeData;
	
	import flash.geom.Matrix3D;
	
	public class CAnimationTrackData extends CSafeData
	{
		static public const DATA_OFFSET:uint = CLibrary.POINTER_SIZE*3 + CLibrary.INT_SIZE;
		
		public var name:String;
		
		public function CAnimationTrackData()
		{
			super();
			
			_times = new Vector.<Number>;
			_transformMatrixes = new Vector.<Matrix3D>;
		}
		
		public function get transformMatrixes():Vector.<Matrix3D>
		{
			return _transformMatrixes;
		}
		
		public function get times():Vector.<Number>
		{
			return _times;
		}
		
		protected var _transformMatrixes:Vector.<Matrix3D>;
		protected var _times:Vector.<Number>;
	}
}