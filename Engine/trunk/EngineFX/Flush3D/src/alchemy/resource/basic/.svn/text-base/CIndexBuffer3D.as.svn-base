package alchemy.resource.basic 
{
	import alchemy.core.CLibrary;
	import alchemy.manager.CRenderSystem;
	import alchemy.tools.CDebug;
	
	import flash.display3D.IndexBuffer3D;
	import flash.utils.ByteArray;

	public class CIndexBuffer3D extends CLowLevelResource
	{
		public function get instance():IndexBuffer3D
		{
			return _indexBuffer;
		}
		
		public function get numIndices():uint
		{
			return _numIndices;
		}
		
		public function CIndexBuffer3D(pointer:uint, handle:uint, interfacePointer:uint, renderSystem:CRenderSystem)
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
				//CLibrary.memory.position = 0;
				CLibrary.memory.position = _dataPointer;
				var dataPointer:uint = CLibrary.memory.readUnsignedInt();
				CLibrary.memory.position = dataPointer;
				
				
				if(dataPointer)
					_indexBuffer.uploadFromByteArray(CLibrary.memory, dataPointer, startDirty, endDirty - startDirty + 1);
			}
		}
		
		private function init():void
		{
			var pointers:Array = getPointers();
			_dataPointer       = pointers[0];
			_startDirtyPointer = pointers[1];
			_endDirtyPointer   = pointers[2];
			
			_numIndices        = pointers[3];
			
			try
			{
				_indexBuffer = renderSystem.context.createIndexBuffer(_numIndices);
			}
			catch(e:Error)
			{
				CDebug.error(e.message);
			}
		}
		
		private var _indexBuffer:IndexBuffer3D;
		
		private var _dataPointer      :uint;
		private var _startDirtyPointer:uint;
		private var _endDirtyPointer  :uint;
		
		private var _numIndices       :uint;
	}
}