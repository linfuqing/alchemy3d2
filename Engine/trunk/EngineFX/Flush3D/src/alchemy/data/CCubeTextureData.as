package alchemy.data
{
	import alchemy.event.CLoaderEvent;
	import alchemy.tools.CDebug;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.geom.Matrix;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	public class CCubeTextureData extends CSafeData
	{
		public static const POSITIVE_X:int = 0;
		public static const NEGATIVE_X:int = 1;
		public static const POSITIVE_Y:int = 2;
		public static const NEGATIVE_Y:int = 3;
		public static const POSITIVE_Z:int = 4;
		public static const NEGATIVE_Z:int = 5;
		
		public var size:uint;
		//public var format:int;
		public var usage:uint;
		public var mipLevel:uint;
		
		public function get data():Vector.<ByteArray>
		{
			return _data;
		}
		
		public function CCubeTextureData()
		{
			super();
			
			init();
		}
		
		public function load(url:String, face:int, dispatcher:EventDispatcher):void
		{
			if( _loader && _loader.contentLoaderInfo.hasEventListener(Event.COMPLETE) )
			{
				if(!_loadData)
					_loadData = new Vector.<CLoaderData>();
				
				_loadData.push( new CLoaderData(url, face, dispatcher) );
			}
			else
			{
				_loader     = new Loader();
				_url        = url;
				_dispatcher = dispatcher;
				_face       = face;
				
				_loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onComplete);
				_loader.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress);
				_loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, onIoError);

				_loader.load( new URLRequest(url) );
			}
		}
		
		public function parse(bitmap:BitmapData, face:int):void
		{
			if(!bitmap)
			{
				CDebug.warning("bitmap is null.");
				
				return;
			}
			
			if(face < 0 || face > 5)
			{
				CDebug.warning("Erorr face.");
				
				return;
			}
			
			if(bitmap.width != bitmap.height)
			{
				CDebug.warning("");
				
				return;
			}
			
			var temp:BitmapData = bitmap;
			
			if(size && size != bitmap.width)
			{
				temp = new BitmapData(size, size);
				var transform:Matrix = new Matrix();
				
				transform.scale(size / bitmap.width, size / bitmap.height);
				
				temp.draw(bitmap, transform);
			}
			else
				size = bitmap.width;
			
			mipLevel = 1;
			
			_data[face] = new ByteArray();
			
			var data:ByteArray = _data[face];
			data.endian = Endian.LITTLE_ENDIAN;
			
			var length:uint = temp.width * temp.height;

			var bitmapData:Vector.<uint> = temp.getVector(temp.rect);
			
			for(var i:uint = 0; i  < length; ++ i)
				data.writeUnsignedInt(bitmapData[i]);
			
			data.position = 0;
			
			if(temp != bitmap)
				temp.dispose();
		}
		
		private function onProgress(e:ProgressEvent):void
		{
			_dispatcher.dispatchEvent(e);
		}
		
		private function onIoError(e:IOErrorEvent):void
		{
			_dispatcher.dispatchEvent(e);
		}
		
		private function onComplete(e:Event):void
		{
			_loader.contentLoaderInfo.removeEventListener(Event.COMPLETE, onComplete);
			_loader.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS, onProgress);
			_loader.contentLoaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, onIoError);

			var bitmap:BitmapData = (_loader.content as Bitmap).bitmapData;
			if(bitmap)
			{
				parse(bitmap, _face);
				
				bitmap.dispose();
				
				_dispatcher.dispatchEvent( new CLoaderEvent(CLoaderEvent.LOAD_OK) );
				
				if(_data.indexOf(null) == -1)
					_dispatcher.dispatchEvent( new CLoaderEvent(CLoaderEvent.ALL_LOAD_OK) );
			}
			else
			{
				CDebug.warning("Data is not a bitmap.");
				
				_dispatcher.dispatchEvent( new CLoaderEvent(CLoaderEvent.LOAD_ERROR) );
			}
			
			if(_loadData && _loadData.length)
			{
				var loadData:CLoaderData = _loadData.shift();
				
				load(loadData.url, loadData.face, loadData.dispatcher);
			}
		}
		
		private function init():void
		{
			_data = new Vector.<ByteArray>(6, true);
			size = 0;
			//format = 0;
			mipLevel = 0;
			
			_loader = null;
		}
		
		private var _data:Vector.<ByteArray>;
		private var _url:String;
		private var _loader:Loader;
		private var _dispatcher:EventDispatcher;
		private var _face:int;
		private var _loadData:Vector.<CLoaderData>;
	}
}
import flash.events.EventDispatcher;

class CLoaderData
{
	public function get url():String
	{
		return _url;
	}
	
	public function get face():int
	{
		return _face;
	}
	
	public function get dispatcher():EventDispatcher
	{
		return _dispatcher;
	}
	
	public function CLoaderData(url:String, face:int, dispatcher:EventDispatcher)
	{
		_url        = url;
		_face       = face;
		_dispatcher = dispatcher;
	}
	
	private var _url:String;
	private var _face:int;
	private var _dispatcher:EventDispatcher;
}