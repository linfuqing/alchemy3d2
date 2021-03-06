package alchemy.utils
{
	import com.adobe.flascc.CModule;
	
	public class AlchemyLog
	{
		static public function error(comment:String):void
		{
			throw new Error(comment);
		}
		
		static public function assert(x:Boolean, comment:String):void
		{
			if(!x)
				error(comment);
		}
		
		static public function warning(comment:String):void
		{
			CModule.activeConsole.consoleWrite("[warning]" + comment);
			//trace("[warning]" + comment);
		}
		
		static public function log(input:String):void
		{
			CModule.activeConsole.consoleWrite("[log]" + input);
			//trace("[log]" + input);
		}
	}
}