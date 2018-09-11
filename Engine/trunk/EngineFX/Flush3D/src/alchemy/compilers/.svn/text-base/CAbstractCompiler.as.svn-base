package alchemy.compilers
{
	import alchemy.core.CLibrary;
	import alchemy.core.CObject;
	import alchemy.effect.CEffectCode;
	import alchemy.manager.CRenderSystem;
	import alchemy.manager.CSceneManager;
	import alchemy.tools.CDebug;
	
	import flash.utils.ByteArray;

	public class CAbstractCompiler extends CObject
	{
		static public function get canCreateEffectCode():Boolean
		{
			return _canCreateEffectCode;
		}
		
		public function CAbstractCompiler()
		{
			var pointers:Array = initPointers();
			
			CDebug.assert(pointers != null, "Abstract class can not be created.");
			
			_pointer = pointers[0];
			
			super(pointers[1]);
			
			_interfacePointer = pointers[2];
			
			init();
		}
		
		public function createEffectCode():CEffectCode
		{
			if(_interfacePointer)
			{
				_canCreateEffectCode = true;
				var pointers:Array = CLibrary.method.CreateEffectCode(_interfacePointer);
				var effectCode:CEffectCode = new CEffectCode(pointers[0], pointers[1]);
				_canCreateEffectCode = false;
				
				return effectCode;
			}
			
			CDebug.warning("No compiler interface pointer.");
			
			return null;
		}
		
		public function createSkyEffectCode( offsetPointer:uint = 0 ):CEffectCode
		{
			if(_interfacePointer)
			{
				_canCreateEffectCode = true;
				var pointers:Array = CLibrary.method.CreateSkyPassDefiner(_interfacePointer, offsetPointer);
				var effectCode:CEffectCode = new CEffectCode(pointers[0], pointers[1]);
				_canCreateEffectCode = false;
				
				return effectCode;
			}
			
			CDebug.warning("No compiler interface pointer.");
			
			return null;
		}
		
		public function createEffectCodeFromDeferredRenderingPipeline(SceneManager:CSceneManager):CEffectCode
		{
			if(SceneManager && _interfacePointer)
			{
				_canCreateEffectCode = true;
				var pointers:Array = CLibrary.method.CreateEffectCodeFromDeferredRenderingPipeline(_interfacePointer, SceneManager.pointer);
				var effectCode:CEffectCode = new CEffectCode(pointers[0], pointers[1]);
				_canCreateEffectCode = false;
				
				return effectCode;
			}
			
			CDebug.warning("No compiler interface pointer.");
			
			return null;
		}
		
		public function createProgram(renderSystem:CRenderSystem, vertexCode:String, fragmentCode:String):uint
		{
			return CLibrary.method.CreateProgram3D(renderSystem.pointer, _interfacePointer, vertexCode, fragmentCode);
		}
		
		public function getSkyPlanePointers(
			length:Number,
			width:Number,
			height:Number,
			texture:uint,
			ambientLight:Boolean,
			emissive:Boolean,
			UVOffset:Boolean):Array
		{
			return CLibrary.method.CreateSkyPlane(length, width, height, texture, _interfacePointer, ambientLight, emissive, UVOffset);
		}

		protected function initPointers():Array
		{
			return null;
		}
		
		protected function compile(source:String, type:int):ByteArray
		{
			return null;
		}
		
		private function init():void
		{
			fill(compile);
		}
		
		private var _interfacePointer:uint;
		
		static private var _canCreateEffectCode:Boolean = false;
	}
}