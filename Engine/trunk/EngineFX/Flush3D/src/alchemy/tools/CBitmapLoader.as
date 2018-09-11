package alchemy.tools
{
	import alchemy.event.CLoaderEvent;
	
	import flash.display.Bitmap;
	import flash.display.Loader;
	import flash.display.LoaderInfo;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.URLRequest;
	import flash.utils.Dictionary;
	
	import mx.events.Request;

	public class CBitmapLoader extends EventDispatcher
	{
		public function CBitmapLoader()
		{
			super()
			init();
		}
		
		public function getBitmap(index:uint):Bitmap
		{
			return (_bitmap && _bitmap.length > index) ? _bitmap[index] : null;
		}
		
		public function addURL(url:String):void
		{
			if(!url)
				return;
			
			if(!_url || _bitmap)
			{
				Destroy();
				
				_url = new Vector.<String>();
			}
			
			_url.push(url);
		}
		
		public function start():void
		{
			_bitmap = new Vector.<Bitmap>(_url.length, true);
			var length:uint = _bitmap.length;
			for(var i:uint = 0; i < length; ++ i)
				_bitmap[i] = null;
			
			_currentLoaderIndex = 0;
			_contentMap = new Dictionary;
			onComplete();
		}
		
		public function Destroy():void
		{
			if(_bitmap)
			{
				var bitmap:Bitmap;
				var length:uint = _bitmap.length;
				for(var i:uint = 0; i < length; ++ i)
				{
					bitmap = _bitmap[i];
					if(bitmap)
						bitmap.bitmapData.dispose();
				}
			}
			
			_bitmap = null;
		}
		
		private function init():void
		{
			_url                = null;
			_bitmap             = null;
			_currentLoaderIndex = 0
		}
		
		private function onComplete(e:Event = null):void
		{
			if(e)
			{
				_bitmap[_currentLoaderIndex ++] = (e.target as LoaderInfo).content as Bitmap;
				(_contentMap[(e.target as LoaderInfo)] as Loader).unloadAndStop();
				
				dispatchEvent(e);
				dispatchEvent( new CLoaderEvent(CLoaderEvent.LOAD_OK) );
			}

			if(_bitmap.indexOf(null) == -1)
			{
				dispatchEvent( new CLoaderEvent(CLoaderEvent.ALL_LOAD_OK) );
				
				removeEventListener(Event.COMPLETE, onComplete);
			}
			else
			{
				var loader:Loader = new Loader();
				loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onComplete);
				loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, onIOError);
				loader.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress);
				loader.load( new URLRequest(_url[_currentLoaderIndex]) );
				_contentMap[loader.contentLoaderInfo] = loader;
			}
		}
		
		private function onIOError(e:IOErrorEvent):void
		{
			removeEventListener(IOErrorEvent.IO_ERROR, onIOError);
			
			dispatchEvent(e);
		}
		
		private function onProgress(e:ProgressEvent):void
		{
			removeEventListener(ProgressEvent.PROGRESS, onProgress);
			
			dispatchEvent(e);
		}
		
		private var _url:Vector.<String>;
		private var _bitmap:Vector.<Bitmap>;
		private var _currentLoaderIndex:uint;
		private var _contentMap:Dictionary;
	}
}