package alchemy.data
{
	import alchemy.core.CLibrary;
	import alchemy.event.CLoaderEvent;
	import alchemy.tools.CDebug;
	import alchemy.tools.CMath;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.display3D.Context3DTextureFormat;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	public class CTextureData extends CSafeData
	{
		public static const RENDER_TARGET:int = 2;
		
		public static const SIZE:uint = 6 * CLibrary.INT_SIZE;
		
		public var data:ByteArray;
		public var width:uint;
		public var height:uint;
		//public var format:int;
		public var usage:uint;
		public var mipLevel:uint;
		public var name:String;
		
		public function CTextureData()
		{
			super();
			
			init();
		}
		
		public function load(url:String, dispatcher:EventDispatcher):void
		{
			_loader     = new Loader();
			_dispatcher = dispatcher;
			
			_loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onComplete);
			_loader.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress);
			_loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, onIoError);
			
			_loader.load( new URLRequest(url) );
		}
		
		public function parse(bitmap:BitmapData):void
		{
			if(!bitmap)
			{
				CDebug.warning("bitmap is null.");
				
				return;
			}
			
			width    = bitmap.width;
			height   = bitmap.height;
			mipLevel = 0;
			
			data = new ByteArray();
			data.endian = Endian.LITTLE_ENDIAN;
			
			var length:uint = width * height;
			var bitmapData:Vector.<uint> = bitmap.getVector(bitmap.rect);
			for(var i:uint = 0; i  < length; ++ i)
				data.writeUnsignedInt(bitmapData[i]);
			
			data.position = 0;
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
				parse(bitmap);
				
				bitmap.dispose();
				
				_dispatcher.dispatchEvent( new CLoaderEvent(CLoaderEvent.LOAD_OK) );
			}
			
			CDebug.warning("Data is not a bitmap.");
			
			_dispatcher.dispatchEvent( new CLoaderEvent(CLoaderEvent.LOAD_ERROR) );
		}
		
		private function init():void
		{
			data = null;
			width = 0;
			height = 0;
			//format = 0;
			mipLevel = 0;
			
			_loader = null;
		}
		
		private var _loader:Loader;
		private var _dispatcher:EventDispatcher;
	}
}