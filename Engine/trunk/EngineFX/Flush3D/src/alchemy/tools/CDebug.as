package alchemy.tools
{
	public class CDebug
	{
		public static function error(comment:String):void
		{
			//throw new Error(comment);
			trace(comment);
		}
		
		public static function assert(x:Boolean, comment:String):void
		{
			if(!x)
				error(comment);
		}
		
		public static function warning(comment:String):void
		{
			trace(comment);
		}
		
		public static function log(input:String):void
		{
			//trace(input);
		}
		
		public function CDebug()
		{
		}
	}
}