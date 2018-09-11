package alchemy.core
{
	import cmodule.alchemy.CLibInit;
	
	import flash.utils.ByteArray;
	
	public class CLibrary
	{
		public static const INT_SIZE  :uint   = 4;
		public static const FLOAT_SIZE:uint   = 4;
		public static const POINTER_SIZE:uint = 4;
		public static const BOOL_SIZE:uint    = 1;
		
		public static function get memory():ByteArray
		{
			return _memory;
		}
		
		public static function get method():Object
		{
			return _method;
		}
		
		public static function get instance():CLibrary
		{
			return _library;
		}
		
		public function CLibrary()
		{
			super();
			
			if(!_memory || !_method)
			{
				var clib:CLibInit = new CLibInit();
				_method = clib.init();
				
				var ns:Namespace = new Namespace("cmodule.alchemy");
				_memory = (ns::gstate).ds;
			}
		}
		
		private static var _memory:ByteArray = null;
		private static var _method:Object    = null;
		
		private static var _library:CLibrary = new CLibrary();
	}
}