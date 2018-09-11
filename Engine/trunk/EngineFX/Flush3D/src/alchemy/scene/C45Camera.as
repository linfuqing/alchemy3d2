package alchemy.scene
{
	import flash.geom.Vector3D;
	
	public class C45Camera
	{
		private static const sin45:Number = 0.70710678;
		
		public function C45Camera(hostCamera:CCamera, node:CSceneNode)
		{
			_hostCamera = hostCamera;
			
			_target = node;
			this.maxDistance = 5000;
			this.minDistance = 50;
			this.distance = 500;
			this.panAngle = 0;
		}
		
		protected var _hostCamera:CCamera;
		
		private var _target:CSceneNode;
		private var _panAngle:Number;
		private var _dist:Number;
		private var _maxDist:Number;
		private var _minDist:Number;
		
		
		public function set panAngle(val:Number):void
		{
			_panAngle = val%360;
		}
		public function get panAngle():Number
		{
			return _panAngle;
		}
		
		public function set distance(val:Number):void
		{
			if(val > maxDistance)
				_dist = maxDistance;
			else if(val < minDistance)
				_dist = minDistance;
			else
				_dist = val;
		}
		public function get distance():Number
		{
			return _dist;
		}
		
		public function set maxDistance(val:Number):void
		{
			_maxDist = val;
		}
		public function get maxDistance():Number
		{
			return _maxDist;
		}
		
		public function set minDistance(val:Number):void
		{
			_minDist = val;
		}
		public function get minDistance():Number
		{
			return _minDist;
		}
		
		private function updateBak():void
		{
			var newX:Number, newY:Number, newZ:Number;
			
			var r:Number = newY = sin45*distance;
			var ptX:Number = _target.x, ptZ:Number = -1*r+_target.z;
			
			//			this.target.x++;
			//			if(target.x>2000)
			//				target.x=-2000;
			//			
			//			this.target.z++;
			//			if(target.z>1000)
			//				target.z=-1000;
			//			
			//			panAngle++;
			//			if(this.panAngle>360)
			//				panAngle = 0;
			
			var l:Number = (panAngle*Math.PI)/180;
			var sinL:Number = Math.sin(l);
			var cosL:Number = Math.cos(l);
			
			newX = (ptX - _target.x) * cosL + (ptZ - _target.z) * sinL + _target.x;
			newZ = -(ptX - _target.x) * sinL + (ptZ - _target.z) * cosL + _target.z;
			
			_hostCamera.x = newX; _hostCamera.y = newY; _hostCamera.z = newZ;
			_hostCamera.rotateY = panAngle;//+180
		}
		
		public function update():void
		{
			//updateBak();
			//return;
			
			///////////////////////////////////////////////////////////////////////////////////////////
			
//			this.target.x++;
//			if(target.x>2000)
//				target.x=-2000;
//			
//			this.target.z++;
//			if(target.z>1000)
//				target.z=-1000;
//			
//			panAngle++;
//			if(this.panAngle>360)
//				panAngle = 0;
			
			///////////////////////////////////////////////////////////////////////////////////////////
			
			var newX:Number, newY:Number, newZ:Number;
			var ptX:Number, ptY:Number, ptZ:Number;
			
			var tiltangle:Number = 90*distance/maxDistance;
			var r:Number = distance;
			
			_hostCamera.rotateX = tiltangle;
			
			ptY = _target.y, ptZ = r;
			
			var l:Number = (tiltangle*Math.PI)/180;
			var sinL:Number = Math.sin(l);
			var cosL:Number = Math.cos(l);
			
			newY = (ptY - _target.y) * cosL + (ptZ - 0) * sinL + _target.y;
			newZ = -(ptY - _target.y) * sinL + (ptZ - 0) * cosL + 0;
			
			///////////////////////////////////////////////////////////////////////////////////////////
			
			r = Math.abs(newZ);
			
			///////////////////////////////////////////////////////////////////////////////////////////
			
			ptX = _target.x, ptZ = -1*r+_target.z;
			
			l = (panAngle*Math.PI)/180;
			sinL = Math.sin(l);
			cosL = Math.cos(l);
			
			newX = (ptX - _target.x) * cosL + (ptZ - _target.z) * sinL + _target.x;
			newZ = -(ptX - _target.x) * sinL + (ptZ - _target.z) * cosL + _target.z;
			
			_hostCamera.x = newX; _hostCamera.y = newY; _hostCamera.z = newZ;
			_hostCamera.rotateY = panAngle;//+180
		}
	}
}