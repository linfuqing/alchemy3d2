package alchemy.core 
{
	import flash.events.EventDispatcher;

	public class CPointer extends EventDispatcher
	{
		public function get pointer():uint
		{
			return _pointer;
		}
		
		public function CPointer()
		{
		}
		
		public function destroy():void
		{
			
		}
		
		protected var _pointer:uint;
	}
}