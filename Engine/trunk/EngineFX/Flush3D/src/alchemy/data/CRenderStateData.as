package alchemy.data
{
	public class CRenderStateData extends CSafeData
	{
		static public const FILL_MODE:uint  			= 0;
		
		/****************
		 *Fill Mode 
		 ***************/
		public static const FILL_MODE_POINT    :uint = 0;
		public static const FILL_MODE_WIREFRAME:uint = 1;
		public static const FILL_MODE_SOLID    :uint = 2;
		
		
		static public const SHADE_MODE:uint 			= 1;
		
		/****************
		 *Shade Mode 
		 ***************/
		public static const SHADE_MODE_FLAT   :uint = 0;
		public static const SHADE_MODE_GOURAUD:uint = 1;
		public static const SHADE_MODE_PHONG  :uint = 2;
		
		
		static public const DEPTH_WRITEE_ENABLE:uint 	= 2;
		static public const DEPTH_FUNC:uint 			= 3;
		
		static public const SRC_BLEND:uint 			= 4;
		static public const DEST_BLEND:uint 			= 5;
		static public const ALPHA_REF:uint 			= 6;
		static public const ALPHA_FUNC:uint 			= 7;
		
		/****************
		 *Blend Mode 
		 ***************/
		public static const BLEND_MODE_ZERO              :uint = 0;
		public static const BLEND_MODE_ONE               :uint = 1;
		public static const BLEND_MODE_SRC_COLOR         :uint = 2;
		public static const BLEND_MODE_INV_SRC_COLOR     :uint = 3;
		public static const BLEND_MODE_SRC_ALPHA         :uint = 4;
		public static const BLEND_MODE_INV_SRC_ALPHA     :uint = 5;
		public static const BLEND_MODE_DEST_ALPHA        :uint = 6;
		public static const BLEND_MODE_INV_DEST_ALPHA    :uint = 7;
		public static const BLEND_MODE_DEST_COLOR        :uint = 8;
		public static const BLEND_MODE_INV_DEST_COLOR    :uint = 9;
		public static const BLEND_MODE_SRC_ALPHA_SAT     :uint = 10;
		public static const BLEND_MODE_BOTH_SRC_ALPHA    :uint = 11;
		public static const BLEND_MODE_BOTH_INV_SRC_ALPHA:uint = 12;
		public static const TOTAL_BLEND_MODE             :uint = 13;
		
		static public const CULL_MODE:uint 			= 8;
		
		/****************
		 *Cull Mode 
		 ***************/
		public static const CULL_MODE_NONE :int = 0;
		public static const CULL_MODE_CCW  :int = 1;
		public static const CULL_MODE_CW   :int = 2;
		public static const TOTAL_CULL_MODE:int = 3;
		
		
		static public const STENCIL_FAIL:uint 		= 9;
		static public const STENCIL_Z_FAIL:uint 		= 10;
		static public const STENCIL_PASS:uint 		= 11;
		static public const STENCIL_FUNC:uint 		= 12;
		static public const STENCIL_REF:uint 			= 13;
		static public const STENCIL_READ_MASK:uint 	= 14;
		static public const STENCIL_WRITE_MASK:uint 	= 15;
		static public const CCW_STENCIL_FAIL:uint 	= 16;
		static public const CCW_STENCIL_Z_FAIL:uint 	= 17;
		static public const CCW_STENCIL_PASS:uint 	= 18;
		static public const CCW_STENCIL_FUNC:uint 	= 19;
		
		/****************
		 *STENCILOP Mode 
		 ***************/
		public static const STENCIL_OP_KEEP    :int = 0;
		public static const STENCIL_OP_THROW   :int = 1;
		public static const STENCIL_OP_REPLACE :int = 2;
		public static const STENCIL_OP_INCR_SAT:int = 3;
		public static const STENCIL_OP_DECR_SAT:int = 4;
		public static const STENCIL_OP_INVERT  :int = 5;
		public static const STENCIL_OP_INCR    :int = 6;
		public static const STENCIL_OP_DECR    :int = 7;
		public static const TOTAL_STENCIL_OP   :int = 8;
		
		/****************
		 *Compare Mode 
		 ***************/
		public static const CMP_FUNC_NEVER        :int = 0;
		public static const CMP_FUNC_LESS         :int = 1;
		public static const CMP_FUNC_EQUAL        :int = 2;
		public static const CMP_FUNC_LESS_EQUAL   :int = 3;
		public static const CMP_FUNC_GREATER      :int = 4;
		public static const CMP_FUNC_NOT_EQUAL    :int = 5;
		public static const CMP_FUNC_GREATER_EQUAL:int = 6;
		public static const CMP_FUNC_ALWAYS       :int = 7;
		public static const TOTAL_CMP_FUNC        :int = 8;
		
		static public const COLOR_WRITE_ENABLE:uint 	= 20;
		
		/****************
		 *Color Mode 
		 ***************/
		public static const COLOR_WRITE_ENABLE_ALPHA:uint = 0;
		public static const COLOR_WRITE_ENABLE_BLUE :uint = 1;
		public static const COLOR_WRITE_ENABLE_GREEN:uint = 2;
		public static const COLOR_WRITE_ENABLE_RED  :uint = 3;
		public static const TOTAL_COLOR_WRITE_ENABLE:uint = 4;
		
		public var renderState:uint;
		
		public var value:int;
		
		public function CRenderStateData()
		{
			
		}
	}
}