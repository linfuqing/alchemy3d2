package alchemy.resource.basic 
{
	import alchemy.core.CLibrary;
	import alchemy.data.CTextureData;
	import alchemy.manager.CRenderSystem;
	import alchemy.tools.CDebug;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.CubeTexture;
	import flash.display3D.textures.Texture;
	import flash.display3D.textures.TextureBase;
	import flash.geom.Matrix;
	import flash.geom.Matrix3D;
	import flash.utils.ByteArray;

	public class CTexture extends CLowLevelResource
	{
		public static const TYPE_NORMAL:int = 0;
		public static const TYPE_CUBE  :int = 1;
		public static const TYPE_VOLUME:int = 2;
		
		public static const FORMAT_ARGB:int = 1;
		
		public static const USAGE_RENDER_TARGET:int = 2;
		
		public static function getTextureFormat(format:int):String
		{
			return Context3DTextureFormat.BGRA;
		}
		
		public function get instance():TextureBase
		{
			return _texture;
		}
		
		public function get widthShift():uint
		{
			return _widthShift;
		}
		
		public function get heightShift():uint
		{
			return _heightShift;
		}
		
		public function get format():uint
		{
			return _format;
		}
		
		public function get usage():uint
		{
			return _usage;
		}
		
		public function CTexture(pointer:uint, handle:uint, interfacePointer:uint, renderSystem:CRenderSystem)
		{
			super(pointer, handle, interfacePointer, renderSystem);
			
			rebuild();
		}
		
		public function updata():void
		{
			if( ( _usage & (1 << USAGE_RENDER_TARGET) ) == 0 )
			{
				CLibrary.memory.position = _mipLevelDirtyFlagPointer;
				var mipLevelDirtyFlag:uint = CLibrary.memory.readUnsignedInt();
				
				if(mipLevelDirtyFlag)
				{
					CLibrary.memory.position = _dataPointer;
					var dataPointer:uint = CLibrary.memory.readUnsignedInt();
					if(dataPointer)
					{
						var cubeMapFacesPointerOffset:uint = 0;
						var i:uint;
						var j:uint;
						var faceDirtyFlag:uint;
						var byteSize:uint;
						var shift:uint;
						var bitmap:BitmapData;
						var transform:Matrix;
						switch(_type)
						{
							case TYPE_CUBE:
								/*if(_mipLevel)
								{*/
									bitmap = new BitmapData(1 << _widthShift, 1 << _heightShift, true);
									transform = new Matrix();
									transform.scale(0.5, 0.5);
									
									CLibrary.memory.position = _cubeMapFacesPointer;
									faceDirtyFlag = CLibrary.memory.readUnsignedInt();
									
									byteSize = (1 << _widthShift) * (1 << _heightShift) * 4;
									shift = Math.min(_widthShift, _heightShift) + 1;
									for(i = 0; i < 6; ++ i)
									{
										if( (faceDirtyFlag & (1 << i) ) != 0 )
										{
											_cubeTexture.uploadFromByteArray(CLibrary.memory, dataPointer, i, 0);
											CLibrary.memory.position = dataPointer;
											bitmap.setPixels(bitmap.rect, CLibrary.memory);
											for(j = 1; j < shift; ++ j)
											{
												bitmap.draw(bitmap, transform, null, null, null, true);
												try
												{
													_cubeTexture.uploadFromBitmapData(bitmap, i, j);
												}
												catch(e:Error)
												{
													CDebug.warning(e.message);
												}
											}
										}
										
										dataPointer += byteSize;
									}
									
									bitmap.dispose();
								/*}
								else
								{
									for(j = 0; j < 6; ++ j)
									{
										cubeMapFacesPointerOffset = 0;
										for(i = 0; i < _mipLevel; ++ i)
										{
											if( ( mipLevelDirtyFlag & (1 << i) ) != 0 )
											{
												CLibrary.memory.position = _cubeMapFacesPointer + cubeMapFacesPointerOffset;
												faceDirtyFlag = CLibrary.memory.readUnsignedInt();
												
												if( (faceDirtyFlag & (1 << j) ) != 0 )
												{
													try
													{
														_cubeTexture.uploadFromByteArray(CLibrary.memory, dataPointer, j, i);
													}
													catch(e:Error)
													{
														CDebug.warning(e.message);
													}
												}
												
											}
											
											dataPointer += ( 1 << ( _widthShift - i) ) * ( 1 << (_heightShift - i) ) * 4;
											
											cubeMapFacesPointerOffset += CLibrary.INT_SIZE;
										}
									}
								}*/
								break;
							default:
								if(_mipLevel)
								{
									for(i = 0; i < _mipLevel; ++ i)
									{
										if( ( mipLevelDirtyFlag & (1 << i) ) != 0 )
										{
											try
											{
												_normalTexture.uploadFromByteArray(CLibrary.memory, dataPointer, i);
											}
											catch(e:Error)
											{
												CDebug.warning(e.message);
											}
										}
										
										dataPointer += ( 1 << (_widthShift - i) ) * ( 1 << (_heightShift - i) ) * 4;
									}
								}
								else
								{
									bitmap = new BitmapData(1 << _widthShift, 1 << _heightShift, true);
									transform = new Matrix();
									transform.scale(0.5, 0.5);
									
									shift = Math.min(_widthShift, _heightShift) + 1;
									
									_normalTexture.uploadFromByteArray(CLibrary.memory, dataPointer, i);
									CLibrary.memory.position = dataPointer;
									bitmap.setPixels(bitmap.rect, CLibrary.memory);
									for(i = 0; i < shift; ++ i)
									{
										bitmap.draw(bitmap, transform, null, null, null, true);
										//if( ( mipLevelDirtyFlag & (1 << i) ) != 0 )
											_normalTexture.uploadFromBitmapData(bitmap, i);
									}
									
									bitmap.dispose();
								}
								break;
						}
					}
				}
			}
		}
		
		override public function rebuild():void
		{
			var pointers:Array        = getPointers();
			
			_type                     = pointers[0];
			
			_widthShift               = pointers[1];
			
			_heightShift              = pointers[2];
			_depthShift               = pointers[3];
			
			_format                   = pointers[4];
			
			_usage                    = pointers[5];
			
			_mipLevel                 = pointers[6];
			
			_mipLevelDirtyFlagPointer = pointers[7];
			
			_dataPointer              = pointers[8];
			
			_cubeMapFacesPointer      = pointers[9];
			
			try
			{
				if(_texture)
					_texture.dispose();
				
				var isRenderTartget:Boolean = ( _usage & (1 << USAGE_RENDER_TARGET) ) != 0;
				
				switch(_type)
				{
					case TYPE_CUBE:
						_cubeTexture = renderSystem.context.createCubeTexture(1 << _widthShift, getTextureFormat(_format), isRenderTartget);
						_texture = _cubeTexture;
						
						if(isRenderTartget)
						{
							for(var i:uint = 0; i < 6; ++ i)
							{
								renderSystem.context.setRenderToTexture(_texture, false, 0, i);
								renderSystem.context.clear(1, 1, 1);
							}
							
							renderSystem.context.setRenderToBackBuffer();
						}
					break;
					
					default:
						_normalTexture = renderSystem.context.createTexture(1 << _widthShift, 1 << _heightShift, getTextureFormat(_format), isRenderTartget);
						_texture       = _normalTexture;
						_type          = TYPE_NORMAL;
						
						if(isRenderTartget)
						{
							renderSystem.context.setRenderToTexture(_texture);
							renderSystem.context.clear(1, 1, 1);
							renderSystem.context.setRenderToBackBuffer();
						}
					break;
				}
			}
			catch(e:Error)
			{
				CDebug.error(e.message);
			}
		}
		
		private var _texture:TextureBase;
		private var _normalTexture:Texture;
		private var _cubeTexture:CubeTexture;
		
		private var _type:int;
		
		private var _widthShift:uint;
		private var _heightShift:uint;
		private var _depthShift:uint;
		
		private var _format:int;
		
		private var _usage:uint;
		
		private var _mipLevel:uint;
		
		private var _mipLevelDirtyFlagPointer:uint;
		
		private var _dataPointer:uint;
		
		private var _cubeMapFacesPointer:uint;
	}
}