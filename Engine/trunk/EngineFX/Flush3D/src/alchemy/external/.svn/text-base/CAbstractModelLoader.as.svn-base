package alchemy.external
{
	import alchemy.data.CSceneData;
	import alchemy.data.CVertexElementData;
	import alchemy.event.CLoaderEvent;
	import alchemy.external.parser.ISceneDataParser;
	import alchemy.scene.CSceneNode;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	
	public class CAbstractModelLoader extends EventDispatcher
	{
		public function CAbstractModelLoader(textureLoader:CAbstractTexturesLoader = null, addedElementDatas:Vector.<CVertexElementData> = null)
		{
			super(null);
			_loader = new URLLoader;
			_textureLoader = textureLoader;
			
			_addedElementData = addedElementDatas;
		}
		
		public function load(url:String):void
		{
			var urlRequest:URLRequest = new URLRequest(url);
			_loader.addEventListener(Event.COMPLETE,_onLoadOK);
			_loader.addEventListener(ProgressEvent.PROGRESS,_onLoadProgress);
			_loader.addEventListener(IOErrorEvent.IO_ERROR,_onLoadError);
			_loader.load(urlRequest);
			_load(url);
		}
		
		public function parse(data:*):Boolean
		{
			return _parse(data);
		}
		
		public function get sceneData():uint
		{
			return _sceneDataHandle;
		}
		
		protected function _load(url:String):void
		{
			
		}
		
		protected function _parse(data:*):Boolean
		{
			return false;
		}
		
		protected function _onLoadOK(e:Event):void
		{
			_cleanListeners(_loader);
			parse(_loader.data);
		}
		
		protected function _onLoadError(e:IOErrorEvent):void
		{
			_cleanListeners(_loader);
			
			if(this.hasEventListener(CLoaderEvent.LOAD_ERROR))
				this.dispatchEvent(new CLoaderEvent(CLoaderEvent.LOAD_ERROR));
		}
		
		protected function _onLoadProgress(e:ProgressEvent):void
		{
			if(this.hasEventListener(ProgressEvent.PROGRESS))
				this.dispatchEvent(new ProgressEvent(ProgressEvent.PROGRESS,false,false,e.bytesLoaded,e.bytesTotal));
		}
		
		protected function _cleanListeners(val:EventDispatcher):void
		{
			val.removeEventListener(Event.COMPLETE,_onLoadOK);
			val.removeEventListener(ProgressEvent.PROGRESS,_onLoadProgress);
			val.removeEventListener(IOErrorEvent.IO_ERROR,_onLoadError);
		}
		
		protected var _addedElementData:Vector.<CVertexElementData>;
		
		protected var _loader:URLLoader;
		protected var _sceneDataHandle:uint;
		protected var _textureLoader:CAbstractTexturesLoader;
	}
}