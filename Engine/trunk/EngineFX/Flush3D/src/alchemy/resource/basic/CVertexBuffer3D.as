package alchemy.resource.basic 
{
	import alchemy.core.CLibrary;
	import alchemy.manager.CRenderSystem;
	import alchemy.tools.CDebug;
	
	import flash.display3D.Context3DVertexBufferFormat;
	import flash.display3D.VertexBuffer3D;
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	public class CVertexBuffer3D extends CLowLevelResource
	{
		public function get instance():VertexBuffer3D
		{
			return _vertexBuffer;
		}
		
		public function get numVertices():uint
		{
			return _numVertices;
		}
		
		public function get data32PerVertex():uint
		{
			return _data32PerVertex;
		}
		
		public function writeDatas(data:ByteArray):void
		{
			if(data != null)
			{
				var pointer:uint = CLibrary.method.CreateBuffer(data.length);
				
				CLibrary.memory.position = pointer;
				CLibrary.memory.writeBytes(data);
				
				CLibrary.method.WriteDataToVertexBuffer(_pointer,pointer,data.length);
				
				CLibrary.method.DestroyBuffer(pointer);
			}
		}
		
		public function CVertexBuffer3D(pointer:uint, handle:uint, interfacePointer:uint, renderSystem:CRenderSystem)
		{
			super(pointer, handle, interfacePointer, renderSystem);

			init();
		}
		
		public function updata():void
		{
			var startDirty:uint;
			var endDirty:uint;
			
			CLibrary.memory.position = _startDirtyPointer;
			startDirty = CLibrary.memory.readUnsignedInt();
			
			CLibrary.memory.position = _endDirtyPointer;
			endDirty = CLibrary.memory.readUnsignedInt();
			
			if(startDirty < endDirty)
			{
//				CLibrary.memory.position = 0;
				CLibrary.memory.position = _dataPointer;
				var dataPointer:uint = CLibrary.memory.readUnsignedInt();
				if(dataPointer)
				{
					CLibrary.memory.position = dataPointer;
					
//					for(var i:int=0; i< 8; ++i)
//					{
//						trace("&&&&&&&&&&&&&&&&&&");
//						
//						trace( CLibrary.memory.readFloat() );
//						trace( CLibrary.memory.readFloat() );
//						trace( CLibrary.memory.readFloat() );
//						trace( CLibrary.memory.readFloat() );
//					}
//					
//					trace("&&&&&&&&&END&&&&&&&&&");
								
					_vertexBuffer.uploadFromByteArray(CLibrary.memory, dataPointer, startDirty, endDirty - startDirty + 1);
				}
			}
		}
		
		private function init():void
		{
			var pointers:Array = getPointers();
			_dataPointer       = pointers[0];
			_startDirtyPointer = pointers[1];
			_endDirtyPointer   = pointers[2];
			
			_numVertices       = pointers[3];
			_data32PerVertex   = pointers[4];

			try
			{
				_vertexBuffer = renderSystem.context.createVertexBuffer(_numVertices, _data32PerVertex);
			}
			catch(e:Error)
			{
				CDebug.error(e.message);
			}
		}
		
		private var _vertexBuffer:VertexBuffer3D;
		
		private var _dataPointer      :uint;
		private var _startDirtyPointer:uint;
		private var _endDirtyPointer  :uint;
		private var _dirtyPointer		:uint
		
		private var _numVertices      :uint;
		private var _data32PerVertex  :uint;
	}
}