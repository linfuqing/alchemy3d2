package alchemy.astep.manager
{
	import alchemy.astep.iores.IORes;
	import alchemy.astep.iores.SceneDataRes;
	import alchemy.astep.iores.TextureDataRes;
	import alchemy.astep.utility.Utility;
	
	import flash.utils.Dictionary;

	public class IOResManager
	{
		static public const SCENE_DATA_TYPE:uint 		= 0;
		static public const TEXTURE_DATA_TYPE:uint 	= 1;
		
		static private var _instance:IOResManager;
		
		public function IOResManager()
		{
			Utility.assert(_instance == null,"IOResManager - must only the single instance");
			
			if(!_instance)
			{
				_textureRes 	= new Dictionary;
				_sceneDataRes 	= new Dictionary;
			}
		}
		
		static public function get instance():IOResManager
		{
			if(!_instance)
				_instance = new IOResManager;
			
			return _instance;
		}
		
		public function obtainRes( url:String, type:uint ):IORes
		{
			var res:IORes;
			switch( type )
			{
				case TEXTURE_DATA_TYPE:
					res = _textureRes[url];
					if(res == null)
					{
						res = new TextureDataRes;
						_textureRes[url] = res;
					}
					break;
				case SCENE_DATA_TYPE:
					res = _sceneDataRes[url];
					if(res == null)
					{
						res = new SceneDataRes;
						_sceneDataRes[url] = res;
					}
					break;
			}
			
			return res;
		}
		
		public function reset():void
		{
			_textureRes = new Dictionary;
			_sceneDataRes = new Dictionary;
		}
		
		private var _textureRes:Dictionary;
		private var _sceneDataRes:Dictionary;
	}
}