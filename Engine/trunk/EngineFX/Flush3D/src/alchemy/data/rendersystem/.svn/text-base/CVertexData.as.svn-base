package alchemy.data.rendersystem
{
	import alchemy.core.CLibrary;
	import alchemy.data.CMemoryData;
	import alchemy.data.CVertexElementData;
	
	import flash.display3D.Context3DVertexBufferFormat;

	public class CVertexData extends CMemoryData
	{
		private static const _VERTEX_BUFFER_HANDLE_OFFSET:uint = 0;
		private static const _OFFSET_IN_BYTES_OFFSET     :uint = _VERTEX_BUFFER_HANDLE_OFFSET + 2;
		private static const _TYPE_OFFSET                :uint = _OFFSET_IN_BYTES_OFFSET +  + CLibrary.INT_SIZE;
		
		public static const MAXINUM_VERTEX_BUFFER_COUNT:uint = 8;

		public static const VERTEX_BUFFER_DATA_SIZE    :uint = _TYPE_OFFSET;
		public static const SIZE                       :uint = _TYPE_OFFSET + CLibrary.INT_SIZE;
		
		public function get vertexBufferHandle():uint
		{
			CLibrary.memory.position = _pointer + _VERTEX_BUFFER_HANDLE_OFFSET;
			return CLibrary.memory.readUnsignedShort();
		}
		
		public function get offsetInBytes():uint
		{
			CLibrary.memory.position = _pointer + _OFFSET_IN_BYTES_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get type():int
		{
			CLibrary.memory.position = _pointer + _TYPE_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get format():String
		{
			var result:String = null;
			switch(type)
			{
				case CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT1:
					result = Context3DVertexBufferFormat.FLOAT_1;
					break;
				case CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT2:
					result = Context3DVertexBufferFormat.FLOAT_2;
					break;
				case CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT3:
					result = Context3DVertexBufferFormat.FLOAT_3;
					break;
				case CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT4:
					result = Context3DVertexBufferFormat.FLOAT_4;
					break;
			}
			
			return result;
		}
		
		public function CVertexData()
		{
			super();
		}
	}
}