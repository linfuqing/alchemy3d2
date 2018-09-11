package alchemy.data
{
	import alchemy.core.CLibrary;

	public class CVertexElementData extends CSafeData
	{
		public static const VERTEX_ELEMENT_TYPE_FLOAT1    :int = CLibrary.FLOAT_SIZE * 1;
		public static const VERTEX_ELEMENT_TYPE_FLOAT2    :int = CLibrary.FLOAT_SIZE * 2;
		public static const VERTEX_ELEMENT_TYPE_FLOAT3    :int = CLibrary.FLOAT_SIZE * 3;
		public static const VERTEX_ELEMENT_TYPE_FLOAT4    :int = CLibrary.FLOAT_SIZE * 4;
		
		public static const ELEMENT_USAGE_POSITION        :int = 0;
		public static const ELEMENT_USAGE_BLENDWEIGHT     :int = 1;
		public static const ELEMENT_USAGE_BLENDINDICES    :int = 2;
		public static const ELEMENT_USAGE_INSTANTCEINDICES:int = 3;
		public static const ELEMENT_USAGE_NORMAL          :int = 4;
		public static const ELEMENT_USAGE_PSIZE           :int = 5;
		public static const ELEMENT_USAGE_TEXCOORD        :int = 6;
		public static const ELEMENT_USAGE_TANGENT         :int = 7;
		public static const ELEMENT_USAGE_BINORMAL        :int = 8;
		//public static const ELEMENT_USAGE_TESSFACTOR  :int = 8;
		public static const ELEMENT_USAGE_POSITIONT       :int = 9;
		public static const ELEMENT_USAGE_COLOR           :int = 10;
		public static const ELEMENT_USAGE_FOG             :int = 11;
		public static const ELEMENT_USAGE_DEPTH           :int = 12;
		public static const ELEMENT_USAGE_SAMPLE          :int = 13;
		
		public var stream:uint;
		public var offset:uint;
		
		public var type:int;
		public var usage:int;
		
		public function CVertexElementData()
		{
			super();
		}
	}
}