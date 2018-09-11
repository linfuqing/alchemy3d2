package alchemy.animation
{
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.effect.CAbstractEffectInfo;
	import alchemy.effect.CEffectCode;
	import alchemy.resource.extend.CMesh;
	import alchemy.scene.CBone;
	import alchemy.tools.CDebug;
	
	public class CSkinMeshController extends CPointer
	{
		static public var SKIN_SOFTWARE:uint = 1;
		static public var SKIN_HARDWARE:uint = 2;
		
		public function CSkinMeshController( sourcePointer:uint = 0 )
		{
			_pointer = sourcePointer;
			if(_pointer)
				init();
		}
		
		public function reset(mesh:CMesh, skinType:uint, skinInfo:CSkinInfo, rootBone:CBone):Boolean
		{
			_pointer =  CLibrary.method.CreateSkinMeshController(mesh.pointer, skinType, skinInfo.pointer, rootBone.pointer);
			if(!_pointer)
				return false;
			
			init();
			
			return true;
		}
		
		protected function init():void
		{
			var ptrs:Array = CLibrary.method.GetSkinControllerInfo(_pointer);
			_rootBone = ptrs[0];
			_skinInfo = new CSkinInfo( ptrs[1] );
			_skinType = ptrs[2];
			
			if(_skinType == SKIN_HARDWARE)
				_skinHardwareEffect = new CAbstractEffectInfo( ptrs[3] );
			else
				_skinHardwareEffect = null;
		}
		
		public function get hardwareSkinEffect():CAbstractEffectInfo
		{
			return _skinHardwareEffect;
		}
		
		public function get rootBone():CBone
		{
			return _rootBone;
		}
		
		public function get skinInfo():CSkinInfo
		{
			return _skinInfo;
		}
		
		private var _rootBone:CBone;
		private var _skinInfo:CSkinInfo;
		private var _skinType:uint;
		private var _skinHardwareEffect:CAbstractEffectInfo;
	}
}