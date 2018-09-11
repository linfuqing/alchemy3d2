package alchemy.data.rendermethod
{
	import alchemy.data.CSafeData;
	
	public class CVertexElementParameterData extends CSafeData
	{
		public var type:int;
		public var usage:int;
		public var index:uint;
		
		public function CVertexElementParameterData(type:int, usage:int, index:uint)
		{
			this.type  = type;
			this.usage = usage;
			this.index = index;
				
			super();
		}
	}
}