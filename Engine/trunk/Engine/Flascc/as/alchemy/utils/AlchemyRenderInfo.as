package alchemy.utils
{
	public class AlchemyRenderInfo
	{
		public var driverInfo:String;
		
		public var drawCount:uint;
		public var faceCount:uint;
		
		public function AlchemyRenderInfo()
		{
			reset();
		}
		
		public function reset():void
		{
			drawCount = 0;
			faceCount = 0;
		}
	}
}