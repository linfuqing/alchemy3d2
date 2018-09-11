package alchemy.scene
{
	import alchemy.compilers.CAbstractCompiler;
	import alchemy.core.CLibrary;
	import alchemy.manager.CMeshManager;
	import alchemy.manager.CSceneManager;
	import alchemy.resource.extend.CRenderMethod;
	
	public class CSkyDome extends CSceneNode
	{
		static public var TEXTURE_CUBE:uint = 0;
		static public var TEXTURE_2D:uint = 1;
		
		public function CSkyDome(radius:Number, phiDelta:Number, 
								 thetaDelta:Number, texture:uint,
								 vecOffsetTextures:Vector.<uint> = null, meshHandle:uint = 0)
		{
			CMeshManager.instance;
			_radius = radius;
			_phiDelta = phiDelta;
			_thetaDelta = thetaDelta;
			_texture = texture;
			
			if(vecOffsetTextures != null && vecOffsetTextures.length > 0)
			{
				_offsetTextures = new Vector.<uint>;
				_offsetTextures = _offsetTextures.concat(vecOffsetTextures);
			}
			
			_meshHandle = meshHandle;
			
			super(0, 0, 0);
		}
		
		public function setEffectRender(effect:uint, renderMethod:CRenderMethod):void
		{
			CLibrary.method.SetEffectRenderToSkyDome(_pointer, effect, renderMethod ? renderMethod.pointer : null);
		}
		
		public function SetUVSpeed(uSpeed:Number, vSpeed:Number):void
		{
			CLibrary.method.SetUVSpeedToSkyDomeMethod(_pointer, uSpeed, vSpeed);
		}
		
		override protected function initPointers(pts:Array):uint
		{
			var index:uint = super.initPointers(pts);
			
			return index;
		}
		
		override protected function getSceneNodePointers():Array
		{
			var texturePointer:uint = 0;
			var texturesLen:uint = 0;
			if(_offsetTextures != null)
			{
				texturePointer = CLibrary.method.CreateBuffer( _offsetTextures.length * CLibrary.INT_SIZE );
				
				for(var i:int=0; i<_offsetTextures.length; ++i)
					CLibrary.memory.writeInt( _offsetTextures[i] );
				
				texturesLen = _offsetTextures.length;
			}
				
			var ptsArray:Array;
			
			if(_meshHandle == 0)
				ptsArray = CLibrary.method.CreateSkyDome(_radius, _phiDelta, _thetaDelta, _texture, texturePointer, texturesLen);
			else
				ptsArray = CLibrary.method.CreateSkyDomeWithMesh(_meshHandle, _texture, texturePointer, texturesLen);
			
			if(texturePointer)
				CLibrary.method.DestroyBuffer( texturePointer );
			
			
			return ptsArray;
		}
		
		protected var _meshHandle:uint;
		protected var _texture:uint;
		protected var _offsetTextures:Vector.<uint>;
		protected var _radius:Number;
		protected var _phiDelta:Number;
		protected var _thetaDelta:Number;
	}
}