package alchemy.data
{
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	public class CVertexBufferData extends CSafeData
	{
		//public static const VERTEX_USAGE_WRITE_ONLY_BIT:int = 0;
		//public static const VERTEX_USAGE_DYNAMIC_BIT   :int = 1;
		
		public var usage:uint;
		
		public function get data():ByteArray
		{
			_data.endian = Endian.LITTLE_ENDIAN;
			
			return _data;
		}
		
		public function CVertexBufferData()
		{
			super();
			
			init();
		}
		
		private function init():void
		{
			_data = new ByteArray();
		}
		
		private var _data:ByteArray;
		private var _usage:uint;
	}
}