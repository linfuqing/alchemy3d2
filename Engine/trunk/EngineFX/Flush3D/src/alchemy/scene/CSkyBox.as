package alchemy.scene
{
	import alchemy.compilers.CAbstractCompiler;
	import alchemy.core.CLibrary;
	import alchemy.manager.CSceneManager;

	public class CSkyBox extends CSceneNode
	{
		public function CSkyBox(width:Number, height:Number, depth:Number, texture:uint)
		{
			_width = width;
			_height = height;
			_depth = depth;
			_texture = texture;
			
			super(0, 0, 0);
		}
		
		public function get width():Number
		{
			return _width;
		}
		
		public function get height():Number
		{
			return _height;
		}
		
		public function get depth():Number
		{
			return _depth;
		}
		
		override protected function initPointers(pts:Array):uint
		{
			var index:uint = super.initPointers(pts);
			
			return index;
		}
		
		override protected function getSceneNodePointers():Array
		{
			var ptrs:Array = CLibrary.method.CreateSkyBox(_width, _height, _depth, _texture);
			return ptrs;
		}
		
		protected var _texture:uint;
		protected var _width:Number;
		protected var _height:Number;
		protected var _depth:Number;
		
		protected var _renderInfoPointer:uint;
	}
}