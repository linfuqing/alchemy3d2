package alchemy.astep.iores
{
	import alchemy.animation.CAnimationController;
	import alchemy.event.CLoaderEvent;
	import alchemy.external.CDAEModelLoader;
	
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.utils.Dictionary;

	public class SceneDataRes extends IORes
	{
		public function SceneDataRes()
		{
			super();
		}
		
		override public function load(url:String):void
		{
			var daeLoader:CDAEModelLoader = new CDAEModelLoader();
			daeLoader.addEventListener(CLoaderEvent.ALL_LOAD_OK, onLoadOK);
			daeLoader.addEventListener(ProgressEvent.PROGRESS, onLoadProgress);
			daeLoader.addEventListener(CLoaderEvent.LOAD_ERROR, onLoadError);
			daeLoader.load(url);
			
			_sceneDataHandle = 0;
			_animationController = null;
			_state = IORes.LOADING;
		}
		
		public function get sceneData():uint
		{
			return _sceneDataHandle;
		}
		
		public function get animationController():CAnimationController
		{
			return _animationController;
		}
		
		public function get materialDataLib():Dictionary
		{
			return _materialLib;
		}
		
		private function onLoadOK( e:CLoaderEvent ):void
		{
			var loader:CDAEModelLoader = e.target as CDAEModelLoader;
			loader.removeEventListener(CLoaderEvent.ALL_LOAD_OK, onLoadOK);
			loader.removeEventListener(ProgressEvent.PROGRESS, onLoadProgress);
			loader.removeEventListener(CLoaderEvent.LOAD_ERROR, onLoadError);
			
			_state = IORes.LOAD_OK;
			
			_sceneDataHandle = loader.sceneData;
			_animationController = loader.animationController;
			_materialLib = loader.materialLib;
			
			if( hasEventListener(Event.COMPLETE) )
				dispatchEvent( new Event(Event.COMPLETE) );
		}
		
		private function onLoadError( e:CLoaderEvent ):void
		{
			var loader:CDAEModelLoader = e.target as CDAEModelLoader;
			loader.removeEventListener(CLoaderEvent.ALL_LOAD_OK, onLoadOK);
			loader.removeEventListener(ProgressEvent.PROGRESS, onLoadProgress);
			loader.removeEventListener(CLoaderEvent.LOAD_ERROR, onLoadError);
			
			_state = IORes.UN_LOAD;
			
			if( hasEventListener(IOErrorEvent.IO_ERROR) )
				dispatchEvent( new IOErrorEvent(IOErrorEvent.IO_ERROR) );
		}
		
		private function onLoadProgress( e:ProgressEvent ):void
		{
			if( hasEventListener(ProgressEvent.PROGRESS) )
				dispatchEvent( e.clone() );
		}
		
		private var _sceneDataHandle:uint;
		private var _materialLib:Dictionary;
		private var _animationController:CAnimationController;
	}
}