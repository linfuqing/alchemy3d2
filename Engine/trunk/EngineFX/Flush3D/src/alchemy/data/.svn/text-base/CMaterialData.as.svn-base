package alchemy.data
{
	public class CMaterialData extends CSafeData
	{
		public var ambient:uint;
		public var diffuse:uint;
		public var specular:uint;
		public var emissive:uint;
		public var power:Number;
		public var name:String;
		
		public function get textureDataHandles():Vector.<int>
		{
			return _textureDataHandles;
		}
		
		public function CMaterialData()
		{
			super();
			
			init();
		}
		
		private function init():void
		{
			ambient      = 0;
			diffuse      = 0;
			specular     = 0;
			emissive     = 0;
			power        = 0;
			
			_textureDataHandles = new Vector.<int>;
		}
		
		private var _textureDataHandles:Vector.<int>;
	}
}