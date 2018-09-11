package alchemy.utils
{
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.system.System;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;
	import flash.utils.getTimer;

	public class AlchemyFPS extends Sprite
	{
		private static var instance:AlchemyFPS;
		private static const maxMemory:uint = uint.MAX_VALUE;
		private static const diagramWidth:uint = 90;
		private static const tfDelayMax:int = 10;
		private static const diagramHeight:uint = 40;
		
		private const skinsChanged:int = 0;
		private const hgap:int = 2;
		private const textHeight:int = 10;

		public function AlchemyFPS(renderInfo:AlchemyRenderInfo = null, autoResize:Boolean = true)
		{
			_renderInfo = renderInfo;
			_autoResize = autoResize;
			
			this.addEventListener(Event.ADDED_TO_STAGE, run);
			this.addEventListener(Event.REMOVED_FROM_STAGE, stop);
		}

		private function run(ev:Event):void
		{
			if (instance == null)
			{
				fps = new TextField();
				mem = new TextField();

				mouseEnabled = false;
				mouseChildren = false;

				var tf:TextFormat = new TextFormat("Tahoma", 10, 0xcccccc);

				fps.defaultTextFormat = tf;
				fps.autoSize = TextFieldAutoSize.LEFT;
				fps.text = "FPS: " + Number(stage.frameRate).toFixed(2);
				fps.selectable = false;
				fps.x = -diagramWidth - 2;
				addChild(fps);

				currentY += textHeight + hgap;

				mem.defaultTextFormat = tf;
				mem.autoSize = TextFieldAutoSize.LEFT;
				mem.text = "MEM: " + bytesToString(System.totalMemory);
				mem.selectable = false;
				mem.x = -diagramWidth - 2;
				mem.y = currentY;
				addChild(mem);

				currentY += textHeight + hgap;
				
				if(_renderInfo)
				{
					_drawCount = new TextField();
					_drawCount.defaultTextFormat = tf;
					_drawCount.autoSize = TextFieldAutoSize.LEFT;
					_drawCount.text = "DRAW: " + _renderInfo.drawCount;
					_drawCount.selectable = false;
					_drawCount.x = - diagramWidth - 2;
					_drawCount.y = currentY;
					addChild(_drawCount);
					
					currentY += textHeight + hgap;
					
					_faceCount = new TextField();
					_faceCount.defaultTextFormat = tf;
					_faceCount.autoSize = TextFieldAutoSize.LEFT;
					_faceCount.text = "FACES: " + _renderInfo.faceCount;
					_faceCount.selectable = false;
					_faceCount.x = - diagramWidth - 2;
					_faceCount.y = currentY;
					addChild(_faceCount);
					
					currentY += textHeight + hgap;
					
					_driverInfo = new TextField();
					_driverInfo.defaultTextFormat = tf;
					_driverInfo.autoSize = TextFieldAutoSize.LEFT;
					_driverInfo.text = "DRIVER INFO: " + _renderInfo.driverInfo;
					_driverInfo.selectable = false;
					_driverInfo.x = - diagramWidth - 2;
					_driverInfo.y = currentY;
					addChild(_driverInfo);
					
					currentY += textHeight + hgap;
				}

//				if (scene)
//				{
//					objects = new TextField();
//					objects.defaultTextFormat = tf;
//					objects.autoSize = TextFieldAutoSize.LEFT;
//					objects.text = "OBJECTS: " + scene.nNodes;
//					objects.selectable = false;
//					objects.x = -diagramWidth - 2;
//					objects.y = currentY;
//					addChild(objects);
//					
//					currentY += textHeight + hgap;
//					
//					faces = new TextField();
//					faces.defaultTextFormat = tf;
//					faces.autoSize = TextFieldAutoSize.LEFT;
//					faces.text = "TRIANGLES: " + scene.nFaces;
//					faces.selectable = false;
//					faces.x = -diagramWidth - 2;
//					faces.y = currentY;
//					addChild(faces);
//					
//					currentY += textHeight + hgap;
//					
//					rendered = new TextField();
//					rendered.defaultTextFormat = tf;
//					rendered.autoSize = TextFieldAutoSize.LEFT;
//					rendered.text = "TRI RENDERED: " + scene.nRenderList;
//					rendered.selectable = false;
//					rendered.x = -diagramWidth - 2;
//					rendered.y = currentY;
//					addChild(rendered);
//
//					currentY += textHeight + hgap;
//
//					culled = new TextField();
//					culled.defaultTextFormat = tf;
//					culled.autoSize = TextFieldAutoSize.LEFT;
//					culled.text = "TRI CULLED: " + scene.nCullList;
//					culled.selectable = false;
//					culled.x = -diagramWidth - 2;
//					culled.y = currentY;
//					addChild(culled);
//
//					currentY += textHeight + hgap;
//
//					clipped = new TextField();
//					clipped.defaultTextFormat = tf;
//					clipped.autoSize = TextFieldAutoSize.LEFT;
//					clipped.text = "TRI CLIPPED: " + scene.nClippList;
//					clipped.selectable = false;
//					clipped.x = -diagramWidth - 2;
//					clipped.y = currentY;
//					addChild(clipped);
//
//					currentY += textHeight + hgap;
//				}

				diagram = new BitmapData(diagramWidth, diagramHeight, true, 0x10ffffff);
				var bmp:Bitmap = new Bitmap(diagram);
				bmp.y = currentY + hgap;
				bmp.x = -diagramWidth;
				addChildAt(bmp, 0);
				
				diagramTimer = getTimer();
				tfTimer = getTimer();
				
				instance = this;
			}
			
			addEventListener(Event.ENTER_FRAME, onEnterFrame);
			
			if(_autoResize)
			{
				stage.addEventListener(Event.RESIZE, onResize);
				
				onResize();
			}
		}
		
		private function stop(e:Event):void
		{
			removeEventListener(Event.ENTER_FRAME, onEnterFrame);
			
			if(_autoResize)
				stage.removeEventListener(Event.RESIZE, onResize);
		}

		private function bytesToString(value:uint):String
		{
			var str:String;
			if (value < 1024)
			{
				str = String(value) + "b";
			}
			else if (value < 10240)
			{
				str = Number(value / 1024).toFixed(2) + "kb";
			}
			else if (value < 102400)
			{
				str = Number(value / 1024).toFixed(1) + "kb";
			}
			else if (value < 1048576)
			{
				str = (value / 1024 >> 0) + "kb";
			}
			else if (value < 10485760)
			{
				str = Number(value / 1048576).toFixed(2) + "mb";
			}
			else if (value < 104857600)
			{
				str = Number(value / 1048576).toFixed(1) + "mb";
			}
			else
			{
				str = (value / 1048576 >> 0) + "mb";
			}
			return str;
		}

		private function onEnterFrame(e:Event):void
		{
			tfDelay++;
			
			if (tfDelay >= tfDelayMax)
			{
				tfDelay = 0;
				fps.text = "FPS: " + Number(1000 * tfDelayMax / (getTimer() - tfTimer)).toFixed(2);
				tfTimer = getTimer();
			}
			
			var t:Number = 1000 / (getTimer() - diagramTimer);
			var f:Number = t > stage.frameRate ? (1) : (t / stage.frameRate);
			diagramTimer = getTimer();
			diagram.scroll(1, 0);
			diagram.fillRect(new Rectangle(0, 0, 1, diagram.height), 0x10ffffff);
			diagram.setPixel32(0, diagramHeight * (1 - f), 0xffcccccc);

			mem.text = "MEM: " + bytesToString(System.totalMemory);
			
//			if (scene) objects.text = "OBJECTS: " + scene.nNodes;
//			if (scene) faces.text = "TRIANGLES: " + scene.nFaces;
//			if (scene) rendered.text = "TRI RENDERED: " + scene.nRenderList;
//			if (scene) culled.text = "TRI CULLED: " + scene.nCullList;
//			if (scene) clipped.text = "TRI CLIPPED: " + scene.nClippList;
			
			if(_renderInfo)
			{
				_drawCount.text = "DRAW: " + _renderInfo.drawCount;
				_faceCount.text = "FACES: " + _renderInfo.faceCount;
				
				_renderInfo.reset();
			}

			var s:Number = skins == 0 ? (0) : (skinsChanged / skins);
			diagram.setPixel32(0, diagramHeight * (1 - s), 0xffff6600);
			var m:Number = System.totalMemory / maxMemory;
			diagram.setPixel32(0, diagramHeight * (1 - m), 0xffcccc00);
		}

		private function onResize(e:Event = null):void
		{
			var p:Point = parent.globalToLocal(new Point(this.width, 0));
			x = p.x;
			y = p.y;
		}
		
		private var currentY:int;
		private var diagramTimer:int;
		private var tfTimer:int;
		
		private var fps:TextField;
		private var mem:TextField;
		/*private var objects:TextField;
		private var faces:TextField;
		private var rendered:TextField;
		private var culled:TextField;
		private var clipped:TextField;*/
		private var _driverInfo:TextField;
		private var _drawCount:TextField;
		private var _faceCount:TextField;
		
		private var diagram:BitmapData;
		private var skins:int = -1;
		private var tfDelay:int = 0;
		
		private var _autoResize:Boolean;
		
		private var _renderInfo:AlchemyRenderInfo;
	}
}
