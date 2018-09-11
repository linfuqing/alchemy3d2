package alchemy.manager 
{
	import alchemy.animation.CSkinMeshController;
	import alchemy.constant.CBuffer;
	import alchemy.core.CLibrary;
	import alchemy.core.CObject;
	import alchemy.core.CPointer;
	import alchemy.custom.CRenderStateHandler;
	import alchemy.data.CMaterialData;
	import alchemy.data.CMeshData;
	import alchemy.data.CMeshSegmentData;
	import alchemy.data.CTextureData;
	import alchemy.data.CVertexElementData;
	import alchemy.data.rendermethod.CParameterInfoData;
	import alchemy.data.rendermethod.CParameterListData;
	import alchemy.data.rendermethod.CVertexElementParameterData;
	import alchemy.effect.CAbstractEffectInfo;
	import alchemy.effect.CEffectCode;
	import alchemy.effect.blendmethod.CAbstractBlendMethod;
	import alchemy.flush3D;
	import alchemy.resource.basic.CEffect;
	import alchemy.resource.basic.CTexture;
	import alchemy.resource.extend.CHighLevelResource;
	import alchemy.resource.extend.CLight;
	import alchemy.resource.extend.CMaterial;
	import alchemy.resource.extend.CMesh;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.scene.C45Camera;
	import alchemy.scene.CCamera;
	import alchemy.scene.CSceneNode;
	import alchemy.scene.CSceneObject;
	import alchemy.scene.ISceneContainer;
	import alchemy.tools.CDebug;
	
	import flash.geom.ColorTransform;
	import flash.utils.Dictionary;
	
	use namespace flush3D;

	public class CSceneManager extends CAbstractResourcePool implements ISceneContainer
	{
		private const RESOURCE_TYPE_MESH         :int = 11;
		private const RESOURCE_TYPE_MATERIAL     :int = 12;
		private const RESOURCE_TYPE_LIGHT        :int = 13;
		private const RESOURCE_TYPE_RENDER_METHOD:int = 14;
		
		static public const SHADE_PHONG:uint 			= 1;
		static public const SHADE_BLINN_PHONG:uint 	= 2;
		
		static private var _instance:CSceneManager;
		
		static public function getInstance():CSceneManager
		{
			return _instance;
		}
		
		public function get canCreateHighLevelResource():Boolean
		{
			return _canCreateHighLevelResource;
		}
		
		public function get camera():CCamera
		{
			return _camera;
		}
		
		public function get renderSystem():CRenderSystem
		{
			return _renderSystem;
		}
		
		public function get ambientLight():ColorTransform
		{
			CLibrary.memory.position  = _ambientLightPointer;
			
			_color.redMultiplier   = CLibrary.memory.readFloat();
			_color.greenMultiplier = CLibrary.memory.readFloat();
			_color.blueMultiplier  = CLibrary.memory.readFloat();
			_color.alphaMultiplier = CLibrary.memory.readFloat();
			
			return _color;
		}
		
		public function set ambientLight(val:ColorTransform):void
		{
			CLibrary.memory.position = _ambientLightPointer;
			CLibrary.memory.writeFloat(val.redMultiplier);
			CLibrary.memory.writeFloat(val.greenMultiplier);
			CLibrary.memory.writeFloat(val.blueMultiplier);
			CLibrary.memory.writeFloat(val.alphaMultiplier);
		}
		
		public function set backgroundColor(value:uint):void
		{
			CLibrary.memory.position = _backgroundColorPointer;
			CLibrary.memory.writeUnsignedInt(value);
		}
		
		public function get backgroundColor():uint
		{
			CLibrary.memory.position = _backgroundColorPointer;
			return CLibrary.memory.readUnsignedInt();
		}
		
		public function CSceneManager(renderSystem:CRenderSystem)
		{
			CDebug.assert(CGraphicsManager.canCreateSceneManager, "");
			CDebug.assert(renderSystem != null                  , "Need renderSystem.");
			CDebug.assert(CSceneNodeFactory.instance != null    , "Need Scene node factory.");
			
			_renderSystem      = renderSystem;
			
			var pointers:Array = CLibrary.method.CreateSceneManager(_renderSystem.pointer);
			
			_pointer = pointers[0];
			
			super(pointers[1]);
			
			init();
			
			_instance = this;
		}
		
		public function createMesh( meshDataHandle:uint ):CMesh
		{
			var meshHandle:uint = CLibrary.method.CreateMesh(_pointer, meshDataHandle);
			if(meshHandle)
				return _meshPool[meshHandle-1];
			
			return null;
		}
		
		public function createMaterial( materialDataHandle:uint = 0 ):CMaterial
		{
			var materialHandle:uint = CLibrary.method.CreateMaterial(_pointer, materialDataHandle);
			if(materialHandle)
				return _materialPool[materialHandle-1];
			
			return null;
		}
		
		public function createScene( sceneDataHandle:uint, skinType:uint = 1, materialInfoMap:Dictionary = null, textureInfoMap:Dictionary = null, meshInfoMap:Dictionary = null ):CSceneNode
		{
			var flag:uint = materialInfoMap ? 1 : 0;
			flag = textureInfoMap ? (flag |= 2) : flag;
			flag = meshInfoMap ? (flag |= 4) : flag;
			
			var values:Array = CLibrary.method.CreateScene(_pointer, sceneDataHandle, skinType, flag, null, null, null);
			
			if(!values)
				return null;
			
			var materialPtr:uint = materialInfoMap ? CLibrary.method.CreateBuffer( (CLibrary.INT_SIZE * 2) * values[1] ) : 0;
			var texturePtr:uint = textureInfoMap ? CLibrary.method.CreateBuffer( (CLibrary.INT_SIZE * 2) * values[2] ) : 0;
			var meshPtr:uint = meshInfoMap ? CLibrary.method.CreateBuffer( (CLibrary.INT_SIZE * 2) * values[3] ) : 0;
			
			CLibrary.method.CreateScene(null, sceneDataHandle, skinType, flag, materialPtr, texturePtr, meshPtr);
			
			var i:uint,uLen:uint, key:int;
			
			if(materialPtr && materialInfoMap)
			{
				uLen = values[1];
				CLibrary.memory.position = materialPtr;
				for(i=0; i<uLen; ++i)
				{
					key = CLibrary.memory.readUnsignedInt();
					materialInfoMap[ key ] = CLibrary.memory.readUnsignedShort();
					CLibrary.memory.position += 2;
				}
				
				CLibrary.method.DestroyBuffer(materialPtr);
			}
			
			if(texturePtr && textureInfoMap)
			{
				uLen = values[1];
				CLibrary.memory.position = texturePtr;
				for(i=0; i<uLen; ++i)
				{
					key = CLibrary.memory.readUnsignedInt();
					textureInfoMap[ key ] = CLibrary.memory.readUnsignedShort();
					CLibrary.memory.position += 2;
				}
				
				CLibrary.method.DestroyBuffer(texturePtr);
			}
			
			if(meshPtr && meshInfoMap)
			{
				uLen = values[1];
				CLibrary.memory.position = materialPtr;
				for(i=0; i<uLen; ++i)
				{
					key = CLibrary.memory.readUnsignedInt();
					meshInfoMap[ key ] = CLibrary.memory.readUnsignedShort();
					CLibrary.memory.position += 2;
				}
				
				CLibrary.method.DestroyBuffer(meshPtr);
			}
			
			return values[0];
		}
		
//		public function createMesh(data:CMeshData):CMesh
//		{
//			if(!data)
//			{
//				CDebug.warning("No data");
//				
//				return null;
//			}
//			
//			var streamCount:uint = data.streamCount;
//			
//			if(streamCount > data.vertexData.length)
//			{
//				CDebug.warning("Error mesh data");
//				
//				return null;
//			}
//			
//			var meshDataPointer:uint = CLibrary.method.CreateMeshData(0);
//			var vertexElementPointer:uint;
//			var vertexUsagePointer:uint;
//			var attributeRangePointer:uint;
//			var vertexDataPointer:uint;
//			var indexDataPointer:uint;
//			
//			var vertexElementLength:uint;
//			var attributeRangeLength:uint;
//			
//			CLibrary.memory.position = meshDataPointer;
//			CLibrary.memory.writeUnsignedInt(data.numVertices);
//			CLibrary.memory.writeUnsignedInt(data.numIndices);
//			CLibrary.memory.writeUnsignedInt(data.indexUsage);
//			CLibrary.memory.writeInt(CMeshData.INDEX_FORMAT_16BIT);
//			
//			vertexElementPointer = CLibrary.memory.position;
//			CLibrary.memory.writeUnsignedInt(0);
//			CLibrary.memory.writeUnsignedInt(vertexElementLength = data.vertexDeclaration.length);
//			
//			vertexUsagePointer = CLibrary.memory.position;
//			CLibrary.memory.writeUnsignedInt(0);
//			
//			attributeRangePointer = CLibrary.memory.position;
//			CLibrary.memory.writeUnsignedInt(0);
//			CLibrary.memory.writeUnsignedInt(attributeRangeLength = data.attributeRange.length);
//			
//			vertexDataPointer = CLibrary.memory.position;
//			CLibrary.memory.writeUnsignedInt(0);
//			
//			indexDataPointer = CLibrary.memory.position;
//			CLibrary.memory.writeUnsignedInt(0);
//			
//			CLibrary.method.CreateMeshData(meshDataPointer);
//			
//			var i:uint;
//			
//			var vertexElement:CVertexElementData;
//			CLibrary.memory.position = vertexElementPointer;
//			CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
//			for(i = 0; i < vertexElementLength; i ++)
//			{
//				vertexElement = data.vertexDeclaration[i];
//				CLibrary.memory.writeUnsignedInt(vertexElement.stream);
//				CLibrary.memory.writeUnsignedInt(vertexElement.offset);
//				CLibrary.memory.writeInt(vertexElement.type);
//				CLibrary.memory.writeInt(vertexElement.usage);
//				CLibrary.memory.writeUnsignedInt(i);
//				//CLibrary.memory.writeUnsignedInt(vertexElement.vertexUsage);
//			}
//			
//			CLibrary.memory.position = vertexUsagePointer;
//			CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
//			for(i = 0; i < streamCount; i ++)
//				CLibrary.memory.writeUnsignedInt(data.vertexData[i].usage);
//			
//			var attributeRange:CMeshSegmentData;
//			CLibrary.memory.position = attributeRangePointer;
//			CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
//			for(i = 0; i < attributeRangeLength; ++ i)
//			{
//				attributeRange = data.attributeRange[i];
//				CLibrary.memory.writeUnsignedInt(attributeRange.startIndex);
//				CLibrary.memory.writeUnsignedInt(attributeRange.indexCount);
//				//CLibrary.memory.writeInt(CMeshSegmentData.TRIANGLE_TYPE_LIST);
//			}
//			
//			CLibrary.memory.position = indexDataPointer;
//			CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
//			CLibrary.memory.writeBytes( data.indexData, 0, data.numIndices * (1 << CMeshData.INDEX_FORMAT_16BIT) );
//			
//			CLibrary.method.CreateMeshData(meshDataPointer);
//			
//			var vertexDataOffset:uint = 0;
//			for(i = 0; i < streamCount; ++ i)
//			{
//				CLibrary.memory.position = vertexDataPointer;
//				CLibrary.memory.position = CLibrary.memory.readUnsignedInt() + vertexDataOffset;
//				CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
//				CLibrary.memory.writeBytes(data.vertexData[i].data);
//				
//				vertexDataOffset += CLibrary.POINTER_SIZE;
//			}
//			
//			var handle:uint = CLibrary.method.CreateMesh(_pointer, meshDataPointer);
//			
//			//_canCreateHighLevelResource = true;
//			//var mesh:CMesh = new CMesh(pointers[0], _meshPool.length + 1, pointers[1], this);
//			//_canCreateHighLevelResource = false;
//			
//			//_meshPool.push(mesh);
//			//add(mesh, _meshPool);
//			
//			CLibrary.method.DestroyMeshData(meshDataPointer);
//			
//			var mesh:CMesh = null;
//			if(handle)
//				mesh = _meshPool[handle - 1];
//			
//			return mesh;
//		}
		
//		public function createMaterial(data:CMaterialData = null):CMaterial
//		{
//			//var material:CMaterial;
//			var handle:uint;
//			//var pointers:Array;
//			if(data)
//			{
//				var materialDataPointer:uint = CLibrary.method.CreateMaterialData(0);
//				var textureDataPointer:uint;
//				
//				var textureDataLength:uint;
//				
//				CLibrary.memory.position = materialDataPointer;
//				CLibrary.memory.writeUnsignedInt(data.ambient);
//				CLibrary.memory.writeUnsignedInt(data.diffuse);
//				CLibrary.memory.writeUnsignedInt(data.specular);
//				CLibrary.memory.writeUnsignedInt(data.emissive);
//				CLibrary.memory.writeFloat(data.power);
//				
//				textureDataPointer = CLibrary.memory.position;
//				CLibrary.memory.writeUnsignedInt(0);
//				
//				CLibrary.memory.writeUnsignedInt(textureDataLength = data.textureData.length);
//				
//				CLibrary.method.CreateMaterialData(materialDataPointer);
//				CLibrary.memory.position = textureDataPointer;
//				var textureDataOffsetPointer:uint = CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
//				var textureData:CTextureData;
//				var i:uint;
//				for(i = 0; i < textureDataLength; i ++)
//				{
//					textureData = data.textureData[i];
//					CLibrary.memory.writeUnsignedInt(0);
//					CLibrary.memory.writeUnsignedInt(textureData.width);
//					CLibrary.memory.writeUnsignedInt(textureData.height);
//					CLibrary.memory.writeUnsignedInt(CTexture.FORMAT_ARGB);
//					CLibrary.memory.writeUnsignedInt(textureData.usage);
//					CLibrary.memory.writeUnsignedInt(textureData.mipLevel);
//				}
//				
//				CLibrary.method.CreateMaterialData(materialDataPointer);
//				for(i = 0; i < textureDataLength; i ++)
//				{
//					textureData = data.textureData[i];
//					
//					if(data.textureData[i].data)
//					{
//						CLibrary.memory.position = textureDataOffsetPointer;
//						CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
//						CLibrary.memory.writeBytes(data.textureData[i].data);
//					}
//					
//					textureDataOffsetPointer += CTextureData.SIZE;
//				}
//			
//				handle = CLibrary.method.CreateMaterial(_pointer, materialDataPointer);
//			}
//			else
//				handle = CLibrary.method.CreateMaterial(_pointer, 0); 
//			
//			//_canCreateHighLevelResource = true;
//			//material = new CMaterial(pointers[0], _materialPool.length + 1, pointers[1], this);
//			//_canCreateHighLevelResource = false;
//			
//			//_materialPool.push(material);
//			//add(material, _materialPool);
//			
//			CLibrary.method.DestroyMaterialData(materialDataPointer);
//			var material:CMaterial = null;
//			
//			if(handle)
//				material = _materialPool[handle - 1];
//			
//			return material;
//		}
		
		public function createLight():CLight
		{
			var handle:uint = CLibrary.method.CreateLight(_pointer);
			
			var light:CLight = null;
			if(handle)
				light = _lightPool[handle - 1];
			
			return light;
		}
		
		public function createRenderMethod(data:Vector.<CParameterListData>, effect:uint, renderSetHandler:CRenderStateHandler = null):CRenderMethod
		{
			if(!data)
			{
				CDebug.warning("No data");
				
				return null;
			}
			
			var passLength           :uint = data.length;
			var parametersPointer    :uint = CLibrary.method.CreateParameterList(0, passLength);
			var parameterListPointer :uint = parametersPointer;
			
			var parameterFlag        :uint;
			var arrayParameterFlag   :uint;
			//var matrixParameterFlag  :uint;
			//var cameraParameterFlag  :uint;
			var materialParameterFlag:uint;
			var constantParameterFlag:uint;
			
			var offsetPointer:uint;
			
			var parameterList:CParameterListData;
			var parameter:CParameterInfoData;
			var i:uint;

			for(var k:uint = 0; k < passLength; ++ k)
			{
				parameterFlag         = 0;
				arrayParameterFlag    = 0;
				//matrixParameterFlag   = 0;
				//cameraParameterFlag   = 0;
				materialParameterFlag = 0;
				constantParameterFlag = 0;
				
				parameterList = data[k];
				offsetPointer = parameterListPointer;
				for(i = 0; i < CParameterListData.TOTAL_PARAMETER_LABEL; ++ i)
				{
					parameter = parameterList.parameters[i];
					if(parameter)
					{
						CLibrary.memory.position = offsetPointer;
						CLibrary.memory.writeInt(parameter.type);
						//CLibrary.memory.writeUnsignedInt(parameter.pass);
						CLibrary.memory.writeUnsignedInt(parameter.alphaIndex);
						CLibrary.memory.writeUnsignedInt(parameter.betaIndex);
						
						parameterFlag |= 1 << i;
					}
					
					offsetPointer += CParameterInfoData.SIZE;
				}
				
				for(i = 0; i < CParameterListData.TOTAL_ARRAY_LABEL; ++ i)
				{
					parameter = parameterList.arrayParameters[i];
					if(parameter)
					{
						CLibrary.memory.position = offsetPointer;
						CLibrary.memory.writeInt(parameter.type);
						//CLibrary.memory.writeUnsignedInt(parameter.pass);
						CLibrary.memory.writeUnsignedInt(parameter.alphaIndex);
						CLibrary.memory.writeUnsignedInt(parameter.betaIndex);
						
						arrayParameterFlag |= 1 << i;
					}
					
					offsetPointer += CParameterInfoData.SIZE;
				}
				
				/*for(i = 0; i < CParameterListData.TOTAL_MATRIX_LABEL; ++ i)
				{
					parameter = parameterList.matrixParameters[i];
					if(parameter)
					{
						CLibrary.memory.position = offsetPointer;
						CLibrary.memory.writeInt(parameter.type);
						//CLibrary.memory.writeUnsignedInt(parameter.pass);
						CLibrary.memory.writeUnsignedInt(parameter.alphaIndex);
						CLibrary.memory.writeUnsignedInt(parameter.betaIndex);
						
						matrixParameterFlag |= 1 << i;
					}
					
					offsetPointer += CParameterInfoData.SIZE;
				}
				
				for(i = 0; i < CParameterListData.TOTAL_CAMERA_LABEL; ++ i)
				{
					parameter = parameterList.cameraParameters[i];
					if(parameter)
					{
						CLibrary.memory.position = offsetPointer;
						CLibrary.memory.writeInt(parameter.type);
						//CLibrary.memory.writeUnsignedInt(parameter.pass);
						CLibrary.memory.writeUnsignedInt(parameter.alphaIndex);
						CLibrary.memory.writeUnsignedInt(parameter.betaIndex);
						
						cameraParameterFlag |= 1 << 1;
					}
					
					offsetPointer += CParameterInfoData.SIZE;
				}*/
				
				for(i = 0; i < CParameterListData.TOTAL_MATERIAL_LABEL; ++ i)
				{
					parameter = parameterList.materialParameters[i];
					if(parameter)
					{
						CLibrary.memory.position = offsetPointer;
						CLibrary.memory.writeInt(parameter.type);
						//CLibrary.memory.writeUnsignedInt(parameter.pass);
						CLibrary.memory.writeUnsignedInt(parameter.alphaIndex);
						CLibrary.memory.writeUnsignedInt(parameter.betaIndex);
						
						materialParameterFlag |= 1 << i;
					}
					
					offsetPointer += CParameterInfoData.SIZE;
				}
				
				for(i = 0; i < CParameterListData.TOTAL_CONSTANT_LABEL; ++ i)
				{
					parameter = parameterList.constantParameters[i];
					if(parameter)
					{
						CLibrary.memory.position = offsetPointer;
						CLibrary.memory.writeInt(parameter.type);
						//CLibrary.memory.writeUnsignedInt(parameter.pass);
						CLibrary.memory.writeUnsignedInt(parameter.alphaIndex);
						CLibrary.memory.writeUnsignedInt(parameter.betaIndex);
						
						constantParameterFlag |= 1 << i;
					}
					
					offsetPointer += CParameterInfoData.SIZE;
				}
				
				var camerasLength :uint = parameterList.cameraParameters.length;
				var lightsLength :uint = parameterList.lightParameters.length;
				var texturesLength:uint = parameterList.textureParameters.length;
				
				var textureLength:uint = parameterList.textures.length;

				//var vertexElementLength:uint = parameterList.vertexElementParameters.length;
				
				CLibrary.memory.position = offsetPointer;
				CLibrary.memory.writeUnsignedInt(0);
				CLibrary.memory.writeUnsignedInt(0);
				CLibrary.memory.writeUnsignedInt(0);
				CLibrary.memory.writeUnsignedInt(0);
				
				CLibrary.memory.writeUnsignedInt(parameterFlag);
				CLibrary.memory.writeUnsignedInt(arrayParameterFlag);
				//CLibrary.memory.writeUnsignedInt(matrixParameterFlag);
				//CLibrary.memory.writeUnsignedInt(cameraParameterFlag);
				CLibrary.memory.writeUnsignedInt(materialParameterFlag);
				CLibrary.memory.writeUnsignedInt(constantParameterFlag);
				
				var parametersList:Vector.<Vector.<CParameterInfoData>>;
				var parameters:Vector.<CParameterInfoData>;
				
				var j:uint;
				
				var camerasFlagOffsetPointer:uint = CLibrary.memory.position;
				CLibrary.memory.writeUnsignedInt(0);
				var lightsFlagOffsetPointer:uint = CLibrary.memory.position;
				CLibrary.memory.writeUnsignedInt(0);
				var texturesFlagOffsetPointer:uint = CLibrary.memory.position;
				CLibrary.memory.writeUnsignedInt(0);
				
				CLibrary.memory.writeUnsignedInt(camerasLength);
				CLibrary.memory.writeUnsignedInt(lightsLength);
				CLibrary.memory.writeUnsignedInt(textureLength);
				
				/*var vertexElementOffsetPointer:uint = CLibrary.memory.position;
				CLibrary.memory.writeUnsignedInt(0);
				CLibrary.memory.writeUnsignedInt(vertexElementLength);*/
				
				parameterListPointer = CLibrary.method.CreateParameterList(parameterListPointer, passLength);
				
				CLibrary.memory.position = offsetPointer;
				var camerasOffsetPointer:uint = CLibrary.memory.readUnsignedInt();
				CLibrary.memory.position = camerasFlagOffsetPointer;
				var cameraFlagOffsetPointer:uint = CLibrary.memory.readUnsignedInt();
				
				parametersList = parameterList.cameraParameters;
				
				var cameraOffsetPointer:uint;
				var cameraFlag:uint;
				for(j = 0; j < camerasLength; ++ j)
				{
					cameraFlag = 0;
					CLibrary.memory.position = camerasOffsetPointer;
					cameraOffsetPointer = CLibrary.memory.readUnsignedInt();
					parameters = parametersList[j];
					for(i = 0; i < CParameterListData.TOTAL_LIGHT_LABEL; ++ i)
					{
						parameter = parameters[i];
						if(parameter)
						{
							CLibrary.memory.position = lightOffsetPointer;
							CLibrary.memory.writeInt(parameter.type);
							//CLibrary.memory.writeUnsignedInt(parameter.pass);
							CLibrary.memory.writeUnsignedInt(parameter.alphaIndex);
							CLibrary.memory.writeUnsignedInt(parameter.betaIndex);
							
							cameraFlag |= 1 << i;
						}
						
						cameraOffsetPointer += CParameterInfoData.SIZE;
					}
					
					CLibrary.memory.position = cameraFlagOffsetPointer;
					CLibrary.memory.writeUnsignedInt(cameraFlag);
					
					camerasOffsetPointer += CLibrary.INT_SIZE;
					
					cameraFlagOffsetPointer += CLibrary.INT_SIZE;
				}
				
				CLibrary.memory.position = offsetPointer + CLibrary.POINTER_SIZE;
				var lightsOffsetPointer:uint = CLibrary.memory.readUnsignedInt();
				CLibrary.memory.position = lightsFlagOffsetPointer;
				var lightFlagOffsetPointer:uint = CLibrary.memory.readUnsignedInt();
				
				parametersList = parameterList.lightParameters;
				
				var lightOffsetPointer:uint;
				var lightFlag:uint;
				for(j = 0; j < lightsLength; ++ j)
				{
					lightFlag = 0;
					CLibrary.memory.position = lightsOffsetPointer;
					lightOffsetPointer = CLibrary.memory.readUnsignedInt();
					parameters = parametersList[j];
					for(i = 0; i < CParameterListData.TOTAL_LIGHT_LABEL; ++ i)
					{
						parameter = parameters[i];
						if(parameter)
						{
							CLibrary.memory.position = lightOffsetPointer;
							CLibrary.memory.writeInt(parameter.type);
							//CLibrary.memory.writeUnsignedInt(parameter.pass);
							CLibrary.memory.writeUnsignedInt(parameter.alphaIndex);
							CLibrary.memory.writeUnsignedInt(parameter.betaIndex);
							
							lightFlag |= 1 << i;
						}
						
						lightOffsetPointer += CParameterInfoData.SIZE;
					}
					
					CLibrary.memory.position = lightFlagOffsetPointer;
					CLibrary.memory.writeUnsignedInt(lightFlag);
					
					lightsOffsetPointer += CLibrary.INT_SIZE;
					
					lightFlagOffsetPointer += CLibrary.INT_SIZE;
				}
				
				CLibrary.memory.position = offsetPointer + CLibrary.POINTER_SIZE + CLibrary.POINTER_SIZE;
				var texturesOffsetPointer:uint = CLibrary.memory.readUnsignedInt();
				CLibrary.memory.position = texturesFlagOffsetPointer;
				var textureFlagOffsetPointer:uint = CLibrary.memory.readUnsignedInt();
				
				parametersList = parameterList.textureParameters;
				
				var textureOffsetPointer:uint;
				var textureFlag:uint;
				for(j = 0; j < texturesLength; ++ j)
				{
					textureFlag = 0;
					CLibrary.memory.position = texturesOffsetPointer;
					lightOffsetPointer = CLibrary.memory.readUnsignedInt();
					parameters = parametersList[j];
					if(parameters)
					{
						for(i = 0; i < CParameterListData.TOTAL_TEXTURE_LABEL; ++ i)
						{
							parameter = parameters[i];
							if(parameter)
							{
								CLibrary.memory.position = textureFlagOffsetPointer;
								CLibrary.memory.writeInt(parameter.type);
								//CLibrary.memory.writeUnsignedInt(parameter.pass);
								CLibrary.memory.writeUnsignedInt(parameter.alphaIndex);
								CLibrary.memory.writeUnsignedInt(parameter.betaIndex);
								
								textureFlag |= 1 << i;
							}
							
							textureOffsetPointer += CParameterInfoData.SIZE;
						}
					}
					
					CLibrary.memory.position = textureFlagOffsetPointer;
					CLibrary.memory.writeUnsignedInt(textureFlag);
					
					texturesOffsetPointer += CLibrary.INT_SIZE;
					
					textureFlagOffsetPointer += CLibrary.INT_SIZE;
				}
				
				CLibrary.memory.position = offsetPointer + CLibrary.INT_SIZE * 2;
				CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
				parameters = parameterList.textures;
				for(i = 0; i < textureLength; i ++)
				{
					parameter = parameters[i];
					
					CLibrary.memory.writeInt(parameter.type);
					//CLibrary.memory.writeUnsignedInt(parameter.pass);
					CLibrary.memory.writeUnsignedInt(parameter.alphaIndex);
					CLibrary.memory.writeUnsignedInt(parameter.betaIndex);
				}
				
				/*CLibrary.memory.position = vertexElementOffsetPointer;
				CLibrary.memory.position = CLibrary.memory.readUnsignedInt();
				
				var vertexElementParameter:CVertexElementParameterData;
				for(i = 0; i < vertexElementLength; i ++)
				{
					vertexElementParameter = parameterList.vertexElementParameters[i];
					
					CLibrary.memory.writeInt(vertexElementParameter.type);
					CLibrary.memory.writeInt(vertexElementParameter.usage);
					CLibrary.memory.writeUnsignedInt(vertexElementParameter.index);
				}*/
			}
			
			var handle:uint = CLibrary.method.CreateRenderMethod(_pointer, parametersPointer, passLength, effect, renderSetHandler ? renderSetHandler.pointer : 0);
			
			//_canCreateHighLevelResource = true;
			//var renderMethod:CRenderMethod = new CRenderMethod(pointers[0], _renderMethodPool.length + 1, pointers[1], this);
			//_canCreateHighLevelResource = false;
			
			CLibrary.method.DestroyParameterList(parametersPointer, passLength);
			
			//_renderMethodPool.push(renderMethod);
			//add(renderMethod, _renderMethodPool);
			
			var renderMethod:CRenderMethod = null;
			
			if(handle)
				renderMethod = _renderMethodPool[handle - 1];
			
			return renderMethod;
		}
		
		public function createRenderMethodFromEffectCode(effectCode:CEffectCode, renderSetHandler:CRenderStateHandler = null):CRenderMethod
		{
			if(!effectCode)
			{
				CDebug.warning("effect code can not be null");
				
				return null;
			}
			
			var handle:uint = CLibrary.method.CreateRenderMethodFromEffectCode(_pointer, effectCode.pointer, renderSetHandler ? renderSetHandler.pointer : 0);
			
			var renderMethod:CRenderMethod = null;
			
			if(handle)
				renderMethod = _renderMethodPool[handle - 1];
			
			return renderMethod;
		}
		
//		public function createSceneObject(meshHandle:uint, subset:uint, materialHandle:uint, renderMethodHandle:uint, source:CSceneObject = null):CSceneObject
//		{
//			var retVal:CSceneObject = CLibrary.method.CreateSceneObject(_pointer, meshHandle, subset, materialHandle, renderMethodHandle, source ? source.pointer : 0);
//			
//			retVal._subset = subset;
//			retVal._meshHandle = meshHandle;
//			retVal._materialHandle = materialHandle;
//			//retVal._renderMethodHandle = renderMethodHandle;
//			//retVal._sceneManager = this;
//			
//			return retVal;
//		}
		
		public function AddEffectInfoToPass(effectCode:CEffectCode, effectInfoBlendMethod:CAbstractBlendMethod, specularEffect:CAbstractEffectInfo = null, normalMapEffect:CAbstractEffectInfo = null):void
		{
			effectCode.addSceneEffectInfo(this, effectInfoBlendMethod, specularEffect, normalMapEffect);
		}
		
		public function getMesh(handle:uint):CMesh
		{
			return _meshPool[handle - 1];
		}
		
		public function getMaterial(handle:uint):CMaterial
		{
			return _materialPool[handle - 1];
		}
		
		public function getRenderMethod(handle:uint):CRenderMethod
		{
			return _renderMethodPool[handle - 1];
		}
		
		public function getLight(handle:uint):CLight
		{
			return _lightPool[handle - 1];
		}
		
		public function addLight(lightHandle:uint, shadeMode:int):void
		{
			CLibrary.method.AddLightToSceneManager(_pointer, lightHandle, shadeMode);
		}
		
		public function addCamera(camera:CCamera):int
		{
			return CLibrary.method.AddCameraToSceneManager(_pointer, camera.pointer);
		}
		
		public function addChild(child:CSceneNode):void
		{
			if(child == null)
			{
				CDebug.warning("child is null.");
				
				return;
			}
			
			child.addToScene(this);
		}
		
		public function removeChild(child:CSceneNode):void
		{
			if(child == null)
			{
				CDebug.warning("child is null.");
				
				return;
			}
			
			child.removeFromScene(this);
		}
		
		public function render(elapse:Number):void
		{
			var amibentLight:ColorTransform = this.ambientLight;
			_renderSystem.context.clear(amibentLight.redMultiplier, amibentLight.redMultiplier, amibentLight.greenMultiplier, amibentLight.blueMultiplier);
			CLibrary.method.RenderScene(_pointer, elapse);
		}
		
		override public function destroy():void
		{
			
		}
		
		private function init():void
		{
			//_canCreateHighLevelResource = true;
			
			/**
			 * 需改动
			 */
			//_renderSystem = new CRenderSystem(...);
			
			//_canCreateHighLevelResource = false;
			
			//_sceneNodefactoryPointer = pointers[2];
			
			var pointers:Array = getPointers();
			
			_camera = pointers[0];
			
			_ambientLightPointer = pointers[1];
			_backgroundColorPointer = pointers[2];
			
			fill(create, removeResource);
			
			////////////////////////////////
			
			_meshPool         = new Vector.<CMesh>();
			_materialPool     = new Vector.<CMaterial>();
			_renderMethodPool = new Vector.<CRenderMethod>();
			_lightPool        = new Vector.<CLight>();
			
			_color            = new ColorTransform();
		}
		
		private function create(pointer:uint, interfacePointer:uint, type:int, handle:uint):CHighLevelResource
		{
			var resource:CHighLevelResource = null;
			
			_canCreateHighLevelResource = true;
			switch(type)
			{
				case RESOURCE_TYPE_MESH:
					//unlock( resource = new CMesh(pointer, lock(_meshPool), interfacePointer, this) );
					if(handle == _meshPool.length + 1)
						_meshPool.push( resource = new CMesh(pointer, handle, interfacePointer, this) );
					else if(handle && handle <= _meshPool.length)
					{
						resource = _meshPool[handle - 1];
						if(resource && resource.pointer == pointer)
							resource.rebuild();
						else
							resource = _meshPool[handle - 1] = new CMesh(pointer, handle, interfacePointer, this);
					}
					else
						CDebug.error("Mesh create handle error.");
					break;
				case RESOURCE_TYPE_MATERIAL:
					//unlock( resource = new CMaterial(pointer, lock(_materialPool), interfacePointer, this) );
					if(handle == _materialPool.length + 1)
						_materialPool.push( resource = new CMaterial(pointer, handle, interfacePointer, this) );
					else if(handle && handle <= _materialPool.length)
					{
						resource = _materialPool[handle - 1];
						if(resource && resource.pointer == pointer)
							resource.rebuild();
						else
							resource = _materialPool[handle - 1] = new CMaterial(pointer, handle, interfacePointer, this);
					}
					else
						CDebug.error("material create handle error.");
					break;
				case RESOURCE_TYPE_LIGHT:
					//unlock( resource = new CLight(pointer, lock(_lightPool), interfacePointer, this) );
					if(handle == _lightPool.length + 1)
						_lightPool.push( resource = new CLight(pointer, handle, interfacePointer, this) );
					else if(handle && handle <= _lightPool.length)
					{
						resource = _lightPool[handle - 1];
						if(resource && resource.pointer == pointer)
							resource.rebuild();
						else
							resource = _lightPool[handle - 1] = new CLight(pointer, handle, interfacePointer, this);
					}
					else
						CDebug.error("Light create handle error.");
					break;
				case RESOURCE_TYPE_RENDER_METHOD:
					//unlock( resource = new CRenderMethod(pointer, lock(_renderMethodPool), interfacePointer, this) );
					if(handle == _renderMethodPool.length + 1)
						_renderMethodPool.push( resource = new CRenderMethod(pointer, handle, interfacePointer, this) );
					else if(handle && handle <= _renderMethodPool.length)
					{
						resource = _renderMethodPool[handle - 1];
						if(resource && resource.pointer == pointer)
							resource.rebuild();
						else
							resource = _renderMethodPool[handle - 1] = new CRenderMethod(pointer, handle, interfacePointer, this);
					}
					else
						CDebug.error("Render method create handle error.");
					break;
				default:
					CDebug.warning("Error type.");
			}
			_canCreateHighLevelResource = false;
			
			return resource;
		}
		
		private function removeResource(handle:int, type:int):void
		{
			if(handle)
			{
				switch(type)
				{
					case RESOURCE_TYPE_MESH:
						_meshPool[handle - 1] = null;
						break;
					case RESOURCE_TYPE_MATERIAL:
						_materialPool[handle - 1] = null;
						break;
					case RESOURCE_TYPE_LIGHT:
						_lightPool[handle - 1] = null;
						break;
					case RESOURCE_TYPE_RENDER_METHOD:
						_renderMethodPool[handle - 1] = null;
						break;
				}
			}
		}
		
		private var _renderSystem:CRenderSystem;
		
		private var _camera:CCamera;
		private var _scene:CSceneNode;
		
		private var _meshPool:Vector.<CMesh>;
		private var _materialPool:Vector.<CMaterial>;
		private var _renderMethodPool:Vector.<CRenderMethod>;
		private var _lightPool:Vector.<CLight>;

		private var _canCreateHighLevelResource:Boolean;
		
		private var _color:ColorTransform;
		
		private var _ambientLightPointer:uint;
		private var _backgroundColorPointer:uint;
	}
}