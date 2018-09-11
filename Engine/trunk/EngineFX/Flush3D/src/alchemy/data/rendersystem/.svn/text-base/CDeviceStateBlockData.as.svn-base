package alchemy.data.rendersystem
{
	import alchemy.core.CLibrary;
	import alchemy.data.CMemoryData;
	
	public class CDeviceStateBlockData extends CMemoryData
	{
		public static const FILL_MODE:uint = 0;
			
		public static const SHADE_MODE:uint = 1;
			
		public static const DEPTH_WRITE_ENABLE:uint = 2;
		public static const DEPTH_FUNC:uint = 3;
			
		public static const SRC_BLEND:uint = 4;
		public static const DEST_BLEND:uint = 5;
		public static const ALPHA_REF:uint = 6;
		public static const ALPHA_FUNC:uint = 7;
			
		public static const CULL_MODE:uint = 8;
			
		public static const STENCIL_FAIL:uint = 9;
		public static const STENCIL_Z_FAIL:uint = 10;
		public static const STENCIL_PASS:uint = 11;
		public static const STENCIL_FUNC:uint = 12;
		public static const STENCIL_REF:uint = 13;
		public static const STENCIL_READ_MASK:uint = 14;
		public static const STENCIL_WRITE_MASK:uint = 15;
		public static const CCW_STENCIL_FAIL:uint = 16;
		public static const CCW_STENCIL_Z_FAIL:uint = 17;
		public static const CCW_STENCIL_PASS:uint = 18;
		public static const CCW_STENCIL_FUNC:uint = 19;
		
		public static const COLOR_WRITE_ENABLE:uint = 20;
		
		public static const TOTAL_RENDER_STATE:uint = 21;
		
		private static const _DEPTH_FUNC_OFFSET:uint = DEPTH_FUNC * CLibrary.INT_SIZE;
		
		private static const _SRC_BLEND_OFFSET:uint = SRC_BLEND * CLibrary.INT_SIZE;
		private static const _DEST_BLEND_OFFSET:uint = DEST_BLEND * CLibrary.INT_SIZE;
		private static const _CULL_MODE_OFFSET:uint = CULL_MODE * CLibrary.INT_SIZE;
		private static const _STENCIL_FUNC_OFFSET:uint = STENCIL_FUNC * CLibrary.INT_SIZE;
		private static const _STENCIL_FAIL_OFFSET:uint = STENCIL_FAIL * CLibrary.INT_SIZE;
		private static const _STENCIL_Z_FAIL_OFFSET:uint = STENCIL_Z_FAIL * CLibrary.INT_SIZE;
		private static const _STENCIL_PASS_OFFSET:uint = STENCIL_PASS * CLibrary.INT_SIZE;
		private static const _CCW_STENCIL_FAIL_OFFSET:uint = CCW_STENCIL_FAIL * CLibrary.INT_SIZE;
		private static const _CCW_STENCIL_Z_FAIL_OFFSET:uint = CCW_STENCIL_Z_FAIL * CLibrary.INT_SIZE;
		private static const _CCW_STENCIL_PASS_OFFSET:uint = CCW_STENCIL_PASS * CLibrary.INT_SIZE;
		private static const _CCW_STENCIL_FUNC_OFFSET:uint = CCW_STENCIL_FUNC * CLibrary.INT_SIZE;
		private static const _COLOR_WRITE_ENABLE_OFFSET:uint = COLOR_WRITE_ENABLE * CLibrary.INT_SIZE;
		
		private static const _STENCIL_REF_OFFSET:uint = STENCIL_REF * CLibrary.INT_SIZE;
		private static const _STENCIL_READ_MASK_OFFSET:uint = STENCIL_READ_MASK * CLibrary.INT_SIZE;
		private static const _STENCIL_WRITE_MASK_OFFSET:uint = STENCIL_WRITE_MASK * CLibrary.INT_SIZE;
		
		private static const _DEPTH_WRITE_ENABLE_OFFSET:uint = DEPTH_WRITE_ENABLE * CLibrary.INT_SIZE;
		
		private static const _DIRTY_FLAG_OFFSET:uint = TOTAL_RENDER_STATE * CLibrary.INT_SIZE;
		
		public static const SIZE:uint = _DIRTY_FLAG_OFFSET + CLibrary.INT_SIZE;
		
		public function get depthFunc():uint
		{
			CLibrary.memory.position = _DEPTH_FUNC_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get srcBlend():uint
		{
			CLibrary.memory.position = _SRC_BLEND_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get destBlend():uint
		{
			CLibrary.memory.position = _DEST_BLEND_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get cullMode():uint
		{
			CLibrary.memory.position = _CULL_MODE_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get stencilFunc():uint
		{
			CLibrary.memory.position = _STENCIL_FUNC_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get stencilFail():uint
		{
			CLibrary.memory.position = _STENCIL_FAIL_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get stencilZFail():uint
		{
			CLibrary.memory.position = _STENCIL_Z_FAIL_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get stencilPass():uint
		{
			CLibrary.memory.position = _STENCIL_PASS_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get CCWStencilFail():uint
		{
			CLibrary.memory.position = _CCW_STENCIL_FAIL_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get CCWStencilZFail():uint
		{
			CLibrary.memory.position = _CCW_STENCIL_Z_FAIL_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get CCWStencilPass():uint
		{
			CLibrary.memory.position = _CCW_STENCIL_PASS_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get CCWStencilFunc():uint
		{
			CLibrary.memory.position = _CCW_STENCIL_FUNC_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get colorWriteEnable():uint
		{
			CLibrary.memory.position = _COLOR_WRITE_ENABLE_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get stencilRef():uint
		{
			CLibrary.memory.position = _STENCIL_REF_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedByte();
		}
		
		public function get stencilReadMask():uint
		{
			CLibrary.memory.position = _STENCIL_READ_MASK_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedByte();
		}
		
		public function get stencilWriteMask():uint
		{
			CLibrary.memory.position = _STENCIL_WRITE_MASK_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedByte();
		}
		
		public function get depthWriteEnable():Boolean
		{
			CLibrary.memory.position = _DEPTH_WRITE_ENABLE_OFFSET + _pointer;
			
			return CLibrary.memory.readBoolean();
		}
		
		public function get dirtyFlag():uint
		{
			CLibrary.memory.position = _DIRTY_FLAG_OFFSET + _pointer;
			
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function CDeviceStateBlockData()
		{
			super();
		}
	}
}