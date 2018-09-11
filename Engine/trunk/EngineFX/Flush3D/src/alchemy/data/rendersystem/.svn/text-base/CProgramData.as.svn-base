package alchemy.data.rendersystem
{
	import alchemy.core.CLibrary;
	import alchemy.data.CMemoryData;
	import alchemy.manager.CRenderSystem;

	public class CProgramData extends CMemoryData
	{
		private static const  _PROGRAM_HANDLE_OFFSET                :uint = 0;
		/*private static const _VERTEX_PARAMETER_SIZE_OFFSET          :uint = _PROGRAM_HANDLE_OFFSET + 2;
		private static const _FRAGMENT_PARAMETER_SIZE_OFFSET        :uint = _VERTEX_PARAMETER_SIZE_OFFSET + CLibrary.INT_SIZE * CRenderSystem.MAXINUM_VERTEX_PARAMETER_COUNT;
		private static const _VERTEX_INDEX_OFFSET                   :uint = _FRAGMENT_PARAMETER_SIZE_OFFSET + CLibrary.INT_SIZE * CRenderSystem.MAXINUM_FRAGMENT_PARAMETER_COUNT;
		private static const _FRAGMENT_INDEX_OFFSET                 :uint = _VERTEX_INDEX_OFFSET + CLibrary.INT_SIZE * CRenderSystem.MAXINUM_VERTEX_PARAMETER_COUNT;
		private static const _VERTEX_PARAMETER_COUNT_OFFSET         :uint = _FRAGMENT_INDEX_OFFSET + CLibrary.INT_SIZE * CRenderSystem.MAXINUM_FRAGMENT_PARAMETER_COUNT;
		private static const _FRAGMENT_PARAMETER_COUNT_OFFSET       :uint = _VERTEX_PARAMETER_COUNT_OFFSET + CLibrary.INT_SIZE;*/
		private static const _DIRTY_VERTEX_PARAMETER_OFFSET_OFFSET  :uint = _PROGRAM_HANDLE_OFFSET + 2;//_FRAGMENT_PARAMETER_COUNT_OFFSET + CLibrary.INT_SIZE;
		private static const _DIRTY_VERTEX_PARAMETER_COUNT_OFFSET   :uint = _DIRTY_VERTEX_PARAMETER_OFFSET_OFFSET + CLibrary.INT_SIZE;
		private static const _DIRTY_FRAGMENT_PARAMETER_OFFSET_OFFSET:uint = _DIRTY_VERTEX_PARAMETER_COUNT_OFFSET + CLibrary.INT_SIZE;
		private static const _DIRTY_FRAGMENT_PARAMETER_COUNT_OFFSET :uint = _DIRTY_FRAGMENT_PARAMETER_OFFSET_OFFSET + CLibrary.INT_SIZE;
		private static const _TEXTURE_FLAG_OFFSET                   :uint = _DIRTY_FRAGMENT_PARAMETER_COUNT_OFFSET + CLibrary.INT_SIZE;
		
		public static const SIZE:uint = _TEXTURE_FLAG_OFFSET + CLibrary.INT_SIZE;
		
		public function get programHandle():uint
		{
			CLibrary.memory.position = _pointer + _PROGRAM_HANDLE_OFFSET;
			return CLibrary.memory.readUnsignedShort();
		}
		
		/*public function get vertexParameterCount():uint
		{
			CLibrary.memory.position = _pointer + _VERTEX_PARAMETER_COUNT_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get fragmentParameterCount():uint
		{
			CLibrary.memory.position = _pointer + _FRAGMENT_PARAMETER_COUNT_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}*/
		
		public function get dirtyVertexParameterOffset():uint
		{
			CLibrary.memory.position = _pointer + _DIRTY_VERTEX_PARAMETER_OFFSET_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get dirtyVertexParameterCount():uint
		{
			CLibrary.memory.position = _pointer + _DIRTY_VERTEX_PARAMETER_COUNT_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get dirtyFragmentParameterOffset():uint
		{
			CLibrary.memory.position = _pointer + _DIRTY_FRAGMENT_PARAMETER_OFFSET_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get dirtyFragmentParameterCount():uint
		{
			CLibrary.memory.position = _pointer + _DIRTY_FRAGMENT_PARAMETER_COUNT_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get textureFlag():uint
		{
			CLibrary.memory.position = _pointer + _TEXTURE_FLAG_OFFSET;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function CProgramData()
		{
			super();
		}
		
		/*public function getVertexParameterSize(index:uint = 0):uint
		{
			if(index)
				CLibrary.memory.position = _pointer + _VERTEX_PARAMETER_SIZE_OFFSET + index * CLibrary.INT_SIZE;
			else
				CLibrary.memory.position = _pointer + _VERTEX_PARAMETER_SIZE_OFFSET;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function getFragmentParameterSize(index:uint = 0):uint
		{
			if(index)
				CLibrary.memory.position = _pointer + _FRAGMENT_PARAMETER_SIZE_OFFSET + index * CLibrary.INT_SIZE;
			else
				CLibrary.memory.position = _pointer + _FRAGMENT_PARAMETER_SIZE_OFFSET;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function getVertexParameterIndex(index:uint = 0):uint
		{
			if(index)
				CLibrary.memory.position = _pointer + _VERTEX_INDEX_OFFSET + index * CLibrary.INT_SIZE;
			else
				CLibrary.memory.position = _pointer + _VERTEX_INDEX_OFFSET;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function getFragmentParameterIndex(index:uint = 0):uint
		{
			if(index)
				CLibrary.memory.position = _pointer + _FRAGMENT_INDEX_OFFSET + index * CLibrary.INT_SIZE;
			else
				CLibrary.memory.position = _pointer + _FRAGMENT_INDEX_OFFSET;
			
			return CLibrary.memory.readUnsignedInt();
		}*/
	}
}