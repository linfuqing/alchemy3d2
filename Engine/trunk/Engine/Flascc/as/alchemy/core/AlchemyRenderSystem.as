package alchemy.core
{
	import alchemy.utils.AlchemyLog;
	import alchemy.utils.AlchemyRenderInfo;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DClearMask;
	import flash.display3D.Context3DProgramType;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.Context3DVertexBufferFormat;
	import flash.display3D.IndexBuffer3D;
	import flash.display3D.Program3D;
	import flash.display3D.VertexBuffer3D;
	import flash.display3D.textures.CubeTexture;
	import flash.display3D.textures.Texture;
	import flash.display3D.textures.TextureBase;
	import flash.utils.ByteArray;

	public class AlchemyRenderSystem
	{
		public static const TYPE_SIZE_INT:uint = 4;
		public static const TYPE_SIZE_FLOAT:uint = 4;
		public static const TYPE_SIZE_POINTER:uint = 4;
		
		private const RESOURCE_TYPE_VERTEX_BUFFER     :int = 2;
		private const RESOURCE_TYPE_INDEX_BUFFER      :int = 3;
		private const RESOURCE_TYPE_CONSTANT_BUFFER   :int = 4;
		private const RESOURCE_TYPE_SHADER            :int = 5;
		//private const RESOURCE_TYPE_EFFECT            :int = 6;
		private const RESOURCE_TYPE_TEXTURE           :int = 7;
		
		public static const TEXTURE_TYPE_NORMAL:int = 0;
		public static const TEXTURE_TYPE_CUBE  :int = 1;
		public static const TEXTURE_TYPE_VOLUME:int = 2;
		
		public static const TEXTURE_MIP_MAP_SURFACE_SHIFT:uint = 5;
		public static const TEXTURE_MIP_MAP_SURFACE_MASK:uint = (1 << TEXTURE_MIP_MAP_SURFACE_SHIFT) - 1;
		
		public static const CLEAR_STENCIL_BIT    :int = 0;
		public static const CLEAR_TARGET_BIT     :int = 1;
		public static const CLEAR_ZBUFFER_BIT    :int = 2;
		
		public static const VERTEX_ELEMENT_TYPE_FLOAT1:int = AlchemyRenderSystem.TYPE_SIZE_INT * 1;
		public static const VERTEX_ELEMENT_TYPE_FLOAT2:int = AlchemyRenderSystem.TYPE_SIZE_INT * 2;
		public static const VERTEX_ELEMENT_TYPE_FLOAT3:int = AlchemyRenderSystem.TYPE_SIZE_INT * 3;
		public static const VERTEX_ELEMENT_TYPE_FLOAT4:int = AlchemyRenderSystem.TYPE_SIZE_INT * 4;
		
		public static const MAXINUM_VERTEX_BUFFER_COUNT:uint = 8;
		
		/****************
		 *Fill Mode 
		 ***************/
		public static const FILL_MODE_POINT    :uint = 0;
		public static const FILL_MODE_WIREFRAME:uint = 1;
		public static const FILL_MODE_SOLID    :uint = 2;
		
		/****************
		 *Shade Mode 
		 ***************/
		public static const SHADE_MODE_FLAT   :uint = 0;
		public static const SHADE_MODE_GOURAUD:uint = 1;
		public static const SHADE_MODE_PHONG  :uint = 2;
		
		/****************
		 *Blend Mode 
		 ***************/
		public static const BLEND_MODE_ZERO              :uint = 0;
		public static const BLEND_MODE_ONE               :uint = 1;
		public static const BLEND_MODE_SRC_COLOR         :uint = 2;
		public static const BLEND_MODE_INV_SRC_COLOR     :uint = 3;
		public static const BLEND_MODE_SRC_ALPHA         :uint = 4;
		public static const BLEND_MODE_INV_SRC_ALPHA     :uint = 5;
		public static const BLEND_MODE_DEST_ALPHA        :uint = 6;
		public static const BLEND_MODE_INV_DEST_ALPHA    :uint = 7;
		public static const BLEND_MODE_DEST_COLOR        :uint = 8;
		public static const BLEND_MODE_INV_DEST_COLOR    :uint = 9;
		public static const BLEND_MODE_SRC_ALPHA_SAT     :uint = 10;
		public static const BLEND_MODE_BOTH_SRC_ALPHA    :uint = 11;
		public static const BLEND_MODE_BOTH_INV_SRC_ALPHA:uint = 12;
		public static const TOTAL_BLEND_MODE             :uint = 13;
		
		/****************
		 *Cull Mode 
		 ***************/
		public static const CULL_MODE_NONE :int = 0;
		public static const CULL_MODE_CCW  :int = 1;
		public static const CULL_MODE_CW   :int = 2;
		public static const TOTAL_CULL_MODE:int = 3;
		
		/****************
		 *STENCILOP Mode 
		 ***************/
		public static const STENCIL_OP_KEEP    :int = 0;
		public static const STENCIL_OP_THROW   :int = 1;
		public static const STENCIL_OP_REPLACE :int = 2;
		public static const STENCIL_OP_INCR_SAT:int = 3;
		public static const STENCIL_OP_DECR_SAT:int = 4;
		public static const STENCIL_OP_INVERT  :int = 5;
		public static const STENCIL_OP_INCR    :int = 6;
		public static const STENCIL_OP_DECR    :int = 7;
		public static const TOTAL_STENCIL_OP   :int = 8;
		
		/****************
		 *Compare Mode 
		 ***************/
		public static const CMP_FUNC_NEVER        :int = 0;
		public static const CMP_FUNC_LESS         :int = 1;
		public static const CMP_FUNC_EQUAL        :int = 2;
		public static const CMP_FUNC_LESS_EQUAL   :int = 3;
		public static const CMP_FUNC_GREATER      :int = 4;
		public static const CMP_FUNC_NOT_EQUAL    :int = 5;
		public static const CMP_FUNC_GREATER_EQUAL:int = 6;
		public static const CMP_FUNC_ALWAYS       :int = 7;
		public static const TOTAL_CMP_FUNC        :int = 8;
		
		/****************
		 *Color Mode 
		 ***************/
		public static const COLOR_WRITE_ENABLE_ALPHA:uint = 0;
		public static const COLOR_WRITE_ENABLE_BLUE :uint = 1;
		public static const COLOR_WRITE_ENABLE_GREEN:uint = 2;
		public static const COLOR_WRITE_ENABLE_RED  :uint = 3;
		public static const TOTAL_COLOR_WRITE_ENABLE:uint = 4;
		
		public static const FILL_MODE:uint = 0;
		
		public static const SHADE_MODE:uint = 1;
		
		public static const DEPTH_WRITE_ENABLE:uint = 2;
		public static const DEPTH_FUNC:uint = 3;
		
		public static const SRC_BLEND:uint = 4;
		public static const DEST_BLEND:uint = 5;
		public static const ALPHA_REF:uint = 6;
		public static const ALPHA_FUNC:uint = 7;
		
		public static const CULL_MODE:uint = 8;
		
		public static const STENCIL_FAIL:uint = 9;
		public static const STENCIL_Z_FAIL:uint = 10;
		public static const STENCIL_PASS:uint = 11;
		public static const STENCIL_FUNC:uint = 12;
		public static const STENCIL_REF:uint = 13;
		public static const STENCIL_READ_MASK:uint = 14;
		public static const STENCIL_WRITE_MASK:uint = 15;
		public static const CCW_STENCIL_FAIL:uint = 16;
		public static const CCW_STENCIL_Z_FAIL:uint = 17;
		public static const CCW_STENCIL_PASS:uint = 18;
		public static const CCW_STENCIL_FUNC:uint = 19;
		
		public static const COLOR_WRITE_ENABLE:uint = 20;
		
		public static const TOTAL_RENDER_STATE:uint = 21;
		
		public static const RENDER_STATE_BLOCK_DIRTY_FLAG_OFFSET:uint = TOTAL_RENDER_STATE * AlchemyRenderSystem.TYPE_SIZE_INT;
		
		public static const RENDER_STATE_BLOCK_SIZE             :uint = RENDER_STATE_BLOCK_DIRTY_FLAG_OFFSET + AlchemyRenderSystem.TYPE_SIZE_INT;
		
		/**
		 * Vertex Data
		 * */
		public static const VERTEX_DATA_VERTEX_BUFFER_HANDLE_OFFSET:uint = 0;
		public static const VERTEX_DATA_OFFSET_IN_BYTES_OFFSET     :uint = VERTEX_DATA_VERTEX_BUFFER_HANDLE_OFFSET + 2;
		public static const VERTEX_DATA_TYPE_OFFSET                :uint = VERTEX_DATA_OFFSET_IN_BYTES_OFFSET + TYPE_SIZE_INT;
		
		public static const VERTEX_DATA_VERTEX_BUFFER_DATA_SIZE    :uint = VERTEX_DATA_TYPE_OFFSET;
		public static const VERTEX_DATA_SIZE                       :uint = VERTEX_DATA_TYPE_OFFSET + TYPE_SIZE_INT;
		
		/**
		 * Program Data
		 * */
		public static const PROGRAM_DATA_PROGRAM_HANDLE_OFFSET                 :uint = 0;
		public static const PROGRAM_DATA_DIRTY_VERTEX_PARAMETER_OFFSET_OFFSET  :uint = PROGRAM_DATA_PROGRAM_HANDLE_OFFSET + 2;
		public static const PROGRAM_DATA_DIRTY_VERTEX_PARAMETER_COUNT_OFFSET   :uint = PROGRAM_DATA_DIRTY_VERTEX_PARAMETER_OFFSET_OFFSET + TYPE_SIZE_INT;
		public static const PROGRAM_DATA_DIRTY_FRAGMENT_PARAMETER_OFFSET_OFFSET:uint = PROGRAM_DATA_DIRTY_VERTEX_PARAMETER_COUNT_OFFSET + TYPE_SIZE_INT;
		public static const PROGRAM_DATA_DIRTY_FRAGMENT_PARAMETER_COUNT_OFFSET :uint = PROGRAM_DATA_DIRTY_FRAGMENT_PARAMETER_OFFSET_OFFSET + TYPE_SIZE_INT;
		public static const PROGRAM_DATA_SIZE                                  :uint = PROGRAM_DATA_DIRTY_FRAGMENT_PARAMETER_COUNT_OFFSET + TYPE_SIZE_INT;
		
		/**
		 * Render Data
		 * */
		public static const RENDER_DATA_RENDER_STATE_BLOCK_DATA_OFFSET:uint = 0;
		public static const RENDER_DATA_RENDER_TARGET_HANDLE_OFFSET   :uint = RENDER_DATA_RENDER_STATE_BLOCK_DATA_OFFSET + RENDER_STATE_BLOCK_SIZE;
		public static const RENDER_DATA_RENDER_TARGET_SURFACE_OFFSET  :uint = RENDER_DATA_RENDER_TARGET_HANDLE_OFFSET + 2;
		public static const RENDER_DATA_VERTEX_DATA_OFFSET            :uint = RENDER_DATA_RENDER_TARGET_SURFACE_OFFSET + TYPE_SIZE_INT;
		public static const RENDER_DATA_INDEX_BUFFER_HANDLE_OFFSET    :uint = RENDER_DATA_VERTEX_DATA_OFFSET + MAXINUM_VERTEX_BUFFER_COUNT * VERTEX_DATA_SIZE;
		public static const RENDER_DATA_PROGRAM_DATA_OFFSET           :uint = RENDER_DATA_INDEX_BUFFER_HANDLE_OFFSET + 2;
		public static const RENDER_DATA_START_OFFSET                  :uint = RENDER_DATA_PROGRAM_DATA_OFFSET + PROGRAM_DATA_SIZE;
		public static const RENDER_DATA_TRIANGLE_COUNT_OFFSET         :uint = RENDER_DATA_START_OFFSET + TYPE_SIZE_INT;
		public static const RENDER_DATA_FLAG_OFFSET                   :uint = RENDER_DATA_TRIANGLE_COUNT_OFFSET + TYPE_SIZE_INT;
		
		public static const RENDER_DATA_SIZE                          :uint = RENDER_DATA_FLAG_OFFSET + TYPE_SIZE_INT;
		
		public static const RENDER_DATA_FLAG_RENDER_TARGET_DIRTY_BIT  :uint = 0;
		public static const RENDER_DATA_FLAG_PROGRAM_DIRTY_BIT        :uint = 1;
		public static const RENDER_DATA_FLAG_INDEX_BUFFER_DIRTY_BIT   :uint = 2;
		
		public static const RENDER_DATA_FLAG_VERTEX_INDEX_DIRTY_SHIFT :uint = 3;
		public static const RENDER_DATA_FLAG_TEXTURE_DIRTY_SHIFT      :uint = RENDER_DATA_FLAG_VERTEX_INDEX_DIRTY_SHIFT  + MAXINUM_VERTEX_BUFFER_COUNT;
		
		public static const MAXINUM_BIT_TABLE:Vector.<uint> = Vector.<uint>([
			0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8]);
		
		public static const MININUM_BIT_TABLE:Vector.<uint> = Vector.<uint>([
			0, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			7, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			8, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			7, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1]);
		
		public function set renderInfo(value:AlchemyRenderInfo):void
		{
			if(_context3D)
				value.driverInfo = _context3D.driverInfo;
			
			_renderInfo = value;
		}
		
		public function enableErrorChecking(value:Boolean):void
		{
			_context3D.enableErrorChecking = value;
		}
		
		public function AlchemyRenderSystem(context3D:Context3D)
		{
			AlchemyLog.assert(context3D != null, "Context3D must not be null.");
			
			_context3D = context3D;
			
			context3D.enableErrorChecking = true;
		}
		
		public function configureBackBuffer(width:uint, height:uint, antiAlias:int):void
		{
			if(width && height)
				_context3D.configureBackBuffer(width, height, antiAlias);
		}
		
		public static function getTextureFormat(format:int):String
		{
			return Context3DTextureFormat.BGRA;
		}
		
		public function createVertexBuffer3D(numVertices:uint, data32PerVertex:uint):void
		{
			var vertexBuffer3D:VertexBuffer3D;
			try
			{
				vertexBuffer3D = _context3D.createVertexBuffer(numVertices, data32PerVertex);
			}
			catch(e:Error)
			{
				AlchemyLog.error(e.message);
			}
			
			if(!_vertexBuffer3DPool)
				_vertexBuffer3DPool = new Vector.<VertexBuffer3D>();
			
			addResourceToPool(vertexBuffer3D, _vertexBuffer3DPool);
		}
		
		public function createIndexBuffer3D(numIndices:uint):void
		{
			var indexBuffer3D:IndexBuffer3D;
			try
			{
				indexBuffer3D = _context3D.createIndexBuffer(numIndices);
			}
			catch(e:Error)
			{
				AlchemyLog.error(e.message);
			}
			
			if(!_indexBuffer3DPool)
				_indexBuffer3DPool = new Vector.<IndexBuffer3D>();
			
			addResourceToPool(indexBuffer3D, _indexBuffer3DPool);
		}
		
		public function createTexture(textureType:int, width:uint, height:uint, format:int, optimizeForRenderToTexture:Boolean, handle:uint):void
		{
			var texture:TextureBase;
			try
			{
				switch(textureType)
				{
					case TEXTURE_TYPE_CUBE:
						texture = _context3D.createCubeTexture(width, getTextureFormat(format), optimizeForRenderToTexture);
						break;
					
					default:
						texture = _context3D.createTexture(width, height, getTextureFormat(format), optimizeForRenderToTexture);
						break;
				}
			}
			catch(e:Error)
			{
				AlchemyLog.error(e.message);
			}
			
			if(!_texturePool)
				_texturePool = new Vector.<TextureBase>();
			
			addResourceToPool(texture, _texturePool, handle);
		}
		
		public function createProgram3D(vertexCode:ByteArray, fragmentCode:ByteArray):void
		{
			var program3D:Program3D;
			try
			{
				program3D = _context3D.createProgram();
				
				program3D.upload(vertexCode, fragmentCode);
			}
			catch(e:Error)
			{
				AlchemyLog.error(e.message);
			}
			
			if(!_program3DPool)
				_program3DPool = new Vector.<Program3D>();
			
			addResourceToPool(program3D, _program3DPool);
		}
		
		public function uploadVertexBuffer3D(handle:uint, data:ByteArray, pointer:uint, startVertex:uint, numVertices:uint):void
		{
			var vertexBuffer3D:VertexBuffer3D = (handle && handle <= _vertexBuffer3DPool.length) ? _vertexBuffer3DPool[handle - 1] : null;
			if(vertexBuffer3D)
			{
				try
				{
					vertexBuffer3D.uploadFromByteArray(data, pointer, startVertex, numVertices);
				}
				catch(e:Error)
				{
					AlchemyLog.error(e.message);
				}
			}
			else
				AlchemyLog.warning("Error vertex buffer handle");
		}
		
		public function uploadIndexBuffer3D(handle:uint, data:ByteArray, pointer:uint, startOffset:uint, count:uint):void
		{
			var indexBuffer3D:IndexBuffer3D = (handle && handle <= _indexBuffer3DPool.length) ? _indexBuffer3DPool[handle - 1] : null;
			if(indexBuffer3D)
			{
				try
				{
					indexBuffer3D.uploadFromByteArray(data, pointer, startOffset, count);
				}
				catch(e:Error)
				{
					AlchemyLog.error(e.message);
				}
			}
			else
				AlchemyLog.warning("Error index buffer handle");
		}
		
		public function uploadTexture(handle:uint, textureType:int, data:ByteArray, pointer:uint, surface:uint):void
		{
			var texture:TextureBase = (handle && handle <= _texturePool.length) ? _texturePool[handle - 1] : null;
			if(texture)
			{
				try
				{
					switch(textureType)
					{
						case TEXTURE_TYPE_CUBE:
							(texture as CubeTexture).uploadFromByteArray(data, pointer, surface >> TEXTURE_MIP_MAP_SURFACE_SHIFT, surface & TEXTURE_MIP_MAP_SURFACE_MASK);
							break;
						
						default:
							(texture as Texture).uploadFromByteArray(data, pointer, surface & TEXTURE_MIP_MAP_SURFACE_MASK);
							break;
					}
				}
				catch(e:Error)
				{
					AlchemyLog.error(e.message);
				}
			}
			else
				AlchemyLog.warning("Error texture handle");
		}
		
		public function clear(color:uint, flag:uint, renderTargetHandle:uint, surface:uint):void
		{
			var renderTarget:TextureBase = (renderTargetHandle && renderTargetHandle <= _texturePool.length) ? _texturePool[renderTargetHandle - 1] : null;
			
			var mask:uint = 0;
			if(renderTarget)
				mask = Context3DClearMask.STENCIL | Context3DClearMask.COLOR | Context3DClearMask.DEPTH;
			else
			{
				if( ( flag & (1 << CLEAR_STENCIL_BIT) ) != 0 )
					mask |= Context3DClearMask.STENCIL;
				
				if( ( flag & (1 << CLEAR_TARGET_BIT) ) != 0 )
					mask |= Context3DClearMask.COLOR;
				
				if( (flag & (1 << CLEAR_ZBUFFER_BIT) ) != 0 )
					mask |= Context3DClearMask.DEPTH;
			}
			
			if(_currentRenderTarget != renderTarget)
			{
				if(renderTarget)
					_context3D.setRenderToTexture(renderTarget, (flag & (1 << CLEAR_ZBUFFER_BIT) ) != 0, 0, surface >> TEXTURE_MIP_MAP_SURFACE_SHIFT);
				else
					_context3D.setRenderToBackBuffer();
				
				_currentRenderTarget = renderTarget;
			}
			
			var inverce255:Number = 1 / 255;
			
			var red:Number = ( (color >> 16) & 0xff ) * inverce255;
			var green:Number = ( (color >> 8) & 0xff ) * inverce255;
			var blue:Number = (color & 0xff) * inverce255;
			var alpha:Number = ( (color >> 24) & 0xff ) * inverce255;
			
			_context3D.clear(red, green, blue, alpha, 1.0, 0, mask);
		}
		
		public function draw(pointer:uint, data:ByteArray, programVertexParameterPointer:uint, programFragmentParameterPointer:uint, programTexturePointer:uint):void
		{
			data.position = pointer + RENDER_DATA_FLAG_OFFSET;
			var flag:uint = data.readUnsignedInt();
			
			if(!renderStateBlockHandler)
				renderStateBlockHandler = new RenderStateBlockHandler();
			
			renderStateBlockHandler.handler(pointer + RENDER_DATA_RENDER_STATE_BLOCK_DATA_OFFSET, data, _context3D);
			
			if( (flag & (1 << RENDER_DATA_FLAG_RENDER_TARGET_DIRTY_BIT) ) != 0 )
			{
				data.position = pointer + RENDER_DATA_RENDER_TARGET_HANDLE_OFFSET;
				var renderTargetHandle:uint = data.readUnsignedShort();
				if(renderTargetHandle)
				{
					var renderTarget:TextureBase = (renderTargetHandle <= _texturePool.length) ? _texturePool[renderTargetHandle - 1] : null;
					if(renderTarget)
					{
						data.position = pointer + RENDER_DATA_RENDER_TARGET_SURFACE_OFFSET;
						_context3D.setRenderToTexture(renderTarget, true, 0, data.readUnsignedInt() >> TEXTURE_MIP_MAP_SURFACE_SHIFT);
					}
					else
						AlchemyLog.warning("Error render target handle.");
				}
				else
					_context3D.setRenderToBackBuffer();
			}

			var vertexFlag:uint = (flag >> RENDER_DATA_FLAG_VERTEX_INDEX_DIRTY_SHIFT) & 0xff;
			if(vertexFlag)
			{
				var vertexEnd  :uint = MAXINUM_BIT_TABLE[vertexFlag];
				var vertexBegin:uint = MININUM_BIT_TABLE[vertexFlag] - 1;
				
				var vertexDataPointer:uint = pointer + RENDER_DATA_VERTEX_DATA_OFFSET + vertexBegin * VERTEX_DATA_SIZE;
				while(vertexBegin != vertexEnd)
				{
					if( ( vertexFlag & (1 << vertexBegin) ) != 0 )
						vertexDataHandler(vertexDataPointer, data, vertexBegin);
					
					vertexDataPointer += VERTEX_DATA_SIZE;
					
					++ vertexBegin;
				}
			}
			
			var programDirtyFlag:uint = 1 << RENDER_DATA_FLAG_PROGRAM_DIRTY_BIT;
			programDataHandler(
				pointer + RENDER_DATA_PROGRAM_DATA_OFFSET, data, 
				(flag & programDirtyFlag) == programDirtyFlag, 
				programVertexParameterPointer, 
				programFragmentParameterPointer, 
				programTexturePointer, 
				flag >> RENDER_DATA_FLAG_TEXTURE_DIRTY_SHIFT);
			
			if(!_currentIndexBuffer ||  flag & (1 << RENDER_DATA_FLAG_INDEX_BUFFER_DIRTY_BIT) )
			{
				data.position = pointer + RENDER_DATA_INDEX_BUFFER_HANDLE_OFFSET;
				var indexBufferHandle:uint = data.readUnsignedShort();
				var indexBuffer:IndexBuffer3D = (indexBufferHandle && indexBufferHandle <= _indexBuffer3DPool.length) ? _indexBuffer3DPool[indexBufferHandle - 1] : null;
				if(indexBuffer)
					_currentIndexBuffer = indexBuffer;
				else
					AlchemyLog.warning("Error indexBuffer handle.");
			}
			
			data.position = pointer + RENDER_DATA_START_OFFSET;
			var start:uint = data.readUnsignedInt();
			
			data.position = pointer + RENDER_DATA_TRIANGLE_COUNT_OFFSET;
			var triangleCount:uint = data.readUnsignedInt();
			
			try
			{
				_context3D.drawTriangles(_currentIndexBuffer, start, triangleCount);
				
				if(_renderInfo)
				{
					++ _renderInfo.drawCount;
					
					_renderInfo.faceCount += triangleCount;
				}
			}
			catch(e:Error)
			{
				AlchemyLog.warning(e.message);
			}
		}

		public function present():void
		{
			_context3D.present();
		}
		
		public function removeResource(handle:uint, type:uint):void
		{
			if(handle)
			{
				switch(type)
				{
					case RESOURCE_TYPE_VERTEX_BUFFER:
						removeResourceFromPool(handle, _vertexBuffer3DPool);
						break;
					case RESOURCE_TYPE_INDEX_BUFFER:
						removeResourceFromPool(handle, _indexBuffer3DPool);
						break;
					case RESOURCE_TYPE_SHADER:
						removeResourceFromPool(handle, _program3DPool);
						break;
					case RESOURCE_TYPE_TEXTURE:
						removeResourceFromPool(handle, _texturePool);
						break;
				}
			}
		}

		private function vertexDataHandler(pointer:uint, data:ByteArray, index:uint):void
		{
			data.position = pointer + VERTEX_DATA_VERTEX_BUFFER_HANDLE_OFFSET;
			var handle:uint = data.readUnsignedShort();
			if(handle)
			{
				var vertexBuffer3D:VertexBuffer3D = handle <= _vertexBuffer3DPool.length ? _vertexBuffer3DPool[handle - 1] : null;
				if(vertexBuffer3D)
				{
					data.position = pointer + VERTEX_DATA_OFFSET_IN_BYTES_OFFSET;
					var offsetInBytes:uint = data.readUnsignedInt();
					
					var format:String;
					data.position = pointer + VERTEX_DATA_TYPE_OFFSET;
					var type:int = data.readInt();
					switch(type)
					{
						case VERTEX_ELEMENT_TYPE_FLOAT1:
							format = Context3DVertexBufferFormat.FLOAT_1;
							break;
						case VERTEX_ELEMENT_TYPE_FLOAT2:
							format = Context3DVertexBufferFormat.FLOAT_2;
							break;
						case VERTEX_ELEMENT_TYPE_FLOAT3:
							format = Context3DVertexBufferFormat.FLOAT_3;
							break;
						case VERTEX_ELEMENT_TYPE_FLOAT4:
							format = Context3DVertexBufferFormat.FLOAT_4;
							break;
					}
					
					_context3D.setVertexBufferAt(index, vertexBuffer3D, offsetInBytes >> 2, format);
				}
				else
					AlchemyLog.warning("Error vertex buffer handle.");
			}
			else
				_context3D.setVertexBufferAt(index, null);
		}
		
		private function programDataHandler(pointer:uint, data:ByteArray, programDirty:Boolean, programVertexParameterPointer:uint, programFragmentParameterPointer:uint, programTexturePointer:uint, textureDirtyFlag:uint):void
		{
			var handle:uint;
			if(programDirty)
			{
				data.position = pointer + PROGRAM_DATA_PROGRAM_HANDLE_OFFSET;
				handle = data.readUnsignedShort();
				var program3D:Program3D = (handle && handle <= _program3DPool.length) ? _program3DPool[handle - 1] : null;
				if(program3D)
					_context3D.setProgram(program3D);
				else
				{
					AlchemyLog.warning("Error program handle.");
					
					return;
				}
			}

			var i:uint;

			data.position = pointer + PROGRAM_DATA_DIRTY_VERTEX_PARAMETER_OFFSET_OFFSET;
			var dirtyVertexParameterOffset:uint = data.readUnsignedInt();
			
			data.position = pointer + PROGRAM_DATA_DIRTY_VERTEX_PARAMETER_COUNT_OFFSET;
			var dirtyVertexParameterCount:uint = data.readUnsignedInt();
			
			if(dirtyVertexParameterCount > dirtyVertexParameterOffset)
				_context3D.setProgramConstantsFromByteArray(Context3DProgramType.VERTEX, dirtyVertexParameterOffset >> 2, (dirtyVertexParameterCount - dirtyVertexParameterOffset + 3) >> 2, data, programVertexParameterPointer + dirtyVertexParameterOffset * TYPE_SIZE_FLOAT);
			
			data.position = pointer + PROGRAM_DATA_DIRTY_FRAGMENT_PARAMETER_OFFSET_OFFSET;
			var dirtyFragmentParameterOffset:uint = data.readUnsignedInt();
			
			data.position = pointer + PROGRAM_DATA_DIRTY_FRAGMENT_PARAMETER_COUNT_OFFSET;
			var dirtyFragmentParameterCount:uint = data.readUnsignedInt();
			
			if(dirtyFragmentParameterCount > dirtyFragmentParameterOffset)
				_context3D.setProgramConstantsFromByteArray(Context3DProgramType.FRAGMENT, dirtyFragmentParameterOffset >> 2, (dirtyFragmentParameterCount - dirtyFragmentParameterOffset + 3) >> 2, data, programFragmentParameterPointer + dirtyFragmentParameterOffset * TYPE_SIZE_FLOAT);

			if(textureDirtyFlag)
			{
				var texture:TextureBase;
				
				var end  :uint = MAXINUM_BIT_TABLE[textureDirtyFlag];
				var begin:uint = MININUM_BIT_TABLE[textureDirtyFlag] - 1;
				
				var programTexturePointer:uint = programTexturePointer + (begin << 1);
				for(i = begin; i < end; ++ i)
				{
					if( ( textureDirtyFlag & (1 << i) ) != 0 )
					{
						data.position = programTexturePointer;
						
						handle = data.readUnsignedShort();
						if(handle)
						{
							texture = handle <= _texturePool.length ? _texturePool[handle - 1] : null;
							if(texture)
								_context3D.setTextureAt(i, texture);
							else
								AlchemyLog.warning("Error texture handle.");
						}
						else
							_context3D.setTextureAt(i, null);
					}
					
					programTexturePointer += 2;
				}
			}
		}
		
		private function addResourceToPool(resource:*, pool:*, handle:uint = 0):void
		{
			var index:int;
			if(handle && handle <= pool.length)
			{
				index = handle - 1;
				var currentResource:* = pool[index];
				if(currentResource)
					currentResource.dispose();
				
				pool[index] = resource;
				
				return;
			}
			
			index = pool.indexOf(null);
			if(index == - 1)
				pool.push(resource);
			else
				pool[index] = resource;
		}
		
		private function removeResourceFromPool(handle:uint, pool:*):void
		{
			if(handle && handle <= pool.length)
			{
				var index:uint = handle - 1;
				var resource:* = pool[index];
				if(resource)
					resource.dispose();
				
				pool[index] = null;
			}
			else
				AlchemyLog.warning("Error resource handle");
		}
		
		private var _renderInfo:AlchemyRenderInfo;
		
		private var _context3D:Context3D;
		
		private var _vertexBuffer3DPool:Vector.<VertexBuffer3D>;
		private var _indexBuffer3DPool:Vector.<IndexBuffer3D>;
		private var _texturePool:Vector.<TextureBase>;
		private var _program3DPool:Vector.<Program3D>;
		
		private var renderStateBlockHandler:RenderStateBlockHandler;
		
		private var _currentRenderTarget:TextureBase;
		private var _currentIndexBuffer:IndexBuffer3D;
	}
}
import alchemy.core.AlchemyRenderSystem;

import flash.display3D.Context3D;
import flash.display3D.Context3DBlendFactor;
import flash.display3D.Context3DCompareMode;
import flash.display3D.Context3DStencilAction;
import flash.display3D.Context3DTriangleFace;
import flash.utils.ByteArray;

class RenderStateBlockHandler
{
	public function RenderStateBlockHandler()
	{
		initialization();
	}
	
	public function initialization():void
	{
		_cullMode  = new Vector.<String>(AlchemyRenderSystem.TOTAL_CULL_MODE , true);
		_CMPFunc   = new Vector.<String>(AlchemyRenderSystem.TOTAL_CMP_FUNC  , true);
		_blendMode = new Vector.<String>(AlchemyRenderSystem.TOTAL_BLEND_MODE, true);
		_stencilOP = new Vector.<String>(AlchemyRenderSystem.TOTAL_STENCIL_OP, true);
		
		_cullMode[AlchemyRenderSystem.CULL_MODE_NONE] = Context3DTriangleFace.NONE;
		_cullMode[AlchemyRenderSystem.CULL_MODE_CCW ] = Context3DTriangleFace.BACK;
		_cullMode[AlchemyRenderSystem.CULL_MODE_CW  ] = Context3DTriangleFace.FRONT;
		
		_CMPFunc[AlchemyRenderSystem.CMP_FUNC_NEVER        ] = Context3DCompareMode.NEVER;
		_CMPFunc[AlchemyRenderSystem.CMP_FUNC_LESS         ] = Context3DCompareMode.LESS;
		_CMPFunc[AlchemyRenderSystem.CMP_FUNC_EQUAL        ] = Context3DCompareMode.EQUAL;
		_CMPFunc[AlchemyRenderSystem.CMP_FUNC_LESS_EQUAL   ] = Context3DCompareMode.LESS_EQUAL;
		_CMPFunc[AlchemyRenderSystem.CMP_FUNC_GREATER      ] = Context3DCompareMode.GREATER;
		_CMPFunc[AlchemyRenderSystem.CMP_FUNC_NOT_EQUAL    ] = Context3DCompareMode.NOT_EQUAL;
		_CMPFunc[AlchemyRenderSystem.CMP_FUNC_GREATER_EQUAL] = Context3DCompareMode.GREATER_EQUAL;
		_CMPFunc[AlchemyRenderSystem.CMP_FUNC_ALWAYS       ] = Context3DCompareMode.ALWAYS;
		
		_blendMode[AlchemyRenderSystem.BLEND_MODE_ZERO              ] = Context3DBlendFactor.ZERO;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_ONE               ] = Context3DBlendFactor.ONE;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_SRC_COLOR         ] = Context3DBlendFactor.SOURCE_COLOR;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_INV_SRC_COLOR     ] = Context3DBlendFactor.ONE_MINUS_SOURCE_COLOR;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_SRC_ALPHA         ] = Context3DBlendFactor.SOURCE_ALPHA;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_INV_SRC_ALPHA     ] = Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_DEST_ALPHA        ] = Context3DBlendFactor.DESTINATION_ALPHA;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_INV_DEST_ALPHA    ] = Context3DBlendFactor.ONE_MINUS_DESTINATION_ALPHA;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_DEST_COLOR        ] = Context3DBlendFactor.DESTINATION_COLOR;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_INV_DEST_COLOR    ] = Context3DBlendFactor.ONE_MINUS_DESTINATION_COLOR;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_SRC_ALPHA_SAT     ] = null;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_BOTH_SRC_ALPHA    ] = null;
		_blendMode[AlchemyRenderSystem.BLEND_MODE_BOTH_INV_SRC_ALPHA] = null;
		
		_stencilOP[AlchemyRenderSystem.STENCIL_OP_KEEP    ] = Context3DStencilAction.KEEP;
		_stencilOP[AlchemyRenderSystem.STENCIL_OP_THROW   ] = Context3DStencilAction.ZERO;
		_stencilOP[AlchemyRenderSystem.STENCIL_OP_REPLACE ] = Context3DStencilAction.SET;
		_stencilOP[AlchemyRenderSystem.STENCIL_OP_INCR_SAT] = Context3DStencilAction.INCREMENT_SATURATE;
		_stencilOP[AlchemyRenderSystem.STENCIL_OP_DECR_SAT] = Context3DStencilAction.DECREMENT_SATURATE;
		_stencilOP[AlchemyRenderSystem.STENCIL_OP_INVERT  ] = Context3DStencilAction.INVERT;
		_stencilOP[AlchemyRenderSystem.STENCIL_OP_INCR    ] = Context3DStencilAction.INCREMENT_WRAP;
		_stencilOP[AlchemyRenderSystem.STENCIL_OP_DECR    ] = Context3DStencilAction.DECREMENT_WRAP;
		
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
	}
	
	public function handler(pointer:uint, data:ByteArray, context3D:Context3D):void
	{
		data.position = pointer + AlchemyRenderSystem.RENDER_STATE_BLOCK_DIRTY_FLAG_OFFSET;
		var dirtyFlag:uint = data.readUnsignedInt();
		
		if(dirtyFlag)
		{
			var depthWriteDirty:Boolean = ( dirtyFlag & (1 << AlchemyRenderSystem.DEPTH_WRITE_ENABLE) ) != 0;
			var depthFunctionDirty:Boolean = ( dirtyFlag & (1 << AlchemyRenderSystem.DEPTH_FUNC) ) != 0;
			
			if(depthWriteDirty || depthFunctionDirty)
			{
				if(depthWriteDirty)
				{
					data.position = pointer + AlchemyRenderSystem.DEPTH_WRITE_ENABLE * AlchemyRenderSystem.TYPE_SIZE_INT;
					_depthWriteEnable = data.readUnsignedInt() ? true : false;
				}
				
				if(depthFunctionDirty)
				{
					data.position = pointer + AlchemyRenderSystem.DEPTH_FUNC * AlchemyRenderSystem.TYPE_SIZE_INT;
					_depthFunction = _CMPFunc[data.readUnsignedInt()];
				}
				
				context3D.setDepthTest(_depthWriteEnable, _depthFunction);
			}
			
			var blendSourceDirty:Boolean = ( dirtyFlag & (1 << AlchemyRenderSystem.SRC_BLEND) ) != 0;
			var blendDestinationDirty:Boolean = ( dirtyFlag & (1 << AlchemyRenderSystem.DEST_BLEND) ) != 0;
			if(blendSourceDirty || blendDestinationDirty)
			{
				if(blendSourceDirty)
				{
					data.position = pointer + AlchemyRenderSystem.SRC_BLEND * AlchemyRenderSystem.TYPE_SIZE_INT;
					_blendSource = _blendMode[data.readUnsignedInt()];
				}
				
				if(blendDestinationDirty)
				{
					data.position = pointer + AlchemyRenderSystem.DEST_BLEND * AlchemyRenderSystem.TYPE_SIZE_INT;
					_blendDestination = _blendMode[data.readUnsignedInt()];
				}
				
				context3D.setBlendFactors(_blendSource, _blendDestination);
			}
			
			if( ( dirtyFlag & (1 << AlchemyRenderSystem.CULL_MODE) ) != 0 )
			{
				data.position = pointer + AlchemyRenderSystem.CULL_MODE * AlchemyRenderSystem.TYPE_SIZE_INT;
				_currentCullMode = _cullMode[data.readUnsignedInt()];
				
				context3D.setCulling(_currentCullMode);
			}
			
			var stencilFunctionDirty:Boolean;
			var stencilFailDirty    :Boolean;
			var stencilZFailDirty   :Boolean;
			var stencilPassDirty    :Boolean;
			
			stencilFunctionDirty = ( dirtyFlag & (1 << AlchemyRenderSystem.STENCIL_FUNC  ) ) != 0;
			stencilFailDirty     = ( dirtyFlag & (1 << AlchemyRenderSystem.STENCIL_FAIL  ) ) != 0;
			stencilZFailDirty    = ( dirtyFlag & (1 << AlchemyRenderSystem.STENCIL_Z_FAIL) ) != 0;
			stencilPassDirty     = ( dirtyFlag & (1 << AlchemyRenderSystem.STENCIL_PASS  ) ) != 0;
			if(stencilFunctionDirty || stencilFailDirty || stencilZFailDirty || stencilPassDirty)
			{
				if(stencilFunctionDirty)
				{
					data.position = pointer + AlchemyRenderSystem.STENCIL_FUNC * AlchemyRenderSystem.TYPE_SIZE_INT;
					_stencilFunction = _CMPFunc[data.readUnsignedInt()];
				}
				
				if(stencilFailDirty)
				{
					data.position = pointer + AlchemyRenderSystem.STENCIL_FAIL * AlchemyRenderSystem.TYPE_SIZE_INT;
					_stencilFail = _stencilOP[data.readUnsignedInt()];
				}
				
				if(stencilZFailDirty)
				{
					data.position = pointer + AlchemyRenderSystem.STENCIL_Z_FAIL * AlchemyRenderSystem.TYPE_SIZE_INT;
					_stencilZFail = _stencilOP[data.readUnsignedInt()];
				}
				
				if(stencilPassDirty)
				{
					data.position = pointer + AlchemyRenderSystem.STENCIL_PASS * AlchemyRenderSystem.TYPE_SIZE_INT;
					_stencilPass = _stencilOP[data.readUnsignedInt()];
				}

				context3D.setStencilActions(Context3DTriangleFace.FRONT, _stencilFunction, _stencilPass, _stencilZFail, _stencilFail); 
			}
			
			stencilFunctionDirty = ( dirtyFlag & (1 << AlchemyRenderSystem.CCW_STENCIL_FUNC  ) ) != 0;
			stencilFailDirty     = ( dirtyFlag & (1 << AlchemyRenderSystem.CCW_STENCIL_FAIL  ) ) != 0;
			stencilZFailDirty    = ( dirtyFlag & (1 << AlchemyRenderSystem.CCW_STENCIL_Z_FAIL) ) != 0;
			stencilPassDirty     = ( dirtyFlag & (1 << AlchemyRenderSystem.CCW_STENCIL_PASS  ) ) != 0;
			if(stencilFunctionDirty || stencilFailDirty || stencilZFailDirty || stencilPassDirty)
			{
				if(stencilFunctionDirty)
				{
					data.position = pointer + AlchemyRenderSystem.CCW_STENCIL_FUNC * AlchemyRenderSystem.TYPE_SIZE_INT;
					_CCWStencilFunction = _CMPFunc[data.readUnsignedInt()];
				}
				
				if(stencilFailDirty)
				{
					data.position = pointer + AlchemyRenderSystem.CCW_STENCIL_FAIL * AlchemyRenderSystem.TYPE_SIZE_INT;
					_CCWStencilFail = _stencilOP[data.readUnsignedInt()];
				}
				
				if(stencilZFailDirty)
				{
					data.position = pointer + AlchemyRenderSystem.CCW_STENCIL_Z_FAIL * AlchemyRenderSystem.TYPE_SIZE_INT;
					_CCWStencilZFail = _stencilOP[data.readUnsignedInt()];
				}
				
				if(stencilPassDirty)
				{
					data.position = pointer + AlchemyRenderSystem.CCW_STENCIL_PASS * AlchemyRenderSystem.TYPE_SIZE_INT;
					_CCWStencilPass = _stencilOP[data.readUnsignedInt()];
				}
				
				context3D.setStencilActions(Context3DTriangleFace.BACK, _CCWStencilFunction, _CCWStencilPass, _CCWStencilZFail, _CCWStencilFail); 
			}
			
			var stencilRefDirty:Boolean  = ( dirtyFlag & (1 << AlchemyRenderSystem.STENCIL_REF) ) != 0;
			var stencilReadMask:Boolean  = ( dirtyFlag & (1 << AlchemyRenderSystem.STENCIL_READ_MASK) ) != 0;
			var stencilWriteMask:Boolean = ( dirtyFlag & (1 << AlchemyRenderSystem.STENCIL_WRITE_MASK) ) != 0;
			
			if(stencilRefDirty || stencilReadMask || stencilWriteMask)
			{
				if(stencilRefDirty)
				{
					data.position = pointer + AlchemyRenderSystem.STENCIL_REF * AlchemyRenderSystem.TYPE_SIZE_INT;
					_stencilRef = data.readUnsignedInt();
				}
				
				if(stencilReadMask)
				{
					data.position = pointer + AlchemyRenderSystem.STENCIL_READ_MASK * AlchemyRenderSystem.TYPE_SIZE_INT;
					_stencilReadMask = data.readUnsignedInt();
				}
				
				if(stencilWriteMask)
				{
					data.position = pointer + AlchemyRenderSystem.STENCIL_WRITE_MASK * AlchemyRenderSystem.TYPE_SIZE_INT;
					_stencilWriteMask = data.readUnsignedInt();
				}
				
				context3D.setStencilReferenceValue(_stencilRef, _stencilReadMask, _stencilWriteMask);
			}
			
			if( ( dirtyFlag & (1 << AlchemyRenderSystem.COLOR_WRITE_ENABLE) ) != 0 )
			{
				data.position = pointer + AlchemyRenderSystem.COLOR_WRITE_ENABLE * AlchemyRenderSystem.TYPE_SIZE_INT;
				_colorWriteEnble = data.readUnsignedInt();
				
				context3D.setColorMask(
					( _colorWriteEnble & (1 << AlchemyRenderSystem.COLOR_WRITE_ENABLE_RED) ) != 0,
					( _colorWriteEnble & (1 << AlchemyRenderSystem.COLOR_WRITE_ENABLE_GREEN) ) != 0,
					( _colorWriteEnble & (1 << AlchemyRenderSystem.COLOR_WRITE_ENABLE_BLUE) ) != 0,
					( _colorWriteEnble & (1 << AlchemyRenderSystem.COLOR_WRITE_ENABLE_ALPHA) ) != 0);
			}
		}
	}
	
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
	
	private var _stencilRef:uint;
	private var _stencilReadMask:uint;
	private var _stencilWriteMask:uint;
	
	private var _colorWriteEnble:uint;
}