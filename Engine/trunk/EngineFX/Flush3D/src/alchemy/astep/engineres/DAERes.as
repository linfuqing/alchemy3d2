package alchemy.astep.engineres
{
	import alchemy.animation.CAnimationController;
	
	import flash.utils.Dictionary;

	public class DAERes extends BasicRes
	{
		public function DAERes()
		{
			
		}
		
		public function build( url:String, dataHandle:uint = 0, sourceAnimationController:CAnimationController = null, sourceMaterialLib:Dictionary = null ):void
		{
			_url = url;
			_sceneDataHandle = dataHandle;
			_animationController = sourceAnimationController;
			_materialLib = sourceMaterialLib;
		}
		
		public function setMaterialLib( materialLib:Dictionary ):void
		{
			_materialLib = materialLib;
		}
		
		public function getMaterialDataByName( name:String ):uint
		{
			return _materialLib ? _materialLib[name] : 0;
		}
		
		public function set animationController( val:CAnimationController ):void
		{
			_animationController = val;
		}
		
		public function set sceneDataHandle( val:uint ):void
		{
			_sceneDataHandle = val;
		}
		
		public function get url():String
		{
			return _url;
		}
		
		public function get sceneDataHandle():uint
		{
			return _sceneDataHandle;
		}
		
		public function get animationController():CAnimationController
		{
			return _animationController;
		}
		
		override public function get type():String
		{
			return RES_TYPE_DAE;
		}
		
		private var _url:String;
		private var _sceneDataHandle:uint;
		private var _materialLib:Dictionary;
		private var _animationController:CAnimationController;
	}
}