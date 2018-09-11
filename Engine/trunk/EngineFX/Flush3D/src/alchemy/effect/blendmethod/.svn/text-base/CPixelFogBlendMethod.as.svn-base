package alchemy.effect.blendmethod
{
	import alchemy.core.CLibrary;
	import alchemy.tools.CDebug;

	public class CPixelFogBlendMethod extends CAbstractBlendMethod
	{
		static public var LINEAR_FOG:uint = 0;
		static public var EXP:uint = 1;
		static public var EXP2:uint = 2;
		
		public function CPixelFogBlendMethod( start:Number, end:Number )
		{
			_fogEndPointer = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE );
			_fogDistancePointer = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE );
			
			if(start >= end)
				end = start + 1.0;
			
			_fogStart = start;
			_fogEnd = end;
			
			CLibrary.memory.position = _fogEndPointer;
			CLibrary.memory.writeFloat(end);
			
			CLibrary.memory.position = _fogDistancePointer;
			CLibrary.memory.writeFloat(end-start);
			
			var ptrs:Array = CLibrary.method.CreatePixelFogBlend(LINEAR_FOG, _fogEndPointer, _fogDistancePointer, null);
			CDebug.assert(ptrs != null, "CPixelFogBlendMethod create error!" );
			
			super( ptrs[0] );
			
			_hostPointer = ptrs[1];
		}
		
		public function set fogStart( val:Number ):void
		{
			if(_fogDistancePointer != 0)
			{
				_fogStart = val;
				CLibrary.memory.position = _fogDistancePointer;
				CLibrary.memory.writeFloat( _fogEnd - _fogStart );
			}
		}
		
		public function get fogStart():Number
		{
			return _fogEnd-_fogStart;
		}
		
		public function set fogEnd( val:Number ):void
		{
			if(_fogEndPointer != 0)
			{
				_fogEnd = val;
				CLibrary.memory.position = _fogEndPointer;
				CLibrary.memory.writeFloat(_fogEnd);
			}
		}
		
		public function get fogEnd():Number
		{
			return _fogEnd;
		}
		
		override public function destroy():void
		{
			if(_fogEndPointer != 0)
			{
				CLibrary.method.DestroyBuffer(_fogEndPointer);
				_fogEndPointer = 0;
			}
			
			if(_fogDistancePointer != 0)
			{
				CLibrary.method.DestroyBuffer(_fogDistancePointer);
				_fogDistancePointer = 0;
			}
		}
		
		private var _fogStart:Number;
		private var _fogEnd:Number;
		
		private var _hostPointer:uint;
		private var _fogEndPointer:uint;
		private var _fogDistancePointer:uint;
	}
}