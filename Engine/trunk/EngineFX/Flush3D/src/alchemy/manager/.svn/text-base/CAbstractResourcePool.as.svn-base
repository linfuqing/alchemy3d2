package alchemy.manager
{
	import alchemy.core.CObject;
	import alchemy.tools.CDebug;

	public class CAbstractResourcePool extends CObject
	{
		public function CAbstractResourcePool(interfacePointer:uint)
		{
			super(interfacePointer);
		}
		
		protected function lock(pool:Object):uint
		{
			if(!pool)
			{
				CDebug.warning("Pool is null.");
				
				return 0;
			}
			
			_pool = pool;
			
			_index = pool.indexOf(null);
			
			_index = _index == - 1 ? pool.length : _index
			
			return _index + 1;
		}
		
		protected function unlock(resource:Object):Boolean
		{
			if(!_pool)
			{
				CDebug.warning("Pool is null.");
				
				return false;
			}
			
			if(_index == _pool.length)
				_pool.push(resource);
			else
				_pool[_index] = resource;
			
			_pool = null;
			
			return true;
		}
		
		private function init():void
		{
			_pool  = null;
		}
		
		private var _pool:Object;
		private var _index:int;
	}
}