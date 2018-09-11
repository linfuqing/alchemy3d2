package alchemy.data
{
	import alchemy.core.CLibrary;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	import mx.core.ByteArrayAsset;

	public class CMeshData extends CSafeData
	{
		//public static const INDEX_USAGE_WRITE_ONLY_BIT:int = 0;
		//public static const INDEX_USAGE_DYNAMIC_BIT   :int = 1;
		
		public static const INDEX_FORMAT_16BIT        :int = 0x1;
		public static const INDEX_FORMAT_32BIT        :int = 0x2;
		
		public var numVertices:uint;
		public var numIndices:uint;
		
		public var indexUsage:uint;
		//public var indexFormat:uint;
		public var name:String;
		
		public function get vertexData():Vector.<CVertexBufferData>
		{
			return _vertexData;
		}
		
		public function get indexData():ByteArray
		{
			_indexData.endian = Endian.LITTLE_ENDIAN;
			
			return _indexData;
		}
		
		public function get vertexDeclaration():Vector.<CVertexElementData>
		{
			return _vertexDeclaration;
		}
		
		public function get attributeRange():Vector.<CMeshSegmentData>
		{
			return _attributeRange;
		}
		
		public function get streamCount():uint
		{
			var maxStream:uint = 0;
			var elementCount:uint = _vertexDeclaration.length;
			for(var i:uint = 0; i < elementCount; i ++)
			{
				if(_vertexDeclaration[i].stream > maxStream)
					maxStream = _vertexDeclaration[i].stream;
			}
			
			return maxStream + 1;
		}
		
		public function CMeshData()
		{
			super();
			
			init();
		}
		
		public function getStreamSize(stream:uint):uint
		{
			var elementCount:uint = _vertexDeclaration.length;
			var maxOffsetElement:uint = elementCount;
			var maxOffset:uint = 0;
			
			for(var i:uint = 0; i < elementCount; i ++)
			{
				if(_vertexDeclaration[i].stream == stream && _vertexDeclaration[i].offset > maxOffset)
				{
					maxOffset        = _vertexDeclaration[i].offset;
					maxOffsetElement = i;
				}
			}
			
			if(maxOffsetElement == elementCount)
				return 0;
			
			return maxOffset + _vertexDeclaration[maxOffsetElement].type;
		}

		private function init():void
		{
			numVertices = 0;
			numIndices  = 0;
			
			indexUsage  = 0;
			//indexFormat = 0;
			
			_vertexData = new Vector.<CVertexBufferData>();
			_indexData  = new ByteArray();
			
			_vertexDeclaration = new Vector.<CVertexElementData>();
			_attributeRange    = new Vector.<CMeshSegmentData>();
		}

		private var _vertexData:Vector.<CVertexBufferData>;
		private var _indexData:ByteArray;
		
		private var _vertexDeclaration:Vector.<CVertexElementData>;
		private var _attributeRange:Vector.<CMeshSegmentData>;
	}
}