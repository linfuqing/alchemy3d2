package alchemy.manager 
{
	import alchemy.compilers.CAGALAssembler;
	import alchemy.compilers.CAbstractCompiler;
	import alchemy.core.CLibrary;
	import alchemy.core.CObject;
	import alchemy.core.CPointer;
	import alchemy.data.CCubeTextureData;
	import alchemy.data.CRenderStateData;
	import alchemy.data.CTextureData;
	import alchemy.data.CVertexBufferData;
	import alchemy.data.rendermethod.CParameterInfoData;
	import alchemy.data.rendersystem.CClearData;
	import alchemy.data.rendersystem.CDeviceStateBlockData;
	import alchemy.data.rendersystem.CProgramData;
	import alchemy.data.rendersystem.CRenderData;
	import alchemy.data.rendersystem.CVertexData;
	import alchemy.effect.CEffectCode;
	import alchemy.resource.basic.CEffect;
	import alchemy.resource.basic.CIndexBuffer3D;
	import alchemy.resource.basic.CLowLevelResource;
	import alchemy.resource.basic.CProgram3D;
	import alchemy.resource.basic.CTexture;
	import alchemy.resource.basic.CVertexBuffer3D;
	import alchemy.resource.extend.CHighLevelResource;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.tools.CDebug;
	import alchemy.tools.CMath;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendFactor;
	import flash.display3D.Context3DClearMask;
	import flash.display3D.Context3DCompareMode;
	import flash.display3D.Context3DProgramType;
	import flash.display3D.Context3DStencilAction;
	import flash.display3D.Context3DTriangleFace;
	import flash.display3D.Context3DVertexBufferFormat;
	import flash.display3D.IndexBuffer3D;
	import flash.display3D.Program3D;
	import flash.display3D.VertexBuffer3D;
	import flash.geom.Matrix3D;
	import flash.utils.ByteArray;

	public class CRenderSystem extends CAbstractResourcePool
	{
		private const RESOURCE_TYPE_VERTEX_BUFFER     :int = 2;
		private const RESOURCE_TYPE_INDEX_BUFFER      :int = 3;
		private const RESOURCE_TYPE_CONSTANT_BUFFER   :int = 4;
		private const RESOURCE_TYPE_SHADER            :int = 5;
		private const RESOURCE_TYPE_EFFECT            :int = 6;
		private const RESOURCE_TYPE_TEXTURE           :int = 7;
		
		private static const CUBE_TEXTURE_SURFACE_SHIFT:int = 5;
		
		public static const MAXINUM_TEXTURE_COUNT           :uint = 8;
		
		public static const MAXINUM_VERTEX_BUFFER_COUNT     :uint = 8;
		public static const MAXINUM_VERTEX_PARAMETER_COUNT  :uint = 128;
		public static const MAXINUM_FRAGMENT_PARAMETER_COUNT:uint = 28;
		public static const MAXINUM_PARAMETER_COUNT         :uint = MAXINUM_VERTEX_PARAMETER_COUNT + MAXINUM_FRAGMENT_PARAMETER_COUNT;
		
		public function get context():Context3D
		{
			return _context;
		}
		
		public function get canCreateLowLevelResource():Boolean
		{
			return _canCreateLowLevelResource;
		}
		
		public function get currentRenderDataCount():uint
		{
			CLibrary.memory.position = _currentRenderDataCountPointer;
			return CLibrary.memory.readUnsignedInt();
		}
		
		/*public function get programVertexParameterCount():uint
		{
			CLibrary.memory.position = _programVertexParameterCountPointer;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function get programFragmentParameterCount():uint
		{
			CLibrary.memory.position = _programFragmentParameterCountPointer;
			return CLibrary.memory.readUnsignedInt();
		}*/
		
		public function CRenderSystem(totalRenderDataCount:uint, context:Context3D)
		{
			CDebug.assert(CGraphicsManager.canCreateRenderSystem, "");
			CDebug.assert(context != null                       , "need context3d.");
			
			_totalRenderDataCount = totalRenderDataCount;
			_context              = context;
			
			_complier             = new CAGALAssembler();
			
			var pointers:Array = CLibrary.method.CreateRenderSystem(_complier.pointer, _totalRenderDataCount);
			_pointer = pointers[0];
		
			super(pointers[1]);
			
			init();
		}
		
		public function configureBackBuffer(width:uint, height:uint, antiAlias:int):void
		{
			if(width && height)
			{
				_context.configureBackBuffer(width, height, antiAlias);
				
				CLibrary.method.SetViewportToContext3D(_pointer, width, height);
				//CLibrary.memory.position = _viewportWidthPointer;
				//CLibrary.memory.writeUnsignedInt(width);
				
				//CLibrary.memory.position = _viewportHeightPointer;
				//CLibrary.memory.writeUnsignedInt(height);
				
				//CLibrary.memory.position = _viewportDirtyPointer;
				//CLibrary.memory.writeBoolean(true);
			}
		}
		
		public function createTexture(bitmap:BitmapData, usage:uint, format:int = 1, mipLevel:uint = 0):CTexture
		{
			var data:Vector.<uint> = bitmap.getVector(bitmap.rect);
			var length:uint = data.length;
			var pointer:uint = CLibrary.method.CreateBuffer(length * CLibrary.INT_SIZE);
			
			CLibrary.memory.position = pointer;
			for(var i:uint = 0; i < length; ++ i)
				CLibrary.memory.writeUnsignedInt(data[i]);
			
			var handle:uint = CLibrary.method.CreateTexture(_pointer, bitmap.width, bitmap.height, usage, format, mipLevel, pointer);
			CLibrary.method.DestroyBuffer(pointer);
			
			if(handle)
				return _texturePool[handle - 1];
			
			CDebug.warning("Create texture fail.");
			
			return null;
		}
		
		public function createTextureWithData(data:CTextureData):CTexture
		{
			if(!data || !data.data)
				return null;
			
			var length:uint = data.data.length;
			var pointer:uint = length == 0 ? 0 : CLibrary.method.CreateBuffer(length);
			
			if(pointer)
			{
				CLibrary.memory.position = pointer;
				CLibrary.memory.writeBytes(data.data, 0, data.data.length);
			}
			
			var handle:uint = CLibrary.method.CreateTexture(_pointer, data.width, data.height, data.usage, CTexture.FORMAT_ARGB, data.mipLevel, pointer);
			
			if(pointer)
				CLibrary.method.DestroyBuffer(pointer);
			
			if(handle)
				return _texturePool[handle - 1];
			
			CDebug.warning("Create texture fail.");
			
			return null;
		}
		
		public function createCubeTexture(data:CCubeTextureData):CTexture
		{
			var pointer:uint = 0;
			var dataPointer:uint = 0;
			if(data.data.indexOf(null) == -1)
			{
				var length:uint = data.size * data.size;
				var i:uint;
				var size:uint = length * CLibrary.INT_SIZE;
				
				dataPointer = CLibrary.method.CreateBuffer(length * CLibrary.INT_SIZE * 6);
				CLibrary.memory.position = dataPointer;
				for(i = 0; i < 6; ++ i)
					CLibrary.memory.writeBytes(data.data[i], 0, size );
				
				var offset:uint = 0;
				
				pointer = CLibrary.method.CreateBuffer(6 * CLibrary.POINTER_SIZE);
				CLibrary.memory.position = pointer;
				for(i = 0; i < 6; ++ i)
				{
					CLibrary.memory.writeUnsignedInt(dataPointer + offset);
					
					offset += size;
				}
			}
			
			var handle:uint = CLibrary.method.CreateCubeTexture(_pointer, data.size, data.usage, CTexture.FORMAT_ARGB, 1, pointer);
			
			if(dataPointer)
				CLibrary.method.DestroyBuffer(dataPointer);
			
			if(pointer)
				CLibrary.method.DestroyBuffer(pointer);
			
			if(handle)
				return _texturePool[handle - 1];
			
			CDebug.warning("Create texture fail.");
			
			return null;
		}
		
		public function createProgram(compiler:CAbstractCompiler, vertexCode:String, fragmentCode:String):CProgram3D
		{
			var handle:uint = compiler.createProgram(this, vertexCode, fragmentCode);
			
			if(handle)
				return _programPool[handle - 1];
			
			CDebug.warning("Create program fail.");
			
			return null;
		}
		
		public function createEffect(passCount:uint):CEffect
		{
			var handle:uint = CLibrary.method.CreateEffect(_pointer, passCount);
			
			if(handle)
				return _effectPool[handle - 1];
			
			CDebug.warning("Create effect fail.");
			
			return null;
		}
		
		public function createEffectFromEffectCode(effectCode:CEffectCode):CEffect
		{
			if(!effectCode)
			{
				CDebug.warning("effect code can not be null");
				
				return null;
			}
			
			var handle:uint = CLibrary.method.CreateEffectFromEffectCode(_pointer, effectCode.pointer);
			
			if(handle)
				return _effectPool[handle - 1];
			
			CDebug.warning("Create effect fail.");
			
			return null;
		}
		
		public function getVertexBuffer(handle:uint):CVertexBuffer3D
		{
			return _vertexBufferPool[handle - 1];
		}
		
		public function getIndexBuffer(handle:uint):CIndexBuffer3D
		{
			return _indexBufferPool[handle - 1];
		}
		
		public function getProgram(handle:uint):CProgram3D
		{
			return _programPool[handle - 1];
		}
		
		public function getTexture(handle:uint):CTexture
		{
			return _texturePool[handle - 1];
		}
		
		public function getEffect(handle:uint):CEffect
		{
			return _effectPool[handle - 1];
		}
		
		private function init():void
		{
			var pointers:Array = getPointers();
			
			_renderDataQueuePointer               = pointers[0];
			_currentRenderDataCountPointer        = pointers[1];
			_programVertexParameterPointer        = pointers[2];
			//_programVertexParameterCountPointer   = pointers[3];
			_programFragmentParameterPointer      = pointers[3];
			//_programFragmentParameterCountPointer = pointers[5];
			_programTexturePointer                = pointers[4];
			//_viewportWidthPointer                 = pointers[5];
			//_viewportHeightPointer                = pointers[6];
			//_viewportDirtyPointer                 = pointers[7];
			
			fill(create, removeResource, render);
			
			_vertexBufferPool = new Vector.<CVertexBuffer3D>();
			_indexBufferPool  = new Vector.<CIndexBuffer3D>();
			_programPool      = new Vector.<CProgram3D>();
			_texturePool      = new Vector.<CTexture>();
			_effectPool       = new Vector.<CEffect>();
			
			_parameter        = new Vector.<Number>(4 * Math.max(MAXINUM_VERTEX_PARAMETER_COUNT, MAXINUM_FRAGMENT_PARAMETER_COUNT), true);
			
			var length:uint = _parameter.length;
			for(var i:uint = 0; i < length; ++ i)
				_parameter[i] = 0;
			//_matrixParameter  = new Vector.<Number>(16, true);
			
			//_matrix           = new Matrix3D();
			
			_renderData       = new CRenderData();
			_vertexBufferData = new CVertexData();
			
			_indexBuffer   = null;
			
			_currentRenderTarget = 0;
			
			_cullMode  = new Vector.<String>(CRenderStateData.TOTAL_CULL_MODE , true);
			_CMPFunc   = new Vector.<String>(CRenderStateData.TOTAL_CMP_FUNC  , true);
			_blendMode = new Vector.<String>(CRenderStateData.TOTAL_BLEND_MODE, true);
			_stencilOP = new Vector.<String>(CRenderStateData.TOTAL_STENCIL_OP, true);
			
			_cullMode[CRenderStateData.CULL_MODE_NONE] = Context3DTriangleFace.NONE;
			_cullMode[CRenderStateData.CULL_MODE_CCW ] = Context3DTriangleFace.BACK;
			_cullMode[CRenderStateData.CULL_MODE_CW  ] = Context3DTriangleFace.FRONT;
			
			_CMPFunc[CRenderStateData.CMP_FUNC_NEVER        ] = Context3DCompareMode.NEVER;
			_CMPFunc[CRenderStateData.CMP_FUNC_LESS         ] = Context3DCompareMode.LESS;
			_CMPFunc[CRenderStateData.CMP_FUNC_EQUAL        ] = Context3DCompareMode.EQUAL;
			_CMPFunc[CRenderStateData.CMP_FUNC_LESS_EQUAL   ] = Context3DCompareMode.LESS_EQUAL;
			_CMPFunc[CRenderStateData.CMP_FUNC_GREATER      ] = Context3DCompareMode.GREATER;
			_CMPFunc[CRenderStateData.CMP_FUNC_NOT_EQUAL    ] = Context3DCompareMode.NOT_EQUAL;
			_CMPFunc[CRenderStateData.CMP_FUNC_GREATER_EQUAL] = Context3DCompareMode.GREATER_EQUAL;
			_CMPFunc[CRenderStateData.CMP_FUNC_ALWAYS       ] = Context3DCompareMode.ALWAYS;
			
			_blendMode[CRenderStateData.BLEND_MODE_ZERO              ] = Context3DBlendFactor.ZERO;
			_blendMode[CRenderStateData.BLEND_MODE_ONE               ] = Context3DBlendFactor.ONE;
			_blendMode[CRenderStateData.BLEND_MODE_SRC_COLOR         ] = Context3DBlendFactor.SOURCE_COLOR;
			_blendMode[CRenderStateData.BLEND_MODE_INV_SRC_COLOR     ] = Context3DBlendFactor.ONE_MINUS_SOURCE_COLOR;
			_blendMode[CRenderStateData.BLEND_MODE_SRC_ALPHA         ] = Context3DBlendFactor.SOURCE_ALPHA;
			_blendMode[CRenderStateData.BLEND_MODE_INV_SRC_ALPHA     ] = Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA;
			_blendMode[CRenderStateData.BLEND_MODE_DEST_ALPHA        ] = Context3DBlendFactor.DESTINATION_ALPHA;
			_blendMode[CRenderStateData.BLEND_MODE_INV_DEST_ALPHA    ] = Context3DBlendFactor.ONE_MINUS_DESTINATION_ALPHA;
			_blendMode[CRenderStateData.BLEND_MODE_DEST_COLOR        ] = Context3DBlendFactor.DESTINATION_COLOR;
			_blendMode[CRenderStateData.BLEND_MODE_INV_DEST_COLOR    ] = Context3DBlendFactor.ONE_MINUS_DESTINATION_COLOR;
			_blendMode[CRenderStateData.BLEND_MODE_SRC_ALPHA_SAT     ] = null;
			_blendMode[CRenderStateData.BLEND_MODE_BOTH_SRC_ALPHA    ] = null;
			_blendMode[CRenderStateData.BLEND_MODE_BOTH_INV_SRC_ALPHA] = null;
			
			_stencilOP[CRenderStateData.STENCIL_OP_KEEP    ] = Context3DStencilAction.KEEP;
			_stencilOP[CRenderStateData.STENCIL_OP_THROW   ] = Context3DStencilAction.ZERO;
			_stencilOP[CRenderStateData.STENCIL_OP_REPLACE ] = Context3DStencilAction.SET;
			_stencilOP[CRenderStateData.STENCIL_OP_INCR_SAT] = Context3DStencilAction.INCREMENT_SATURATE;
			_stencilOP[CRenderStateData.STENCIL_OP_DECR_SAT] = Context3DStencilAction.DECREMENT_SATURATE;
			_stencilOP[CRenderStateData.STENCIL_OP_INVERT  ] = Context3DStencilAction.INVERT;
			_stencilOP[CRenderStateData.STENCIL_OP_INCR    ] = Context3DStencilAction.INCREMENT_WRAP;
			_stencilOP[CRenderStateData.STENCIL_OP_DECR    ] = Context3DStencilAction.DECREMENT_WRAP;
			
			_depthWriteEnable   = true;
			_depthFunction      = Context3DCompareMode.ALWAYS;
			
			_blendSource        = Context3DBlendFactor.ONE;
			_blendDestination   = Context3DBlendFactor.ZERO;
			
			_currentCullMode    = Context3DTriangleFace.NONE;
			
			_stencilFunction    = Context3DCompareMode.ALWAYS;
			_stencilFail        = Context3DStencilAction.KEEP;
			_stencilZFail       = Context3DStencilAction.KEEP;
			_stencilPass        = Context3DStencilAction.KEEP;
			
			_CCWStencilFail     = Context3DStencilAction.KEEP;
			_CCWStencilZFail    = Context3DStencilAction.KEEP;
			_CCWStencilPass     = Context3DStencilAction.KEEP;
			_CCWStencilFunction = Context3DCompareMode.ALWAYS;
			
			_canCreateLowLevelResource = false;
		}
		
		private function create(pointer:uint, interfacePointer:uint, type:int, handle:uint):CLowLevelResource
		{
			var resource:CLowLevelResource = null;
			
			_canCreateLowLevelResource = true;
			switch(type)
			{
				case RESOURCE_TYPE_VERTEX_BUFFER:
					if(handle == _vertexBufferPool.length + 1)
						_vertexBufferPool.push( resource = new CVertexBuffer3D(pointer, handle, interfacePointer, this) );
					else if(handle && handle <= _vertexBufferPool.length)
					{
						resource = _vertexBufferPool[handle - 1];
						if(resource && resource.pointer == pointer)
							resource.rebuild();
						else
							resource = _vertexBufferPool[handle - 1] = new CVertexBuffer3D(pointer, handle, interfacePointer, this);
					}
					else
						CDebug.error("Vertex buffer create handle error.");
					break;
				case RESOURCE_TYPE_INDEX_BUFFER:
					//unlock( resource = new CIndexBuffer3D(pointer, lock(_indexBufferPool), interfacePointer, this) );
					if(handle == _indexBufferPool.length + 1)
						_indexBufferPool.push( resource = new CIndexBuffer3D(pointer, handle, interfacePointer, this) );
					else if(handle && handle <= _indexBufferPool.length)
					{
						resource = _indexBufferPool[handle - 1];
						if(resource && resource.pointer == pointer)
							resource.rebuild();
						else
							resource = _indexBufferPool[handle - 1] = new CIndexBuffer3D(pointer, handle, interfacePointer, this);
					}
					else
						CDebug.error("Index buffer create handle error.");
					break;
				case RESOURCE_TYPE_SHADER:
					//unlock( resource = new CProgram3D(pointer, lock(_programPool), interfacePointer, this) );
					if(handle == _programPool.length + 1)
						_programPool.push( resource = new CProgram3D(pointer, handle, interfacePointer, this) );
					else if(handle && handle <= _programPool.length)
					{
						resource = _programPool[handle - 1];
						if(resource && resource.pointer == pointer)
							resource.rebuild();
						else
							resource = _programPool[handle - 1] = new CProgram3D(pointer, handle, interfacePointer, this);
					}
					else
						CDebug.error("Program create handle error.");
					break;
				case RESOURCE_TYPE_EFFECT:
					//unlock( resource = new CEffect(pointer, lock(_effectPool), interfacePointer, this) );
					if(handle == _effectPool.length + 1)
						_effectPool.push( resource = new CEffect(pointer, handle, interfacePointer, this) );
					else if(handle && handle <= _effectPool.length)
					{
						resource = _effectPool[handle - 1];
						if(resource && resource.pointer == pointer)
							resource.rebuild();
						else
							resource = _effectPool[handle - 1] = new CEffect(pointer, handle, interfacePointer, this);
					}
					else
						CDebug.error("Effect create handle error.");
					break;
				case RESOURCE_TYPE_TEXTURE:
					if(handle == _texturePool.length + 1)
						_texturePool.push( resource = new CTexture(pointer, handle, interfacePointer, this) );
					else if(handle && handle <= _texturePool.length)
					{
						resource = _texturePool[handle - 1];
						if(resource && resource.pointer == pointer)
							resource.rebuild();
						else
							resource = _texturePool[handle - 1] = new CTexture(pointer, handle, interfacePointer, this);
					}
					else
						CDebug.error("Texture create handle error.");
					break;
			}
			_canCreateLowLevelResource = false;
			
			return resource;
		}
		
		private function removeResource(handle:int, type:int):void
		{
			if(handle)
			{
				switch(type)
				{
					case RESOURCE_TYPE_VERTEX_BUFFER:
						_vertexBufferPool[handle - 1] = null;
						break;
					case RESOURCE_TYPE_INDEX_BUFFER:
						_indexBufferPool[handle - 1] = null;
						break;
					case RESOURCE_TYPE_SHADER:
						_programPool[handle - 1] = null;
						break;
					case RESOURCE_TYPE_EFFECT:
						_effectPool[handle - 1] = null;
						break;
					case RESOURCE_TYPE_TEXTURE:
						_texturePool[handle - 1] = null;
						break;
				}
			}
		}
		
		private function render():void
		{
			var currentRenderDataCount:uint = this.currentRenderDataCount;
			var renderDataPointer:uint = _renderDataQueuePointer;
			
			//_programVertexParameterPointerOffset   = 0;
			//_programFragmentParameterPointerOffset = 0;
			_programTexturePointerOffset   = 0;

			for(_currentRenderDataIndex = 0; _currentRenderDataIndex < currentRenderDataCount; ++ _currentRenderDataIndex)
			{
				_renderData.attach(renderDataPointer);
				
				renderDataHandler(_renderData);
				
				renderDataPointer += CRenderData.SIZE;
			}
		}
		
		private function renderDataHandler(data:CRenderData):void
		{
			var flag:uint = data.flag;
			
			deviceStateBlockHandler(data.deviceStateBlockData);
			
			var isRenderTargetDirty:Boolean = ( (flag & (1 << CRenderData.FLAG_RENDER_TARGET_DIRTY_BIT) ) != 0 );
			
			if(isRenderTargetDirty)
				renderTargetHandler(true, data.renderTargetHandle, data.renderTargetSurface, ( ( flag & (1 << CRenderData.FLAG_CLEAR_DIRTY_BIT) ) != 0 ) ? data.clearData : null);
			else
				renderTargetHandler(false, 0, 0, ( ( flag & (1 << CRenderData.FLAG_CLEAR_DIRTY_BIT) ) != 0 ) ? data.clearData : null);
			
			var vertexIndexFlag :uint = (flag >> CRenderData.FLAG_VERTEX_INDEX_DIRTY_SHIFT ) & 0xff;
			var vertexBufferFlag:uint = (flag >> CRenderData.FLAG_VERTEX_BUFFER_DIRTY_SHIFT) & 0xff;
			
			//this.context.setCulling(Context3DTriangleFace.NONE);
			
			if(vertexIndexFlag || vertexBufferFlag)
			{
				var vertexFlag :uint = vertexIndexFlag | vertexBufferFlag;
				var vertexEnd  :uint = CMath.maxinumBitTable[vertexFlag];
				var vertexBegin:uint = CMath.mininumBitTable[vertexFlag] - 1;
				
				var vertexData:CVertexData = data.beginReadVertexData(vertexBegin);
				
				var isVertexIndexDirty:Boolean;
				var isVertexBufferDirty:Boolean;
				
				while(vertexData && vertexBegin != vertexEnd)
				{
					isVertexIndexDirty = ( vertexIndexFlag & (1 << vertexBegin) ) != 0;
					isVertexBufferDirty = ( vertexBufferFlag & (1 << vertexBegin) ) != 0;
					if(isVertexIndexDirty || isVertexBufferDirty)
						vertexDataHandler(vertexData, vertexBegin, isVertexIndexDirty, isVertexBufferDirty);
					
					vertexData = data.nextVertexData();
					
					++ vertexBegin;
				}
			}
			
			var programDirtyFlag:uint = 1 << CRenderData.FLAG_PROGRAM_DIRTY_BIT;
			programDataHandler(data.programData, (flag & programDirtyFlag) == programDirtyFlag, flag >> CRenderData.FLAG_TEXTURE_DIRTY_SHIFT);
			
			if(!_indexBuffer ||  flag & (1 << CRenderData.FLAG_INDEX_BUFFER_DIRTY_BIT) )
			{
				var indexBuffer:CIndexBuffer3D = _indexBufferPool[data.indexBufferHandle - 1];
				if(indexBuffer)
				{
					indexBuffer.updata();
					
					_indexBuffer = indexBuffer;
				}
				else
					CDebug.warning("Error indexBuffer handle.");
			}

			try
			{
				_context.drawTriangles(_indexBuffer.instance, data.start, data.triangleCount);
			}
			catch(e:Error)
			{
				CDebug.error(e.message);
			}
		}
		
		
		private function deviceStateBlockHandler(data:CDeviceStateBlockData):void
		{
			var dirtyFlag:uint = data.dirtyFlag;
			
			if(dirtyFlag)
			{
				var depthWriteDirty:Boolean = ( dirtyFlag & (1 << CDeviceStateBlockData.DEPTH_WRITE_ENABLE) ) != 0;
				var depthFunctionDirty:Boolean = ( dirtyFlag & (1 << CDeviceStateBlockData.DEPTH_FUNC) ) != 0;
				
				if(depthWriteDirty || depthFunctionDirty)
				{
					if(depthWriteDirty)
						_depthWriteEnable = data.depthWriteEnable;
					
					if(depthFunctionDirty)
						_depthFunction = _CMPFunc[data.depthFunc];
					
					try
					{
						context.setDepthTest(_depthWriteEnable, _depthFunction);
					}
					catch(e:Error)
					{
						CDebug.error(e.message);
					}
				}
				
				var blendSourceDirty:Boolean = ( dirtyFlag & (1 << CDeviceStateBlockData.SRC_BLEND) ) != 0;
				var blendDestinationDirty:Boolean = ( dirtyFlag & (1 << CDeviceStateBlockData.DEST_BLEND) ) != 0;
				if(blendSourceDirty || blendDestinationDirty)
				{
					if(blendSourceDirty)
						_blendSource = _blendMode[data.srcBlend];
					
					if(blendDestinationDirty)
						_blendDestination = _blendMode[data.destBlend];

					try
					{
						context.setBlendFactors(_blendSource, _blendDestination);
					}
					catch(e:Error)
					{
						CDebug.error(e.message);
					}
				}
				
				if( ( dirtyFlag & (1 << CDeviceStateBlockData.CULL_MODE) ) != 0 )
				{
					_currentCullMode = _cullMode[data.cullMode];
					
					try
					{
						context.setCulling(_currentCullMode);
					}
					catch(e:Error)
					{
						CDebug.error(e.message);
					}
				}
				
				var stencilFunctionDirty:Boolean;
				var stencilFailDirty    :Boolean;
				var stencilZFailDirty   :Boolean;
				var stencilPassDirty    :Boolean;
				
				stencilFunctionDirty = ( dirtyFlag & (1 << CDeviceStateBlockData.STENCIL_FUNC  ) ) != 0;
				stencilFailDirty     = ( dirtyFlag & (1 << CDeviceStateBlockData.STENCIL_FAIL  ) ) != 0;
				stencilZFailDirty    = ( dirtyFlag & (1 << CDeviceStateBlockData.STENCIL_Z_FAIL) ) != 0;
				stencilPassDirty     = ( dirtyFlag & (1 << CDeviceStateBlockData.STENCIL_PASS  ) ) != 0;
				if(stencilFunctionDirty || stencilFailDirty || stencilZFailDirty || stencilPassDirty)
				{
					if(stencilFunctionDirty)
						_stencilFunction = _CMPFunc[data.stencilFunc];
					
					if(stencilFailDirty)
						_stencilFail = _stencilOP[data.stencilFail];
					
					if(stencilZFailDirty)
						_stencilZFail = _stencilOP[data.stencilZFail];
					
					if(stencilPassDirty)
						_stencilPass = _stencilOP[data.stencilPass];
					
					try
					{
						_context.setStencilActions(Context3DTriangleFace.FRONT, _stencilFunction, _stencilPass, _stencilZFail, _stencilFail); 
					}
					catch(e:Error)
					{
						CDebug.error(e.message);
					}
				}
				
				stencilFunctionDirty = ( dirtyFlag & (1 << CDeviceStateBlockData.CCW_STENCIL_FUNC  ) ) != 0;
				stencilFailDirty     = ( dirtyFlag & (1 << CDeviceStateBlockData.CCW_STENCIL_FAIL  ) ) != 0;
				stencilZFailDirty    = ( dirtyFlag & (1 << CDeviceStateBlockData.CCW_STENCIL_Z_FAIL) ) != 0;
				stencilPassDirty     = ( dirtyFlag & (1 << CDeviceStateBlockData.CCW_STENCIL_PASS  ) ) != 0;
				if(stencilFunctionDirty || stencilFailDirty || stencilZFailDirty || stencilPassDirty)
				{
					if(stencilFunctionDirty)
						_CCWStencilFunction = _CMPFunc[data.CCWStencilFunc];
					
					if(stencilFailDirty)
						_CCWStencilFail = _stencilOP[data.CCWStencilFail];
					
					if(stencilZFailDirty)
						_CCWStencilZFail = _stencilOP[data.CCWStencilZFail];
					
					if(stencilPassDirty)
						_CCWStencilPass = _stencilOP[data.CCWStencilPass];
					
					try
					{
						_context.setStencilActions(Context3DTriangleFace.BACK, _CCWStencilFunction, _CCWStencilPass, _CCWStencilZFail, _CCWStencilFail); 
					}
					catch(e:Error)
					{
						CDebug.error(e.message);
					}
				}
				
				var stencilRefDirty:Boolean  = ( dirtyFlag & (1 << CDeviceStateBlockData.STENCIL_REF) ) != 0;
				var stencilReadMask:Boolean  = ( dirtyFlag & (1 << CDeviceStateBlockData.STENCIL_READ_MASK) ) != 0;
				var stencilWriteMask:Boolean = ( dirtyFlag & (1 << CDeviceStateBlockData.STENCIL_WRITE_MASK) ) != 0;
				
				if(stencilRefDirty || stencilReadMask || stencilWriteMask)
				{
					try
					{
						_context.setStencilReferenceValue(data.stencilRef, data.stencilReadMask, data.stencilWriteMask);
					}
					catch(e:Error)
					{
						CDebug.error(e.message);
					}
				}
				
				if( ( dirtyFlag & (1 << CDeviceStateBlockData.COLOR_WRITE_ENABLE) ) != 0 )
				{
					var colorWriteEnble:uint = data.colorWriteEnable;
					
					try
					{
						_context.setColorMask(
							( colorWriteEnble & (1 << CRenderStateData.COLOR_WRITE_ENABLE_RED) ) != 0,
							( colorWriteEnble & (1 << CRenderStateData.COLOR_WRITE_ENABLE_GREEN) ) != 0,
							( colorWriteEnble & (1 << CRenderStateData.COLOR_WRITE_ENABLE_BLUE) ) != 0,
							( colorWriteEnble & (1 << CRenderStateData.COLOR_WRITE_ENABLE_ALPHA) ) != 0);
					}
					catch(e:Error)
					{
						CDebug.error(e.message);
					}
				}
			}
		}
		
		private function renderTargetHandler(isRenderTargetDirty:Boolean, renderTargetHandle:uint, renderTargetSurface:uint, clearData:CClearData):void
		{
			if(isRenderTargetDirty)
			{
				if(renderTargetHandle)
				{
					var texture:CTexture = _texturePool[renderTargetHandle - 1];
					
					try
					{
						_context.setRenderToTexture(texture.instance, clearData && (clearData.flag & (1 << CClearData.ZBUFFER_BIT) ) != 0, 0, renderTargetSurface >> CUBE_TEXTURE_SURFACE_SHIFT);
						
						_currentRenderTarget = renderTargetHandle;
					}
					catch(e:Error)
					{
						CDebug.error(e.message);
					}
				}
				else
				{
					try
					{
						_context.setRenderToBackBuffer();
						
						_currentRenderTarget = 0;
					}
					catch(e:Error)
					{
						CDebug.error(e.message);
					}
				}
			}
			
			if(renderTargetHandle || clearData)
			{
				var red:Number = 0;
				var green:Number = 0;
				var blue:Number = 0;
				var alpha:Number = 1;
				
				if(clearData)
				{
					var color:uint = clearData.color;
					var inverce255:Number = 1 / 255;
					
					red = ( (color >> 16) & 0xff ) * inverce255;
					green = ( (color >> 8) & 0xff ) * inverce255;
					blue = (color & 0xff) * inverce255;
					alpha = ( (color >> 24) & 0xff ) * inverce255;
				}
				
				var mask:uint = 0;
				
				if(renderTargetHandle)
					mask = Context3DClearMask.STENCIL | Context3DClearMask.COLOR | Context3DClearMask.DEPTH;
				else
				{
					var flag:uint = clearData.flag;
					
					if( ( flag & (1 << CClearData.STENCIL_BIT) ) != 0 )
						mask |= Context3DClearMask.STENCIL;
					
					if( ( flag & (1 << CClearData.TARGET_BIT) ) != 0 )
						mask |= Context3DClearMask.COLOR;
					
					if( (flag & (1 << CClearData.ZBUFFER_BIT) ) != 0 )
						mask |= Context3DClearMask.DEPTH;
				}
				
				try
				{
					_context.clear(red, green, blue, alpha, 1.0, 0, mask);
				}
				catch(e:Error)
				{
					CDebug.error(e.message);
				}
			}
		}
		
		private function vertexDataHandler(data:CVertexData, index:uint, isVertexIndexDirty:Boolean, isVertexBufferDirty:Boolean):void
		{
			var handle:uint = data.vertexBufferHandle;
			if(handle)
			{
				var vertexBuffer:CVertexBuffer3D = _vertexBufferPool[handle - 1];
				if(vertexBuffer)
				{
					if(isVertexBufferDirty)
						vertexBuffer.updata();
					
					if(isVertexIndexDirty)
					{
						try
						{
							_context.setVertexBufferAt(index, vertexBuffer.instance, data.offsetInBytes >> 2, data.format);
						}
						catch(e:Error)
						{
							CDebug.error(e.message);
						}
					}
				}
				else
					CDebug.warning("Error vertexBuffer handle.");
			}
			else
				_context.setVertexBufferAt(index, null);
		}
		
		private function programDataHandler(data:CProgramData, programDirty:Boolean, textureDirtyFlag:uint):void
		{
			var handle:uint;
			if(programDirty)
			{
				handle = data.programHandle;
				var program:CProgram3D;
				if( handle && ( (program = _programPool[handle - 1]) != null ) )
				{
					try
					{
						_context.setProgram(program.instance);
					}
					catch(e:Error)
					{
						CDebug.error(e.message);
					}
				}
				else
				{
					CDebug.warning("Error program handle.");
					
					return;
				}
			}
			
			//var parameterCount:uint;
			//var parameterIndex:uint;
			var parameterSize:uint;

			var i:uint;
			//var j:uint;
			
			var memory:ByteArray = CLibrary.memory;
			
			var dirtyVertexParameterOffset:uint = data.dirtyVertexParameterOffset;
			var dirtyVertexParameterCount:uint = data.dirtyVertexParameterCount;
			if(dirtyVertexParameterCount > dirtyVertexParameterOffset)
			{
				parameterSize = dirtyVertexParameterCount - dirtyVertexParameterOffset;
				memory.position = _programVertexParameterPointer + dirtyVertexParameterOffset * CLibrary.FLOAT_SIZE;
				for(i = 0; i < parameterSize; ++ i)
					_parameter[i] = memory.readFloat();
				
				_context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, (dirtyVertexParameterOffset - _currentRenderDataIndex * MAXINUM_VERTEX_PARAMETER_COUNT * 4) >> 2, _parameter, (parameterSize + 3) >> 2);
			}
			
			var dirtyFragmentParameterOffset:uint = data.dirtyFragmentParameterOffset;
			var dirtyFragmentParameterCount:uint = data.dirtyFragmentParameterCount;
			if(dirtyFragmentParameterCount > dirtyFragmentParameterOffset)
			{
				parameterSize = dirtyFragmentParameterCount - dirtyFragmentParameterOffset;
				memory.position = _programFragmentParameterPointer + dirtyFragmentParameterOffset * CLibrary.FLOAT_SIZE;
				for(i = 0; i < parameterSize; ++ i)
					_parameter[i] = memory.readFloat();
				
				_context.setProgramConstantsFromVector(Context3DProgramType.FRAGMENT, (dirtyFragmentParameterOffset - _currentRenderDataIndex * MAXINUM_FRAGMENT_PARAMETER_COUNT * 4) >> 2, _parameter, (parameterSize + 3) >> 2);
			}
			
			//const STRIDE:uint = CLibrary.FLOAT_SIZE * 4;
			/*parameterCount = data.vertexParameterCount;
			if(parameterCount)
			{
				var programVertexParameterOffset:uint = _currentRenderDataIndex * MAXINUM_VERTEX_PARAMETER_COUNT * 4;
				var programVertexParameterPointerOffset:uint = _programVertexParameterPointer + programVertexParameterOffset * CLibrary.FLOAT_SIZE;
				var programVertexOffset:uint = data.dirtyVertexParameterOffset - programVertexParameterOffset;
				for(i = 0; i < parameterCount; ++ i)
				{
					parameterSize = data.getVertexParameterSize(i);
					
					parameterIndex = data.getVertexParameterIndex(i);
					
					memory.position = programVertexParameterPointerOffset + parameterIndex * CLibrary.FLOAT_SIZE;// * STRIDE;
					parameterIndex = parameterIndex - programVertexOffset;
					parameterSize += parameterIndex;
					for(j = parameterIndex; j < parameterSize; ++ j)
						_parameter[j] = memory.readFloat();
					
					//for(j = parameterSize; j < 4; ++ j)
					//	_parameter[j] = 0;
					
					//_context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, parameterIndex, _parameter, (parameterSize + 3) / 4);
					
					//_programVertexParameterPointerOffset += parameterSize * CLibrary.FLOAT_SIZE;
				}
				
				_context.setProgramConstantsFromVector(Context3DProgramType.VERTEX, programVertexOffset >> 2, _parameter, (data.dirtyVertexParameterCount - programVertexParameterOffset + 3) >> 2);
			}
			
			parameterCount = data.fragmentParameterCount;
			if(parameterCount)
			{
				var programFragmentParameterOffset:uint = _currentRenderDataIndex * MAXINUM_FRAGMENT_PARAMETER_COUNT * 4;
				var programFragmentParameterPointerOffset:uint = _programFragmentParameterPointer + programFragmentParameterOffset * CLibrary.FLOAT_SIZE;
				var programFragmentOffset:uint = data.dirtyFragmentParameterOffset - programFragmentParameterOffset;
				for(i = 0; i < parameterCount; ++ i)
				{
					parameterSize = data.getFragmentParameterSize(i);
					
					parameterIndex = data.getFragmentParameterIndex(i);
					
					memory.position = programFragmentParameterPointerOffset + parameterIndex * CLibrary.FLOAT_SIZE;// * STRIDE;
					parameterIndex = parameterIndex - programFragmentOffset;
					parameterSize += parameterIndex;
					for(j = parameterIndex; j < parameterSize; ++ j)
						_parameter[j] = memory.readFloat();
	
					//for(j = parameterSize; j < 4; ++ j)
					//	_parameter[j] = 0;
					
					//_context.setProgramConstantsFromVector(Context3DProgramType.FRAGMENT, parameterIndex, _parameter, (parameterSize + 3) / 4);
					
					//_programFragmentParameterPointerOffset += parameterSize * CLibrary.FLOAT_SIZE;
				}
				
				_context.setProgramConstantsFromVector(Context3DProgramType.FRAGMENT, programFragmentOffset >> 2, _parameter, (data.dirtyFragmentParameterCount - programFragmentParameterOffset + 3) >> 2);
			}*/
			
			if(textureDirtyFlag)
			{
				var texture:CTexture;
				var end  :uint = CMath.maxinumBitTable[textureDirtyFlag];
				var begin:uint = CMath.mininumBitTable[textureDirtyFlag] - 1;
				
				var programTexturePointer:uint = _programTexturePointer + _programTexturePointerOffset + (begin << 1);
				var textureFlag:uint = data.textureFlag;
				for(i = begin; i < end; ++ i)
				{
					if( ( textureDirtyFlag & (1 << i) ) != 0 )
					{
						memory.position = programTexturePointer;
						
						handle = CLibrary.memory.readUnsignedShort();
						if(handle)
						{
							if( (texture = _texturePool[handle - 1]) != null )
							{
								if(  ( textureFlag & (1 << i) ) != 0 )
									texture.updata();
							
								_context.setTextureAt(i, texture.instance);
							}
							else
								CDebug.warning("Error texture handle.");
						}
						else
							_context.setTextureAt(i, null);
					}
					
					programTexturePointer += 2;
				}
			}
			
			_programTexturePointerOffset += MAXINUM_TEXTURE_COUNT << 1;
		}
		
		private var _context:Context3D;
		
		private var _indexBuffer:CIndexBuffer3D;
		
		//private var _programVertexParameterPointerOffset:uint;
		//private var _vertexFirstRegister:uint;
		
		//private var _programFragmentParameterPointerOffset:uint;
		//private var _fragmentFirstRegister:uint;
		private var _currentRenderDataIndex:uint;
		
		private var _programTexturePointerOffset:uint;
		
		private var _parameter:Vector.<Number>;

		private var _renderData:CRenderData;
		private var _vertexBufferData:CVertexData;
		
		private var _vertexBufferPool:Vector.<CVertexBuffer3D>;
		private var _indexBufferPool:Vector.<CIndexBuffer3D>;
		private var _programPool:Vector.<CProgram3D>;
		private var _texturePool:Vector.<CTexture>;
		private var _effectPool:Vector.<CEffect>;
		
		private var _renderDataQueuePointer:uint;
		private var _currentRenderDataCountPointer:uint;
		private var _programVertexParameterPointer:uint;
		//private var _programVertexParameterCountPointer:uint;
		private var _programFragmentParameterPointer:uint;
		//private var _programFragmentParameterCountPointer:uint;
		private var _programTexturePointer:uint;
		//private var _viewportWidthPointer:uint;
		//private var _viewportHeightPointer:uint;
		//private var _viewportDirtyPointer:uint;
		
		private var _totalRenderDataCount:uint;
		
		private var _currentRenderTarget:uint;
		
		private var _cullMode :Vector.<String>;
		private var _CMPFunc  :Vector.<String>;
		private var _blendMode:Vector.<String>;
		private var _stencilOP:Vector.<String>;
		
		private var _depthWriteEnable:Boolean;
		private var _depthFunction:String;
		
		private var _blendSource:String;
		private var _blendDestination:String;
		
		private var _currentCullMode:String;
		
		private var _stencilFunction:String;
		private var _stencilFail:String;
		private var _stencilZFail:String;
		private var _stencilPass:String;
		
		private var _CCWStencilFail:String;
		private var _CCWStencilZFail:String;
		private var _CCWStencilPass:String;
		private var _CCWStencilFunction:String;
		
		private var _complier:CAGALAssembler;
		
		private var _canCreateLowLevelResource:Boolean;
	}
}