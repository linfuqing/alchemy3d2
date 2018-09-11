package alchemy.resource.extend 
{
	import alchemy.core.CLibrary;
	import alchemy.data.CMeshSegmentData;
	import alchemy.manager.CSceneManager;
	import alchemy.resource.basic.CIndexBuffer3D;
	import alchemy.tools.CDebug;
	
	import flash.utils.ByteArray;

	public class CMesh extends CHighLevelResource
	{
		public  function set indexBuffer(val:CIndexBuffer3D):void
		{
			_indexBuffer = val;

			//CLibrary.method.SetIndexBufferFromMesh(val.pointer);
		}
		
		public function get indexBuffer():CIndexBuffer3D
		{
			return _indexBuffer;
		}
		
		public function addAttribute(val:CMeshSegmentData):void
		{
			//CLibrary.method.AddAttributeToMesh(_pointer,val.pointer);
		}
		
		public function removeAttribute(val:CMeshSegmentData):void
		{
			//CLibrary.method.RemoveAttributeFromMesh(_pointer,val.pointer);
		}
		
		public function computeTangent(uvIndex:uint = 0):Boolean
		{
			var value:int = CLibrary.method.ComputeMeshTangent(_pointer, uvIndex);
			
			return value == 0 ? false : true;
		}
		
		public function computeNormal():Boolean
		{
			var value:int = CLibrary.method.ComputeMeshNormal(_pointer);
			
			return value == 0 ? false : true;
		}
		
		public function lockAttribute(attributeIndex:uint, flag:uint):CMeshSegmentData
		{
			if(flag == 0)
				return _attributeRange;
			
			var datas:Array            = CLibrary.method.GetAttributeDataFromMesh(_pointer, attributeIndex);
			_attributeRange.startIndex = datas[0];
			_attributeRange.indexCount = datas[1];
			
			_attributeIndex = attributeIndex;
			
			return _attributeRange;
		}
		
		public function unlockAttribute():void
		{
			CLibrary.method.SetAttributeDataFromMesh(_pointer, _attributeIndex, _attributeRange.startIndex, _attributeRange.indexCount);
		}
		
		public function CMesh(pointer:uint, handle:uint, interfacePointer:uint, sceneManager:CSceneManager)
		{
			super(pointer, handle, interfacePointer, sceneManager);
			
			init();
		}
		
		public function clone():CMesh
		{
			return null;
		}
		
		override public function destroy():void
		{
			
		}
		
		private function init():void
		{
			var pointers:Array = getPointers();
			
			_indexBufferHandle = pointers[0];
			
			CDebug.assert( ( _indexBuffer = sceneManager.renderSystem.getIndexBuffer(_indexBufferHandle) ) != null, "" );
			
			_attributeRange = new CMeshSegmentData();
		}
		
		private var _indexBuffer:CIndexBuffer3D;
		
		private var _attributeRange:CMeshSegmentData;
		private var _attributeIndex:uint;
		
		private var _indexBufferHandle:uint;
	}
}