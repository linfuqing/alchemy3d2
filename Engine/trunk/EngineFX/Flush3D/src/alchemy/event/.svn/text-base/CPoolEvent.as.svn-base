package alchemy.event
{
	import flash.events.Event;
	
	public class CPoolEvent extends Event
	{
		public static const RENDER_SYSTEM_CREATED:String = "Render System Created";
		
		public function get handle():uint
		{
			return _handle;
		}
		
		public function CPoolEvent(type:String, handle:uint, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
			
			_handle = handle;
		}
		
		private var _handle:uint;
	}
}