package com.adobe.flascc
{
	import com.adobe.flascc.vfs.ISpecialFile;
	
	import flash.display.DisplayObjectContainer;
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.text.TextField;
	import flash.ui.Keyboard;

  /**
  * A basic implementation of a console for flascc apps.
  * The PlayerKernel class delegates to this for things like read/write
  * so that console output can be displayed in a TextField on the Stage.
  */
	public class Console extends Sprite implements ISpecialFile
	{
		/**
		 * The callback to call when flascc code calls the posix exit() function. Leave null to exit silently.
		 * @private
		 */
		public var exitHook:Function;
		
		/**
		 * Provide a way to get the TextField's text.
		 */
		public function get consoleText():String
		{
			return _textField ? _textField.text : null;
		}
		
		/**
		* To Support the preloader case you might want to have the Console
		* act as a child of some other DisplayObjectContainer.
		*/
		public function Console(container:DisplayObjectContainer = null)
		{
			CModule.rootSprite = container ? container.root : this;
			
			if(container)
			{
				container.addChild(this);
				onAddedToStage(null);
			} 
			else
				addEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
		}
		
		/**
		* All of the real flascc init happens in this method
		* which is either run on startup or once the SWF has
		* been added to the stage.
		*/
		private function onAddedToStage(e:Event):void
		{
			_inputContainer = new Sprite();
			addChild(_inputContainer);
			
			stage.frameRate = 60;
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align     = StageAlign.TOP_LEFT;
			
			_textField = new TextField();
			_textField.multiline = true;
			_textField.width = stage.stageWidth;
			_textField.height = stage.stageHeight;
			_textField.background = true;
			//_textField.backgroundColor = 0xFFFFFF;
			_textField.visible = false;
			_inputContainer.addChild(_textField);

			addEventListener(Event.ENTER_FRAME, onEnterFrame);
			
			stage.addEventListener(Event.RESIZE, onResize);
			stage.addEventListener(KeyboardEvent.KEY_UP, onKeyUp);
			
			CModule.vfs.console = this;
			//CModule.startBackground(this);
			CModule.startAsync(this);

			CModule.callI( CModule.getPublicSymbol("FlasccMain"), new Vector.<int>() );
		}
		
		/**
		* The PlayerKernel implementation will use this function to handle
		* C process exit requests
		*/
		public function exit(code:int):Boolean
		{
			// default to unhandled
			return exitHook ? exitHook(code) : false;
		}
		
		/**
		* The PlayerKernel implementation will use this function to handle
		* C IO write requests to the file "/dev/tty" (e.g. output from
		* printf will pass through this function). See the ISpecialFile
		* documentation for more information about the arguments and return value.
		*/
		public function write(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
		{
			var str:String = CModule.readString(bufPtr, nbyte)
			consoleWrite(str);
			return nbyte;
		}
		
		/**
		* The PlayerKernel implementation will use this function to handle
		* C IO read requests to the file "/dev/tty" (e.g. reads from stdin
		* will expect this function to provide the data). See the ISpecialFile
		* documentation for more information about the arguments and return value.
		*/
		public function read(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
		{
			return 0;
		}
		
		/**
		* The PlayerKernel implementation will use this function to handle
		* C fcntl requests to the file "/dev/tty" 
		* See the ISpecialFile documentation for more information about the
		* arguments and return value.
		*/
		public function fcntl(fd:int, com:int, data:int, errnoPtr:int):int
		{
			return 0;
		}
		
		/**
		* The PlayerKernel implementation will use this function to handle
		* C ioctl requests to the file "/dev/tty" 
		* See the ISpecialFile documentation for more information about the
		* arguments and return value.
		*/
		public function ioctl(fd:int, com:int, data:int, errnoPtr:int):int
		{
			return 0;
		}
		
		/**
		* Helper function that traces to the flashlog text file and also
		* displays output in the on-screen textfield console.
		*/
		public function consoleWrite(s:String):void
		{
			if(_textField)
			{
				_textField.appendText(s);
				_textField.scrollV = _textField.maxScrollV;
			}
		}
		
		/**
		* The enterFrame callback will be run once every frame. UI thunk requests should be handled
		* here by calling CModule.serviceUIRequests() (see CModule ASdocs for more information on the UI thunking functionality).
		*/
		private function onEnterFrame(e:Event):void
		{
			CModule.serviceUIRequests();
		}
		
		private function onResize(e:Event):void
		{
			_textField.width = stage.stageWidth;
			_textField.height = stage.stageHeight;
		}
		
		private function onKeyUp(e:KeyboardEvent):void
		{
			switch(e.keyCode)
			{
				case Keyboard.BACKQUOTE:
					_textField.visible = !_textField.visible;
					break;
			}
		}
		
		private var _inputContainer:DisplayObjectContainer;
		private var _textField:TextField;
	}
}
