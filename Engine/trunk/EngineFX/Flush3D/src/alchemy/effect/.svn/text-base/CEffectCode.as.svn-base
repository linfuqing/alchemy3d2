package alchemy.effect
{
	import alchemy.animation.CSkinInfo;
	import alchemy.animation.CSkinMeshController;
	import alchemy.compilers.CAbstractCompiler;
	import alchemy.core.CLibrary;
	import alchemy.core.CPointer;
	import alchemy.data.CVertexElementData;
	import alchemy.effect.blendmethod.CAbstractBlendMethod;
	import alchemy.effect.blendmethod.CBidirectionalBlendMethod;
	import alchemy.effect.blendmethod.CEffectInfoAddition;
	import alchemy.effect.blendmethod.CEffectInfoModulation;
	import alchemy.effect.postprocess.CDepthFieldEffect;
	import alchemy.manager.CSceneManager;
	import alchemy.resource.extend.CMesh;
	import alchemy.scene.CBone;
	import alchemy.scene.CSceneObject;
	import alchemy.tools.CDebug;

	public class CEffectCode extends CPointer
	{
		public static const SELECTARG1:int = 0;
		public static const SELECTARG2:int = 1;
		public static const MODULATE:int = 2;
		public static const MODULATE2X:int = 3;
		public static const MODULATE4X:int = 4;
		public static const ADD:int = 5;
		public static const ADDSIGNED:int = 6;
		public static const ADDSIGNED2X:int = 7;
		public static const SUBTRACT:int = 8;
		public static const ADDSMOOTH:int = 9;
		public static const BLENDDIFFUSEALPHA:int = 10;
		public static const BLENDTEXTUREALPHA:int = 11;
		public static const BLENDFACTORALPHA:int = 12;
		public static const BLENDTEXTUREALPHAPM:int = 13;
		public static const BLENDCURRENTALPHA:int = 14;
		public static const PREMODULATE:int = 15;
		public static const MODULATEALPHA_ADDCOLOR:int = 16;
		public static const MODULATECOLOR_ADDALPHA:int = 17;
		public static const MODULATEINVALPHA_ADDCOLOR:int = 18;
		public static const MODULATEINVCOLOR_ADDALPHA:int = 19;
		public static const BUMPENVMAP:int = 20;
		public static const BUMPENVMAPLUMINANCE:int = 21;
		public static const DOTPRODUCT3:int = 22;
		public static const MULTIPLYADD:int = 23;
		public static const LERP:int = 25;
		
		public function CEffectCode(/*compilerInterfacePointer:uint = 0*/effectCodePointer:uint = 0, passDefinerPointer:uint = 0)
		{
			CDebug.assert(CAbstractCompiler.canCreateEffectCode, "effect code must be created by compiler.");
			
			//var pointers:Array = CLibrary.method.CreateEffectCode(compilerInterfacePointer);
			
			_pointer            = effectCodePointer;//pointers[0];
			_passDefinerPointer = passDefinerPointer;//pointers[1];
		}
		
		public function addInfo(info:CAbstractEffectInfo, blendMode:int):void
		{
			if(info)
				CLibrary.method.AddInfoToPassDefiner(_passDefinerPointer, info.pointer, blendMode);
		}
		
		public function addColorKey(type:uint = 16, isResult:Boolean = false):uint
		{
			if(_passDefinerPointer)
				return CLibrary.method.AddColorKeyToPassDefiner(_passDefinerPointer, type, isResult);
			
			return 0;
		}
		
		public function addColorKeyInfo(info:CAbstractEffectInfo, blendMethod:CAbstractBlendMethod, inputColorKey:uint = 0, outputColorKey:uint = 0):void
		{
			if(_passDefinerPointer)
				CLibrary.method.AddColorKeyAndInfoToPassDefiner(_passDefinerPointer, info ? info.pointer : null, blendMethod ? blendMethod.pointer : null, inputColorKey, outputColorKey);
		}
		
		public function addInfoFromColorKey(inputKey:uint, blendMethod:CAbstractBlendMethod, sourceColorKey:uint = 0, outputColorKey:uint = 0):void
		{
			if(_passDefinerPointer)
				CLibrary.method.AddColorKeyInfoToPassDefiner(_passDefinerPointer, inputKey, blendMethod ? blendMethod.pointer : null, sourceColorKey, outputColorKey);
		}
		
		public function addInfoForBlendMethod(info:CAbstractEffectInfo, blendMethod:CAbstractBlendMethod):void
		{
			CLibrary.method.AddInfoForBlendMethodToPassDefiner(_passDefinerPointer, info.pointer, blendMethod ? blendMethod.pointer : 0);
		}
		
		public function addSceneEffectInfo(sceneManager:CSceneManager, effectInfoBlendMethod:CAbstractBlendMethod, specularEffect:CAbstractEffectInfo = null, normalMapEffect:CAbstractEffectInfo = null, outputKey:uint = 0, intputKey:uint = 0):void
		{
			CLibrary.method.AddEffectInfoToPassFromSceneManager(sceneManager.pointer, _passDefinerPointer, effectInfoBlendMethod ? effectInfoBlendMethod.pointer : 0, specularEffect ? specularEffect.pointer : 0, normalMapEffect ? normalMapEffect.pointer : 0, outputKey, intputKey);
		}
		
		public function addLightInfo(alphaTextureEffect:CTextureEffect, sourceKey:uint, lightColorEffect:CAbstractEffectInfo):void
		{
			var texColorKey:uint = addColorKey();
			addColorKeyInfo(alphaTextureEffect, null, 0, texColorKey);
			addInfoFromColorKey(sourceKey, new CBidirectionalBlendMethod(CBidirectionalBlendMethod.POSITIVATE), texColorKey);
			addInfoFromColorKey(texColorKey, new CEffectInfoModulation(CEffectInfoModulation.SOURCE_ALPHA));
			
			var otherKey:uint = addColorKey();
			addColorKeyInfo(lightColorEffect, new CBidirectionalBlendMethod(CBidirectionalBlendMethod.NEGATIVATE), texColorKey, otherKey);
			addInfoFromColorKey(otherKey, new CEffectInfoAddition(CEffectInfoAddition.SOURCE_ALPHA) );
		}
		
		public function setSkinInfo(skinType:uint, skinInfo:CSkinInfo,rootBone:CBone, sceneObj:CSceneObject):uint
		{
			return CLibrary.method.SetSkinInfoToSceneObject(sceneObj.pointer, skinType, skinInfo.pointer,rootBone.pointer,_passDefinerPointer);
		}
		
		public function buildPass():uint
		{
			return CLibrary.method.EndDefinePassDefiner(_passDefinerPointer);
		}
		
		public function createSkinMeshController(mesh:CMesh, skinType:uint, skinInfo:CSkinInfo, rootBone:CBone):uint
		{
			return CLibrary.method.CreateSkinMeshController(mesh.pointer, skinType, skinInfo.pointer, rootBone.pointer,  this.pointer);
		}
		
		
		/********************************
		 * 
		 * 	build effect program
		 *
		 *******************************/
//		public function createProgram(EffectType:uint):uint
//		{
//			return CLibrary.method.CreateProgramInstance(_passDefinerPointer,EffectType);
//		}
		
//		public function createShadowMapProgram(uLightIndex:uint):Array
//		{
//			return CLibrary.method.CreateShadowMapProgram(uLightIndex, _passDefinerPointer);
//		}
//		
//		public function createEnvironmentDisplayProgram(uSamplerIndex:uint,program:CEnvironmentPromgram, blendMethod:uint):Array
//		{
//			return CLibrary.method.CreateEnvironmentDisplayProgram(program.pointer, uSamplerIndex, _passDefinerPointer, blendMethod);
//		}
//		
//		public function createDistanceInfoProgram(depthField:CDepthFieldEffect):Array
//		{
//			return CLibrary.method.CreateDistanceInfoProgram(depthField.pointer, _passDefinerPointer);
//		}
		
		/********************************
		 * 
		 * 	build effectinfo
		 *
		 *******************************/
//		public function createShadowMapDisplayEffectInfo( uLightIndex:uint, uSamplerIndex:uint ):Array
//		{
//			return CLibrary.method.CreateShadowMapDisplayEffectInfo( _passDefinerPointer, uLightIndex, uSamplerIndex );
//		}
		
		private var _passDefinerPointer:uint;
	}
}