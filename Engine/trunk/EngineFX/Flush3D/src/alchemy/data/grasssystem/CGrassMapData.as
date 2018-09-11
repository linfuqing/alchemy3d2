package alchemy.data.grasssystem
{
	import alchemy.core.CLibrary;
	import alchemy.data.CSafeData;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class CGrassMapData extends CSafeData
	{
		static private const _DATA_OFFSET:uint = 0;
		static private const _COLOR_MAP_OFFSET:uint = _DATA_OFFSET + CLibrary.POINTER_SIZE;
		static private const _STRIDE_OFFSET:uint = _COLOR_MAP_OFFSET + CLibrary.INT_SIZE;
		static private const _MAP_USAGE_OFFSET:uint = _STRIDE_OFFSET + CLibrary.INT_SIZE;
		static private const _CHANNEL_MASK_OFFSET:uint = _MAP_USAGE_OFFSET + CLibrary.INT_SIZE;
		static private const _CHANNEL_SCALE_OFFSET:uint = _CHANNEL_MASK_OFFSET + CLibrary.POINTER_SIZE;
		static private const _PITCH_OFFSET:uint = _CHANNEL_SCALE_OFFSET + CLibrary.POINTER_SIZE;
		
		static public const SIZE:uint = _PITCH_OFFSET + CLibrary.INT_SIZE;
		
		static public const GRASS_MAP_USAGE_COLOR_ALPHA:int = 0;
		static public const GRASS_MAP_USAGE_COLOR_RED:int = 1;
		static public const GRASS_MAP_USAGE_COLOR_GREEN:int = 2;
		static public const GRASS_MAP_USAGE_COLOR_BLUE:int = 3;
			
		static public const GRASS_MAP_USAGE_SCALE_X:int = 4;
		static public const GRASS_MAP_USAGE_SCALE_Y:int = 5;
		static public const GRASS_MAP_USAGE_SCALE_Z:int = 6;
			
		static public const GRASS_MAP_USAGE_ROTATION_X:int = 7;
		static public const GRASS_MAP_USAGE_ROTATION_Y:int = 8;
		static public const GRASS_MAP_USAGE_ROTATION_Z:int = 9;
			
		static public const GRASS_MAP_USAGE_HEIGHT:int = 0;
		
		public var colorMask:uint;
		//UINT uStride;
		//UINT uPitch;
		
		public function get colorData():ByteArray
		{
			return _colorData;
		}
		
		public function get mapUsage():uint
		{
			return _mapUsage;
		}
		
		public function get channelMask():Vector.<uint>
		{
			return _channelMask.concat();
		}
		
		public function get channelScale():Vector.<Number>
		{
			return _channelScale.concat();
		}
		
		public function CGrassMapData()
		{
			super();
			
			init();
		}
		
		public function push(mapUsage:uint, channelMask:uint, channelScale:Number):void
		{
			_mapUsage |= 1 << mapUsage;
			_channelMask.push(channelMask);
			_channelScale.push(channelScale);
		}
		
		private function init():void
		{
			colorMask = 0;
			
			_mapUsage = 0;
			_colorData = new ByteArray();
			_colorData.endian = Endian.LITTLE_ENDIAN;
			
			_channelMask = new Vector.<uint>();
			_channelScale = new Vector.<Number>();
		}
		
		private var _colorData:ByteArray;
		private var _mapUsage:uint;
		private var _channelMask:Vector.<uint>;
		private var _channelScale:Vector.<Number>;
	}
}