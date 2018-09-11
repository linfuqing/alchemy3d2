package alchemy.effect
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;
	
	import flash.geom.Vector3D;

	public class CPixelColorEffect extends CAbstractEffectInfo
	{
		public function CPixelColorEffect( color:Vector3D )
		{
			CDebug.assert(color != null, "color null!" );
			
			_colorPointer = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE * 4 );
			
			CLibrary.memory.position = _colorPointer;
			CLibrary.memory.writeFloat( color.x );
			CLibrary.memory.writeFloat( color.y );
			CLibrary.memory.writeFloat( color.z );
			CLibrary.memory.writeFloat( color.w );
			
			var ptrs:Array = CLibrary.method.CreatePixelColorEffect( _colorPointer );
			
			CDebug.assert(ptrs != null, "CPixelColorEffect create error!" );
			
			super(ptrs[0]);
			
			_hostPointer = ptrs[1];
		}
		
		override public function destroy():void
		{
			if(_colorPointer != 0)
				CLibrary.method.DestroyBuffer( _colorPointer );
			
			_colorPointer = 0;
		}
		
		private var _hostPointer:uint;
		private var _colorPointer:uint;
	}
}