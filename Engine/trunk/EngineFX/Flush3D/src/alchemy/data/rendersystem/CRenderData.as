package alchemy.data.rendersystem
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.data.CMemoryData;
	import alchemy.tools.CDebug;
	
	public class CRenderData extends CMemoryData
	{
		static private const _DEVICE_STATE_BLOCK_DATA_OFFSET:uint = 0;
		static private const _CLEAR_DATA_OFFSET             :uint = _DEVICE_STATE_BLOCK_DATA_OFFSET + CDeviceStateBlockData.SIZE;
		static private const _RENDER_TARGET_HANDLE_OFFSET   :uint = _CLEAR_DATA_OFFSET + CClearData.SIZE;
		static private const _RENDER_TARGET_SURFACE_OFFSET  :uint = _RENDER_TARGET_HANDLE_OFFSET + 2;
		static private const _VERTEX_DATA_OFFSET            :uint = _RENDER_TARGET_SURFACE_OFFSET + CLibrary.INT_SIZE;
		static private const _INDEX_BUFFER_HANDLE_OFFSET    :uint = _VERTEX_DATA_OFFSET + CVertexData.MAXINUM_VERTEX_BUFFER_COUNT * CVertexData.SIZE;
		static private const _PROGRAM_DATA_OFFSET           :uint = _INDEX_BUFFER_HANDLE_OFFSET + 2;
		static private const _START_OFFSET                  :uint = _PROGRAM_DATA_OFFSET + CProgramData.SIZE;
		static private const _TRIANGLE_COUNT_OFFSET         :uint = _START_OFFSET + CLibrary.INT_SIZE;
		static private const _FLAG_OFFSET                   :uint = _TRIANGLE_COUNT_OFFSET + CLibrary.INT_SIZE;
		
		static private const _END_VERTEX_DATA_OFFSET        :uint = _INDEX_BUFFER_HANDLE_OFFSET;
		
		public static const SIZE:uint = _FLAG_OFFSET + CLibrary.INT_SIZE;
		
		public static const FLAG_CLEAR_DIRTY_BIT           :uint = 0;
		public static const FLAG_RENDER_TARGET_DIRTY_BIT   :uint = 1;
		public static const FLAG_PROGRAM_DIRTY_BIT         :uint = 2;
		public static const FLAG_DRAW_INDEXED_TRIANGLES_BIT:uint = 3;
		public static const FLAG_INDEX_BUFFER_DIRTY_BIT    :uint = 4;
		
		public static const FLAG_VERTEX_BUFFER_DIRTY_SHIFT :uint = 5;
		public static const FLAG_VERTEX_INDEX_DIRTY_SHIFT  :uint = FLAG_VERTEX_BUFFER_DIRTY_SHIFT + CVertexData.MAXINUM_VERTEX_BUFFER_COUNT;
		public static const FLAG_TEXTURE_DIRTY_SHIFT       :uint = FLAG_VERTEX_INDEX_DIRTY_SHIFT  + CVertexData.MAXINUM_VERTEX_BUFFER_COUNT;
		
		public function get renderTargetHandle():uint
		{
			CLibrary.memory.position = _pointer + _RENDER_TARGET_HANDLE_OFFSET;
			return CLibrary.memory.readUnsignedShort();
		}
		
		public function get renderTargetSurface():uint
		{
			CLibrary.memory.position = _pointer + _RENDER_TARGET_SURFACE_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get indexBufferHandle():uint
		{
			CLibrary.memory.position = _pointer + _INDEX_BUFFER_HANDLE_OFFSET;
			return CLibrary.memory.readUnsignedShort();
		}
		
		public function get start():uint
		{
			CLibrary.memory.position = _pointer + _START_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get triangleCount():uint
		{
			CLibrary.memory.position = _pointer + _TRIANGLE_COUNT_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get flag():uint
		{
			CLibrary.memory.position = _pointer + _FLAG_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get deviceStateBlockData():CDeviceStateBlockData
		{
			_deviceStateBlockData.attach(_pointer + _DEVICE_STATE_BLOCK_DATA_OFFSET);
			
			return _deviceStateBlockData;
		}
		
		public function get clearData():CClearData
		{
			_clearData.attach(_pointer + _CLEAR_DATA_OFFSET);
			
			return _clearData;
		}
		
		public function get programData():CProgramData
		{
			_programData.attach(_pointer + _PROGRAM_DATA_OFFSET);
			
			return _programData;
		}
		
		public function CRenderData()
		{
			super();
			
			init();
		}
		
		public function beginReadVertexData(index:uint = 0):CVertexData
		{		
			_vertexDataOffset = index ? (_VERTEX_DATA_OFFSET + CVertexData.SIZE * index) : _VERTEX_DATA_OFFSET;
			
			if(_vertexDataOffset >= _END_VERTEX_DATA_OFFSET)
			{
				_vertexDataOffset = 0;
				
				CDebug.warning("Error index.");
				
				return null;
			}
			
			_vertexData.attach(_pointer + _vertexDataOffset);
			
			return _vertexData;
		}
		
		public function nextVertexData():CVertexData
		{
			if(_vertexDataOffset == 0 || (_vertexDataOffset + CVertexData.SIZE) >= _END_VERTEX_DATA_OFFSET)
			{
				_vertexDataOffset = 0;
				
				return null;
			}
			
			_vertexDataOffset += CVertexData.SIZE;
			
			_vertexData.attach(_pointer + _vertexDataOffset);
			
			return _vertexData;
		}
		
		public function endReadVertexData():void
		{
			_vertexDataOffset = 0;
		}
		
		private function init():void
		{
			_deviceStateBlockData = new CDeviceStateBlockData();
			_clearData            = new CClearData();
			_vertexData           = new CVertexData();
			_programData          = new CProgramData();
			
			_vertexDataOffset     = 0;
		}
		
		private var _deviceStateBlockData:CDeviceStateBlockData;
		private var _clearData:CClearData;
		private var _vertexData:CVertexData;
		private var _programData:CProgramData;
		
		private var _vertexDataOffset:uint;
	}
}