package alchemy.core
{
	import alchemy.utils.AlchemyFPS;
	import alchemy.utils.AlchemyLog;
	
	import com.adobe.flascc.CModule;
	import com.adobe.utils.AGALMiniAssembler;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.DisplayObjectContainer;
	import flash.display.Loader;
	import flash.display.LoaderInfo;
	import flash.display.Stage;
	import flash.display.Stage3D;
	import flash.display3D.Context3DProgramType;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.geom.Rectangle;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	import flash.utils.getTimer;

	public class AlchemyGraphicsManager
	{
		static public function get instance():AlchemyGraphicsManager
		{
			return _instance;
		}
		
		public function AlchemyGraphicsManager()
		{
		}
		
		public function getShaderCode(index:uint):ByteArray
		{
			return index < _shaderCodes.length ? _shaderCodes[index] : null;
		}
		
		public function getRenderSystem(index:uint):AlchemyRenderSystem
		{
			if(index < _renderSystems.length)
				return _renderSystems[index];
			
			return null;
		}
		
		public function createRenderSystem(x:Number, y:Number, width:Number, height:Number, antiAlias:int):void
		{
			var freeStage:int = getFreeStage();
			if(freeStage != -1)
			{
				var stage3D:Stage3D = CModule.rootSprite.stage.stage3Ds[freeStage];
				if(stage3D)
				{
					_antiAlias = antiAlias;
					
					_viewportWidth = width;
					_viewportHeight = height;
					
					stage3D.x = x;
					stage3D.y = y;
					
					stage3D.addEventListener(Event.CONTEXT3D_CREATE, onContext3DCreate);
					stage3D.requestContext3D();
					
					return;
				}
				
				AlchemyLog.warning("Stage3D index out of range.");
				
				return;
			}
			
			AlchemyLog.warning("No free stage3D.");
		}
		
		public function compileAGALShaderSource(vertex:Boolean, sourcePointer:uint, length:uint):uint
		{
			var source:String = CModule.readString(sourcePointer, length);
			
			AlchemyLog.log( (vertex ? "[Vertex Shader]\n" : "[Fragment Shader]\n") + source );
			
			if(_shaderCodeDictionary)
			{
				var shaderCodeHandle:uint = _shaderCodeDictionary[source];
				if(shaderCodeHandle)
					return shaderCodeHandle - 1;
			}
			else
				_shaderCodeDictionary = new Dictionary(true);
			
			var assembler:AGALMiniAssembler = new AGALMiniAssembler(true);
			var shaderCode:ByteArray = assembler.assemble(vertex ? Context3DProgramType.VERTEX : Context3DProgramType.FRAGMENT, source);
			
			if(!_shaderCodes)
				_shaderCodes = new Vector.<ByteArray>();
			
			var result:uint = _shaderCodes.length;
			_shaderCodeDictionary[source] = _shaderCodes.push(shaderCode);
			
			return result;
		}
		
		public function runLoop():void
		{
			_callbackToRunLoopSymbol = CModule.getPublicSymbol("CallbackToRunLoop");
			_elapseTime = new Vector.<int>(1, true);
			_elapseTime[0] = 0.0;
			
			var root:DisplayObjectContainer = CModule.rootSprite as DisplayObjectContainer;
			if(root)
				root.stage.addEventListener(Event.ENTER_FRAME, onEnterFrame);
		}
		
		public function loadBitmapData(pathPointer:uint, length:uint):void
		{
			var path:String = CModule.readString(pathPointer, length);
			var loader:Loader = new Loader;
			
			var loaderInfo:LoaderInfo = loader.contentLoaderInfo;
			
			if(!_pathDictionary)
				_pathDictionary = new Dictionary;
			
			_pathDictionary[loaderInfo] = pathPointer;
			
			loaderInfo.addEventListener(Event.COMPLETE, onBitmapComplete);
			loaderInfo.addEventListener(IOErrorEvent.IO_ERROR, onIOError);
			loaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress);
			
			loader.load( new URLRequest(path) );
		}
		
		public function loadData(pathPointer:uint, length:uint):void
		{
			var path:String = CModule.readString(pathPointer, length);
			var loader:URLLoader = new URLLoader;
			
			if(!_pathDictionary)
				_pathDictionary = new Dictionary;
			
			_pathDictionary[loader] = pathPointer;
			
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, onDataComplete);
			loader.addEventListener(IOErrorEvent.IO_ERROR, onIOError);
			loader.addEventListener(ProgressEvent.PROGRESS, onProgress);
			loader.load( new URLRequest(path) );
		}
		
		private function onDataComplete(e:Event):void
		{
			var loader:URLLoader = (e.target as URLLoader);
			var data:ByteArray = loader.data as ByteArray;
			
			if(!_data)
				_data = new Vector.<int>(3, true);
			
			var length:uint = data.length;
			var pointer:uint = _data[0] = CModule.malloc(length);
			data.position = 0;
			CModule.writeBytes(pointer, length, data);
			
			_data[1] = length;
			_data[2] = _pathDictionary[loader];

			if(!_callbackToCreateDataSymbol)
				_callbackToCreateDataSymbol = CModule.getPublicSymbol("CallbackToCreateData");
			
			CModule.callI(_callbackToCreateDataSymbol, _data);
			
			CModule.free(pointer);
		}
		
		private function onBitmapComplete(e:Event):void
		{
			var loaderInfo:LoaderInfo = e.target as LoaderInfo;
			var bitmap:Bitmap = loaderInfo.content as Bitmap;
			var bitmapData:BitmapData = bitmap ? bitmap.bitmapData : null;
			if(bitmapData)
			{
				var data:Vector.<uint> = bitmapData.getVector(bitmapData.rect);
				
				if(!_bitmapData)
					_bitmapData = new Vector.<int>(4, true);
				
				var length:uint = data.length;
				var pointer:uint = _bitmapData[0] = CModule.malloc(length * 4);
				
				for(var i:uint = 0; i < length; ++ i)
				{
					CModule.write32(pointer, data[i]);
					
					pointer += 4;
				}
				
				_bitmapData[1] = bitmap.width;
				_bitmapData[2] = bitmap.height;
				
				_bitmapData[3] = _pathDictionary[loaderInfo];
				
				if(!_callbackToCreateBitmapDataSymbol)
					_callbackToCreateBitmapDataSymbol = CModule.getPublicSymbol("CallbackToCreateBitmapData");
				
				CModule.callI(_callbackToCreateBitmapDataSymbol, _bitmapData);
				
				CModule.free(_bitmapData[0]);
				
				bitmapData.dispose();
			}
		}
		
		private function onIOError(e:IOErrorEvent):void
		{
			(e.target as LoaderInfo).removeEventListener(IOErrorEvent.IO_ERROR, onIOError);
			
			AlchemyLog.warning(e.text);
		}
		
		private function onProgress(e:ProgressEvent):void
		{
			//(e.target as LoaderInfo).removeEventListener(ProgressEvent.PROGRESS, onProgress);
			
			//AlchemyLog.log(e);
		}
		
		private function onContext3DCreate(e:Event):void
		{
			var stage3D:Stage3D = e.target as Stage3D;
			
			var renderSystem:AlchemyRenderSystem = new AlchemyRenderSystem(stage3D.context3D);

			if(!_renderSystems)
				_renderSystems = new Vector.<AlchemyRenderSystem>();
			
			var renderSystemIndex:uint = _renderSystems.length;
			
			_renderSystems.push(renderSystem);
			
			stage3D.removeEventListener(Event.CONTEXT3D_CREATE, onContext3DCreate);
			
			var autoResetViewport:Boolean = !(_viewportWidth && _viewportHeight);
			
			if(autoResetViewport)
			{
				var stage:Stage = CModule.rootSprite.stage;
				_viewportWidth  = stage.stageWidth;
				_viewportHeight = stage.stageHeight;
				
				CModule.rootSprite.stage.addEventListener(Event.RESIZE, onResize);
				
				if(!_callbackToResetViewportSymbol)
				{
					_callbackToResetViewportSymbol = CModule.getPublicSymbol("CallbackToResetViewport");
					
					_viewport = new Vector.<int>(2, true);
				}
			}
			
			if(!_callbackToCreateRenderSystemSymbol)
				_callbackToCreateRenderSystemSymbol = CModule.getPublicSymbol("CallbackToCreateRenderSystem");
			
			CModule.callI(_callbackToCreateRenderSystemSymbol, Vector.<int>([renderSystemIndex, _viewportWidth, _viewportHeight, _antiAlias, autoResetViewport]) );
		}
		
		private function getFreeStage():int
		{
			if(!_useStage)
			{
				var length:uint = CModule.rootSprite.stage.stage3Ds.length;
				_useStage = new Vector.<Boolean>(length, true);
				for(var i:uint = 0; i < length; ++ i)
					_useStage[i] = false;
			}
			
			var length:uint = _useStage.length;
			for(var i:uint = 0; i < length; i ++)
			{
				if(!_useStage[i])
				{
					_useStage[i] = true;
					
					return i;
				}
			}
			
			return - 1;
		}
		
		private function onResize(e:Event):void
		{
			var stage:Stage = CModule.rootSprite.stage;
			if( (_viewport[0] = stage.stageWidth) != 0 && (_viewport[1] = stage.stageHeight) != 0 )
				CModule.callI(_callbackToResetViewportSymbol, _viewport);
		}
		
		private function onEnterFrame(e:Event):void
		{
			var time:int = getTimer();
			CModule.callI(_callbackToRunLoopSymbol, _elapseTime);
			_elapseTime[0] = getTimer() - time;
		}
		
		static private var _instance:AlchemyGraphicsManager = new AlchemyGraphicsManager();

		private var _renderSystems:Vector.<AlchemyRenderSystem>;
		
		private var _useStage:Vector.<Boolean>;
		
		private var _antiAlias:int;
		
		private var _viewportWidth:Number;
		private var _viewportHeight:Number;
		
		private var _callbackToCreateRenderSystemSymbol:int;
		
		private var _callbackToResetViewportSymbol:int;
		private var _viewport:Vector.<int>
		
		private var _shaderCodes:Vector.<ByteArray>;
		private var _shaderCodeDictionary:Dictionary;
		
		private var _callbackToRunLoopSymbol:int;
		private var _elapseTime:Vector.<int>;
		
		private var _callbackToCreateBitmapDataSymbol:int;
		private var _bitmapData:Vector.<int>;
		
		private var _callbackToCreateDataSymbol:int;
		private var _data:Vector.<int>;
		
		private var _pathDictionary:Dictionary;
	}
}