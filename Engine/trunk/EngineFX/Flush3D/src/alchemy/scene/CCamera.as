package alchemy.scene 
{
	import alchemy.core.CLibrary;
	import alchemy.data.CSphereData;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;

	public class CCamera extends CSceneNode
	{
		public function CCamera(pointer:uint = 0, interfacePointer:uint = 0, sceneNodePointer:uint = 0)
		{
			super(pointer, interfacePointer, sceneNodePointer);
		}
		
		override protected function initPointers(pts:Array):uint
		{
			var index:uint   = super.initPointers(pts);
			
			_viewMatrixPointer = pts[index++];
			_projMatrixPointer = pts[index++];
			
			return index;
		}
		
		public function get viewMatrix():Matrix3D
		{
			var rawData:Vector.<Number> = new Vector.<Number>(16,true);
			CLibrary.memory.position = _viewMatrixPointer;
			
			for(var i:int=0; i<16; ++i)
				rawData[i] = CLibrary.memory.readFloat();
			
			var mat:Matrix3D = new Matrix3D(rawData);
			
			return mat;
		}
		
		public function get projMatrix():Matrix3D
		{
			var rawData:Vector.<Number> = new Vector.<Number>(16,true);
			CLibrary.memory.position = _projMatrixPointer;
			
			for(var i:int=0; i<16; ++i)
				rawData[i] = CLibrary.memory.readFloat();
			
			var mat:Matrix3D = new Matrix3D(rawData);
			
			return mat;
		}
		
		public function setProjection(fov:Number, nearPlane:Number, farPlane:Number, aspect:Number = 0):void
		{
			_fov = fov; 
			_nearPlane = nearPlane;
			_farPlane = farPlane;
			
			setAspect(aspect ? aspect : _aspect);
		}
		
		public function setAspect(aspect:Number = 0):void
		{
			CLibrary.method.SetProjectionToCamera(_pointer, _fov, _nearPlane, _farPlane, _aspect = aspect);
		}
		
		public function setOrghInfo( cameraSphere:CSphereData, directional:Vector3D ):void
		{
			if(cameraSphere && directional)
			{
				if(_directionalPtr == 0 || _spherePtr == 0)
				{
					_spherePtr = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE*4 );
					_directionalPtr = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE*3 );
				}
				
				CLibrary.memory.position = _spherePtr;
				CLibrary.memory.writeFloat(cameraSphere.orin.x);
				CLibrary.memory.writeFloat(cameraSphere.orin.y);
				CLibrary.memory.writeFloat(cameraSphere.orin.z);
				CLibrary.memory.writeFloat(cameraSphere.radius);
				
				CLibrary.memory.position = _directionalPtr;
				CLibrary.memory.writeFloat(directional.x);
				CLibrary.memory.writeFloat(directional.y);
				CLibrary.memory.writeFloat(directional.z);
				
				CLibrary.method.SetOrghMatrixToCamera(_pointer, _spherePtr, _directionalPtr);
			}
		}
		
		public function fillSphere( sphere:CSphereData ):void
		{
			if(_pointer)
			{
				var infos:Array = CLibrary.method.GetSphereFromCamera(_pointer);
				if(infos)
				{
					sphere.orin.x = infos[0];
					sphere.orin.y = infos[1];
					sphere.orin.z = infos[2];
					
					sphere.radius = infos[3];
				}
			}
		}

		override protected function getSceneNodePointers():Array
		{
			return CLibrary.method.CreateCamera();
		}

		private var _spherePtr:uint;
		private var _directionalPtr:uint;
		
		protected var _viewMatrixPointer:uint;
		protected var _projMatrixPointer:uint;
		
		private var _fov:uint;
		private var _nearPlane:uint;
		private var _farPlane:uint;
		private var _aspect:uint;
	}
}