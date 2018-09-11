package alchemy.external
{
	import alchemy.constant.CBuffer;
	import alchemy.data.CTextureData;
	import alchemy.event.CTextureLoaderEvent;
	import alchemy.manager.CRenderSystem;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.display.LoaderInfo;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.net.URLRequest;
	import flash.utils.Dictionary;
	
	public class CSinglePathTextureLoader extends CAbstractTexturesLoader
	{
		public function CSinglePathTextureLoader(basicPath:String)
		{
			super(); 
			
			_curIndex = 0;
			_basicPath = basicPath;
			
			_pathNames = new Vector.<String>;
		}
		
		override public function addTextureName(fileName:String):void
		{
			if(_textureLib == null)
				_textureLib = new Dictionary;
			
			if(_loaderToFileName == null)
				_loaderToFileName = new Dictionary;
			
			_textureNames.push(fileName);
			
			_pathNames.push(_basicPath+"/"+fileName);
		}
		
		override public function start():void
		{
			var loader:Loader;// = new Loader;
			
			for(var i:uint=0; i<_pathNames.length; ++i)
			{
				loader = new Loader;
				loader.contentLoaderInfo.addEventListener(Event.COMPLETE,onLoadComplete);
				loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR,onLoadError);
				loader.load(new URLRequest( _pathNames[i] ));
				
				var helpObj:Object = new Object;
				helpObj.loader = loader;
				helpObj.fileName = _textureNames[i];
				helpObj.contentInfo = loader.contentLoaderInfo;
				
				_loaderToFileName[loader.contentLoaderInfo] = helpObj;
			}
		}
		
		protected function onLoadComplete(e:Event):void
		{
			var loader:LoaderInfo = e.target as LoaderInfo;
			loader.removeEventListener(Event.COMPLETE,onLoadComplete);
			loader.removeEventListener(IOErrorEvent.IO_ERROR,onLoadError);
			
			var objInfo:Object = _loaderToFileName[loader];
			
			var textureData:CTextureData = new CTextureData;
			textureData.parse((loader.content as Bitmap).bitmapData);
			
			_textureLib[objInfo.fileName] = textureData;
			
			(loader.content as Bitmap).bitmapData.dispose();
			
			++_curIndex;
			
			var texLoader:Loader = objInfo.loader;
			texLoader.unloadAndStop();
			
			var event:CTextureLoaderEvent = new CTextureLoaderEvent(CTextureLoaderEvent.TEXTURE_COMPLETE_ONE);
			event.curTextureName = objInfo.fileName;
			
			if(this.hasEventListener(CTextureLoaderEvent.TEXTURE_COMPLETE_ONE) == true)
				this.dispatchEvent(event);
			
			onChecked();
		}
		
		protected function onLoadError(e:IOErrorEvent):void
		{
			var content:LoaderInfo = e.target as LoaderInfo;
			content.removeEventListener(Event.COMPLETE,onLoadComplete);
			content.removeEventListener(IOErrorEvent.IO_ERROR,onLoadError);
			
			var objInfo:Object = _loaderToFileName[content];
			
			var event:CTextureLoaderEvent = new CTextureLoaderEvent(CTextureLoaderEvent.TEXTURE_LOADERROR);
			event.curTextureName = objInfo.fileName;
			
			++_curIndex;
			
			if(this.hasEventListener(CTextureLoaderEvent.TEXTURE_LOADERROR) == true)
				this.dispatchEvent(event);
			
			onChecked();
		}
		
		protected function onChecked():void
		{
			if(_curIndex >= _textureNames.length)
			{
				var event:CTextureLoaderEvent = new CTextureLoaderEvent(CTextureLoaderEvent.TEXTURE_ALL_LOADCOMPLETE);
				
				_curIndex = 0;
				if(this.hasEventListener(CTextureLoaderEvent.TEXTURE_ALL_LOADCOMPLETE) == true)
					this.dispatchEvent(event);
				
				return;
			}
		}
		
		override public function destroyResources():void
		{
			_textureLib = null;
			_loaderToFileName = null;
				
			super.destroyResources();
		}
		
		protected var _pathNames:Vector.<String>;
		protected var _loaderToFileName:Dictionary;
		protected var _curIndex:int;
		protected var _basicPath:String;
	}
}