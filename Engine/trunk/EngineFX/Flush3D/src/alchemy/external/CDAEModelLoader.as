package alchemy.external
{
	import alchemy.animation.CAnimationController;
	import alchemy.animation.CAnimationSet;
	import alchemy.animation.CSkinInfo;
	import alchemy.constant.CBuffer;
	import alchemy.core.CLibrary;
	import alchemy.data.CMaterialData;
	import alchemy.data.CMeshData;
	import alchemy.data.CMeshSegmentData;
	import alchemy.data.CSceneData;
	import alchemy.data.CSceneNodeData;
	import alchemy.data.CSkinData;
	import alchemy.data.CTextureData;
	import alchemy.data.CVertexBufferData;
	import alchemy.data.CVertexElementData;
	import alchemy.data.animation.CAnimationSetData;
	import alchemy.data.animation.CAnimationTrackData;
	import alchemy.data.animation.CBoneData;
	import alchemy.data.animation.CSkinMeshData;
	import alchemy.event.CLoaderEvent;
	import alchemy.event.CTextureLoaderEvent;
	import alchemy.external.parser.ISceneDataParser;
	import alchemy.manager.CMeshManager;
	import alchemy.resource.extend.CMesh;
	import alchemy.scene.CSceneNode;
	import alchemy.tools.CDebug;
	import alchemy.tools.CMath;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	import flash.utils.Endian;
	
	public class CDAEModelLoader extends CAbstractModelLoader
	{
		static private const MAX_VERTEX_NUM:uint  = 0xffff;
		static private const MAX_INDICES_NUM:uint = 524287;
		
		public function CDAEModelLoader(textureLoader:CAbstractTexturesLoader = null,addedElementDatas:Vector.<CVertexElementData> = null, isCreate:Boolean = true)
		{
			super(textureLoader,addedElementDatas);
			
			_isCreate = isCreate;
		}
		
		public function get animationController():CAnimationController
		{
			return _animController;
		}
		
		public function get animSetData():CAnimationSetData
		{
			return _animSetData;
		}
		
		public function get materialLib():Dictionary
		{
			return _materialLib;
		}
		
		public function get textureUrls():Vector.<String>
		{
			return _textureUrls;
		}
		
		public function get textureUrlHandleMap():Dictionary
		{
			return _textureHandleURLMap;
		}
		
		public function get fileName():String
		{
			return _fileName;
		}
		
		override protected function _load(url:String):void
		{
			var st:int = url.lastIndexOf("/");
			
			if(!_textureLoader)
				_textureLoader = new CSinglePathTextureLoader( _basicUrl = url.substring(0, st) );
			
			var end:int = url.lastIndexOf(".");
			
			_fileName = url.substring( st+1, end);
			
			_textureLoader.addEventListener(CTextureLoaderEvent.TEXTURE_COMPLETE_ONE, _onTextureComplete);
			_textureLoader.addEventListener(CTextureLoaderEvent.TEXTURE_LOADERROR, _onTextureError);
			_textureLoader.addEventListener(CTextureLoaderEvent.TEXTURE_ALL_LOADCOMPLETE, _onAllTextureComplete);
		}
		
		override protected function _parse(data:*):Boolean
		{
			try
			{
				_collada = new XML(data);
			}
			catch(e:*)
			{
				CDebug.log("Junk byte!?");
				var pos:int = String(data).indexOf("</COLLADA>"); 
				_collada = new XML( String(data).substring(0,  pos+String("</COLLADA>").length ) );
			}
			
			default xml namespace = _collada.namespace();
			
			_yUp = (_collada["asset"].up_axis == "Y_UP")||(String(_collada["asset"].up_axis) == "");
			
			CDebug.log( _yUp ? "current axis : Y_UP" : " current axis : Z_UP " );
			
			_isSkinMesh = String( _collada["library_controllers"].controller ) != "";
			if(!_isSkinMesh)
				_isSkinMesh = String( _collada["library_animations"].animation ) != "";
			
			_geometryList = new Vector.<CSceneNodeData>;
			_geometryNames = new Vector.<String>;
			_geometryXMLList = new Vector.<XML>;
			_geometryControllerXMLList = new Vector.<XML>;
			
			_materialLib = new Dictionary;
			_skinNodes = new Vector.<CSceneNodeData>;
			_textureMap = new Dictionary;
			
			_parseTextures();
			
			return true;
		}
		
		private function _parseTextures():void
		{
			_textureUrls = new Vector.<String>;
			
			if( String(_collada["library_images"]).length == 0)
			{
				_parseScene();
				_sceneDataHandle = CMeshManager.instance.addSceneData( _sceneData );
				
				if(this.hasEventListener(CLoaderEvent.ALL_LOAD_OK))
					this.dispatchEvent(new CLoaderEvent(CLoaderEvent.ALL_LOAD_OK));
				return;
			}
			var images:XMLList = _collada["library_images"][0].image;
			var url:String;
			var vecID:Vector.<String>;
			for each( var imageXML:XML in images )
			{
				url = imageXML.init_from[0].toString();
				var index:int = url.lastIndexOf("\\");
				if(index != -1)
					url =  url.substr(index+1);
				
				vecID = _textureMap[ url ];
				if(vecID == null)
				{
					vecID = new Vector.<String>;
					_textureMap[ url ] = vecID;
				}
				
				vecID.push( String(imageXML.@id) );
				
				_textureMap[ url ] = vecID;
				_textureLoader.addTextureName( url );
				
				_textureUrls.push( _basicUrl + url );
			}
			
			_textureLoader.start();
		}
		
		protected function _parseScene():Boolean
		{
			var sceneXML:XMLList = _collada["library_visual_scenes"].visual_scene["node"];
			if(sceneXML == null)
			{
				CDebug.log("------------- No scene to parse -------------");
				return false;
			}
			
			CDebug.log("------------- Begin Parse Scene -------------");
			
			_sceneData = new CSceneData;
			
			_rootNode = _sceneData.createNodeData( CSceneNodeData.NODE_TYPE );
			CDebug.assert(_rootNode != null,"CDAEModelLoader::_parseScene _rootNode create fail!");
			if(!_rootNode)
				return false;
			
			for each (var childNode:XML in sceneXML)
			_parseNode(childNode,_rootNode);
			
			for(var i:uint = 0; i < _geometryList.length; i ++)
				_parseGemotry(
					_geometryNames[i],
					_geometryXMLList[i],
					_geometryControllerXMLList[i],
					_geometryList[i]);
			
			_parseAnimationSet();
			
			CDebug.log("------------- End Parse Scene -------------");
			
			return true;
		}
		
		protected function _parseNode( node:XML, parent:CSceneNodeData ):void
		{
			var curNode:CSceneNodeData;
			
			if( String(node["instance_controller"]) != "" || String(node["instance_geometry"]) != "" || String(node["instance_geometry"].@url) != "")
				curNode = _sceneData.createNodeData( CSceneNodeData.OBJECT_TYPE, parent );
			else
				curNode = _isSkinMesh ? _sceneData.createNodeData( CSceneNodeData.BONE_TYPE, parent ) : _sceneData.createNodeData( CSceneNodeData.NODE_TYPE, parent );
			
			if(!_rootBone && curNode.type == CSceneNodeData.BONE_TYPE)
				_rootBone = curNode;
			
			curNode.name = String(node.@name);
			curNode.id   = node.@id;
			
			CDebug.log("Parse Node : " + curNode.id + " : " + curNode.name);
			
			var transform:Matrix3D;
			var geo:XML;
			var ctrlr:XML;
			
			for each (var childNode:XML in node.children())
			{
				switch( String(childNode.localName()) )
				{
					case "matrix":
						transform = _getNodeMatrix(_getNumAry(childNode),curNode,parent);
						break;
					
					case "node":
						_parseNode(childNode, curNode);
						break;
					
					case "instance_geometry":
						geo = _collada["library_geometries"].geometry.(@id == _getId(childNode.@url))[0];
						_geometryList.push( curNode );
						_geometryNames.push(geo.@id);
						_geometryXMLList.push(geo);
						_geometryControllerXMLList.push(null);
						break;
					case "instance_controller":
						ctrlr = _collada["library_controllers"].controller.(@id == _getId(childNode.@url))[0];
						geo = _collada["library_geometries"].geometry.(@id == _getId(ctrlr.skin.@source))[0];
						_geometryList.push( curNode );
						_geometryNames.push(geo.@id);
						_geometryXMLList.push(geo);
						_geometryControllerXMLList.push(_collada["library_controllers"].controller.(@id == _getId(childNode.@url))[0]);
						break;
				}
			}
			
			if(transform != null)
				curNode.transform.append(transform);
		}
		
		protected function _parseGemotry(name:String, geometry:XML, controller:XML, hostObject:CSceneNodeData):void
		{
			//return _parseGemotrySingleStream(name, geometry, controller, hostObject);
			var polygonsList:XMLList = geometry["mesh"].polygons;
			var len:uint = polygonsList.length();
			var i:uint;
			
			if(len > 0)
			{
				var triangles:XML = polygonsList[0];
				var inputXMLList:XMLList = triangles["input"];
				var vertexXML:XML;
				var texCoordXML:XML;
				
				var semantic:String;
				var uvNum:uint = 0;
				var triangleNum:uint;
				var isSplitMesh:Boolean = false;
				
				for each(var inputXML:XML in inputXMLList)
				{
					semantic = String( inputXML.@semantic );
					switch(semantic)
					{
						case "VERTEX":
							vertexXML = inputXML;
							break;
						case "TEXCOORD":
							texCoordXML = inputXML;
							++uvNum;
							break;
					}
				}
				
				if(uvNum > 1)
					return _parseGemotrySingleStream(name, geometry, controller, hostObject);
				
				if(uvNum > 0)
				{
					len = uint( geometry["mesh"].source.(@id == _getId(texCoordXML.@source))[0].float_array.@count )/2;
					if(len > MAX_VERTEX_NUM)
						return _parseGemotrySingleStream(name, geometry, controller, hostObject);
				}
				else
				{
					vertexXML = geometry["mesh"].vertices.(@id == _getId(vertexXML.@source))[0];
					len = uint( geometry["mesh"].source.(@id == _getId(vertexXML.input.@source))[0].float_array.@count)/3;
					if(len > MAX_VERTEX_NUM)
						return _parseGemotrySingleStream(name, geometry, controller, hostObject);
				}
				
				triangleNum = 0;
				len = polygonsList.length();
				for(i=0; i<len; ++i)
				{
					triangles = polygonsList[i];
					inputXMLList = triangles["input"];
					triangleNum += ( uint( String(triangles.@count) ) * 3);
					
					if(triangleNum > MAX_INDICES_NUM)
					{
						isSplitMesh = true;
						break;
					}
				}
				
				if(isSplitMesh)
					return _parseGemotrySingleStream(name, geometry, controller, hostObject);
				
				_parseGemotryOptimize(name, geometry, controller, hostObject);
			}
		}
		
		protected function _parseSkinGemotry(controller:XML, externalIndices:Dictionary, hostObject:CSceneNodeData, otherObjects:Vector.<CSceneNodeData> = null, isReplace:Boolean = false):void
		{
			if(controller)
			{
				var skin:XML = controller["skin"][0];
				var jointId:String = _getId(skin["joints"].input.(@semantic == "JOINT")[0].@source);
				var tmp:String = skin["source"].(@id == jointId)["Name_array"].toString();
				
				tmp = tmp.replace(/\n/g, " ");
				var nameArray:Array = tmp.split(" ");
				var bind_shape:Matrix3D = _getAxisMatrix( _getNumAry( skin["bind_shape_matrix"][0].toString() ) );
				
				var bindMatrixId:String = _getId(skin["joints"].input.(@semantic == "INV_BIND_MATRIX").@source);
				var float_array:Vector.<Number> = _getNumAry( skin["source"].(@id == bindMatrixId)[0].float_array.toString() );
				
				var i:uint;
				var id:String;
				var skinInfoData:CSkinData = new CSkinData();
				var boneDataes:Vector.<CBoneData> = skinInfoData.boneData;
				var boneData:CBoneData;
				var bone:CSceneNodeData;
				var jointNames:Vector.<String> = new Vector.<String>;
				var boneDataMap:Dictionary = new Dictionary;
				var length:uint = float_array.length;
				
				for(i = 0; i < length; i += 16)
				{
					if( _rootBone.name == ( id = nameArray[i / 16] ) ||  ( (bone = _sceneData.findChildByName( id )) && bone.type == CSceneNodeData.BONE_TYPE) )
					{
						jointNames.push( id );
						boneData = new CBoneData;
						boneData.name = id;
						boneDataes.push(boneData);
						
						boneDataMap[id] = boneData;
					}
					else
						CDebug.warning("error joint not found" + id);
					
					boneData.offsetMatrix.append( bind_shape );
					boneData.offsetMatrix.append( _getBoneInvalidMatrix(float_array.slice(i, i+16)) );
				}
				
				tmp = skin["vertex_weights"][0].@count;
				var weightsId:String = _getId(skin["vertex_weights"].input.(@semantic == "WEIGHT")[0].@source);
				
				tmp = skin["source"].(@id == weightsId)["float_array"].toString();
				var weights:Vector.<Number> = _getNumAry(tmp);
				
				tmp = skin["vertex_weights"].vcount.toString();
				var vCount:Vector.<uint> = _getUintAry(tmp);
				
				tmp = skin["vertex_weights"].v.toString();
				var v:Vector.<uint> = _getUintAry(tmp);
				
				
				var indices:Array;
				var vCountLen:int = vCount.length;
				var c:int = 0, j:uint, triIndex:uint = 0;
				var boneName:String;
				var weightVal:Number;
				var weightIndex:uint;
				
				for(i=0; i<vCountLen; ++i)
				{
					c = vCount[i];
					j=0;
					
					indices = externalIndices[i];
					while (j < c)
					{
						boneName =  jointNames[ int(  v[triIndex] ) ];
						++triIndex;
						
						weightVal = Number( weights[ int( v[triIndex] ) ] );
						++triIndex;
						
						boneData = boneDataMap[boneName];
						
						if(weightVal > 0)
						{
							if(!isReplace)
							{
								boneData.indices.push(i);
								boneData.weights.push( weightVal );
							}
							
							for each(weightIndex in indices)
							{
								boneData.indices.push(weightIndex);
								boneData.weights.push( weightVal );
							}
						}
						
						++j;
					}
				}
				
				
				hostObject.rootBone = _sceneData.sceneNodesDatas.indexOf(_rootBone)+1;
				hostObject.skinInfoHandle = CMeshManager.instance.addSkinData(skinInfoData);
				hostObject.isUpdate = true;
				
				if(otherObjects)
				{
					for(i=0; i<otherObjects.length; ++i)
					{
						otherObjects[i].rootBone = hostObject.rootBone;
						otherObjects[i].skinInfoHandle = hostObject.skinInfoHandle;
						otherObjects[i].isUpdate = false;
					}
				}
			}
		}
		
		protected function _parseGemotryOptimize(name:String, geometry:XML, controller:XML, hostObject:CSceneNodeData):void
		{
			CDebug.log("Parse Mesh : "+ name);
			
			var meshData:CMeshData = new CMeshData;
			meshData.name = name;
			
			var attribute:CMeshSegmentData;
			var daeIndex:Dictionary = new Dictionary;
			
			var sourcePositionIndices:Vector.<uint> = new Vector.<uint>;
			var sourcePosition:Vector.<Number>;
			var positionStream:CVertexBufferData;
			
			var sourceNormalIndices:Vector.<uint> = new Vector.<uint>;
			var sourceNormal:Vector.<Number>;
			var normalStream:CVertexBufferData;
			
			var sourceUVIndices:Vector.<uint> = new Vector.<uint>;
			
			var sourceUVS:Vector.<Number> = new Vector.<Number>;
			
			var curUVStream:CVertexBufferData;
			
			var sourceColorIndices:Vector.<uint> = new Vector.<uint>;
			var sourceColor:Vector.<Number>;
			var colorStream:CVertexBufferData;
			
			var verticesXML:XML;
			
			var triangles:XML;
			var polygonsList:XMLList = geometry["mesh"].polygons;
			var len:uint = polygonsList.length();
			
			var i:uint,j:uint, k:uint, index:uint, triangleNum:uint, stream:uint = 0;
			var vertexElement:CVertexElementData;
			
			var pXMLList:XMLList;
			var pVals:Vector.<uint>;
			var curIndex:uint;
			var uvIndex:uint;
			
			var inputXMLList:XMLList;
			var inputXML:XML;
			var semantic:String;
			
			var materials:Vector.<uint> = new Vector.<uint>;
			var firstMaterial:int = -1;
			var indicesData:Vector.<uint>;
			
			for(i=0; i<len; ++i)
			{
				triangles = polygonsList[i];
				inputXMLList = triangles["input"];
				triangleNum = uint( String(triangles.@count) );
				
				if(triangleNum == 0)
					continue;
				
				if(attribute == null)
				{
					attribute = new CMeshSegmentData;
					meshData.attributeRange.push(attribute);
				}
				else
				{
					meshData.attributeRange.push( new CMeshSegmentData );
					meshData.attributeRange[ meshData.attributeRange.length-1 ].startIndex = meshData.numIndices;
					attribute = meshData.attributeRange[ meshData.attributeRange.length-1 ];
				}
				
				materials.push( _parseMaterial( triangles.@material ) );
				
				attribute.indexCount = triangleNum*3;
				
				meshData.numIndices += attribute.indexCount;
				
				if(meshData.vertexDeclaration.length == 0)
				{
					for each(inputXML in inputXMLList)
					{
						semantic = String( inputXML.@semantic );
						switch(semantic)
						{
							case "VERTEX":
								vertexElement = new CVertexElementData;
								vertexElement.stream = stream++;
								vertexElement.type = CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT4;
								vertexElement.usage = CVertexElementData.ELEMENT_USAGE_POSITION;
								verticesXML = geometry["mesh"].vertices.(@id == _getId(inputXML.@source))[0];
								sourcePosition = _getNumAry( geometry["mesh"].source.(@id == _getId(verticesXML.input.@source))[0].float_array.toString() );
								meshData.vertexDeclaration.push(vertexElement);
								
								if(!controller)
									sourcePosition = CMath.leftMulTransformVectors(hostObject.transform, sourcePosition, 3);
								
								meshData.vertexData.push( ( positionStream = new CVertexBufferData ) );
								positionStream.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
								break;
							case "NORMAL":
								vertexElement = new CVertexElementData;
								vertexElement.stream = stream++;
								vertexElement.type = CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT3;
								vertexElement.usage = CVertexElementData.ELEMENT_USAGE_NORMAL;
								sourceNormal = _getNumAry( geometry["mesh"].source.(@id == _getId(inputXML.@source))[0].float_array.toString() );
								meshData.vertexDeclaration.push(vertexElement);
								
								if(!controller)
									sourceNormal = CMath.leftMulTransformVectors(hostObject.transform, sourceNormal, 3, true);
								
								meshData.vertexData.push( ( normalStream = new CVertexBufferData ) );
								normalStream.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
								break;
							case "TEXCOORD":
								vertexElement = new CVertexElementData;
								vertexElement.stream = stream++;
								vertexElement.type = CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT2;
								vertexElement.usage = CVertexElementData.ELEMENT_USAGE_TEXCOORD;
								
								sourceUVS = _getNumAry( geometry["mesh"].source.(@id == _getId(inputXML.@source))[0].float_array.toString() );
								meshData.vertexDeclaration.push(vertexElement);
								
								meshData.vertexData.push( ( curUVStream = new CVertexBufferData ) );
								curUVStream.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
								break;
							case "COLOR":
								vertexElement = new CVertexElementData;
								vertexElement.stream = stream++;
								vertexElement.type = CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT4;
								vertexElement.usage = CVertexElementData.ELEMENT_USAGE_COLOR;
								sourceColor = _getNumAry( geometry["mesh"].source.(@id == _getId(inputXML.@source))[0].float_array.toString() );
								meshData.vertexDeclaration.push(vertexElement);
								
								meshData.vertexData.push( ( colorStream = new CVertexBufferData ) );
								colorStream.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
								break;
							default:
								break;
						}
					}
					
					if(_addedElementData != null)
					{
						var streamIndex:int = stream;
						for each( vertexElement in _addedElementData)
						{
							vertexElement.stream = streamIndex++;
							vertexElement.offset = 0;
							meshData.vertexDeclaration.push( vertexElement );
							meshData.vertexData.push( new CVertexBufferData );
						}
					}
				}
				
				pXMLList = triangles["p"];
				for(j=0; j<triangleNum; ++j)
				{
					pVals = _getUintAry( pXMLList[j].toString() );
					index = 0;
					for(k=0; k<3; ++k)
					{
						for each(inputXML in inputXMLList)
						{
							semantic = String( inputXML.@semantic );
							curIndex = pVals[index++];
							switch(semantic)
							{
								case "VERTEX":
									if(positionStream && sourcePosition)
										sourcePositionIndices.push( curIndex );
									break;
								case "NORMAL":
									if(normalStream && sourceNormal)
										sourceNormalIndices.push( curIndex );
									break;
								case "TEXCOORD":
									if(sourceUVS && uvIndex < sourceUVS.length)
										sourceUVIndices.push( curIndex );
									break;
								case "COLOR":
									if(sourceColor && colorStream)
										sourceColorIndices.push( curIndex );
									break;
								default:
									break;
							}
						}
					}
				}
			}
			
			var externalIndices:Dictionary;
			
			if(controller)
				externalIndices = new Dictionary;
			else
				hostObject.transform.identity();
			
			var normalIndex:uint;
			var computeNormals:Vector.<Number>;
			
			//compute Normal 
			if(sourceNormal && sourcePositionIndices)
			{
				computeNormals = new Vector.<Number>(sourcePosition.length);
				len = sourcePositionIndices.length;
				for(i=0; i<len; ++i)
				{
					index = sourcePositionIndices[i];
					normalIndex = sourceNormalIndices[i];
					
					computeNormals[index*3] += sourceNormal[normalIndex*3];
					computeNormals[index*3+1] += sourceNormal[normalIndex*3+1];
					computeNormals[index*3+2] += sourceNormal[normalIndex*3+2];
				}
				
				len = computeNormals.length/3;
				var vecNormal:Vector3D = new Vector3D;
				for(i=0; i<len; ++i)
				{
					vecNormal.x = computeNormals[i*3];
					vecNormal.y = computeNormals[i*3+1];
					vecNormal.z = computeNormals[i*3+2];
					vecNormal.normalize();
					
					computeNormals[i*3] 	= vecNormal.x;
					computeNormals[i*3+1] 	= vecNormal.y;
					computeNormals[i*3+2] 	= vecNormal.z;
				}
			}
			
			//fill color
			var colorIndex:uint;
			var fillColor:Vector.<Number>;
			
			if(sourceColor && sourcePositionIndices)
			{
				fillColor = new Vector.<Number>(sourcePosition.length/3*4);
				
				len = sourcePositionIndices.length;
				for(i=0; i<len; ++i)
				{
					index = sourcePositionIndices[i];
					colorIndex = sourceColorIndices[i];
					
					fillColor[index*4]   = sourceColor[colorIndex*4];
					fillColor[index*4+1] = sourceColor[colorIndex*4+1];
					fillColor[index*4+2] = sourceColor[colorIndex*4+2];
					fillColor[index*4+3] = sourceColor[colorIndex*4+3];
				}
			}
			
			//compute uv
			var computeUVS:Vector.<Number>;
			if(sourceUVS && sourceUVS.length > 0 && sourcePositionIndices)
			{
				indicesData = new Vector.<uint>;
				
				computeUVS = new Vector.<Number>(sourcePosition.length/3*2);
				
				var vertexIndexUVMap:Vector.<Dictionary> = new Vector.<Dictionary>(computeUVS.length, true);
				var indexMap:Dictionary, tmpIndexMap:Dictionary;
				var indexTmp:Object;
				var indicesArray:Array;
				
				len = sourcePositionIndices.length;
				
				for(i=0; i<len; ++i)
				{
					index = sourcePositionIndices[i];
					uvIndex = sourceUVIndices[i];
					indexMap = vertexIndexUVMap[index];
					
					if(indexMap == null)
					{
						indexMap = new Dictionary;
						indexMap[uvIndex] = index;
						vertexIndexUVMap[index] = indexMap;
						
						computeUVS[index*2] = sourceUVS[uvIndex*2];
						computeUVS[index*2+1] = 1.0-sourceUVS[uvIndex*2+1];
						indicesData.push(index);
					}
					else
					{
						indexTmp = indexMap[uvIndex];
						
						if(indexTmp == null)
						{
							if(sourcePosition)
							{
								sourcePosition.push( sourcePosition[3*index] );
								sourcePosition.push( sourcePosition[3*index+1] );
								sourcePosition.push( sourcePosition[3*index+2] );
							}
							
							if(sourceColor && fillColor)
							{
								fillColor.push( fillColor[4*index] );
								fillColor.push( fillColor[4*index+1] );
								fillColor.push( fillColor[4*index+2] );
								fillColor.push( fillColor[4*index+3] );
							}
							
							if(sourceNormal && computeNormals)
							{
								computeNormals.push( computeNormals[3*index] );
								computeNormals.push( computeNormals[3*index + 1] );
								computeNormals.push( computeNormals[3*index + 2] );
							}
							
							computeUVS.push( sourceUVS[2*uvIndex] );
							computeUVS.push( 1.0 - sourceUVS[2*uvIndex+1] );
							
							indexMap[uvIndex] = computeUVS.length/2 - 1;
							
							indicesData.push( computeUVS.length/2 - 1 );
							
							if(externalIndices)
							{
								indicesArray = externalIndices[index];
								if(indicesArray == null)
								{
									indicesArray = new Array;
									externalIndices[index] = indicesArray;
								}
								
								indicesArray.push( sourcePosition.length/3-1 );
							}
						}
						else
							indicesData.push( indexTmp as uint );
					}
				}
			}
			else
				indicesData = sourcePositionIndices;
			
			meshData.indexUsage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
			meshData.indexData.position = 0;
			meshData.numVertices = sourcePosition.length/3;
			meshData.numIndices = indicesData.length;
			
			if(_addedElementData != null)
			{
				var bufData:CVertexBufferData;
				for each( vertexElement in _addedElementData)
				{
					bufData = new CVertexBufferData;
					bufData.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
					bufData.data.length = meshData.numVertices * vertexElement.type;
					bufData.data.position = 0;
					meshData.vertexData.push( bufData );
				}
			}
			
			if(positionStream)
			{
				for(i=0; i<meshData.numVertices; ++i)
				{
					positionStream.data.writeFloat( sourcePosition[i*3] );
					positionStream.data.writeFloat( sourcePosition[i*3+1] );
					positionStream.data.writeFloat( sourcePosition[i*3+2] );
					positionStream.data.writeFloat( 1.0 );
					
					if(normalStream)
					{
						normalStream.data.writeFloat( computeNormals[i*3] );
						normalStream.data.writeFloat( computeNormals[i*3+1] );
						normalStream.data.writeFloat( computeNormals[i*3+2] );
					}
					
					if(colorStream)
					{
						colorStream.data.writeFloat( fillColor[i*4] );
						colorStream.data.writeFloat( fillColor[i*4+1] );
						colorStream.data.writeFloat( fillColor[i*4+2] );
						colorStream.data.writeFloat( fillColor[i*4+3] );
					}
					
					if(curUVStream)
					{
						curUVStream.data.writeFloat( computeUVS[i*2] );
						curUVStream.data.writeFloat( computeUVS[i*2+1] );
					}
				}
				
				for(i=0; i<meshData.numIndices; i+=3)
				{
					meshData.indexData.writeShort( indicesData[i] );
					meshData.indexData.writeShort( indicesData[i+2] );
					meshData.indexData.writeShort( indicesData[i+1] );
				}
			}
			
			if(materials.length > 0)
			{
				hostObject.meshData = CMeshManager.instance.addMeshData(meshData);
				hostObject.subset = 0;
				hostObject.materialData = materials[0];
				hostObject.isUpdate = true;
				
				var otherObjects:Vector.<CSceneNodeData> = new Vector.<CSceneNodeData>;
				if(controller)
					_skinNodes.push(hostObject);
				
				var sceneObjectData:CSceneNodeData;
				len = materials.length;
				for(i=1; i<len; ++i)
				{
					sceneObjectData = _sceneData.createNodeData(CSceneNodeData.OBJECT_TYPE);
					sceneObjectData.materialData = materials[i];
					sceneObjectData.meshData = hostObject.meshData;
					sceneObjectData.subset = i;
					sceneObjectData.name = hostObject.name + i;
					sceneObjectData.parent = hostObject.parent;
					sceneObjectData.transform.copyFrom(hostObject.transform);
					sceneObjectData.isUpdate = false;
					if(controller)
						_skinNodes.push(sceneObjectData);
					
					otherObjects.push(sceneObjectData);
				}
				
				_parseSkinGemotry(controller, externalIndices, hostObject, otherObjects);
			}
		}
		
		protected function _parseGemotrySingleStream(name:String, geometry:XML, controller:XML, hostObject:CSceneNodeData):void
		{
			CDebug.log("Parse Mesh : "+ name);
			var meshData:CMeshData = new CMeshData;
			meshData.name = name;
			
			var attribute:CMeshSegmentData;
			var daeIndex:Dictionary = new Dictionary;
			
			var sourcePosition:Vector.<Number>;
			var positionStream:CVertexBufferData;
			
			var sourceNormal:Vector.<Number>;
			var normalStream:CVertexBufferData;
			
			var uvIndex:uint;
			var curSourceUV:Vector.< Number >;
			var sourceUVS:Vector.< Vector.<Number> > = new Vector.< Vector.<Number> >;
			
			var curUVStream:CVertexBufferData;
			var uvStreams:Vector.< CVertexBufferData > = new Vector.<CVertexBufferData>;
			
			var sourceColor:Vector.<Number>;
			var colorStream:CVertexBufferData;
			
			var verticesXML:XML;
			
			var triangles:XML;
			var polygonsList:XMLList = geometry["mesh"].polygons;
			var len:uint = polygonsList.length();
			
			var i:uint,j:uint, k:uint, index:uint, triangleNum:uint, stream:uint = 0;
			var vertexElement:CVertexElementData;
			
			var pXMLList:XMLList;
			var pVals:Vector.<uint>;
			var curIndex:uint;
			
			var inputXMLList:XMLList;
			var inputXML:XML;
			var semantic:String;
			
			var externalIndices:Dictionary = new Dictionary;
			var indicesData:Vector.<uint> = new Vector.<uint>;
			
			var materials:Vector.<uint> = new Vector.<uint>;
			
			var sourceArray:Array;
			var indicesArray:Vector.<uint> = new Vector.<uint>(3,true);
			
			var isSplitMesh:Boolean = false;
			
			for(i=0; i<len; ++i)
			{
				triangles = polygonsList[i];
				inputXMLList = triangles["input"];
				triangleNum += uint( String(triangles.@count) );
				
				if(triangleNum*3 > 0xffff)
				{
					isSplitMesh = true;
					break;
				}
			}
			
			if(isSplitMesh && controller)
			{
				CDebug.assert(false,"SplitMesh Unsupport Skin Mesh - CDAEModelLoader::_parseGemotrySingleStream()");
				return;
			}
			
			for(i=0; i<len; ++i)
			{
				triangles = polygonsList[i];
				inputXMLList = triangles["input"];
				triangleNum = uint( String(triangles.@count) );
				
				if(triangleNum == 0)
					continue;
				
				if(attribute == null)
				{
					attribute = new CMeshSegmentData;
					meshData.attributeRange.push(attribute);
				}
				else
				{
					meshData.attributeRange.push( new CMeshSegmentData );
					meshData.attributeRange[ meshData.attributeRange.length-1 ].startIndex = meshData.numIndices;
					attribute = meshData.attributeRange[ meshData.attributeRange.length-1 ];
				}
				
				materials.push( _parseMaterial( triangles.@material ) );
				
				attribute.indexCount = triangleNum*3;
				
				if(meshData.vertexDeclaration.length == 0)
				{
					for each(inputXML in inputXMLList)
					{
						semantic = String( inputXML.@semantic );
						switch(semantic)
						{
							case "VERTEX":
								vertexElement = new CVertexElementData;
								vertexElement.stream = stream++;
								vertexElement.type = CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT4;
								vertexElement.usage = CVertexElementData.ELEMENT_USAGE_POSITION;
								verticesXML = geometry["mesh"].vertices.(@id == _getId(inputXML.@source))[0];
								sourcePosition = _getNumAry( geometry["mesh"].source.(@id == _getId(verticesXML.input.@source))[0].float_array.toString() );
								
								if(!controller)
									sourcePosition = CMath.leftMulTransformVectors(hostObject.transform,sourcePosition,3);
								
								meshData.vertexDeclaration.push(vertexElement);
								
								meshData.vertexData.push( ( positionStream = new CVertexBufferData ) );
								positionStream.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
								break;
							case "NORMAL":
								vertexElement = new CVertexElementData;
								vertexElement.stream = stream++;
								vertexElement.type = CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT3;
								vertexElement.usage = CVertexElementData.ELEMENT_USAGE_NORMAL;
								sourceNormal = _getNumAry( geometry["mesh"].source.(@id == _getId(inputXML.@source))[0].float_array.toString() );
								meshData.vertexDeclaration.push(vertexElement);
								
								if(!controller)
									sourceNormal = CMath.leftMulTransformVectors(hostObject.transform,sourceNormal,3, true);
								
								meshData.vertexData.push( ( normalStream = new CVertexBufferData ) );
								normalStream.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
								break;
							case "TEXCOORD":
								vertexElement = new CVertexElementData;
								vertexElement.stream = stream++;
								vertexElement.type = CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT2;
								vertexElement.usage = CVertexElementData.ELEMENT_USAGE_TEXCOORD;
								
								sourceUVS.push( _getNumAry( geometry["mesh"].source.(@id == _getId(inputXML.@source))[0].float_array.toString() ) );
								meshData.vertexDeclaration.push(vertexElement);
								
								meshData.vertexData.push( ( curUVStream = new CVertexBufferData ) );
								curUVStream.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
								uvStreams.push(curUVStream);
								break;
							case "COLOR":
								vertexElement = new CVertexElementData;
								vertexElement.stream = stream++;
								vertexElement.type = CVertexElementData.VERTEX_ELEMENT_TYPE_FLOAT4;
								vertexElement.usage = CVertexElementData.ELEMENT_USAGE_COLOR;
								sourceColor = _getNumAry( geometry["mesh"].source.(@id == _getId(inputXML.@source))[0].float_array.toString() );
								meshData.vertexDeclaration.push(vertexElement);
								
								meshData.vertexData.push( ( colorStream = new CVertexBufferData ) );
								colorStream.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
								break;
							default:
								break;
						}
					}
				}
				
				pXMLList = triangles["p"];
				for(j=0; j<triangleNum; ++j)
				{
					pVals = _getUintAry( pXMLList[j].toString() );
					index = 0;
					for(k=0; k<3; ++k)
					{
						uvIndex = 0;
						for each(inputXML in inputXMLList)
						{
							semantic = String( inputXML.@semantic );
							curIndex = pVals[index++];
							switch(semantic)
							{
								case "VERTEX":
									if(positionStream && sourcePosition)
									{
										positionStream.data.writeFloat( sourcePosition[curIndex*3] );
										positionStream.data.writeFloat( sourcePosition[curIndex*3+1] );
										positionStream.data.writeFloat( sourcePosition[curIndex*3+2] );
										positionStream.data.writeFloat( 1.0 );
										
										indicesArray[k] = curIndex;
									}
									break;
								case "NORMAL":
									if(normalStream && sourceNormal)
									{
										normalStream.data.writeFloat( sourceNormal[curIndex*3] );
										normalStream.data.writeFloat( sourceNormal[curIndex*3+1] );
										normalStream.data.writeFloat( sourceNormal[curIndex*3+2] );
									}
									break;
								case "TEXCOORD":
									if(sourceUVS && uvStreams && uvIndex < sourceUVS.length && uvIndex < uvStreams.length)
									{
										curSourceUV = sourceUVS[uvIndex];
										curUVStream = uvStreams[uvIndex];
										curUVStream.data.writeFloat( curSourceUV[curIndex*2] );
										curUVStream.data.writeFloat( 1.0 - curSourceUV[curIndex*2+1] );
									}
									++uvIndex;
									break;
								case "COLOR":
									if(sourceColor && colorStream)
									{
										colorStream.data.writeFloat( sourceColor[curIndex*3] );
										colorStream.data.writeFloat( sourceColor[curIndex*3+1] );
										colorStream.data.writeFloat( sourceColor[curIndex*3+2] );
										colorStream.data.writeFloat( sourceColor[curIndex*3+3] );
									}
									break;
								default:
									break;
							}
						}
					}
					
					if(isSplitMesh)
					{
						indicesData.push(j*3+meshData.numIndices);
						indicesData.push(j*3+2+meshData.numIndices);
						indicesData.push(j*3+1+meshData.numIndices);
					}
					else
					{
						meshData.indexData.writeShort(  j*3+meshData.numIndices  );
						meshData.indexData.writeShort( j*3+2+meshData.numIndices );
						meshData.indexData.writeShort( j*3+1+meshData.numIndices );
					}
					
					sourceArray = externalIndices[ indicesArray[0] ];
					if(sourceArray == null)
					{
						sourceArray = new Array;
						externalIndices[ indicesArray[0] ] = sourceArray;
					}
					sourceArray.push( j*3+meshData.numIndices );
					
					sourceArray = externalIndices[ indicesArray[1] ];
					if(sourceArray == null)
					{
						sourceArray = new Array;
						externalIndices[ indicesArray[1] ] = sourceArray;
					}
					sourceArray.push( j*3+1+meshData.numIndices );
					
					sourceArray = externalIndices[ indicesArray[2] ];
					if(sourceArray == null)
					{
						sourceArray = new Array;
						externalIndices[ indicesArray[2] ] = sourceArray;
					}
					sourceArray.push( j*3+2+meshData.numIndices );
				}
				
				meshData.numIndices += attribute.indexCount;
			}
			
			if(materials.length > 0)
			{
				meshData.numVertices = meshData.numIndices;
				meshData.indexUsage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
				var bufData:CVertexBufferData;
				var streamIndex:int = stream;
				
				for( i=0; i<meshData.vertexData.length; ++i)
					meshData.vertexData[i].data.position = 0;
				
				if(!controller)
					hostObject.transform.identity();
				
				var sceneObjectData:CSceneNodeData;
				
				if(isSplitMesh)
				{
					var externalMeshData:CMeshData;
					var curSceneNodeDatas:Vector.<CSceneNodeData> = new Vector.<CSceneNodeData>;
					var externalSceneNodeDatas:Vector.<CSceneNodeData> = new Vector.<CSceneNodeData>;
					var splitNum:uint = meshData.numVertices/MAX_VERTEX_NUM;
					var lastIndices:int = meshData.numVertices % MAX_VERTEX_NUM;
					var lastAttributeIndex:uint=0;
					var lastAttributeIndicesNum:uint=0;
					var externalMeshDataHandle:uint;
					
					for(i=0; i<splitNum; ++i)
					{
						externalMeshData = new CMeshData;
						externalMeshData.name = meshData.name;
						externalMeshData.indexUsage = meshData.indexUsage;
						externalMeshData.numVertices = MAX_VERTEX_NUM;
						externalMeshData.numIndices = MAX_VERTEX_NUM;
						
						for(j=0; j<meshData.vertexDeclaration.length; ++j)
						{
							externalMeshData.vertexDeclaration.push( meshData.vertexDeclaration[j] );
							bufData = new CVertexBufferData;
							bufData.usage = meshData.vertexData[j].usage;
							bufData.data.writeBytes(meshData.vertexData[j].data, i*MAX_VERTEX_NUM*meshData.vertexDeclaration[j].type, meshData.vertexDeclaration[j].type*MAX_VERTEX_NUM);
							externalMeshData.vertexData.push(bufData);
						}
						
						if(_addedElementData != null)
						{
							streamIndex = stream;
							for each( vertexElement in _addedElementData)
							{
								vertexElement.stream = streamIndex++;
								vertexElement.offset = 0;
								externalMeshData.vertexDeclaration.push( vertexElement );
								bufData = new CVertexBufferData;
								bufData.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
								bufData.data.length = MAX_VERTEX_NUM * vertexElement.type;
								bufData.data.position = 0;
								externalMeshData.vertexData.push( bufData );
							}
						}
						
						for(j=0; j<MAX_VERTEX_NUM; ++j)
							externalMeshData.indexData.writeShort( indicesData[j+i*MAX_VERTEX_NUM] - i*MAX_VERTEX_NUM );
						
						for(j=lastAttributeIndex; j<meshData.attributeRange.length; ++j)
						{
							attribute = new CMeshSegmentData;
							if(lastAttributeIndicesNum > 0)
								attribute.startIndex = 0;
							else
							{
								attribute.startIndex = meshData.attributeRange[j].startIndex - i*MAX_VERTEX_NUM;
								
								if(attribute.startIndex > MAX_VERTEX_NUM)
								{
									i += (attribute.startIndex / MAX_VERTEX_NUM);
									lastAttributeIndex = j;
									break;
								}
							}
							
							sceneObjectData = (i == 0 && j == 0) ? hostObject : _sceneData.createNodeData(CSceneNodeData.OBJECT_TYPE);
							
							sceneObjectData.subset = externalMeshData.attributeRange.push(attribute) - 1;
							sceneObjectData.materialData = materials[j];
							curSceneNodeDatas.push(sceneObjectData);
							externalSceneNodeDatas.push(sceneObjectData);
							
							if(lastAttributeIndicesNum != 0)
							{
								attribute.indexCount = lastAttributeIndicesNum;
								if(attribute.indexCount >= MAX_VERTEX_NUM)
								{
									attribute.indexCount = MAX_VERTEX_NUM;
									lastAttributeIndex = j;
									lastAttributeIndicesNum = lastAttributeIndicesNum - MAX_VERTEX_NUM;
									break;
								}
								
								lastAttributeIndicesNum = 0;
							}
							else
							{
								attribute.indexCount = meshData.attributeRange[j].indexCount;
								if( attribute.startIndex + meshData.attributeRange[j].indexCount > MAX_VERTEX_NUM )
								{
									attribute.indexCount = MAX_VERTEX_NUM - attribute.startIndex;
									lastAttributeIndex = j;
									lastAttributeIndicesNum = meshData.attributeRange[j].indexCount + attribute.startIndex - MAX_VERTEX_NUM;
									break;
								}
							}
						}
						
						if(curSceneNodeDatas.length > 0)
						{
							externalMeshDataHandle = CMeshManager.instance.addMeshData(externalMeshData);
							
							for(j=0; j<curSceneNodeDatas.length; ++j)
								curSceneNodeDatas[j].meshData = externalMeshDataHandle;
							
							curSceneNodeDatas.splice(0,curSceneNodeDatas.length);
						}
					}
					
					if(lastIndices)
					{
						externalMeshData = new CMeshData;
						externalMeshData.name = meshData.name;
						externalMeshData.indexUsage = meshData.indexUsage;
						externalMeshData.numVertices = lastIndices;
						externalMeshData.numIndices = lastIndices;
						
						for(j=0; j<meshData.vertexDeclaration.length; ++j)
						{
							externalMeshData.vertexDeclaration.push( meshData.vertexDeclaration[j] );
							bufData = new CVertexBufferData;
							bufData.usage = meshData.vertexData[j].usage;
							bufData.data.writeBytes(meshData.vertexData[j].data, splitNum*MAX_VERTEX_NUM*meshData.vertexDeclaration[j].type, meshData.vertexDeclaration[j].type*lastIndices);
							externalMeshData.vertexData.push(bufData);
						}
						
						if(_addedElementData != null)
						{
							streamIndex = stream;
							for each( vertexElement in _addedElementData)
							{
								vertexElement.stream = streamIndex++;
								vertexElement.offset = 0;
								externalMeshData.vertexDeclaration.push( vertexElement );
								bufData = new CVertexBufferData;
								bufData.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
								bufData.data.length = lastIndices * vertexElement.type;
								bufData.data.position = 0;
								externalMeshData.vertexData.push( bufData );
							}
						}
						
						for(j=0; j<lastIndices; ++j)
							externalMeshData.indexData.writeShort( indicesData[j+splitNum*MAX_VERTEX_NUM] - splitNum*MAX_VERTEX_NUM );
						
						for(j=lastAttributeIndex; j<meshData.attributeRange.length; ++j)
						{
							attribute = new CMeshSegmentData;
							
							if(lastAttributeIndicesNum > 0)
								attribute.startIndex = 0;
							else
								attribute.startIndex = meshData.attributeRange[j].startIndex - splitNum*MAX_VERTEX_NUM;
							
							sceneObjectData = _sceneData.createNodeData(CSceneNodeData.OBJECT_TYPE);
							sceneObjectData.subset = externalMeshData.attributeRange.push(attribute) - 1;
							sceneObjectData.materialData = materials[j];
							curSceneNodeDatas.push(sceneObjectData);
							externalSceneNodeDatas.push(sceneObjectData);
							
							if(lastAttributeIndicesNum != 0)
							{
								attribute.indexCount = lastAttributeIndicesNum;
								lastAttributeIndicesNum = 0;
							}
							else
								attribute.indexCount = meshData.attributeRange[j].indexCount;
						}
						
						externalMeshDataHandle = CMeshManager.instance.addMeshData(externalMeshData);
						
						for(j=0; j<curSceneNodeDatas.length; ++j)
							curSceneNodeDatas[j].meshData = externalMeshDataHandle;
						
						curSceneNodeDatas.splice(0,curSceneNodeDatas.length);
					}
					
					for(i=1; i<externalSceneNodeDatas.length; ++i)
					{
						sceneObjectData = externalSceneNodeDatas[i];
						sceneObjectData.name = hostObject.name + i;
						sceneObjectData.parent = hostObject.parent;
						sceneObjectData.isUpdate = false;
					}
				}
				else
				{
					if(_addedElementData != null)
					{
						for each( vertexElement in _addedElementData)
						{
							vertexElement.stream = streamIndex++;
							vertexElement.offset = 0;
							meshData.vertexDeclaration.push( vertexElement );
							bufData = new CVertexBufferData;
							bufData.usage = 0;//1 << CBuffer.BUFFER_USAGE_WRITE_ONLY_BIT;
							bufData.data.length = MAX_VERTEX_NUM * vertexElement.type;
							bufData.data.position = 0;
							meshData.vertexData.push( bufData );
						}
					}
					
					meshData.indexData.position = 0;
					
					hostObject.meshData = CMeshManager.instance.addMeshData(meshData);
					hostObject.subset = 0;
					hostObject.materialData = materials[0];
					hostObject.isUpdate = true;
					
					if(controller)
						_skinNodes.push(hostObject);
					
					len = materials.length;
					var otherObjects:Vector.<CSceneNodeData> = new Vector.<CSceneNodeData>;
					for(i=1; i<len; ++i)
					{
						sceneObjectData = _sceneData.createNodeData(CSceneNodeData.OBJECT_TYPE);
						sceneObjectData.materialData = materials[i];
						sceneObjectData.meshData = hostObject.meshData;
						sceneObjectData.subset = i;
						sceneObjectData.name = hostObject.name + i;
						sceneObjectData.parent = hostObject.parent;
						sceneObjectData.isUpdate = false;
						
						otherObjects.push(sceneObjectData);
						
						if(controller)
						{
							_skinNodes.push(hostObject);
							sceneObjectData.transform.copyFrom(hostObject.transform);
						}
					}
					
					_parseSkinGemotry(controller, externalIndices, hostObject, otherObjects, true);
				}
			}
		}
		
		protected function _parseMaterial(sysmboal:String):uint
		{
			if(!sysmboal || sysmboal.length == 0)
				return 0;
			
			if(_materialLib[sysmboal] != null)
				return _materialLib[sysmboal];
			
			var materialXML:XML = _collada["library_materials"].material.(@id == sysmboal)[0];
			if (materialXML) 
			{
				var materialData:CMaterialData = new CMaterialData;
				var textureHandle:uint = 0;
				
				var effectId:String = _getId( materialXML["instance_effect"].@url );
				var effect:XML = _collada["library_effects"].effect.(@id == effectId)[0];
				
				materialData.name = sysmboal;
				
				materialData.emissive = _getColorValue(effect..emission[0]);
				
				materialData.ambient = _getColorValue(effect..ambient[0]);//;
				
				materialData.diffuse = _getColorValue(effect..diffuse[0]);//;
				
				//materialData.diffuse = 0xffffffff;
				
				materialData.specular = 0xffffffff;//_getColorValue(effect..specular[0]);//;
				
				materialData.power = Number(effect..shininess.float[0]);//;
				
				if( String( effect..diffuse.texture ) != null && String( effect..diffuse.texture ).length > 2 )
				{
					textureHandle = uint( _textureMap[ String(effect..diffuse[0].texture[0].@texture) ] );
					
					materialData.diffuse = 0xffffffff;
					
					if(textureHandle)
						materialData.textureDataHandles.push( textureHandle );
				}
				
				if( String( effect..specular.texture ) != null && String( effect..specular.texture ).length > 2 )
				{
					textureHandle = uint( _textureMap[ String(effect..specular[0].texture[0].@texture) ] );
					
					if(textureHandle)
						materialData.textureDataHandles.push( textureHandle );
				}
				
				_materialLib[sysmboal] = CMeshManager.instance.addMaterialData(materialData);
				
				return _materialLib[sysmboal];
			}
			
			_materialLib[sysmboal] = CMeshManager.instance.addMaterialData(materialData);
			
			return _materialLib[sysmboal];
		}
		
		protected function _parseAnimationSet():void
		{
			var anims:XML = _collada["library_animations"][0];
			
			if (!anims) {
				CDebug.warning("------------- No animations to parse -------------");
				return;
			}
			
			CDebug.log("------------- Begin Parse Animation -------------");
			
			var animtionList:XMLList = anims["animation"];
			
			_animSetData = new CAnimationSetData;
			_animSetData.name = _fileName;
			
			for each(var anim:XML in animtionList)
				_parseAnimationTracker(anim, _animSetData.animationTrackerDatas);
			
			if( _isCreate )
			{
				var animSet:CAnimationSet = new CAnimationSet;
				animSet.animationTracks = _animSetData.animationTrackerDatas;
				var animController:CAnimationController = new CAnimationController();
				animController.reset();
				animController.registerAnimationSet(animSet, 1.0, 1.0);
				
				_animController = animController;
			}
			
			
//			for(var i:uint=0; i<_skinNodes.length; ++i)
//				_skinNodes[i].animController = animController;
			
			CDebug.log("------------- End Parse Animation -------------");
		}
		
		protected function _parseAnimationTracker(animation:XML, trackers:Vector.<CAnimationTrackData>):void
		{
			if(animation == null)
				return;
			
			var tracker:CAnimationTrackData = new CAnimationTrackData();
			tracker.name = animation.@name;
			
			var channel:XML = animation["animation"]["channel"][0];
			var id:String = channel.@target;
			var name:String = id.split("/")[0];
			var type:String = id.split("/")[1];
			
			CDebug.assert(name == tracker.name, "error animation-bone name:"+name);
			
			if (!type)
			{
				CDebug.warning("No animation type detected");
				return;
			}
			
			var sampler:XML = animation["animation"].sampler.( @id == _getId(channel.@source) )[0];
			var inputs:XMLList = sampler["input"];
			var semantic:String;
			var sourceXML:XML;
			var vecTimes:Vector.<Number>;
			var vecMatrixes:Vector.<Number>;
			var i:int=0;
			
			for each(var input:XML in inputs)
			{
				semantic = input.@semantic;
				sourceXML = animation["animation"].source.(@id == _getId(input.@source))[0];
				switch(semantic)
				{
					case "INPUT":
						vecTimes = _getNumAry(sourceXML.float_array.toString());
						for(i=0; i<vecTimes.length; ++i)
							tracker.times.push( vecTimes[i] );
						break;
					case "OUTPUT":
						vecMatrixes = _getNumAry(sourceXML.float_array.toString());
						for(i=0; i<vecMatrixes.length; i+=16)
							tracker.transformMatrixes.push( _getAnimBoneMatrix( tracker.name, vecMatrixes.slice(i, i+16),  _yUp) );
						break;
					case "INTERPOLATION":
						break;
				}
			}
			
			trackers.push( tracker );
		}
		
		protected function _getNumAry(val:String):Vector.<Number>
		{
			val = val.split("\r\n").join(" ");
			var strings:Array = val.split(" ");
			var numbers:Vector.<Number> = new Vector.<Number>();
			
			var totalStrings:Number = strings.length;
			var strTmp:String;
			var index:int;
			for (var i:uint = 0; i < totalStrings; ++i)
			{
				strTmp = strings[i];
				index = strTmp.lastIndexOf("\n");
				if(index == -1)
					numbers.push(Number(strTmp));
				else
				{
					numbers.push( Number( strTmp.substring(0,index) ) );
					numbers.push( Number( strTmp.substring(index+1,strTmp.length) ) );
				}
			}
			
			return numbers;
		}
		
		protected function _getTextureName(effectXML:XML):String
		{
			if(effectXML..texture[0] == null)
				return null;
			var textureID:String = String( effectXML..texture[0].@texture );
			if(textureID == null || textureID.length == 0)
				return null;
			
			var textureUrl:String = _collada["library_images"].image.(@id == textureID)[0].init_from[0].toString();
			
			var index:int = textureUrl.lastIndexOf("\\");
			if(index != -1)
				return textureUrl.substr(index+1);
			else
				return textureUrl;
		}
		
		protected function _getColorValue(colorXML:XML):uint
		{
			if (!colorXML || colorXML.length() == 0)
				return 0xFFFFFFFF;
			
			if(!colorXML["color"] || colorXML["color"].length() == 0)
				return 0xFFFFFFFF;
			
			var colorArray:Array = colorXML["color"].split(" ");
			if(colorArray.length <= 0)
				return 0xFFFFFFFF;
			
			return colorArray[1] != "" ? int(colorArray[3]*255 << 24) | int(colorArray[2]*255 << 16) | int(colorArray[1]*255 << 8) | int(colorArray[0]*255) : int(colorArray[4]*255 << 24) | int(colorArray[3]*255 << 16) | int(colorArray[2]*255 << 8) | int(colorArray[0]*255);
		}
		
		protected function _getId(url:String):String
		{
			return url.split("#")[1];
		}
		
		protected function _getUintAry(val:String):Vector.<uint>
		{
			val = val.split("\r\n").join(" ");
			var strings:Array = val.split(" ");
			var numbers:Vector.<uint> = new Vector.<uint>();
			
			var totalStrings:Number = strings.length;
			
			var strTmp:String;
			var index:int;
			for (var i:uint = 0; i < totalStrings; ++i)
			{
				strTmp = strings[i];
				index = strTmp.lastIndexOf("\n");
				if(index == -1)
					numbers.push(uint(strTmp));
				else
				{
					numbers.push( uint( strTmp.substring(0,index) ) );
					numbers.push( uint( strTmp.substring(index+1,strTmp.length) ) );
				}
			}
			
			return numbers;
		}
		
		protected function _getNodeMatrix(rawData:Vector.<Number>, node:CSceneNodeData, parent:CSceneNodeData):Matrix3D
		{
			if(parent == _rootNode)
				return _getAxisMatrix(rawData);
			else
			{
				var matTrans:Matrix3D;
				matTrans = new Matrix3D(rawData);
				matTrans.transpose();
				return matTrans;
			}
		}
		
		protected function _getAnimBoneMatrix(name:String,rawData:Vector.<Number>,yUp:Boolean):Matrix3D
		{
			if(_rootBone.name == name)
				return _getAxisMatrix(rawData);
			else
			{
				var matTrans:Matrix3D = new Matrix3D(rawData);
				matTrans.transpose();
				return matTrans;
			}
		}
		
		protected function _getAxisMatrix(rawData:Vector.<Number>):Matrix3D
		{
			var xyzMatrix:Matrix3D = new Matrix3D;
			var xyzRawData:Vector.<Number> = new Vector.<Number>();
			
			var matInput:Matrix3D = new Matrix3D(rawData);
			matInput.transpose();
			
			if(_yUp)
			{
				xyzRawData.push(
					1,  0,   0,   0,
					0,  1,   0,   0,
					0,  0,   -1,  0,
					0,  0,   0,   1
				);
				xyzMatrix.rawData = xyzRawData;
				
				matInput.append( xyzMatrix );
			}
			else
			{
				xyzRawData.push(
					1,  0,  0,  0,
					0,  0,  1,  0,
					0,  1,  0,  0,
					0,  0,  0,  1
				);
				
				xyzMatrix.rawData = xyzRawData;
				
				matInput.append( xyzMatrix );
			}
			
			return matInput;
		}
		
		protected function _getBoneInvalidMatrix(rawData:Vector.<Number>):Matrix3D
		{
			var xyzInverRawData:Vector.<Number> = new Vector.<Number>;
			var xyzInverMatrix:Matrix3D = new Matrix3D;
			
			var matInput:Matrix3D = new Matrix3D(rawData);
			matInput.transpose();
			
			if(_yUp)
			{
				xyzInverRawData.push(
					1,  0,   0,   0,
					0,  1,   0,   0,
					0,  0,   -1,  0,
					0,  0,   0,   1
				);
				
				xyzInverMatrix.rawData = xyzInverRawData;
				
				matInput.prepend( xyzInverMatrix );
			}
			else
			{
				xyzInverRawData.push(
					1,  0,  0,  0,
					0,  0,  1,  0,
					0,  1,  0,  0,
					0,  0,  0,  1
				);
				
				xyzInverMatrix.rawData = xyzInverRawData;
				
				matInput.prepend( xyzInverMatrix );
			}
			
			return matInput;
		}
		
		protected function _onTextureComplete(e:CTextureLoaderEvent):void
		{
			var textureName:String = e.curTextureName;
			var textureData:CTextureData = _textureLoader.getTextureByName(textureName);
			
			textureData.name = textureName;
			
			if(!_textureHandleURLMap)
				_textureHandleURLMap = new Dictionary;
			
			var textureDataHandle:int = CMeshManager.instance.addTextureData( textureData );
			
			_textureHandleURLMap[textureDataHandle] = _basicUrl + "/" + textureName;
			
			var vecID:Vector.<String> = _textureMap[ textureName ];
			
			if(vecID)
			{
				for(var i:uint=0; i<vecID.length; ++i)
					_textureMap[ vecID[i] ] = textureDataHandle;
			}
			//_textureMap[ _textureMap[textureName] ] = textureDataHandle;
			
			//			var refrenceMaterialDatas:Vector.<CMaterialData> = this._textureReferMaterial[textureName];
			//			for each(var materialData:CMaterialData in refrenceMaterialDatas)
			//				materialData.textureDataHandles.push(textureDataHandle);
		}
		
		protected function _onTextureError(e:CTextureLoaderEvent):void
		{
			CDebug.assert(false,"texture load error :" + e.curTextureName);
			
			if(this.hasEventListener(CLoaderEvent.LOAD_ERROR))
				this.dispatchEvent(new CLoaderEvent(CLoaderEvent.LOAD_ERROR));
		}
		
		protected function _onAllTextureComplete(e:CTextureLoaderEvent):void
		{
			_textureLoader.removeEventListener(CTextureLoaderEvent.TEXTURE_COMPLETE_ONE,_onTextureComplete);
			_textureLoader.removeEventListener(CTextureLoaderEvent.TEXTURE_LOADERROR,_onTextureError);
			_textureLoader.removeEventListener(CTextureLoaderEvent.TEXTURE_ALL_LOADCOMPLETE,_onAllTextureComplete);
			
			if(!_parseScene())
				return;
			
			_sceneDataHandle = CMeshManager.instance.addSceneData( _sceneData );
			
			//			if(_parser)
			//				_parser.onParse(_sceneData);
			
			if(this.hasEventListener(CLoaderEvent.ALL_LOAD_OK))
				this.dispatchEvent(new CLoaderEvent(CLoaderEvent.ALL_LOAD_OK));
			
			_textureLoader.destroyResources();
		}
		
		private var _basicUrl:String;
		private var _textureUrls:Vector.<String>;
		
		private var _isCreate:Boolean;
		private var _collada:XML;
		private var _yUp:Boolean;
		private var _isSkinMesh:Boolean;
		private var _sceneData:CSceneData;
		private var _skinNodes:Vector.<CSceneNodeData>;
		private var _animController:CAnimationController;
		private var _animSetData:CAnimationSetData;
		
		private var _materialLib:Dictionary;
		private var _textureMap:Dictionary;
		private var _textureHandleURLMap:Dictionary;
		
		private var _fileName:String;
		
		private var _rootBone:CSceneNodeData;
		private var _rootNode:CSceneNodeData;
		private var _geometryList:Vector.<CSceneNodeData>;
		private var _geometryNames:Vector.<String>;
		private var _geometryXMLList:Vector.<XML>;
		private var _geometryControllerXMLList:Vector.<XML>;
	}
}