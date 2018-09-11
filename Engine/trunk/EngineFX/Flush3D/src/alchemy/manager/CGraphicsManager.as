package alchemy.manager
{
	import alchemy.core.CObject;
	import alchemy.event.CPoolEvent;
	import alchemy.tools.CDebug;
	
	import flash.display.DisplayObject;
	import flash.display.Stage3D;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.geom.Rectangle;
	
	public class CGraphicsManager extends EventDispatcher
	{
		public static function get canCreateSceneManager():Boolean
		{
			return _canCreateSceneManager;
		}
		
		public static function get canCreateRenderSystem():Boolean
		{
			return _canCreateRenderSystem;
		}
		
		public function CGraphicsManager(container:DisplayObject, lockStageIndex:Vector.<uint> = null)
		{
			CDebug.assert(!_graphicsManagerCreated, "Graphics manager is singleton.");
				
			_container      = container;
			_lockStageIndex = lockStageIndex;
			
			super(null);
			
			init();
		}
		
		public function createRenderSystem(totalRenderDataCount:uint, viewPort:Rectangle, antiAlias:int):void
		{
			var freeStage:int = getFreeStage();
			if(freeStage != -1)
			{
				var stage:Stage3D = _container.stage.stage3Ds[freeStage];
				
				if(stage)
				{
					_totalRenderDataCount = totalRenderDataCount;
					_antiAlias            = antiAlias;
					
					stage.addEventListener(Event.CONTEXT3D_CREATE, onContext3DCreate);
					//stage.viewPort = viewPort;
					stage.x = viewPort.x;
					stage.y = viewPort.y;
					stage.requestContext3D();
					
					_viewPort = viewPort;
					
					return;
				}
				
				CDebug.warning("Stage index out of range.");
				
				return;
			}
			
			CDebug.warning("No free stage.");
		}
		
		public function createSceneManager(renderSystem:CRenderSystem):CSceneManager
		{
			_canCreateSceneManager=  true;
			var sceneManager:CSceneManager = new CSceneManager(renderSystem);
			_canCreateSceneManager = false;
			
			return sceneManager;
		}
		
		public function getRenderSystem(handle:uint):CRenderSystem
		{
			return _renderSystemPool[handle - 1];
		}
		
		private function init():void
		{
			if(_container.stage)
				onAddedToStage();
			else
				_container.addEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
			
			_renderSystemPool = new Vector.<CRenderSystem>();
			
			_graphicsManagerCreated = true;
		}
		
		private function onContext3DCreate(e:Event):void
		{
			var stage:Stage3D = e.target as Stage3D;
			stage.context3D.enableErrorChecking = false;
			
			_canCreateRenderSystem = true;
			var RenderSystem:CRenderSystem = new CRenderSystem(_totalRenderDataCount, stage.context3D);
			_canCreateRenderSystem = false;
			
			RenderSystem.configureBackBuffer(_viewPort.width, _viewPort.height, _antiAlias);
			
			_renderSystemPool.push(RenderSystem);
			
			dispatchEvent( new CPoolEvent(CPoolEvent.RENDER_SYSTEM_CREATED, _renderSystemPool.length) );
			
			stage.removeEventListener(Event.CONTEXT3D_CREATE, onContext3DCreate);
		}
		
		private function onAddedToStage(e:Event = null):void
		{
			if( _container.hasEventListener(Event.ADDED_TO_STAGE) )
				_container.removeEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
			
			_container.addEventListener(Event.REMOVED_FROM_STAGE, onRemoveFromStage);
			
			_useStage = new Vector.<Boolean>(_lockStageIndex ? _lockStageIndex.length : _container.stage.stage3Ds.length, true);
			
			var length:uint = _useStage.length;
			for(var i:uint = 0; i < length; ++ i)
				_useStage[i] = false;
		}
		
		private function onRemoveFromStage(e:Event):void
		{
			_container.removeEventListener(Event.REMOVED_FROM_STAGE, onRemoveFromStage);
			_container.addEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
			
			_useStage = null;
		}
		
		private function getFreeStage():int
		{
			if(_useStage)
			{
				var length:uint = _useStage.length;
				for(var i:uint = 0; i < length; i ++)
					if(!_useStage[i])
						return _lockStageIndex ? _lockStageIndex[i] : i;
			}
			
			return - 1;
		}
		
		private var _renderSystemPool:Vector.<CRenderSystem>;
		
		private var _container:DisplayObject;
		private var _lockStageIndex:Vector.<uint>;
		private var _useStage:Vector.<Boolean>;
		
		private var _totalRenderDataCount:uint;
		private var _antiAlias:int;
		
		private var _viewPort:Rectangle;
		
		private static var _graphicsManagerCreated:Boolean = false;
		private static var _canCreateRenderSystem :Boolean = false;
		private static var _canCreateSceneManager :Boolean = false;
	}
}