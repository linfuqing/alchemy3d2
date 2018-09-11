package alchemy.data.rendersystem
{
	import alchemy.core.CLibrary;
	import alchemy.data.CMemoryData;
	
	public class CClearData extends CMemoryData
	{
		static private const _COLOR_OFFSET:uint = 0;
		static private const _FLAG_OFFSET :uint = _COLOR_OFFSET + 4;
		
		static public const SIZE          :uint = _FLAG_OFFSET + CLibrary.INT_SIZE;
		
		static public const STENCIL_BIT    :int = 0;
		static public const TARGET_BIT     :int = 1;
		static public const ZBUFFER_BIT    :int = 2;
		
		public function get color():uint
		{
			CLibrary.memory.position = _pointer + _COLOR_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get flag():uint
		{
			CLibrary.memory.position = _pointer + _FLAG_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function CClearData()
		{
			super();
		}
	}
}