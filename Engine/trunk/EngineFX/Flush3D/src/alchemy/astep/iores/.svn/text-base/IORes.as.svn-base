package alchemy.astep.iores
{
	import flash.events.EventDispatcher;

	public class IORes extends EventDispatcher
	{
		static public const UN_LOAD:uint = 0;
		static public const LOADING:uint = 1;
		static public const LOAD_OK:uint = 2;
		
		public function IORes()
		{
			_state = UN_LOAD;
		}
		
		public function load( url:String ):void
		{
			
		}
		
		public function get state():uint
		{
			return _state;
		}
		
		protected var _state:uint;
	}
}