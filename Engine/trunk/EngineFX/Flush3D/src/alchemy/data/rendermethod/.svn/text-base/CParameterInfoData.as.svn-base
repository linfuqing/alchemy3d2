package alchemy.data.rendermethod
{
	import alchemy.core.CLibrary;
	import alchemy.data.CSafeData;

	public class CParameterInfoData extends CSafeData
	{
		public static const SIZE:uint = CLibrary.INT_SIZE + CLibrary.INT_SIZE + CLibrary.INT_SIZE;
		
		public static const TYPE_VERTEX_SHADER  :int = 1;
		public static const TYPE_FRAGMENT_SHADER:int = 2;
		public static const TYPE_PROGRAM        :int = 3;
		
		public var type:int;
		//public var pass:uint;
		public var alphaIndex:uint;
		public var betaIndex:uint;
		
		public function CParameterInfoData(type:uint, alphaIndex:uint, betaIndex:uint = 0)
		{
			this.type       = type;
			//this.pass  = pass;
			this.alphaIndex = alphaIndex;
			this.betaIndex  = betaIndex;
		}
	}
}