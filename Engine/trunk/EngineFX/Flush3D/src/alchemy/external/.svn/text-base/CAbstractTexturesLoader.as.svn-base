package alchemy.external
{
	import alchemy.data.CTextureData;
	import alchemy.manager.CRenderSystem;
	import alchemy.resource.basic.CTexture;
	
	import flash.display.BitmapData;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.utils.Dictionary;
	
	public class CAbstractTexturesLoader extends EventDispatcher
	{
		public function CAbstractTexturesLoader()
		{
			super();
			_textureNames = new Vector.<String>;
			_textureLib = new Dictionary;
		} 
		
		public function addTextureName(fileName:String):void
		{
			_textureNames.push(fileName);
		}
		
		public function getTextureByName(name:String):CTextureData
		{
			return _textureLib[name];
		}
		
		public function getTextureNames():Vector.<String>
		{
			return _textureNames;
		}
		
		public function destroyResources():void
		{
			_textureNames.splice(0,_textureNames.length);
			_textureLib = new Dictionary;
		}
		
		public function start():void
		{
			
		}
		
		public function stopLoad():void
		{
			
		}
		
		protected var _textureLib:Dictionary;
		protected var _textureNames:Vector.<String>;
	}
}