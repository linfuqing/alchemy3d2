package alchemy.scene 
{
	import alchemy.core.CLibrary;
	import alchemy.core.CObject;
	import alchemy.core.CPointer;
	import alchemy.flush3D;
	import alchemy.manager.CSceneManager;
	import alchemy.tools.CDebug;
	import alchemy.tools.CMath;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;
	
	use namespace flush3D;

	public class CSceneNode extends CObject implements ISceneContainer
	{
		public static const DIRTY_FLAG_SCALE_BIT      :uint = 0;
		public static const DIRTY_FLAG_ROTATION_BIT   :uint = 1;
		public static const DIRTY_FLAG_TRANSLATION_BIT:uint = 2;
		public static const DIRTY_FLAG_MATRIX_BIT     :uint = 3;
		
		protected var _name:String;
		protected var _id:String;
		
		public function set name(val:String):void
		{
			if(_pointer)
			{
				var dataPtr:uint = CLibrary.method.CreateBuffer( val.length + 1 );
				CLibrary.memory.position = dataPtr;
				CLibrary.memory.writeUTFBytes(val);
				CLibrary.memory.writeByte(0);
				
				CLibrary.method.SetNodeName(_sceneNodePointer, dataPtr);
				
				CLibrary.method.DestroyBuffer(dataPtr);
			}
		}
		
		public function get name():String
		{
			if(_pointer)
			{
				var values:Array = CLibrary.method.GetNodeName(_sceneNodePointer);
				CLibrary.memory.position = values[0];
				return CLibrary.memory.readUTFBytes(values[1]);
			}
			
			return null;
		}
		
		public function set visible( val:Boolean ):void
		{
			CLibrary.method.SetVisibleToSceneNode(_sceneNodePointer, val);
		}
		
		public function get visible():Boolean
		{
			CLibrary.memory.position = _isVisiblePointer;
			return CLibrary.memory.readBoolean();
		}
		
		public function set id(val:String):void
		{
			_id = val;
		}
		
		public function get id():String
		{
			return _id;
		}
		
		public function get parent():CSceneNode
		{
			return _parent;
		}
		
		public function set rotateX(val:Number):void
		{
			CLibrary.memory.position = _rotateXPointer;
			CLibrary.memory.writeFloat(val);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_ROTATION_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function get rotateX():Number
		{
			//updateDirtyData();
			
			CLibrary.memory.position = _rotateXPointer;
			return CLibrary.memory.readFloat();
		}
		
		public function set rotateY(val:Number):void
		{
			CLibrary.memory.position = _rotateYPointer;
			CLibrary.memory.writeFloat(val);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_ROTATION_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function get rotateY():Number
		{
			//updateDirtyData();
			
			CLibrary.memory.position = _rotateYPointer;
			return CLibrary.memory.readFloat();
		}
		
		public function set rotateZ(val:Number):void
		{
			CLibrary.memory.position = _rotateZPointer;
			CLibrary.memory.writeFloat(val);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_ROTATION_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function get rotateZ():Number
		{
			//updateDirtyData();
			
			CLibrary.memory.position = _rotateZPointer;
			return CLibrary.memory.readFloat();
		}
		
		public function set x(val:Number):void
		{
			CLibrary.memory.position = _xPointer;
			CLibrary.memory.writeFloat(val);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_TRANSLATION_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function get x():Number
		{
			//updateDirtyData();
			
			CLibrary.memory.position = _xPointer;
			return CLibrary.memory.readFloat();
		}
		
		public function set y(val:Number):void
		{
			CLibrary.memory.position = _yPointer;
			CLibrary.memory.writeFloat(val);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_TRANSLATION_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function get y():Number
		{
			//updateDirtyData();
			
			CLibrary.memory.position = _yPointer;
			return  CLibrary.memory.readFloat();
		}
		
		public function set z(val:Number):void
		{
			CLibrary.memory.position = _zPointer;
			CLibrary.memory.writeFloat(val);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_TRANSLATION_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function get z():Number
		{
			//updateDirtyData();
			
			CLibrary.memory.position = _zPointer;
			return  CLibrary.memory.readFloat();
		}
		
		public function set scaleX(val:Number):void
		{
			CLibrary.memory.position = _scaleXPointer;
			CLibrary.memory.writeFloat(val);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_SCALE_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function get scaleX():Number
		{
			//updateDirtyData();
			
			CLibrary.memory.position = _scaleXPointer;
			return  CLibrary.memory.readFloat();
		}
		
		public function set scaleY(val:Number):void
		{
			CLibrary.memory.position = _scaleYPointer;
			CLibrary.memory.writeFloat(val);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_SCALE_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function get scaleY():Number
		{
			//updateDirtyData();
			
			CLibrary.memory.position = _scaleYPointer;
			return  CLibrary.memory.readFloat();
		}
		
		public function set scaleZ(val:Number):void
		{
			CLibrary.memory.position = _scaleZPointer;
			CLibrary.memory.writeFloat(val);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_SCALE_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function get scaleZ():Number
		{
			//updateDirtyData();
			
			CLibrary.memory.position = _scaleZPointer;
			return CLibrary.memory.readFloat();
		}
		
		public function set position(value:Vector3D):void
		{
			CLibrary.memory.position = _xPointer;
			CLibrary.memory.writeFloat(value.x);
			CLibrary.memory.position = _yPointer;
			CLibrary.memory.writeFloat(value.y);
			CLibrary.memory.position = _zPointer;
			CLibrary.memory.writeFloat(value.z);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_TRANSLATION_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function get position():Vector3D
		{
			CLibrary.memory.position = _xPointer;
			_vector.x = CLibrary.memory.readFloat();
			CLibrary.memory.position = _yPointer;
			_vector.y = CLibrary.memory.readFloat();
			CLibrary.memory.position = _zPointer;
			_vector.z = CLibrary.memory.readFloat();
			
			_vector.w = 0;
			
			return _vector.clone();
		}
		
		public function get worldPosition():Vector3D
		{
			var float4Offset:uint = CLibrary.FLOAT_SIZE << 2;
			var basicPointer:uint = _worldMatrixPointer + (CLibrary.FLOAT_SIZE << 3);
			CLibrary.memory.position = basicPointer;
			_vector.x = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.y = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.z = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.w = CLibrary.memory.readFloat();
			
			return _vector;
		}
		
		public function set transform(matrix:Matrix3D):void
		{
			var rawData:Vector.<Number> = matrix.rawData;
			CLibrary.memory.position = _localMatrixPointer;
			CLibrary.memory.writeFloat(rawData[0]);
			CLibrary.memory.writeFloat(rawData[4]);
			CLibrary.memory.writeFloat(rawData[8]);
			CLibrary.memory.writeFloat(rawData[12]);
			
			CLibrary.memory.writeFloat(rawData[1]);
			CLibrary.memory.writeFloat(rawData[5]);
			CLibrary.memory.writeFloat(rawData[9]);
			CLibrary.memory.writeFloat(rawData[13]);
			
			CLibrary.memory.writeFloat(rawData[2]);
			CLibrary.memory.writeFloat(rawData[6]);
			CLibrary.memory.writeFloat(rawData[10]);
			CLibrary.memory.writeFloat(rawData[14]);
			
			CLibrary.memory.writeFloat(rawData[3]);
			CLibrary.memory.writeFloat(rawData[7]);
			CLibrary.memory.writeFloat(rawData[11]);
			CLibrary.memory.writeFloat(rawData[15]);
			
			var flag:uint = dirtyFlag;
			flag |= ( (1 << DIRTY_FLAG_MATRIX_BIT) - 1 );
			flag &= ~(1 << DIRTY_FLAG_MATRIX_BIT);
			
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt(flag);
			
			CLibrary.memory.position = _isTransformDirtyPointer;
			CLibrary.memory.writeBoolean(true);
			
			//updateDirtyData();
		}
		
		public function get transform():Matrix3D
		{
			var rawData:Vector.<Number> = new Vector.<Number>(16,true);
			CLibrary.memory.position = _localMatrixPointer;
			
			for(var i:uint=0; i<16; ++i)
				rawData[i] = CLibrary.memory.readFloat();
			
			var mat:Matrix3D = new Matrix3D(rawData);
			return mat;
		}
		
		public function set direction(value:Vector3D):void
		{
			var x:Number;
			var y:Number;
			
			if(value.x == 0 && value.z == 0 && value.y == 0)
			{
				x = 0;
				y = 0;
			}
			else if(value.x == 0 && value.z == 0)
			{
				x = - Math.asin(value.y / value.length);
				y =   0;
			}
			else if(value.z < 0)
			{
				x = - Math.asin(value.y / value.length);
				y =   Math.PI - Math.asin( value.x / Math.sqrt(value.x * value.x + value.z * value.z) );
			}
			else
			{
				x = - Math.asin(value.y / value.length);
				y =   Math.asin( value.x / Math.sqrt(value.x * value.x + value.z * value.z) );
			}
			
			x *= CMath.RAD2DEG;
			y *= CMath.RAD2DEG;
			
			CLibrary.memory.position = _rotateXPointer;
			CLibrary.memory.writeFloat(x);
			CLibrary.memory.position = _rotateYPointer;
			CLibrary.memory.writeFloat(y);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_ROTATION_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function get direction():Vector3D
		{
			var float4Offset:uint = CLibrary.FLOAT_SIZE << 2;
			var basicPointer:uint = _localMatrixPointer + (CLibrary.FLOAT_SIZE << 1);
			CLibrary.memory.position = basicPointer;
			_vector.x = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.y = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.z = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.w = CLibrary.memory.readFloat();
			
			if(_vector.w && _vector.w != 1)
				_vector.project();
			
			_vector.normalize();
			
			return _vector;
		}
		
		public function get normal():Vector3D
		{
			var float4Offset:uint = CLibrary.FLOAT_SIZE << 2;
			var basicPointer:uint = _localMatrixPointer + CLibrary.FLOAT_SIZE;
			CLibrary.memory.position = basicPointer;
			_vector.x = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.y = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.z = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.w = CLibrary.memory.readFloat();
			
			if(_vector.w && _vector.w != 1)
				_vector.project();
			
			_vector.normalize();
			
			return _vector;
		}
		
		public function get tangent():Vector3D
		{
			var float4Offset:uint = CLibrary.FLOAT_SIZE << 2;
			var basicPointer:uint = _localMatrixPointer;
			CLibrary.memory.position = basicPointer;
			_vector.x = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.y = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.z = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.w = CLibrary.memory.readFloat();
			
			if(_vector.w && _vector.w != 1)
				_vector.project();
			
			_vector.normalize();
			
			return _vector;
		}
		
		public function get dirtyFlag():uint
		{
			CLibrary.memory.position = _dirtyFlagPointer;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get childrenLen():int
		{
			return _children.length;
		}
		
		public function CSceneNode(pointer:uint = 0, interfacePointer:uint = 0, sceneNodePointer:uint = 0)
		{
			if(!pointer && !interfacePointer && !sceneNodePointer)
			{
				var pointers:Array = getSceneNodePointers();
				
				pointer          = pointers[0];
				interfacePointer = pointers[1];
				sceneNodePointer = pointers[2];
			}
			
			_pointer          = pointer;
			
			_sceneNodePointer = sceneNodePointer;
			
			super(interfacePointer);
			
			init();
		}
		
		public function forward(step:Number):void
		{
			/*CLibrary.memory.position = _localMatrixPointer + 8 * CLibrary.FLOAT_SIZE;
			_vector.x = CLibrary.memory.readFloat();
			_vector.y = CLibrary.memory.readFloat();
			_vector.z = CLibrary.memory.readFloat();
			_vector.w = CLibrary.memory.readFloat();*/
			var float4Offset:uint = CLibrary.FLOAT_SIZE << 2;
			var basicPointer:uint = _localMatrixPointer + (CLibrary.FLOAT_SIZE << 1);
			CLibrary.memory.position = basicPointer;
			_vector.x = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.y = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.z = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.w = CLibrary.memory.readFloat();
			
			if(_vector.w && _vector.w != 1)
				_vector.project();
			
			_vector.normalize();
			
			_vector.scaleBy(step);
			
			var value:Number;
			
			CLibrary.memory.position = _xPointer;
			value = CLibrary.memory.readFloat();
			CLibrary.memory.position = _xPointer;
			CLibrary.memory.writeFloat(value + _vector.x);
			
			CLibrary.memory.position = _yPointer;
			value = CLibrary.memory.readFloat();
			CLibrary.memory.position = _yPointer;
			CLibrary.memory.writeFloat(value + _vector.y);
			
			CLibrary.memory.position = _zPointer;
			value = CLibrary.memory.readFloat();
			CLibrary.memory.position = _zPointer;
			CLibrary.memory.writeFloat(value + _vector.z);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_TRANSLATION_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function up(step:Number):void
		{
			/*CLibrary.memory.position = _localMatrixPointer + 4 * CLibrary.FLOAT_SIZE;
			_vector.x = CLibrary.memory.readFloat();
			_vector.y = CLibrary.memory.readFloat();
			_vector.z = CLibrary.memory.readFloat();
			_vector.w = CLibrary.memory.readFloat();*/
			var float4Offset:uint = CLibrary.FLOAT_SIZE << 2;
			var basicPointer:uint = _localMatrixPointer + CLibrary.FLOAT_SIZE;
			CLibrary.memory.position = basicPointer;
			_vector.x = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.y = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.z = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.w = CLibrary.memory.readFloat();
			
			if(_vector.w && _vector.w != 1)
				_vector.project();
			
			_vector.normalize();
			
			_vector.scaleBy(step);
			
			var value:Number;
			
			CLibrary.memory.position = _xPointer;
			value = CLibrary.memory.readFloat();
			CLibrary.memory.position = _xPointer;
			CLibrary.memory.writeFloat(value + _vector.x);
			
			CLibrary.memory.position = _yPointer;
			value = CLibrary.memory.readFloat();
			CLibrary.memory.position = _yPointer;
			CLibrary.memory.writeFloat(value + _vector.y);
			
			CLibrary.memory.position = _zPointer;
			value = CLibrary.memory.readFloat();
			CLibrary.memory.position = _zPointer;
			CLibrary.memory.writeFloat(value + _vector.z);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_TRANSLATION_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function right(step:Number):void
		{
			/*CLibrary.memory.position = _localMatrixPointer;
			_vector.x = CLibrary.memory.readFloat();
			_vector.y = CLibrary.memory.readFloat();
			_vector.z = CLibrary.memory.readFloat();
			_vector.w = CLibrary.memory.readFloat();*/
			var float4Offset:uint = CLibrary.FLOAT_SIZE << 2;
			var basicPointer:uint = _localMatrixPointer;
			CLibrary.memory.position = basicPointer;
			_vector.x = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.y = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.z = CLibrary.memory.readFloat();
			basicPointer += float4Offset;
			CLibrary.memory.position = basicPointer;
			_vector.w = CLibrary.memory.readFloat();
			
			if(_vector.w && _vector.w != 1)
				_vector.project();
			
			_vector.normalize();
			
			_vector.scaleBy(step);
			
			var value:Number;
			
			CLibrary.memory.position = _xPointer;
			value = CLibrary.memory.readFloat();
			CLibrary.memory.position = _xPointer;
			CLibrary.memory.writeFloat(value + _vector.x);
			
			CLibrary.memory.position = _yPointer;
			value = CLibrary.memory.readFloat();
			CLibrary.memory.position = _yPointer;
			CLibrary.memory.writeFloat(value + _vector.y);
			
			CLibrary.memory.position = _zPointer;
			value = CLibrary.memory.readFloat();
			CLibrary.memory.position = _zPointer;
			CLibrary.memory.writeFloat(value + _vector.z);
			
			var flag:uint = dirtyFlag;
			CLibrary.memory.position = _dirtyFlagPointer;
			CLibrary.memory.writeUnsignedInt( flag | (1 << DIRTY_FLAG_TRANSLATION_BIT) | (1 << DIRTY_FLAG_MATRIX_BIT) );
		}
		
		public function getChildByIndex(index:int):CSceneNode
		{
			if(index >= 0 && index < _children.length)
				return _children[index];
			
			return null;
		}
		
		public function findChildWithID(findid:String,deep:Boolean = false):CSceneNode
		{
			var len:uint = _children.length;
			var child:CSceneNode;
			for(var i:uint=0; i<len; ++i)
			{
				child = _children[i];
				if(child.id == findid)
					return child;
			}
			
			if(deep)
			{
				for(i=0; i<len; ++i)
				{
					child = _children[i].findChildWithID( findid, deep );
					if(child != null)
						return child;
				}
			}
			
			return null;
		}
		
		public function findChildWithName(findname:String,deep:Boolean = false):CSceneNode
		{
			var len:uint = _children.length;
			var child:CSceneNode;
			for(var i:uint=0; i<len; ++i)
			{
				child = _children[i];
				if(child.name == findname)
					return child;
			}
			
			if(deep)
			{
				for(i=0; i<len; ++i)
				{
					child = _children[i].findChildWithName( findname, deep );
					if(child != null)
						return child;
				}
			}
			
			return null;
		}
		
		public function addChild(child:CSceneNode):void
		{	
			if(child == null)
			{
				CDebug.warning("child is null.");
				
				return;
			}
			
			CLibrary.method.AddChildToSceneNode(_sceneNodePointer, child._sceneNodePointer);
		}
		
		public function removeChild(child:CSceneNode):void
		{
			if(child == null)
			{
				CDebug.warning("child is null.");
				
				return;
			}
			
			CLibrary.method.RemoveChildFromSceneNode(_sceneNodePointer, child._sceneNodePointer);
		}
		
		public function addToScene(sceneManager:CSceneManager):void
		{
			CLibrary.method.AddChildToSceneManager(sceneManager.pointer, _sceneNodePointer);
		}
		
		public function removeFromScene(sceneManager:CSceneManager):void
		{
			CLibrary.method.RemoveChildFromSceneManager(sceneManager.pointer, _sceneNodePointer);
		}
		
		public function clone():CSceneNode
		{
			return CLibrary.method.CloneSceneNode(_sceneNodePointer);
		}
		
		//override function
		protected function initPointers(pts:Array):uint
		{
			var index:uint           = 0;
			_xPointer 		         = pts[index++];
			_yPointer 		         = pts[index++];
			_zPointer 		         = pts[index++];
			_scaleXPointer           = pts[index++];
			_scaleYPointer           = pts[index++];
			_scaleZPointer           = pts[index++];
			_rotateXPointer          = pts[index++];
			_rotateYPointer          = pts[index++];
			_rotateZPointer          = pts[index++];
			_localMatrixPointer      = pts[index++];
			_worldMatrixPointer      = pts[index++];
			_dirtyFlagPointer        = pts[index++];
			_isTransformDirtyPointer = pts[index++];
			_isVisiblePointer		 = pts[index++];
			
			return index;
		}
		
		protected function getSceneNodePointers():Array
		{
			return CLibrary.method.CreateSceneNode();
		}
		
		private function addSceneNode(child:CSceneNode):void
		{
			if(child != null)
			{
				var index:int = _children.indexOf(child);
				if(index == -1)
				{
					_children.push(child);
					child._parent = this;
				}
			}
		}
		
		private function removeSceneNode(child:CSceneNode):void
		{
			if(child != null)
			{
				var index:int = _children.indexOf(child);
				if(index != -1)
					_children.splice(index,1);
			}
		}
		
		private function updateDirtyData(matWorld:Matrix3D = null):void
		{
			var flag:uint = dirtyFlag;
			if( !( flag & (1 << DIRTY_FLAG_MATRIX_BIT) ) )
			{
				var matTransform:Matrix3D = matWorld == null ? transform : matWorld;
				var matInfo:Vector.<Vector3D> = matTransform.decompose();
				
				if( testFlag( DIRTY_FLAG_SCALE_BIT, flag ) )
				{
					this.x = matInfo[0].x;
					this.y = matInfo[0].y;
					this.z = matInfo[0].z;
				}
				
				if( testFlag( DIRTY_FLAG_ROTATION_BIT, flag ) )
				{
					this.rotateX = CMath.rad2deg( matInfo[1].x );
					this.rotateY = CMath.rad2deg( matInfo[1].y );
					this.rotateZ = CMath.rad2deg( matInfo[1].z );
				}
				
				if( testFlag( DIRTY_FLAG_TRANSLATION_BIT, flag ) )
				{
					this.scaleX = matInfo[2].x;
					this.scaleY = matInfo[2].y;
					this.scaleZ = matInfo[2].z;
				}
			}
		}
		
		private function testFlag( flag:uint, curFlag:uint = 0):Boolean
		{
			if(curFlag == 0)
				curFlag = dirtyFlag;
			
			if( (1 << flag) & curFlag)
				return true;
			
			return false;
		}
		
		private function init():void
		{
			initPointers( getPointers() );
			
			fill(addSceneNode, removeSceneNode);
			
			_children = new Vector.<CSceneNode>();
			_matrix   = new Matrix3D;
			_vector   = new Vector3D;
		}
		
		flush3D function get worldMarixPointer():uint
		{
			return _worldMatrixPointer;
		}
		
		private var _children:Vector.<CSceneNode>;
		private var _parent:CSceneNode;
		private var _transform:Matrix3D;
		
		private var _xPointer:uint;
		private var _yPointer:uint;
		private var _zPointer:uint;
		private var _scaleXPointer:uint;
		private var _scaleYPointer:uint;
		private var _scaleZPointer:uint;
		private var _rotateXPointer:uint;
		private var _rotateYPointer:uint;
		private var _rotateZPointer:uint;
		private var _localMatrixPointer:uint;
		private var _worldMatrixPointer:uint;
		private var _dirtyFlagPointer:uint;
		private var _isTransformDirtyPointer:uint;
		private var _isVisiblePointer:uint;
		
		private var _sceneNodePointer:uint;
		
		private var _matrix:Matrix3D;
		private var _vector:Vector3D;
	}
}