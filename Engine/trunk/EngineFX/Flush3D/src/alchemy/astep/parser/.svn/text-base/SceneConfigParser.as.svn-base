package alchemy.astep.parser
{
	import alchemy.animation.CAnimationController;
	import alchemy.astep.engineres.BlendMethodRes;
	import alchemy.astep.engineres.DAERes;
	import alchemy.astep.engineres.EffectInfoRes;
	import alchemy.astep.engineres.MaterialRes;
	import alchemy.astep.engineres.RenderMethodRes;
	import alchemy.astep.engineres.TextureRes;
	import alchemy.astep.engineres.data.EffectBlendData;
	import alchemy.astep.iores.IORes;
	import alchemy.astep.iores.SceneDataRes;
	import alchemy.astep.iores.TextureDataRes;
	import alchemy.astep.manager.EngineResManager;
	import alchemy.astep.manager.IOResManager;
	import alchemy.astep.utility.Utility;
	import alchemy.compilers.CAGALAssembler;
	import alchemy.effect.CEffectCode;
	import alchemy.manager.CSceneManager;
	import alchemy.processor.CSkinMeshProcessor;
	import alchemy.resource.basic.CEffect;
	import alchemy.resource.extend.CMaterial;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.scene.CSceneNode;
	import alchemy.scene.CSceneObject;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	import flash.utils.Dictionary;
	
	public class SceneConfigParser extends EventDispatcher
	{
		public function SceneConfigParser( sceneManager:CSceneManager )
		{
			_sceneManager = sceneManager;
			
		}
		
		public function load( url:String ):void
		{
			var xmlLoader:URLLoader = new URLLoader();
			xmlLoader.addEventListener(Event.COMPLETE, onFileLoadOK);
			xmlLoader.addEventListener(ProgressEvent.PROGRESS, onFileLoadProgress);
			xmlLoader.addEventListener(IOErrorEvent.IO_ERROR, onFileLoadError);
			
			var request:URLRequest = new URLRequest(url);
			xmlLoader.load(request);
		}
		
		public function get sceneNodes():Vector.<CSceneNode>
		{
			return _sceneNodes;
		}
		
		public function getAnimationByNode( node:CSceneNode ):CAnimationController
		{
			return _animControllerMap ? _animControllerMap[node] : null;
		}
		
		public function parseScene( data:* ):void
		{
			_data = new XML(data);
			
			if( _data.@version != "astep" )
				return;
			
			_sceneNodes = new Vector.<CSceneNode>;
			_animControllerMap = new Dictionary;
			_daeMaterialMap = new Dictionary;
			
			var sceneNodesXML:XMLList = _data["library_sceneInfo"].sceneInfo;
			var nodeXML:XML;
			
			_totalTaskNum = 0;
			
			_tmpBuffer = new Dictionary;
			
			for each(nodeXML in sceneNodesXML)
				parseSceneNodeRes(nodeXML);
		}
		
		private function parseSceneNodeRes( nodeXML:XML ):void
		{
			if(!nodeXML)
				return;
			
			var sceneType:String = String(nodeXML.@sceneType);
			switch( sceneType )
			{
				case "entity":
				{
					var daeRes:DAERes = EngineResManager.instance.daeResMap[ String(nodeXML.@daeSource) ];
					if(!daeRes)
					{
						Utility.waring(String(nodeXML.@daeSource) + "is not found!");
						return;
					}
					
					if(daeRes.sceneDataHandle == 0)
					{
						var sceneDataRes:SceneDataRes = IOResManager.instance.obtainRes(daeRes.url, IOResManager.SCENE_DATA_TYPE) as SceneDataRes;
						if(sceneDataRes.state == IORes.LOAD_OK)
							fillMaterialAttacthDAE(daeRes, sceneDataRes);
						else if( sceneDataRes.state == IORes.UN_LOAD )
						{
							++_totalTaskNum;
							sceneDataRes.addEventListener(Event.COMPLETE, onDAEResLoadOK);
							sceneDataRes.addEventListener(IOErrorEvent.IO_ERROR, onDAEResLoadError);
							sceneDataRes.load( daeRes.url );
							_tmpBuffer[sceneDataRes] = daeRes;
						}
					}
				}	
				break;
			}
			
			
			for each( var child:XML in nodeXML.children() )
			{
				if( String( child.localName() ) == "sceneInfo" )
					parseSceneNodeRes( child );
			}
		}
		
		private function parseSceneNode( nodeXML:XML, parent:CSceneNode ):void
		{
			if(!nodeXML)
				return;
			
			var sceneType:String = String(nodeXML.@sceneType);
			var sceneNode:CSceneNode;
			var sceneObject:CSceneObject;
			var renderRes:RenderMethodRes;
			var i:int, j:uint, len:uint;
			var roleNode:CSceneNode;
			
			var attachMaterials:Vector.<MaterialRes>;
			var materialInfo:Dictionary;
			var materialName:String;
			var materialRes:MaterialRes;
			
			switch( sceneType )
			{
				case "entity":
				{
					if(!_sceneManager)
					{
						Utility.waring("sceneManager is null!");
						break;
					}
					var daeRes:DAERes = EngineResManager.instance.daeResMap[ String(nodeXML.@daeSource) ];
					if(!daeRes)
					{
						Utility.waring(String(nodeXML.@daeSource) + "is not found!");
						return;
					}
					
					attachMaterials = _daeMaterialMap[daeRes];
					if(attachMaterials != null)
						materialInfo = new Dictionary;
					
					sceneNode = _sceneManager.createScene(daeRes.sceneDataHandle, 1, materialInfo);
					
					if(!sceneNode)
						break;
					
					roleNode = sceneNode.getChildByIndex(0);
					
					var skinProcessor:CSkinMeshProcessor;
					var material:CMaterial;
					
					if(daeRes.animationController != null)
					{
						for(i=0; i<roleNode.childrenLen; ++i)
						{
							sceneObject = roleNode.getChildByIndex(i) as CSceneObject;
							if(sceneObject)
							{
								skinProcessor = sceneObject.meshProcessor as CSkinMeshProcessor;
								if(skinProcessor)
									skinProcessor.animtionController = daeRes.animationController;
							}
						}
						
						_animControllerMap[sceneNode] = daeRes.animationController;
					}
					
					var materialHandle:uint;
					len = attachMaterials ? attachMaterials.length : 0;
					for(i=0; i<len; ++i)
					{
						materialRes = attachMaterials[i];
						materialHandle = daeRes.getMaterialDataByName(materialRes.name);
						materialHandle = materialInfo[materialHandle];
						
						for(j=0; j<roleNode.childrenLen; ++j)
						{
							sceneObject = roleNode.getChildByIndex(j) as CSceneObject;
							if(sceneObject)
							{
								if( materialHandle == sceneObject.material )
								{
									material = _sceneManager.getMaterial(materialHandle);
									materialRes.material = material;
									break;
								}
							}
						}
					}
					
					parseBasicInfo(nodeXML, sceneNode);
					
					var renderSource:String = String(nodeXML.@renderMethodSource);
					if(renderSource.length > 0)
					{
						if( _renderResMap )
							renderRes = _renderResMap[ renderSource ];
						
						if(renderRes)
							parseRenderMethod(Utility.getAppropriateXML(_data["library_renderMethod"].source, renderSource ),sceneObject);
						else
							renderRes = EngineResManager.instance.renderMethodMap[ renderSource ];
						
						for(i=0; i<roleNode.childrenLen; ++i)
						{
							sceneObject = roleNode.getChildByIndex(i) as CSceneObject;
							if(sceneObject)
							{
								sceneObject.effect = renderRes.effect.handle;
								sceneObject.setRenderMethod( renderRes.renderMethod );
							}
						}
					}
					
					var sceneObjectXMLList:XMLList = nodeXML.sceneObject;
					for each(var objXML:XML in sceneObjectXMLList)
					{
						sceneObject = sceneNode.findChildWithName( objXML.@name, true ) as CSceneObject;
						if(!sceneObject)
						{
							Utility.waring( String(objXML.@name) + "is not found" );
							continue;
						}
						
						if( _renderResMap )
							renderRes = _renderResMap[ String(objXML.@renderMethodSource) ];
						
						if(renderRes)
							parseRenderMethod(Utility.getAppropriateXML(_data["library_renderMethod"].source, String(objXML.@renderMethodSource) ),sceneObject);
						else
							renderRes = EngineResManager.instance.renderMethodMap[ String(objXML.@renderMethodSource) ];
						
						Utility.assert(renderRes != null,"no render method source " + String(objXML.@renderMethodSource) );
						
						if(!renderRes)
							break;
						
						sceneObject.effect = renderRes.effect.handle;
						sceneObject.setRenderMethod( renderRes.renderMethod );
					}
				}	
				break;
			}
			
			if(parent)
				parent.addChild( sceneNode );
			else
				_sceneNodes.push( sceneNode );
			
			for each( var child:XML in nodeXML.children() )
			{
				if( String( child.localName() ) == "sceneInfo" )
					parseSceneNode( child, sceneNode );
			}
		}
		
		private function parseBasicInfo( node:XML, sceneNode:CSceneNode ):void
		{
			sceneNode.x = Number( node.@x );
			sceneNode.y = Number( node.@y );
			sceneNode.z = Number( node.@z );
			
			sceneNode.scaleX = Number( node.@scaleX ) == 0 ? 1 : Number( node.@scaleX );
			sceneNode.scaleY = Number( node.@scaleX ) == 0 ? 1 : Number( node.@scaleX );
			sceneNode.scaleZ = Number( node.@scaleX ) == 0 ? 1 : Number( node.@scaleX );
			
			sceneNode.rotateX = Number( node.@rotateX );	
			sceneNode.rotateY = Number( node.@rotateY );	
			sceneNode.rotateZ = Number( node.@rotateZ );
		}
		
		private function parseRenderMethodLib( xmlLib:XMLList ):void
		{
			if(!xmlLib)
				return;
			
			for each(var nodeXML:XML in xmlLib)
			{
				if(!_renderResMap)
					_renderResMap = new Dictionary;
				
				_renderResMap[ String(nodeXML.@name) ] = new RenderMethodRes;
			}
		}
		
		private function parseRenderMethod( nodeXML:XML, parameter:* ):void
		{
			if(!nodeXML)
				return;
			
			var effectInfoXMLList:XMLList = nodeXML.effectInfo;
			var effectInfoXML:XML;
			
			var effectInfoRes:EffectInfoRes;
			var blendRes:BlendMethodRes;
			var blendData:EffectBlendData;
			
			
			var compiler:CAGALAssembler = new CAGALAssembler;
			var code:CEffectCode = compiler.createEffectCode();
			
			var blendDatas:Vector.<EffectBlendData> = new Vector.<EffectBlendData>;
			
			var effect:CEffect;
			var renderMethod:CRenderMethod;
			
			var renderMethodRes:RenderMethodRes = _renderResMap[ String(nodeXML.@name) ];
			
			for each( effectInfoXML in effectInfoXMLList )
			{
				effectInfoRes = EngineResManager.instance.effectInfoMap[ String(effectInfoXML.@effectSource) ];
				blendData = new EffectBlendData;
				blendData.infoRes = effectInfoRes;
				
				blendRes = EngineResManager.instance.blendMethodMap[ String(effectInfoXML.@blendSource) ];
				code.addInfoForBlendMethod( effectInfoRes.effectInfo, blendRes ? blendRes.blendMethod : null );
				
				blendData.blendRes = blendRes;
				blendDatas.push(blendData);
			}
			
			code.buildPass();
			effect = _sceneManager.renderSystem.createEffectFromEffectCode(code);
			renderMethod = _sceneManager.createRenderMethodFromEffectCode(code);
			
			renderMethodRes.build(blendDatas, effect, renderMethod);
		}
		
		private function parseAll():void
		{
			var nodesXML:XMLList = _data["library_renderMethod"].source;
			var nodeXML:XML;
			
			if(nodesXML)
				parseRenderMethodLib(nodesXML);
			
			nodesXML = _data["library_sceneInfo"].sceneInfo;
			
			for each(nodeXML in nodesXML)
				parseSceneNode(nodeXML, null);
			
			if( hasEventListener(Event.COMPLETE) )
				dispatchEvent( new Event(Event.COMPLETE) );
		}
		
		private function fillMaterialAttacthDAE( daeRes:DAERes, sceneRes:SceneDataRes ):void
		{
			daeRes.sceneDataHandle = sceneRes.sceneData;
			daeRes.animationController = sceneRes.animationController;
			daeRes.setMaterialLib( sceneRes.materialDataLib );
			
			var materialMap:Dictionary = EngineResManager.instance.materialResMap;
			var materialRes:MaterialRes, textureRes:TextureRes, textureDataRes:TextureDataRes;
			var materialResArray:Vector.<MaterialRes>;
			var i:uint, len:uint;
			for each( materialRes in materialMap)
			{
				if(materialRes.materialType == MaterialRes.SOURCE && materialRes.daeSource == daeRes.name)
				{
					len = materialRes.textureNum;
					for(i=0; i<len; ++i)
					{
						textureRes = materialRes.getTextureRes(i);
						if(textureRes.texture == null)
						{
							textureDataRes = IOResManager.instance.obtainRes(textureRes.url, IOResManager.TEXTURE_DATA_TYPE) as TextureDataRes;
							if(textureDataRes.state == IORes.LOAD_OK)
								textureRes.texture = Utility.createTexture(_sceneManager.renderSystem, textureDataRes.textureDataHandle);
							else if( textureDataRes.state == IORes.UN_LOAD )
							{
								++_totalTaskNum;
								textureDataRes.addEventListener(Event.COMPLETE, onTextureResLoadOK);
								textureDataRes.addEventListener(IOErrorEvent.IO_ERROR, onTextureResLoadError);
								_tmpBuffer[textureDataRes] = textureRes;
								textureDataRes.load( textureRes.url );
							}
						}
					}
					
					materialResArray = _daeMaterialMap[daeRes];
					if(materialResArray == null)
					{
						materialResArray = new Vector.<MaterialRes>;
						_daeMaterialMap[daeRes] = materialResArray;
					}
					
					materialResArray.push(materialRes);
				}
			}
		}
		
		private function onTextureResLoadOK( e:Event ):void
		{
			var res:TextureDataRes = e.target as TextureDataRes;
			res.removeEventListener(Event.COMPLETE,onTextureResLoadOK);
			res.removeEventListener(IOErrorEvent.IO_ERROR,onTextureResLoadError);
			
			var texRes:TextureRes = _tmpBuffer[res];
			texRes.texture = Utility.createTexture(_sceneManager.renderSystem, res.textureDataHandle);
			
			if( --_totalTaskNum == 0 )
				parseAll();
		}
		
		private function onTextureResLoadError( e:IOErrorEvent ):void
		{
			var res:TextureDataRes = e.target as TextureDataRes;
			res.removeEventListener(Event.COMPLETE,onTextureResLoadOK);
			res.removeEventListener(IOErrorEvent.IO_ERROR,onTextureResLoadError);
			
			var texRes:TextureRes = _tmpBuffer[res];
			
			Utility.waring( texRes.url + " load error!" );
			
			if( --_totalTaskNum == 0 )
				parseAll();
		}
		
		private function onDAEResLoadOK( e:Event ):void
		{
			var res:SceneDataRes = e.target as SceneDataRes;
			res.removeEventListener(Event.COMPLETE,onDAEResLoadOK);
			res.removeEventListener(IOErrorEvent.IO_ERROR,onDAEResLoadError);
			
			fillMaterialAttacthDAE(_tmpBuffer[res] as DAERes, res);
			
			if( --_totalTaskNum == 0 )
				parseAll();
		}
		
		private function onDAEResLoadError( e:IOErrorEvent ):void
		{
			var res:SceneDataRes = e.target as SceneDataRes;
			res.removeEventListener(Event.COMPLETE,onDAEResLoadOK);
			res.removeEventListener(IOErrorEvent.IO_ERROR,onDAEResLoadError);
			
			var daeRes:DAERes = _tmpBuffer[res];
			
			Utility.waring( daeRes.url + " load error!" );
			
			if( --_totalTaskNum == 0 )
				parseAll();
		}
		
		private function onFileLoadOK( e:Event ):void
		{
			var loader:URLLoader = e.target as URLLoader;
			loader.removeEventListener(Event.COMPLETE, onFileLoadOK);
			loader.removeEventListener(IOErrorEvent.IO_ERROR, onFileLoadError);
			loader.removeEventListener(ProgressEvent.PROGRESS, onFileLoadProgress);
			
			parseScene( loader.data );
		}
		
		private function onFileLoadError( e:IOErrorEvent ):void
		{
			var loader:URLLoader = e.target as URLLoader;
			loader.removeEventListener(Event.COMPLETE, onFileLoadOK);
			loader.removeEventListener(IOErrorEvent.IO_ERROR, onFileLoadError);
			loader.removeEventListener(ProgressEvent.PROGRESS, onFileLoadProgress);
			
			Utility.waring( e.text );
			
			if( hasEventListener(IOErrorEvent.IO_ERROR) )
				dispatchEvent( e.clone() );
		}
		
		private function onFileLoadProgress( e:ProgressEvent ):void
		{
			var loader:URLLoader = e.target as URLLoader;
			if( hasEventListener(ProgressEvent.PROGRESS) )
				dispatchEvent( e.clone() );
		}
		
		private var _data:XML;
		private var _totalTaskNum:int;
		private var _daeMaterialMap:Dictionary;
		private var _sceneNodes:Vector.<CSceneNode>;
		private var _animControllerMap:Dictionary;
		private var _sceneManager:CSceneManager;
		private var _renderResMap:Dictionary;
		private var _tmpBuffer:Dictionary;
	}
}