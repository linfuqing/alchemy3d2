package alchemy.astep.engineres
{
	import alchemy.scene.CCamera;

	public class CameraRes extends BasicRes
	{
		public function CameraRes( sourceCam:CCamera = null )
		{
			_camera = sourceCam;
		}
		
		public function get camera():CCamera
		{
			return _camera;
		}
		
		override public function get type():String
		{
			return RES_TYPE_CAMERA;
		}
		
		private var _camera:CCamera;
	}
}