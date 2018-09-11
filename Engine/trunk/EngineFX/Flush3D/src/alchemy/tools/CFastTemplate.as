package alchemy.tools
{
	import alchemy.event.CPoolEvent;
	import alchemy.manager.CGraphicsManager;
	import alchemy.manager.CRenderSystem;
	import alchemy.manager.CSceneManager;
	import alchemy.scene.CCamera;
	
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.display3D.Context3D;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.utils.Dictionary;
	
	public class CFastTemplate extends Sprite
	{
		public var forwardAcceleration:Number = 5;
		public var upAcceleration:Number = 10;
		public var rightAcceleration:Number = 10;
		
		public var friction:Number = 0.95;
		
		public var mouseInterpolation:Number = 0.05;
		public var mouseMoveX:Number = 0.01;
		public var mouseMoveY:Number = 0.01;
		
		public var elapse:Number = 0.08;
		
		public var enbleAutoCamera:Boolean = true;
		
		public function get graphicsManager():CGraphicsManager
		{
			return _graphicsManager;
		}
		
		public function get sceneManager():CSceneManager
		{
			return _sceneManager;
		}
		
		public function get renderSystem():CRenderSystem
		{
			return _renderSystem;
		}
		
		public function get camera():CCamera
		{
			return _camera;
		}
		
		public function set enbleScene(value:Boolean):void
		{
			if(stage)
			{
				if(value)
					stage.addEventListener(Event.ENTER_FRAME, onEnterFrame);
				else
					stage.removeEventListener(Event.ENTER_FRAME, onEnterFrame);
			}
		}
		
		public function CFastTemplate(
			fov:Number, 
			nearPlane:Number, 
			farPlane:Number, 
			totalRenderDataCount:uint = 1, 
			antiAlias:int = 0,
			maxRenderEntries:uint = 1)
		{
			_fov                  = fov;
			_nearPlane            = nearPlane;
			_farPlane             = farPlane;
			
			_antiAlias            = antiAlias;
			
			_totalRenderDataCount = totalRenderDataCount;
			_maxRenderEntries     = maxRenderEntries;
			
			_graphicsManager      = null;
			_renderSystem         = null;
			_sceneManager         = null;
			
			_forwardSpeed         = 0;
			_upSpeed              = 0;
			_rightSpeed           = 0;
			
			addEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
			
			super();
		}
		
		public function checkKey(keyCode:uint):Boolean
		{
			return _keyCode ? (_keyCode[keyCode] ? true : false) : false;
		}
		
		protected function init():void
		{
			
		}
		
		protected function beginScene():void
		{
			
		}
		
		protected function endScene():void
		{
			
		}
		
		private function onAddedToStage(e:Event):void
		{
			removeEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align     = StageAlign.TOP_LEFT;
			
			_view = new Rectangle(0, 0, stage.stageWidth, stage.stageHeight);
			
			_graphicsManager = new CGraphicsManager(this);
			_graphicsManager.addEventListener(CPoolEvent.RENDER_SYSTEM_CREATED, onCreateOK);
			
			_graphicsManager.createRenderSystem(_totalRenderDataCount, _view, _antiAlias);
			
			stage.addEventListener(Event.RESIZE, onResize);
		}
		
		private function onResize(e:Event):void
		{
			if(stage.stageWidth && stage.stageHeight)
			{
				if(_renderSystem)
				{
					_renderSystem.configureBackBuffer(stage.stageWidth, stage.stageHeight, _antiAlias);
					
					var aspect:Number = stage.stageWidth / stage.stageHeight;
					
					_camera.setProjection(_fov, _nearPlane, _farPlane, aspect);
				}
				else
				{
					_view.width  = stage.stageWidth;
					_view.height = stage.stageHeight;
				}
			}
		}
		
		private function onCreateOK(e:CPoolEvent):void
		{
			_graphicsManager.removeEventListener(CPoolEvent.RENDER_SYSTEM_CREATED, onCreateOK);
			
			_renderSystem = _graphicsManager.getRenderSystem(1);
			
			_sceneManager = _graphicsManager.createSceneManager(_renderSystem);
			
			_camera = _sceneManager.camera;
			
			var aspect:Number = _view.width / _view.height;
			
			_camera.setProjection(_fov, _nearPlane, _farPlane, aspect);
			
			_mouseStartPoint = new Point;
			_mouseEndPoint   = new Point;
			
			_keyCode = new Dictionary();
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
			stage.addEventListener(KeyboardEvent.KEY_UP, onKeyUp);
			
			stage.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDown);
			stage.addEventListener(MouseEvent.MOUSE_UP, onMouseUp);
			stage.addEventListener(MouseEvent.MOUSE_WHEEL, onMouseWheel);
			
			init();
		}
		
		private function onMouseDown(e:MouseEvent):void
		{
			_mouseStartPoint.x = mouseX;
			_mouseStartPoint.y = mouseY;
			
			_mouseBegin = true;
		}
		
		private function onMouseUp(e:MouseEvent):void
		{
			_mouseBegin = false;
		}
		
		private function onMouseWheel(e:MouseEvent):void
		{
			_forwardSpeed = e.delta * forwardAcceleration;
		}
		
		protected function onKeyDown(e:KeyboardEvent):void
		{
			_keyCode[e.keyCode] = true;
			/*switch(e.keyCode)
			{
				case 87:
					_key[0] = true;
					break;
				
				case 65:
					_key[1] = true;
					break;
				
				case 68:
					_key[2] = true;
					break;
				
				case 83:
					_key[3] = true;
					break;
				
				case _keyCode:
					_key[4] = true;
					break;
			}*/
		}
		
		protected function onKeyUp(e:KeyboardEvent):void
		{
			_keyCode[e.keyCode] = false;
			/*switch(e.keyCode)
			{
				case 87:
					_key[0] = false;
					break;
				
				case 65:
					_key[1] = false;
					break;
				
				case 68:
					_key[2] = false;
					break;
				
				case 83:
					_key[3] = false;
					break;
				
				case _keyCode:
					_key[4] = true;
					break;
			}*/
		}
		
		private function onEnterFrame(e:Event=null):void
		{
			beginScene();
			
			if(enbleAutoCamera)
			{
				if(_keyCode[87])
					_upSpeed = upAcceleration;
				
				if(_keyCode[65])
					_rightSpeed = - rightAcceleration;
				
				if(_keyCode[68])
					_rightSpeed = rightAcceleration;
				
				if(_keyCode[83])
					_upSpeed = - upAcceleration;
				
				_camera.right(_rightSpeed);
				_camera.up(_upSpeed);
				_camera.forward(_forwardSpeed);
				
				_rightSpeed *= friction;
				_upSpeed *= friction;
				_forwardSpeed *= friction;
				
				if(_mouseBegin)
				{
					_mouseEndPoint.x = mouseX;
					_mouseEndPoint.y = mouseY;
				}
				
				var point:Point = _mouseEndPoint.subtract(_mouseStartPoint);
				
				_camera.rotateX += point.y * mouseMoveX;
				_camera.rotateY += point.x * mouseMoveY;
				
				point.x *= mouseInterpolation;
				point.y *= mouseInterpolation;
				
				_mouseStartPoint = _mouseStartPoint.add(point);
			}
			
			var context:Context3D = _renderSystem.context;
			
			_sceneManager.render(elapse);
			
			try
			{
				context.present();
			}
			catch(error:Error)
			{
				CDebug.warning(error.message);
			}
			
			endScene();
		}
		
		private var _fov:Number;
		private var _nearPlane:Number;
		private var _farPlane:Number;
		
		private var _totalRenderDataCount:uint;
		private var _antiAlias:int;
		
		private var _maxRenderEntries:uint;
		
		private var _graphicsManager:CGraphicsManager;
		private var _renderSystem:CRenderSystem;
		private var _sceneManager:CSceneManager;
		private var _camera:CCamera;
		
		private var _mouseStartPoint:Point;
		private var _mouseEndPoint:Point;
		private var _mouseBegin:Boolean;
		
		private var _forwardSpeed:Number;
		private var _upSpeed:Number;
		private var _rightSpeed:Number;
		
		private var _keyCode:Dictionary;
		
		private var _view:Rectangle;
	}
}