package alchemy.data.grasssystem
{
	import alchemy.core.CLibrary;
	import alchemy.data.CSafeData;
	
	import flash.utils.ByteArray;
	
	public class CGrassMapInfoData extends CSafeData
	{
		static private const _WIDTH_OFFSET:uint = 0;
		static private const _HEIGHT_OFFSET:uint = _WIDTH_OFFSET + CLibrary.INT_SIZE;
		static private const _SCALE_OFFSET:uint = _HEIGHT_OFFSET + CLibrary.INT_SIZE;
		static private const _GRASS_MAPS_OFFSET:uint = _SCALE_OFFSET + CLibrary.FLOAT_SIZE;
		static private const _GRASS_MAP_COUNT_OFFSET:uint = _GRASS_MAPS_OFFSET + CLibrary.POINTER_SIZE;
		
		static public const SIZE:uint = _GRASS_MAP_COUNT_OFFSET + CLibrary.INT_SIZE;
		
		public var width:uint;
		public var height:uint;
		public var scale:Number;
		
		public function get grassMaps():Vector.<CGrassMapData>
		{
			return _grassMaps;
		}
		
		public function CGrassMapInfoData()
		{
			super();
			
			init();
		}
		
		private function init():void
		{
			_grassMaps = new Vector.<CGrassMapData>();
		}
		
		private var _grassMaps:Vector.<CGrassMapData>;
	}
}