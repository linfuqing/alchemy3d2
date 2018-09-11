package alchemy.scene
{
	import alchemy.compilers.CAbstractCompiler;
	import alchemy.core.CLibrary;
	import alchemy.manager.CMeshManager;
	import alchemy.manager.CSceneManager;
	
	import flash.geom.ColorTransform;

	public class CSkyPlane extends CSceneNode
	{
		public function set UVOffsetX(value:Number):void
		{
			CLibrary.memory.position = _UVOffsetSpeedXPointer;
			CLibrary.memory.writeFloat(value);
		}
		
		public function get UVOffsetX():Number
		{
			CLibrary.memory.position = _UVOffsetSpeedXPointer;
			return CLibrary.memory.readFloat();
		}
		
		public function set UVOffsetY(value:Number):void
		{
			CLibrary.memory.position = _UVOffsetSpeedYPointer;
			CLibrary.memory.writeFloat(value);
		}
		
		public function get UVOffsetY():Number
		{
			CLibrary.memory.position = _UVOffsetSpeedYPointer;
			return CLibrary.memory.readFloat();
		}
		
		public function set emissive(value:ColorTransform):void
		{
			CLibrary.memory.position = _emissivePointer;
			CLibrary.memory.writeFloat(value.redMultiplier);
			CLibrary.memory.writeFloat(value.greenMultiplier);
			CLibrary.memory.writeFloat(value.blueMultiplier);
			CLibrary.memory.writeFloat(value.alphaMultiplier);
		}
		
		public function CSkyPlane(
			length:Number,
			width:Number,
			height:Number,
			texture:uint,
			ambientLight:Boolean,
			emissive:Boolean,
			UVOffset:Boolean)
		{
			_length       = length;
			_width        = width;
			_height       = height;
			_texture      = texture;
			_ambientLight = ambientLight;
			_emissive     = emissive;
			_UVOffset     = UVOffset;
			
			super(0, 0, 0);
		}
		
		override protected function initPointers(pts:Array):uint
		{
			var index:uint = super.initPointers(pts);
			
			_UVOffsetSpeedXPointer = pts[index ++];
			_UVOffsetSpeedYPointer = pts[index ++];
			_emissivePointer       = pts[index ++];
			
			return index;
		}
		
		override protected function getSceneNodePointers():Array
		{
			CMeshManager.instance;
			
			var ptrs:Array = CLibrary.method.CreateSkyPlane(_length, _width, _height, _texture, _ambientLight, _emissive, _UVOffset);
			
			return ptrs;
		}
		
		private var _length:Number;
		private var _width:Number;
		private var _height:Number;
		private var _texture:uint;
		private var _ambientLight:Boolean;
		private var _emissive:Boolean;
		private var _UVOffset:Boolean;
		
		private var _UVOffsetSpeedXPointer:uint;
		private var _UVOffsetSpeedYPointer:uint;
		
		private var _emissivePointer:uint;
	}
}