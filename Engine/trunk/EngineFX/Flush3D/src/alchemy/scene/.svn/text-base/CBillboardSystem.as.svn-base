package alchemy.scene
{
	import alchemy.compilers.CAbstractCompiler;
	import alchemy.core.CLibrary;
	import alchemy.manager.CSceneManager;
	import alchemy.resource.extend.CRenderMethod;
	
	import flash.geom.Point;

	public class CBillboardSystem extends CSceneNode
	{
		static public const BILLBOARD_HARDWARE:uint = 0;
		static public const BILLBOARD_SOFTWARE:uint = 1;
		
		public function CBillboardSystem(maxBillboardNum:uint, mesh:uint, subset:uint, effect:uint, renderMethod:CRenderMethod, material:uint = 0, isDynamic:Boolean = false)
		{
			_maxBillboardNum = maxBillboardNum;
			_mesh = mesh;
			_subset = subset;
			_effect = effect;
			_material = material;
			_renderMethod = renderMethod;
			_isDynamic = isDynamic;
			
			super(0, 0, 0);
		}
		
		public function addBillboard( posX:Number, posY:Number, posZ:Number, fWidth:Number, fHeight:Number ):uint
		{
			if(_pointer && _billboardSetPointer)
			{
				var pt:uint = CLibrary.method.CreateBuffer( CLibrary.FLOAT_SIZE*3 );
				if(!pt)
					return 0;
				
				CLibrary.memory.position = pt;
				CLibrary.memory.writeFloat(posX);
				CLibrary.memory.writeFloat(posY);
				CLibrary.memory.writeFloat(posZ);
				
				var handle:uint = CLibrary.method.AddNodeToBillboardSystem(_billboardSetPointer,pt, fWidth, fHeight);
				
				CLibrary.method.DestroyBuffer(pt);
				
				return handle;
			}
			
			return 0;
		}
		
		public function removeBillboard( handle:uint ):void
		{
			if(_pointer && handle)
				CLibrary.method.RemoveNodeFromBillboardSystem(_pointer,handle);
		}
		
		override public function set rotateX(val:Number):void
		{
			
		}
		
		override public function set rotateY(val:Number):void
		{
			
		}
		
		override public function set rotateZ(val:Number):void
		{
			
		}
		
		override protected function getSceneNodePointers():Array
		{
			var ptrs:Array = CLibrary.method.CreateBillboardSystem(_maxBillboardNum,_effect, _renderMethod ? _renderMethod.pointer : 0, _material, _isDynamic, _mesh, _subset);
			
			_billboardSetPointer = ptrs[3];
			
			return ptrs;
		}
		
		protected var _billboardSetPointer:uint;
		
		protected var _mesh:uint;
		protected var _subset:uint;
		protected var _maxBillboardNum:uint;
		protected var _isDynamic:Boolean;
		protected var _effect:uint;
		protected var _renderMethod:CRenderMethod;
		protected var _material:uint;
	}
}