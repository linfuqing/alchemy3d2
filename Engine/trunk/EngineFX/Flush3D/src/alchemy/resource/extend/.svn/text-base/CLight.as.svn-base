package alchemy.resource.extend
{
	import alchemy.core.CLibrary;
	import alchemy.manager.CSceneManager;
	import alchemy.tools.CDebug;
	
	import flash.geom.ColorTransform;
	import flash.geom.Vector3D;
	
	public class CLight extends CHighLevelResource
	{
		static public var POINT:uint 			= 0;
		static public var SPOT:uint 			= 1;
		static public var DIRECTIONAL:uint 	= 2;
		
		public function set type(val:uint):void
		{
			CDebug.assert( val >= POINT && val <= DIRECTIONAL,"invalid light type");
			_type = val;
			CLibrary.memory.position = _typePointer;
			CLibrary.memory.writeUnsignedInt(_type);
		}
		
		public function get type():uint
		{
			return _type;
		}
		
		public function set ambient(color:ColorTransform):void
		{
			CLibrary.memory.position = _ambientPointer;
			CLibrary.memory.writeFloat( color.redMultiplier );
			CLibrary.memory.writeFloat( color.greenMultiplier );
			CLibrary.memory.writeFloat( color.blueMultiplier );
		}
		
		public function get ambient():ColorTransform
		{
			CLibrary.memory.position = _ambientPointer;
			_color.redMultiplier   = CLibrary.memory.readFloat();
			_color.greenMultiplier = CLibrary.memory.readFloat();
			_color.blueMultiplier  = CLibrary.memory.readFloat();
			
			return _color;
		}
		
		public function set diffuse(color:ColorTransform):void
		{
			CLibrary.memory.position = _diffusePointer;
			CLibrary.memory.writeFloat( color.redMultiplier );
			CLibrary.memory.writeFloat( color.greenMultiplier );
			CLibrary.memory.writeFloat( color.blueMultiplier );
		}
		
		public function set diffuseVector(color:Vector.<Number>):void
		{
			CLibrary.memory.position = _diffusePointer;
			CLibrary.memory.writeFloat( color[0] );
			CLibrary.memory.writeFloat( color[1] );
			CLibrary.memory.writeFloat( color[2] );
		}
		
		public function get diffuse():ColorTransform
		{
			CLibrary.memory.position = _diffusePointer;
			_color.redMultiplier   = CLibrary.memory.readFloat();
			_color.greenMultiplier = CLibrary.memory.readFloat();
			_color.blueMultiplier  = CLibrary.memory.readFloat();
			
			return _color;
		}
		
		public function set specular(color:ColorTransform):void
		{
			CLibrary.memory.position = _specularPointer;
			CLibrary.memory.writeFloat( color.redMultiplier );
			CLibrary.memory.writeFloat( color.greenMultiplier );
			CLibrary.memory.writeFloat( color.blueMultiplier );
		}
		
		public function set specularVector(color:Vector.<Number>):void
		{
			CLibrary.memory.position = _specularPointer;
			CLibrary.memory.writeFloat( color[0] );
			CLibrary.memory.writeFloat( color[1] );
			CLibrary.memory.writeFloat( color[2] );
		}
		
		public function get specular():ColorTransform
		{
			CLibrary.memory.position = _specularPointer;
			_color.redMultiplier   = CLibrary.memory.readFloat();
			_color.greenMultiplier = CLibrary.memory.readFloat();
			_color.blueMultiplier  = CLibrary.memory.readFloat();
			
			return _color;
		}
		
		public function set position(value:Vector3D):void
		{
			CLibrary.memory.position = _positionPointer;
			CLibrary.memory.writeFloat(value.x );
			CLibrary.memory.writeFloat(value.y );
			CLibrary.memory.writeFloat(value.z );
		}
		
		public function get position():Vector3D
		{
			CLibrary.memory.position = _positionPointer;
			_vector.x = CLibrary.memory.readFloat();
			_vector.y = CLibrary.memory.readFloat();
			_vector.z = CLibrary.memory.readFloat();
			_vector.w = 1.0;
			return _vector;
		}
		
		public function set direction(value:Vector3D):void
		{
			CLibrary.memory.position = _directionPointer;
			CLibrary.memory.writeFloat(value.x);
			CLibrary.memory.writeFloat(value.y);
			CLibrary.memory.writeFloat(value.z);
		}
		
		public function get direction():Vector3D
		{
			CLibrary.memory.position = _directionPointer;
			_vector.x = CLibrary.memory.readFloat();
			_vector.y = CLibrary.memory.readFloat();
			_vector.z = CLibrary.memory.readFloat();
			_vector.w = 0.0;
			return _vector;
		}
		
		public function set range(val:Number):void
		{
			_range = val;
			CLibrary.memory.position = _rangePointer;
			CLibrary.memory.writeFloat( _range );
		}
		
		public function get range():Number
		{
			return _range;
		}
		
		public function set falloff(val:Number):void
		{
			_falloff = val;
			CLibrary.memory.position = _falloffPointer;
			CLibrary.memory.writeFloat( _falloff );
		}
		
		public function get falloff():Number
		{
			return _falloff;
		}
		
		public function set attenuation0(val:Number):void
		{
			_attenuation0 = val;
			CLibrary.memory.position = _attenuation0Pointer;
			CLibrary.memory.writeFloat( _attenuation0 );
		}
		
		public function get attenuation0():Number
		{
			return _attenuation0;
		}
		
		public function set attenuation1(val:Number):void
		{
			_attenuation1 = val;
			CLibrary.memory.position = _attenuation1Pointer;
			CLibrary.memory.writeFloat( _attenuation1 );
		}
		
		public function get attenuation1():Number
		{
			return _attenuation1;
		}
		
		public function set attenuation2(val:Number):void
		{
			_attenuation2 = val;
			CLibrary.memory.position = _attenuation2Pointer;
			CLibrary.memory.writeFloat( _attenuation2 );
		}
		
		public function get attenuation2():Number
		{
			return _attenuation2;
		}
		
		public function set theta(val:Number):void
		{
			_theta = val;
			CLibrary.memory.position = _thetaPointer;
			CLibrary.memory.writeFloat( _theta );
		}
		
		public function get theta():Number
		{
			return _theta;
		}
		
		public function set phi(val:Number):void
		{
			_phi = val;
			CLibrary.memory.position = _phiPointer;
			CLibrary.memory.writeFloat( _phi );
		}
		
		public function get phi():Number
		{
			return _phi;
		}
		
		public function get shadowMap():uint
		{
			return _shadowMap;
		}
		
		public function CLight(pointer:uint, handle:uint, interfacePointer:uint, sceneManager:CSceneManager)
		{
			super(pointer, handle, interfacePointer, sceneManager);
			
			init();
		}
		
		private function init():void
		{
			var pts:Array = getPointers();
			_typePointer 			= pts[0];
			_ambientPointer 		= pts[1];
			_diffusePointer 		= pts[2];
			_specularPointer 		= pts[3];
			_positionPointer 		= pts[4];
			_directionPointer 		= pts[5];
			_rangePointer 			= pts[6];
			_falloffPointer 		= pts[7];
			_attenuation0Pointer 	= pts[8];
			_attenuation1Pointer 	= pts[9];
			_attenuation2Pointer 	= pts[10];
			_thetaPointer			= pts[11];
			_phiPointer 			= pts[12];
			
			_color = new ColorTransform();
			_vector = new Vector3D();
		}
		
		public function buildShadowMap(size:Number, fov:Number, near:Number, far:Number):void
		{
			_shadowMap = CLibrary.method.BuildShadowMapFromLight(_pointer, size, fov, near, far);
		}
		
		private var _type:uint;
		private var _color:ColorTransform;
		private var _vector:Vector3D;
		private var _range:Number;
		private var _falloff:Number;
		private var _attenuation0:Number;
		private var _attenuation1:Number;
		private var _attenuation2:Number;
		private var _theta:Number;
		private var _phi:Number;
		
		private var _shadowMap:uint;
	
		private var _typePointer:uint;
		private var _ambientPointer:uint;
		private var _diffusePointer:uint;
		private var _specularPointer:uint;
		private var _positionPointer:uint;
		private var _directionPointer:uint;
		private var _rangePointer:uint;
		private var _falloffPointer:uint;
		private var _attenuation0Pointer:uint;
		private var _attenuation1Pointer:uint;
		private var _attenuation2Pointer:uint;
		private var _thetaPointer:uint;
		private var _phiPointer:uint;
	}
}