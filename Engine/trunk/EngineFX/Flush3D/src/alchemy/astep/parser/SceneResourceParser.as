package alchemy.astep.parser
{
	import alchemy.astep.engineres.BlendMethodRes;
	import alchemy.astep.engineres.CameraRes;
	import alchemy.astep.engineres.DAERes;
	import alchemy.astep.engineres.EffectInfoRes;
	import alchemy.astep.engineres.LightRes;
	import alchemy.astep.engineres.MaterialRes;
	import alchemy.astep.engineres.RenderMethodRes;
	import alchemy.astep.engineres.TextureRes;
	import alchemy.astep.engineres.data.EffectBlendData;
	import alchemy.astep.manager.EngineResManager;
	import alchemy.astep.utility.Utility;
	import alchemy.compilers.CAGALAssembler;
	import alchemy.effect.CDepthEffect;
	import alchemy.effect.CEffectCode;
	import alchemy.effect.CPositionEffect;
	import alchemy.effect.CTextureEffect;
	import alchemy.effect.blendmethod.CAbstractBlendMethod;
	import alchemy.effect.blendmethod.CEffectInfoAddition;
	import alchemy.effect.blendmethod.CEffectInfoModulation;
	import alchemy.manager.CSceneManager;
	import alchemy.resource.basic.CEffect;
	import alchemy.resource.extend.CLight;
	import alchemy.resource.extend.CMaterial;
	import alchemy.resource.extend.CRenderMethod;
	import alchemy.scene.CCamera;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.geom.ColorTransform;
	import flash.geom.Vector3D;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	
	public class SceneResourceParser extends EventDispatcher
	{
		public function SceneResourceParser( sceneManager:CSceneManager = null )
		{
			_sceneMngr = sceneManager;
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
		
		public function parse( data:* ):void
		{
			_dataXML = new XML(data);
			
			if( _dataXML.@version != "astep" )
				return;
			
			var xmlLib:XMLList = _dataXML["library_dae"].source;
			if(xmlLib)
				parseDAELib(xmlLib);
			
			xmlLib = _dataXML["library_texture"].source;
			if(xmlLib)
				parseTextureLib(xmlLib);
			
			xmlLib = _dataXML["library_material"].source;
			if(xmlLib)
				parseMaterialLib(xmlLib);
			
			xmlLib = _dataXML["library_effectInfo"].source;
			if(xmlLib)
				parseEffectInfoLib(xmlLib);
			
			xmlLib = _dataXML["library_blendMethod"].source;
			if(xmlLib)
				parseBlendMethodLib(xmlLib);
			
			xmlLib = _dataXML["library_light"].source;
			if(xmlLib)
				parseLightLib(xmlLib);
			
			xmlLib = _dataXML["library_render"].source;
			if(xmlLib)
				parseRenderLib(xmlLib);
			
			xmlLib = _dataXML["library_camera"].source;
			if(xmlLib)
				parseCameraLib(xmlLib);
			
			xmlLib = _dataXML["library_renderMethod"].source;
			if(xmlLib)
				parseRenderMethodLib(xmlLib);
		}
		
		private function parseEffectInfoLib( xmlLib:XMLList ):void
		{
			if(!xmlLib)
				return;
			
			var res:EffectInfoRes;
			var effectType:String;
			var parameterMap:Object;
			for each( var childXML:XML in xmlLib )
			{
				res = new EffectInfoRes;
				res.name = String( childXML.@name );
				effectType = childXML.@type;
				
				switch( effectType )
				{
					case "PositionEffect":
						res.build(effectType, null, _sceneMngr == null ? null : new CPositionEffect);
						break;
					case "TextureEffect":
						parameterMap = new Object;
						parameterMap["samplerIndex"] = uint( childXML.@samplerIndex );
						parameterMap["texcoordIndex"] = uint( childXML.@texcoordIndex );
						res.build(effectType, parameterMap, _sceneMngr == null ? null : new CTextureEffect(parameterMap["samplerIndex"], parameterMap["texcoordIndex"]) );
						break;
					case "DepthEffect":
						parameterMap = new Object;
						parameterMap["type"] = uint( childXML.@type );
						res.build(effectType, parameterMap, _sceneMngr == null ? null : new CDepthEffect( parameterMap["type"] ) );
						break;
				}
				
				EngineResManager.instance.effectInfoMap[res.name] = res;
			}
		}
		
		private function parseLightLib( xmlLib:XMLList ):void
		{
			if(!xmlLib)
				return;
			
			var defSceneManager:CSceneManager = _sceneMngr ? _sceneMngr : CSceneManager.getInstance();
			
			var light:CLight;
			var lightRes:LightRes;
			
			for each( var lightXML:XML in xmlLib )
			{
				light = defSceneManager.createLight();
				lightRes = new LightRes(light);
				lightRes.name = String( lightXML.@name );
				
				switch( String(lightXML.@type) )
				{
					case "POINT":
						light.type = CLight.POINT;
						break;
					case "SPOT":
						light.type = CLight.SPOT;
						break;
					case "DIRECTIONAL":
						light.type = CLight.DIRECTIONAL;
						break;
				}
				
				var strTemp:String = lightXML.@range;
				if(strTemp != null && strTemp.length > 0)
					light.range = Number( strTemp );
				
				strTemp = lightXML.@falloff;
				if(strTemp != null && strTemp.length > 0)
					light.falloff = Number( strTemp );
				
				strTemp = lightXML.@theta;
				if(strTemp != null && strTemp.length > 0)
					light.theta = Number( strTemp );
				
				strTemp = lightXML.@attenuation0;
				if(strTemp != null && strTemp.length > 0)
					light.attenuation0 = Number( strTemp );
				
				strTemp = lightXML.@attenuation1;
				if(strTemp != null && strTemp.length > 0)
					light.attenuation1 = Number( strTemp );
				
				strTemp = lightXML.@attenuation2;
				if(strTemp != null && strTemp.length > 0)
					light.attenuation2 = Number( strTemp );
				
				var tempArray:Array = lightXML.position.toString().split(" ");
				if(tempArray.length > 1)
					light.position = new Vector3D( tempArray[0], tempArray[1], tempArray[2] );
				
				tempArray = lightXML.direction.toString().split(" ");
				if(tempArray.length > 1)
					light.direction = new Vector3D( tempArray[0], tempArray[1], tempArray[2] );
				
				tempArray = lightXML.diffuse.toString().split(" ");
				if(tempArray.length > 1)
					light.diffuse = new ColorTransform(tempArray[0],tempArray[1],tempArray[2] );
				
				tempArray = lightXML.ambient.toString().split(" ");
				if(tempArray.length > 1)
					light.ambient = new ColorTransform(tempArray[0],tempArray[1],tempArray[2] );
				
				tempArray = lightXML.specular.toString().split(" ");
				if(tempArray.length > 1)
					light.specular = new ColorTransform(tempArray[0],tempArray[1],tempArray[2] );
				
				var shadowXML:XML = lightXML.shadowMap[0];
				if( shadowXML )
					light.buildShadowMap( Number( shadowXML.@size ), Number( shadowXML.@fov ),Number( shadowXML.@nearPlane ),Number( shadowXML.@farPlane ) );
				
				var shadeModle:uint;
				switch( lightXML.shadeMode.toString() )
				{
					case "SHADE_PHONG":
						shadeModle = CSceneManager.SHADE_PHONG;
						break;
					case "SHADE_BLINN_PHONG":
						shadeModle = CSceneManager.SHADE_BLINN_PHONG;
						break;
				}
				
				lightRes.shadeMode = shadeModle;
				EngineResManager.instance.lightResMap[lightRes.name] = lightRes;
			}
		}
		
		private function parseRenderLib( xmlLib:XMLList ):void
		{
			if(!xmlLib)
				return;
		}
		
		private function parseCameraLib( xmlLib:XMLList ):void
		{
			if(!xmlLib)
				return;
			
			var index:uint = 0;
			var cameraRes:CameraRes;
			var camera:CCamera;
			var tempArray:Array;
			for each(var childXML:XML in xmlLib)
			{
				if(index++ == 0 && _sceneMngr)
					camera = _sceneMngr.camera;
				else
					camera = new CCamera;
				
				cameraRes = new CameraRes(camera);
				cameraRes.name = String( childXML.@name );
				
				tempArray = childXML.position.toString().split(" ");
				camera.position = new Vector3D( tempArray[0], tempArray[1], tempArray[2] );
				
				tempArray = childXML.rotate.toString().split(" ");
				camera.rotateX = Number(tempArray[0]);
				camera.rotateY = Number(tempArray[1]);
				camera.rotateZ = Number(tempArray[2]);
				
				camera.setProjection( Number( childXML.@fov ), Number( childXML.@nearPlane ), Number( childXML.@farPlane ) );
				
				EngineResManager.instance.cameraResMap[cameraRes.name] = cameraRes;
			}
		}
		
		private function parseDAELib( xmlLib:XMLList ):void
		{
			if(!xmlLib)
				return;
			
			var daeRes:DAERes;
			for each(var daeXML:XML in xmlLib)
			{
				daeRes = new DAERes;
				daeRes.name = String( daeXML.@name );
				daeRes.build( String(daeXML.@url), 0 );
				
				EngineResManager.instance.daeResMap[daeRes.name] = daeRes;
			}
		}
		
		private function parseMaterialLib( xmlLib:XMLList ):void
		{
			if(!xmlLib)
				return;
			
			var materialRes:MaterialRes;
			var material:CMaterial;
			var type:String, createType:uint;
			var tempArray:Array;
			var daeSource:String;
			
			var texRes:TextureRes;
			var textureXMLList:XMLList, textureXML:XML;
			
			for each( var childXML:XML in xmlLib )
			{
				type = childXML.@type;
				createType = MaterialRes.FIRENEW;
				daeSource = null;
				switch(type)
				{
					case "SOURCE":
						createType = MaterialRes.SOURCE;
						daeSource = childXML.@daeSource;
						break;
					case "FIRENEW":
						createType = MaterialRes.FIRENEW;
						material = _sceneMngr ? _sceneMngr.createMaterial() : null;
						break;
				}
				
				materialRes = new MaterialRes(createType, material);
				materialRes.name = childXML.@name;
				materialRes.daeSource = daeSource;
				
				tempArray = childXML.diffuse.toString().split(" ");
				if(tempArray.length > 1)
					materialRes.diffuseColor = new Vector.<Number>( [ tempArray[0], tempArray[1], tempArray[2], tempArray[3] ] );
				
				tempArray = childXML.ambient.toString().split(" ");
				if(tempArray.length > 1)
					materialRes.ambientColor = new Vector.<Number>( [ tempArray[0], tempArray[1], tempArray[2], tempArray[3] ] );
				
				tempArray = childXML.specular.toString().split(" ");
				if(tempArray.length > 1)
					materialRes.specularColor = new Vector.<Number>( [ tempArray[0], tempArray[1], tempArray[2], tempArray[3] ] );
				
				tempArray = childXML.emissive.toString().split(" ");
				if(tempArray.length > 1)
					materialRes.emissiveColor = new Vector.<Number>( [ tempArray[0], tempArray[1], tempArray[2], tempArray[3] ] );
				
				type = childXML.@power;
				if(type.length > 0)
					materialRes.power = Number(type);
				
				textureXMLList = childXML.texture;
				for each(textureXML in textureXMLList)
				{
					texRes = EngineResManager.instance.textureResMap[ String( textureXML.@textureSource ) ];
					
					if(!texRes)
					{
						Utility.waring(String( textureXML.@textureSource ) + " is not found" );
						continue;
					}
					materialRes.addTextureRes(texRes);
				}
				
				EngineResManager.instance.materialResMap[materialRes.daeSource + materialRes.name] = materialRes;
			}
		}
		
		private function parseTextureLib( xmlLib:XMLList ):void
		{
			if(!xmlLib)
				return;
			
			var textureRes:TextureRes;
			for each( var childXML:XML in xmlLib )
			{
				textureRes = new TextureRes;
				textureRes.name = String( childXML.@name );
				textureRes.build( String( childXML.@url ) );
				EngineResManager.instance.textureResMap[textureRes.name] = textureRes;
			}
		}
		
		private function parseBlendMethodLib( xmlLib:XMLList ):void
		{
			if(!xmlLib)
				return;
			
			var blendMethodRes:BlendMethodRes;
			var blendMethod:CAbstractBlendMethod;
			var type:String;
			var parametersMap:Object;
			var blendFlag:uint, blendStr:String;
			for each(var blendXML:XML in xmlLib)
			{
				type = blendXML.@type;
				blendMethodRes = new BlendMethodRes;
				blendMethodRes.name = blendXML.@name;
				
				if(type == "EffectInfoAdd" || type == "EffectInfoModulation")
				{
					blendStr = String(blendXML.@blendFlag);
					switch(blendStr)
					{
						case "SourceAlpha":
							blendFlag = CEffectInfoAddition.SOURCE_ALPHA;
							break;
						case "DestAlpha":
							blendFlag = CEffectInfoAddition.DEST_ALPHA;
							break;
						case "BlendAlpha":
							blendFlag = CEffectInfoAddition.BLEND_ALPHA;
							break;
					}
				}
				
				switch( type )
				{
					case "EffectInfoAdd":
						blendMethodRes.build(type, null, _sceneMngr == null ? null : new CEffectInfoAddition(blendFlag) );
						break;
					case "EffectInfoModulation":
						blendMethodRes.build(type, null, _sceneMngr == null ? null : new CEffectInfoModulation(blendFlag) );
						break;
				}
				
				EngineResManager.instance.blendMethodMap[blendMethodRes.name] = blendMethodRes;
			}
		}
		
		private function parseRenderMethodLib( xmlLib:XMLList ):void
		{
			if(!xmlLib)
				return;
			
			var effectInfoXMLList:XMLList;
			var effectInfoXML:XML;
			
			var effectInfoRes:EffectInfoRes;
			var blendRes:BlendMethodRes;
			var blendData:EffectBlendData;
			
			var renderMethodRes:RenderMethodRes;
			
			var compiler:CAGALAssembler = new CAGALAssembler;
			var code:CEffectCode;
			
			var blendDatas:Vector.<EffectBlendData> = new Vector.<EffectBlendData>;
			
			var effect:CEffect;
			var renderMethod:CRenderMethod;
			
			for each(var nodeXML:XML in xmlLib)
			{
				code = _sceneMngr ? compiler.createEffectCode() : null;
				renderMethodRes = new RenderMethodRes;
				renderMethodRes.name = nodeXML.@name;
				
				effectInfoXMLList = nodeXML.effectInfo;
				blendDatas.splice(0,blendDatas.length);
				for each( effectInfoXML in effectInfoXMLList )
				{
					effectInfoRes = EngineResManager.instance.effectInfoMap[ String(effectInfoXML.@effectSource) ];
					blendData = new EffectBlendData;
					blendData.infoRes = effectInfoRes;
					
					blendRes = EngineResManager.instance.blendMethodMap[ String(effectInfoXML.@blendSource) ];
					if(code)
						code.addInfoForBlendMethod( effectInfoRes.effectInfo, blendRes ? blendRes.blendMethod : null );
					
					blendData.blendRes = blendRes;
					blendDatas.push(blendData);
				}
				
				if(code)
				{
					code.buildPass();
					effect = _sceneMngr.renderSystem.createEffectFromEffectCode(code);
					renderMethod = _sceneMngr.createRenderMethodFromEffectCode(code);
				}
				
				renderMethodRes.build(blendDatas, effect, renderMethod);
				
				EngineResManager.instance.renderMethodMap[ renderMethodRes.name ] = renderMethodRes;
			}
		}
		
		private function onFileLoadOK( e:Event ):void
		{
			var loader:URLLoader = e.target as URLLoader;
			loader.removeEventListener(Event.COMPLETE, onFileLoadOK);
			loader.removeEventListener(IOErrorEvent.IO_ERROR, onFileLoadError);
			loader.removeEventListener(ProgressEvent.PROGRESS, onFileLoadProgress);
			
			parse(loader.data);
			
			if( hasEventListener(Event.COMPLETE) )
				dispatchEvent( e.clone() );
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
		
		private var _dataXML:XML;
		private var _sceneMngr:CSceneManager;
	}
}