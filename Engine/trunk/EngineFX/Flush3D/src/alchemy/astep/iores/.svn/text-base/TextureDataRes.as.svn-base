package alchemy.astep.iores
{
	import alchemy.data.CTextureData;
	import alchemy.event.CLoaderEvent;
	import alchemy.manager.CMeshManager;
	import alchemy.resource.basic.CTexture;
	import alchemy.tools.CBitmapLoader;
	
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;

	public class TextureDataRes extends IORes
	{
		public function TextureDataRes()
		{
			super();
		}
		
		override public function load(url:String):void
		{
			var bmpLoader:CBitmapLoader = new CBitmapLoader;
			bmpLoader.addURL(url);
			bmpLoader.addEventListener(CLoaderEvent.ALL_LOAD_OK, onLoadOK);
			bmpLoader.addEventListener(ProgressEvent.PROGRESS, onLoading);
			bmpLoader.addEventListener(CLoaderEvent.LOAD_ERROR, onLoadError);
			bmpLoader.start();
			
			_textureDataHandle = 0;
			_state = IORes.LOADING;
		}
		
		public function set textureDataHandle( val:uint ):void
		{
			_textureDataHandle = val;
		}
		
		public function get textureDataHandle():uint
		{
			return _textureDataHandle;
		}
		
		private function onLoadOK( e:CLoaderEvent ):void
		{
			var loader:CBitmapLoader = e.target as CBitmapLoader;
			loader.removeEventListener(CLoaderEvent.ALL_LOAD_OK, onLoadOK);
			loader.removeEventListener(ProgressEvent.PROGRESS, onLoading);
			loader.removeEventListener(CLoaderEvent.LOAD_ERROR, onLoadError);
			
			_state = IORes.LOAD_OK;
			
			var textureData:CTextureData = new CTextureData;
			textureData.parse( loader.getBitmap(0).bitmapData );
			
			_textureDataHandle = CMeshManager.instance.addTextureData(textureData);
			
			loader.Destroy();
			
			if( hasEventListener(Event.COMPLETE) )
				dispatchEvent( new Event(Event.COMPLETE) );
		}
		
		private function onLoadError( e:CLoaderEvent ):void
		{
			var loader:CBitmapLoader = e.target as CBitmapLoader;
			loader.removeEventListener(CLoaderEvent.ALL_LOAD_OK, onLoadOK);
			loader.removeEventListener(ProgressEvent.PROGRESS, onLoading);
			loader.removeEventListener(CLoaderEvent.LOAD_ERROR, onLoadError);
			
			_state = IORes.UN_LOAD;
			
			if( hasEventListener(IOErrorEvent.IO_ERROR) )
				dispatchEvent( new IOErrorEvent(IOErrorEvent.IO_ERROR) );
		}
		
		private function onLoading( e:ProgressEvent ):void
		{
			if( hasEventListener(ProgressEvent.PROGRESS) )
				dispatchEvent( e.clone() );
		}
		
		private var _textureDataHandle:uint;
	}
}