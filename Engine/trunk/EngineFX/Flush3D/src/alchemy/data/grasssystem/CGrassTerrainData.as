package alchemy.data.grasssystem
{
	import alchemy.core.CLibrary;
	import alchemy.data.CSafeData;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;
	
	public class CGrassTerrainData extends CSafeData
	{
		static private var _MESH_OFFSET:uint = 0;
		static private var _UP_OFFSET:uint = _MESH_OFFSET + 4;
		static private var _TRANSFORM_POINTER_OFFSET:uint = _UP_OFFSET + 3 * CLibrary.FLOAT_SIZE;
		
		static public var SIZE:uint = _TRANSFORM_POINTER_OFFSET + CLibrary.POINTER_SIZE;
		
		public var mesh:uint;
		public var transform:Matrix3D;
		
		public function get up():Vector3D
		{
			return _up;
		}
		
		public function CGrassTerrainData()
		{
			super();
			
			init();
		}
		
		private function init():void
		{
			mesh = 0;
			transform = null;
			_up = new Vector3D(0.0, 1.0, 0.0);
		}
		
		private var _up:Vector3D;
	}
}