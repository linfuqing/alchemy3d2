package alchemy.astep.manager
{
	import flash.utils.Dictionary;
	
	import alchemy.astep.utility.Utility;
	
	public class EngineResManager
	{
		static private var _instance:EngineResManager;
		
		public function EngineResManager()
		{
			Utility.assert(_instance == null,"EngineResManager() - must be null!");
			
			_daeResMap 			= new Dictionary;
			_effectInfoMap 		= new Dictionary;
			_cameraResMap 		= new Dictionary;
			_materialResMap 	= new Dictionary;
			_renderMethodMap 	= new Dictionary;
			_textureResMap		= new Dictionary;
			_lightResMap		= new Dictionary;
			_blendMethodMap		= new Dictionary;
		}
		
		static public function get instance():EngineResManager
		{
			if(!_instance)
				_instance = new EngineResManager;
			
			return _instance;
		}
		
		public function reset():void
		{
			_daeResMap 			= new Dictionary;
			_effectInfoMap 		= new Dictionary;
			_cameraResMap 		= new Dictionary;
			_materialResMap 	= new Dictionary;
			_renderMethodMap 	= new Dictionary;
			_textureResMap		= new Dictionary;
			_lightResMap		= new Dictionary;
			_blendMethodMap		= new Dictionary;
		}
		
		public function get daeResMap():Dictionary
		{
			return _daeResMap;
		}
		
		public function get effectInfoMap():Dictionary
		{
			return _effectInfoMap;
		}
		
		public function get cameraResMap():Dictionary
		{
			return _cameraResMap;
		}
		
		public function get materialResMap():Dictionary
		{
			return _materialResMap;
		}
		
		public function get renderMethodMap():Dictionary
		{
			return _renderMethodMap;
		}
		
		public function get textureResMap():Dictionary
		{
			return _textureResMap;
		}
		
		public function get lightResMap():Dictionary
		{
			return _lightResMap;
		}
		
		public function get blendMethodMap():Dictionary
		{
			return _blendMethodMap;
		}
		
		private var _daeResMap:Dictionary;
		private var _effectInfoMap:Dictionary;
		private var _cameraResMap:Dictionary;
		private var _materialResMap:Dictionary;
		private var _lightResMap:Dictionary;
		private var _renderMethodMap:Dictionary;
		private var _textureResMap:Dictionary;
		private var _blendMethodMap:Dictionary;
	}
}