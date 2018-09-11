package alchemy.data.animation
{
	import alchemy.core.CLibrary;
	import alchemy.data.CSafeData;
	
	import flash.geom.Matrix3D;
	import flash.utils.ByteArray;
	
	public class CBoneData extends CSafeData
	{
		static public const BONE_DATA_OFFSET:uint = 3 * CLibrary.POINTER_SIZE + CLibrary.INT_SIZE + 16*CLibrary.FLOAT_SIZE;
		
		public function CBoneData()
		{
			super();
			
			_weights = new Vector.<Number>;
			_indices = new Vector.<uint>;
			_offsetMatrix = new Matrix3D;
			_offsetMatrix.identity();
		}
		
		public function set name(val:String):void
		{
			_name = val;
		}
		
		public function get name():String
		{
			return _name;
		}
		
		public function get weights():Vector.<Number>
		{
			return _weights;
		}
		
		public function get indices():Vector.<uint>
		{
			return _indices;
		}
		
		public function get offsetMatrix():Matrix3D
		{
			return _offsetMatrix;
		}
		
		protected var _name:String;
		protected var _weights:Vector.<Number>;
		protected var _indices:Vector.<uint>;
		protected var _offsetMatrix:Matrix3D;
	}
}