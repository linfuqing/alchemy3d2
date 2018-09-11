#include "AS3/AS3.h"

#include "AlchemyFlasccGraphicsManager.h"

#include "../Graphics/AlchemyBone.h"
#include "../Graphics/AlchemySceneNodeUpdaer.h"
#include "../Graphics/AlchemySceneObject.h"
#include "../Graphics/AlchemyGrassSystem.h"
#include "../Graphics/AlchemySkydome.h"
#include "../Graphics/AlchemyGeometryInstance.h"

#include "../Graphics/AlchemySceneNodeFactory.h"

#include "../Graphics/AlchemySkinMeshProcessor.h"

#include "../Graphics/AlchemyCascadShadowMapRender.h"

#include "../Graphics/AlchemyPositionEffect.h"
#include "../Graphics/AlchemyPixelColorEffect.h"
#include "../Graphics/AlchemyTextureEffect.h"
#include "../Graphics/AlchemyEmptyEffect.h"
#include "../Graphics/AlchemyEnvironmentalEffect.h"
#include "../Graphics/AlchemyNormalMapEffect.h"
#include "../Graphics/AlchemyVertexColorEffect.h"
#include "../Graphics/AlchemyCascadShadowDisplayEffect.h"
#include "../Graphics/AlchemySkyPositionEffect.h"
#include "../Graphics/AlchemyInstancePositionEffect.h"
#include "../Graphics/AlchemyInstancePosition3Scale1Effect.h"

#include "../Graphics/AlchemyEffectInfoAddition.h"
#include "../Graphics/AlchemyEffectInfoModulation.h"
#include "../Graphics/AlchemyPixelFogBlendMethod.h"
#include "../Graphics/AlchemyAlphaTestBlendMethod.h"

#include "../SceneManagement/AlchemyAxisAlignedBoundingBox.h"

using namespace alchemy;

typedef enum
{
	CAST_TRANSFORM,
	CAST_SCENE_NODE,
	CAST_BONE,
	CAST_SCENE_NODE_UPDATER,
	CAST_CAMERA,
	CAST_SCENE_OBJECT,
	CAST_SKY_DOME,
	CAST_GRASS_SYSTEM, 
	CAST_GEOMETRY_INSTANCE,

	CAST_UPDATER, 

	CAST_MESH_PROCESSOR,
	CAST_SKIN_MESH_PROCESSOR,

	CAST_ANIMATION_CONTROLLER, 

	CAST_RENDER_GROUP, 
	CAST_CASCAD_SHADOW_MAP_RENDER
}CASTDATATYPE;

const void* g_pData = ALCHEMY_NULL;

class CRenderSystemHandler :
	public CSmartClass<IEventDispatcher::IListener>
{
public:
	CRenderSystemHandler()
	{

	}

	~CRenderSystemHandler()
	{

	}

	void Handler(const IEventDispatcher::CEvent& Event)
	{
		const CGraphicsManager::CRenderSystemEvent* pEvent = dynamic_cast<const CGraphicsManager::CRenderSystemEvent*>(&Event);
		if(pEvent)
		{
			g_pData = &pEvent->GetRenderSystem();

			inline_as3(
				"import com.adobe.flascc.CModule;\n"
				"import flash.events.Event;\n"
				"CModule.rootSprite.dispatchEvent( new Event(\"Render System Created\") );\n"
				);
		}
	}
};

class CTimer :
	public CSmartClass<ITimerQueue::ITimer>
{
public:
	CTimer()
	{

	}

	~CTimer()
	{

	}

	void Handler(FLOAT fTime, ITimerQueue& TimerQueue)
	{
		g_pData = &TimerQueue;

		inline_as3(
			"import com.adobe.flascc.CModule;\n"
			"import flash.events.Event;\n"
			"CModule.rootSprite.dispatchEvent( new Event(\"Timer Complete\") );\n"
			);
	}
};

CRenderSystemHandler g_RenderSystemHandler;
CTimer g_Timer;
CSceneNodeFactory g_SceneNodeFactory;

void GetDataPointer() __attribute__( (used,
										annotate("as3sig:public function getDataPointer():uint"),
										annotate("as3package:alchemy") ) );

void Cast() __attribute__( (used,
						  annotate("as3sig:public function cast(pointer:uint, from:int, to:int):uint"),
						  annotate("as3package:alchemy") ) );

void GraphicsManagerCreateRenderSystem() __attribute__( (used,
													   annotate("as3sig:public function graphicsManagerCreateRenderSystem(x:Number = 0.0, y:Number = 0.0, width:Number = 0.0, height:Number = 0.0):void"),
													   annotate("as3package:alchemy") ) );

void GraphicsManagerCreateSceneManager() __attribute__( (used,
													   annotate("as3sig:public function graphicsManagerCreateSceneManager(renderSystem:uint):uint"),
													   annotate("as3package:alchemy") ) );

void MeshManagerCreateMeshDataPlane() __attribute__( (used,
													annotate("as3sig:public function meshManagerCreateMeshDataPlane( \
															 width:Number, \
															 height:Number, \
															 widthSegment:uint = 1, \
															 heightSegment:uint = 1, \
															 widthStep:uint = 1, \
															 heightStep:uint = 1, \
															 yUp:Boolean = false, \
															 vertexElementsPointer:uint = 0, \
															 elementCount:uint = 0, \
															 positionOffset:uint = 0, \
															 vertexUsagePointer:uint = 0, \
															 indexUsage:uint = 0):uint"),
															 annotate("as3package:alchemy") ) );

void MeshManagerAddMeshData() __attribute__( (used,
											annotate("as3sig:public function meshManagerAddMeshData(meshDataPointer:uint):uint"),
											annotate("as3package:alchemy") ) );

void RenderSystemGetCompiler() __attribute__( (used,
											 annotate("as3sig:public function renderSystemGetCompiler(renderSystem:uint):uint"),
											 annotate("as3package:alchemy") ) );

void RenderSystemCreateEffectFromEffectCode() __attribute__( (used,
															annotate("as3sig:public function renderSystemCreateEffectFromEffectCode(renderSystem:uint, effectCode:uint):uint"),
															annotate("as3package:alchemy") ) );

void RenderSystemCreateTexture() __attribute__( (used,
											   annotate("as3sig:public function renderSystemCreateTexture(renderSystem:uint, width:uint, height:uint, usage:uint, format:int, mipLevel:uint = 0, dataPointer:uint = 0):uint"),
											   annotate("as3package:alchemy") ) );

void RenderSystemCreateCubeTexture() __attribute__( (used,
											   annotate("as3sig:public function renderSystemCreateCubeTexture( \
														renderSystem:uint, \
														size:uint, \
														usage:uint, \
														format:int, \
														mipLevel:uint = 0, \
														positiveXDataPointer:uint = 0, \
														negativeXDataPointer:uint = 0, \
														positiveYDataPointer:uint = 0, \
														negativeYDataPointer:uint = 0, \
														positiveZDataPointer:uint = 0, \
														negativeZDataPointer:uint = 0):uint"),
											   annotate("as3package:alchemy") ) );

void RenderSystemSetViewport() __attribute__( (used,
											 annotate("as3sig:public function renderSystemSetViewport(renderSystem:uint, width:uint, height:uint, antiAlias:int):void"),
											 annotate("as3package:alchemy") ) );

void RenderSystemClear() __attribute__( (used,
									   annotate("as3sig:public function renderSystemClear(renderSystem:uint, color:uint, flag:uint):void"),
									   annotate("as3package:alchemy") ) );

void RenderSystemPresent() __attribute__( (used,
										 annotate("as3sig:public function renderSystemPresent(renderSystem:uint):void"),
										 annotate("as3package:alchemy") ) );

void SceneManagerGetRenderSystem() __attribute__( (used,
												 annotate("as3sig:public function sceneManagerGetRenderSystem(sceneManager:uint):uint"),
												 annotate("as3package:alchemy") ) );

void SceneManagerGetSceneCamera() __attribute__( (used,
												annotate("as3sig:public function sceneManagerGetSceneCamera(sceneManager:uint):uint"),
												annotate("as3package:alchemy") ) );

void SceneManagerSetBackgroundColor() __attribute__( (used,
													annotate("as3sig:public function sceneManagerSetBackgroundColor(sceneManager:uint, color:uint):void"),
													annotate("as3package:alchemy") ) );

void SceneManagerCreateMesh() __attribute__( (used,
											annotate("as3sig:public function sceneManagerCreateMesh(sceneManager:uint, meshData:uint):uint"),
											annotate("as3package:alchemy") ) );

void SceneManagerCreateMaterial() __attribute__( (used,
												annotate("as3sig:public function sceneManagerCreateMaterial(sceneManager:uint, materialData:uint = 0, textureCachePointer:uint = 0, textureInfoPointer:uint = 0):uint"),
												annotate("as3package:alchemy") ) );

void SceneManagerCreateLight() __attribute__( (used,
											 annotate("as3sig:public function sceneManagerCreateLight(sceneManager:uint):uint"),
											 annotate("as3package:alchemy") ) );

void SceneManagerCreateRenderMethodFromEffectCode() __attribute__( (used,
																  annotate("as3sig:public function sceneManagerCreateRenderMethodFromEffectCode(sceneManager:uint, effectCode:uint, shaderParametersHandler:uint = 0, renderStateHandler:uint = 0):uint"),
																  annotate("as3package:alchemy") ) );

void SceneManagerAddChild() __attribute__( (used,
										  annotate("as3sig:public function sceneManagerAddChild(sceneManager:uint, sceneNode:uint):void"),
										  annotate("as3package:alchemy") ) );

void SceneManagerAddRenderGroup() __attribute__( (used,
												annotate("as3sig:public function sceneManagerAddRenderGroup(sceneManager:uint, renderGroup:uint):void"),
												annotate("as3package:alchemy") ) );

void SceneManagerUpdate() __attribute__( (used,
										annotate("as3sig:public function sceneManagerUpdate(sceneManager:uint, elapseTime:Number = 0.0):void"),
										annotate("as3package:alchemy") ) );

void SceneManagerRender() __attribute__( (used,
										annotate("as3sig:public function sceneManagerRender(sceneManager:uint):void"),
										annotate("as3package:alchemy") ) );

void MeshComputeTangent()  __attribute__( (used,
										 annotate("as3sig:public function meshComputeTangent(mesh:uint, texcoordIndex:uint = 0):void"),
										 annotate("as3package:alchemy") ) );

void MaterialSetAmibient() __attribute__( (used,
										 annotate("as3sig:public function materialSetAmibient(material:uint, r:Number = 0.0, g:Number = 0.0, b:Number = 0.0, a:Number = 1.0):uint"),
										 annotate("as3package:alchemy") ) );

void MaterialSetDiffuse() __attribute__( (used,
										 annotate("as3sig:public function materialSetDiffuse(material:uint, r:Number = 0.0, g:Number = 0.0, b:Number = 0.0):uint"),
										 annotate("as3package:alchemy") ) );

void MaterialSetSpecular() __attribute__( (used,
										annotate("as3sig:public function materialSetSpecular(material:uint, r:Number = 0.0, g:Number = 0.0, b:Number = 0.0):uint"),
										annotate("as3package:alchemy") ) );

void MaterialSetEmissive() __attribute__( (used,
										 annotate("as3sig:public function materialSetEmissive(material:uint, r:Number = 0.0, g:Number = 0.0, b:Number = 0.0):uint"),
										 annotate("as3package:alchemy") ) );

void MaterialSetPower() __attribute__( (used,
									  annotate("as3sig:public function materialSetPower(material:uint, power:Number = 1.0):uint"),
									  annotate("as3package:alchemy") ) );

void MaterialAddTexture() __attribute__( (used,
										annotate("as3sig:public function materialAddTexture(material:uint, texture:uint):uint"),
										annotate("as3package:alchemy") ) );

void MaterialAddLight() __attribute__( (used,
									  annotate("as3sig:public function materialAddLight(material:uint, light:uint, shadeMode:int):Boolean"),
									  annotate("as3package:alchemy") ) );

void MaterialAddCamera() __attribute__( (used,
									   annotate("as3sig:public function materialAddCamera(material:uint, camera:uint):uint"),
									   annotate("as3package:alchemy") ) );

void MaterialAddLightEffectInfoToPass()  __attribute__( (used,
													   annotate("as3sig:public function materialAddLightEffectInfoToPass( \
																material:uint, \
																passDefiner:uint, \
																effectInfoBlendMethod:uint = 0, \
																specularEffect:uint = 0, \
																normalMapEffect:uint = 0, \
																outputKey:uint = 0, \
																inputKey:uint = 0):void"),
																annotate("as3package:alchemy") ) );

void LightSetType() __attribute__( (used,
								  annotate("as3sig:public function lightSetType(light:uint, type:int):void"),
								  annotate("as3package:alchemy") ) );

void LightSetAmibient() __attribute__( (used,
									  annotate("as3sig:public function lightSetAmibient(light:uint, r:Number = 0.0, g:Number = 0.0, b:Number = 0.0, a:Number = 1.0):void"),
									  annotate("as3package:alchemy") ) );

void LightSetDiffuse() __attribute__( (used,
									 annotate("as3sig:public function lightSetDiffuse(light:uint, r:Number = 0.0, g:Number = 0.0, b:Number = 0.0):void"),
										annotate("as3package:alchemy") ) );

void LightSetSpecular() __attribute__( (used,
									  annotate("as3sig:public function lightSetSpecular(light:uint, r:Number = 0.0, g:Number = 0.0, b:Number = 0.0):void"),
									  annotate("as3package:alchemy") ) );

void LightSetPosition() __attribute__( (used,
									  annotate("as3sig:public function lightSetPosition(light:uint, x:Number = 0.0, y:Number = 0.0, z:Number = 0.0):void"),
									  annotate("as3package:alchemy") ) );

void LightSetDirection() __attribute__( (used,
									   annotate("as3sig:public function lightSetDirection(light:uint, x:Number = 0.0, y:Number = 0.0, z:Number = 0.0):void"),
									   annotate("as3package:alchemy") ) );

void LightSetRange() __attribute__( (used,
								   annotate("as3sig:public function lightSetRange(light:uint, range:Number):void"),
								   annotate("as3package:alchemy") ) );

void LightSetFalloff() __attribute__( (used,
									 annotate("as3sig:public function lightSetFalloff(light:uint, falloff:Number):void"),
									 annotate("as3package:alchemy") ) );

void LightSetAttenuation0() __attribute__( (used,
										  annotate("as3sig:public function lightSetAttenuation0(light:uint, attenuation0:Number):void"),
										  annotate("as3package:alchemy") ) );

void LightSetAttenuation1() __attribute__( (used,
										  annotate("as3sig:public function lightSetAttenuation1(light:uint, attenuation1:Number):void"),
										  annotate("as3package:alchemy") ) );

void LightSetAttenuation2() __attribute__( (used,
										  annotate("as3sig:public function lightSetAttenuation2(light:uint, attenuation2:Number):void"),
										  annotate("as3package:alchemy") ) );

void LightSetTheta() __attribute__( (used,
								   annotate("as3sig:public function lightSetTheta(light:uint, theta:Number):void"),
								   annotate("as3package:alchemy") ) );

void LightSetPhi() __attribute__( (used,
								 annotate("as3sig:public function lightSetPhi(light:uint, phi:Number):void"),
								 annotate("as3package:alchemy") ) );

void CreateRenderStateHandler() __attribute__( (used,
											  annotate("as3sig:public function createRenderStateHandler(renderStateInfosPointer:uint, count:uint):uint"),
											  annotate("as3package:alchemy") ) );

void TransformGetRotation() __attribute__( (used,
										  annotate("as3sig:public function transformGetRotation(transform:uint):uint"),
										  annotate("as3package:alchemy") ) );

void TransformGetTranslation() __attribute__( (used,
											 annotate("as3sig:public function transformGetTranslation(transform:uint):uint"),
											 annotate("as3package:alchemy") ) );

void TransformSetScale() __attribute__( (used,
									   annotate("as3sig:public function transformSetScale(transform:uint, x:Number = 0.0, y:Number = 0.0, z:Number = 0.0):void"),
									   annotate("as3package:alchemy") ) );

void TransformSetRotation() __attribute__( (used,
										  annotate("as3sig:public function transformSetRotation(transform:uint, x:Number = 0.0, y:Number = 0.0, z:Number = 0.0):void"),
										  annotate("as3package:alchemy") ) );

void TransformSetTranslation() __attribute__( (used,
											 annotate("as3sig:public function transformSetTranslation(transform:uint, x:Number = 0.0, y:Number = 0.0, z:Number = 0.0):void"),
											 annotate("as3package:alchemy") ) );

void TransformSetDirection() __attribute__( (used,
										   annotate("as3sig:public function transformSetDirection(transform:uint, x:Number = 0.0, y:Number = 0.0, z:Number = 0.0):void"),
										   annotate("as3package:alchemy") ) );

void TransformForward() __attribute__( (used,
									  annotate("as3sig:public function transformForward(transform:uint, numSteps:Number):void"),
									  annotate("as3package:alchemy") ) );

void TransformUp() __attribute__( (used,
								 annotate("as3sig:public function transformUp(transform:uint, numSteps:Number):void"),
								 annotate("as3package:alchemy") ) );

void TransformRight() __attribute__( (used,
									annotate("as3sig:public function transformRight(transform:uint, numSteps:Number):void"),
									annotate("as3package:alchemy") ) );

void TransformSetMatrix() __attribute__( (used,
										annotate("as3sig:public function transformSetMatrix(transform:uint, matrix:uint):void"),
										annotate("as3package:alchemy") ) );

void CreateSceneNode() __attribute__( (used,
									 annotate("as3sig:public function createSceneNode():uint"),
									 annotate("as3package:alchemy") ) );

void SceneNodeSetName() __attribute__( (used,
									  annotate("as3sig:public function sceneNodeSetName(sceneNode:uint, name:uint):void"),
									  annotate("as3package:alchemy") ) );

void SceneNodeSetVisible() __attribute__( (used,
										 annotate("as3sig:public function sceneNodeSetVisible(sceneNode:uint, value:Boolean):void"),
										 annotate("as3package:alchemy") ) );

void SceneNodeGetLocalMatrix() __attribute__( (used,
											 annotate("as3sig:public function sceneNodeGetLocalMatrix(sceneNode:uint):uint"),
											 annotate("as3package:alchemy") ) );

void SceneNodeGetChild() __attribute__( (used,
									   annotate("as3sig:public function sceneNodeGetChild(sceneNode:uint, name:uint):uint"),
									   annotate("as3package:alchemy") ) );

void SceneNodeAddChild() __attribute__( (used,
									   annotate("as3sig:public function sceneNodeAddChild(sceneNode:uint, child:uint):void"),
									   annotate("as3package:alchemy") ) );

void SceneNodeIntersectTree() __attribute__( (used,
											annotate("as3sig:public function sceneNodeIntersectTree( \
													 sceneNode:uint, \
													 ray:uint, \
													 intersectInfosPointer:uint = 0, \
													 intersectInfoCountPointer:uint = 0, \
													 checkHitOn:Boolean = false, \
													 hitOnMask:Boolean = false, \
													 checkFront:Boolean = false, \
													 frontMask:Boolean = false, \
													 hitPointsPointer:uint = 0,\
													 faceNormalsPointer:uint = 0):Boolean"),
											annotate("as3package:alchemy") ) );

void SceneNodeCloneTree() __attribute__( (used,
										annotate("as3sig:public function sceneNodeCloneTree(sceneNode:uint):uint"),
										annotate("as3package:alchemy") ) );

void SceneNodeUpdate() __attribute__( (used,
									 annotate("as3sig:public function sceneNodeUpdate(sceneNode:uint):uint"),
									 annotate("as3package:alchemy") ) );

void CreateBone() __attribute__( (used,
								annotate("as3sig:public function createBone():uint"),
								annotate("as3package:alchemy") ) );

void BoneRegisterAnimationController() __attribute__( (used,
													 annotate("as3sig:public function boneRegisterAnimationController(bone:uint, animationController:uint):void"),
													 annotate("as3package:alchemy") ) );

void CreateSceneNodeUpdater() __attribute__( (used,
											annotate("as3sig:public function createSceneNodeUpdater(sceneManager:uint = 0):uint"),
											annotate("as3package:alchemy") ) );

void SceneNodeUpdaterAddUpdater() __attribute__( (used,
												annotate("as3sig:public function sceneNodeUpdaterAddUpdater(sceneNodeUpdater:uint, updater:uint):void"),
												annotate("as3package:alchemy") ) );

void CreateCamera() __attribute__( (used,
								  annotate("as3sig:public function createCamera():uint"),
								  annotate("as3package:alchemy") ) );

void CameraSetProjection() __attribute__( (used,
										 annotate("as3sig:public function cameraSetProjection(camera:uint, fov:Number, nearPlane:Number, farPlane:Number, aspect:Number = 0.0, leftHand:Boolean = true):uint"),
										 annotate("as3package:alchemy") ) );

void CreateSceneObject() __attribute__( (used,
									   annotate("as3sig:public function createSceneObject(mesh:uint = 0, subset:uint = 0, effect:uint = 0, renderMethod:uint = 0, renderPass:uint = 0, material:uint = 0, sceneManager:uint = 0, sceneObject:uint = 0):uint"),
									   annotate("as3package:alchemy") ) );

void SceneObjectGetMeshProcessor() __attribute__( (used,
												 annotate("as3sig:public function sceneObjectGetMeshProcessor(sceneObject:uint):uint"),
												 annotate("as3package:alchemy") ) );

void SceneObjectSetMesh() __attribute__( (used,
										  annotate("as3sig:public function sceneObjectSetMesh(sceneObject:uint, mesh:uint):void"),
										  annotate("as3package:alchemy") ) );

void SceneObjectSetSubset() __attribute__( (used,
										  annotate("as3sig:public function sceneObjectSetSubset(sceneObject:uint, subset:uint):void"),
										  annotate("as3package:alchemy") ) );

void SceneObjectSetEffect() __attribute__( (used,
												 annotate("as3sig:public function sceneObjectSetEffect(sceneObject:uint, effect:uint):void"),
												 annotate("as3package:alchemy") ) );

void SceneObjectSetRenderMethod() __attribute__( (used,
												 annotate("as3sig:public function sceneObjectSetRenderMethod(sceneObject:uint, renderMethod:uint):void"),
												 annotate("as3package:alchemy") ) );

void SceneObjectSetRenderPass() __attribute__( (used,
												 annotate("as3sig:public function sceneObjectSetRenderPass(sceneObject:uint, renderPass:uint):void"),
												 annotate("as3package:alchemy") ) );

void SceneObjectSetMaterial() __attribute__( (used,
											annotate("as3sig:public function sceneObjectSetMaterial(sceneObject:uint, material:uint):void"),
											annotate("as3package:alchemy") ) );

void SceneObjectSetMeshProcessor() __attribute__( (used,
												 annotate("as3sig:public function sceneObjectSetMeshProcessor(sceneObject:uint, meshProcessor:uint):void"),
												 annotate("as3package:alchemy") ) );

void SceneObjectSetBoundingVolume() __attribute__( (used,
												  annotate("as3sig:public function sceneObjectSetBoundingVolume(sceneObject:uint, boundingVolume:uint):void"),
												  annotate("as3package:alchemy") ) );

void CreateSkyDome() __attribute__( (used,
								   annotate("as3sig:public function createSkyDome( \
											radius:Number = 0.0, \
											phiDelta:Number = 0.0, \
											thetaDelta:Number = 0.0, \
											material:uint = 0, \
											mainTextureIndex:uint = 0, \
											offsetTextureIndicesPointer:uint = 0, \
											offsetNum:uint = 0):uint"),
											annotate("as3package:alchemy") ) );

void CreateGrassSystem() __attribute__( (used,
									   annotate("as3sig:public function createGrassSystem(grassMesh:uint, subset:uint, grassMapInfoPointer:uint, grassSystemTerrain:uint = 0):uint"),
									   annotate("as3package:alchemy") ) );

void CreateGrassSystemMeshTerrain() __attribute__( (used,
												  annotate("as3sig:public function createGrassSystemMeshTerrain(terrainDataPointer:uint, terrainDataCount:uint):uint"),
												  annotate("as3package:alchemy") ) );

void CreateGeometryInstance() __attribute__( (used,
											annotate("as3sig:public function createGeometryInstance():uint"),
											annotate("as3package:alchemy") ) );

void GeometryInstanceSetInstances() __attribute__( (used,
												  annotate("as3sig:public function geometryInstanceSetInstances( \
														    geometryInstance:uint, \
														    instanceCount:Number, \
														    transformCount:uint, \
														    pTransformInfosPointer:uint, \
														    transformInfoCount:uint = 0):void"),
												  annotate("as3package:alchemy") ) );

void GeometryInstanceSetTransform() __attribute__( (used,
												  annotate("as3sig:public function geometryInstanceSetTransform( \
														    geometryInstance:uint, \
														    transformIndex:uint, \
														    transformInfoIndex:uint, \
														    valuesPointer:uint, \
														    keep:Boolean):void"),
												  annotate("as3package:alchemy") ) );

void CreateSkinMeshProcessor() __attribute__( (used,
											 annotate("as3sig:public function createSkinMeshProcessor( \
													  skinType:int, \
													  skinInfo:uint, \
													  bone:uint, \
													  skinParamtersPointer:uint = 0, \
													  elementCount:uint = 0, \
													  maxBoneInfluence:uint = 0):uint"),
													  annotate("as3package:alchemy") ) );

void SkinMeshProcessorGetSkinMeshController() __attribute__( (used,
															annotate("as3sig:public function skinMeshProcessorGetSkinMeshController(skinMeshProcessor:uint):uint"),
															annotate("as3package:alchemy") ) );

void SkinMeshControllerGetRootBone() __attribute__( (used,
												   annotate("as3sig:public function skinMeshControllerGetRootBone(skinMeshController:uint):uint"),
												   annotate("as3package:alchemy") ) );

void SkinMeshControllerGetHardwareSkinEffect() __attribute__( (used,
															 annotate("as3sig:public function skinMeshControllerGetHardwareSkinEffect(skinMeshController:uint):uint"),
															 annotate("as3package:alchemy") ) );

void CreateSkinInfo() __attribute__( (used,
									annotate("as3sig:public function createSkinInfo(boneDataPointer:uint, boneCount:uint):uint"),
									annotate("as3package:alchemy") ) );

void CreateAnimationSet() __attribute__( (used,
										annotate("as3sig:public function createAnimationSet(animationDataPointer:uint, animationCount:uint):uint"),
										annotate("as3package:alchemy") ) );

void CreateAnimationController() __attribute__( (used,
											   annotate("as3sig:public function createAnimationController():uint"),
											   annotate("as3package:alchemy") ) );

void AnimationControllerGetMaxPlayTime() __attribute__( (used,
													   annotate("as3sig:public function animationControllerGetMaxPlayTime(animationController:uint):Number"),
													   annotate("as3package:alchemy") ) );

void AnimationControllerSetPlayTime() __attribute__( (used,
													annotate("as3sig:public function animationControllerSetPlayTime(animationController:uint, playTime:Number):void"),
													annotate("as3package:alchemy") ) );

void AnimationControllerSetIsReplay() __attribute__( (used,
													annotate("as3sig:public function animationControllerSetIsReplay(animationController:uint, isReplay:Boolean):void"),
													annotate("as3package:alchemy") ) );

void AnimationControllerRegisterAnimationSet() __attribute__( (used,
															 annotate("as3sig:public function animationControllerRegisterAnimationSet(animationController:uint, animationSet:uint, weight:Number = 1.0, speed:Number = 1.0):void"),
															 annotate("as3package:alchemy") ) );

void AnimationControllerAdvanceTime() __attribute__( (used,
													annotate("as3sig:public function animationControllerAdvanceTime(animationController:uint, time:Number):void"),
													annotate("as3package:alchemy") ) );

void AnimationControllerUpdate() __attribute__( (used,
											   annotate("as3sig:public function animationControllerUpdate(animationController:uint, elapseTime:Number):void"),
											   annotate("as3package:alchemy") ) );

void CreateCascadShadowMapRender() __attribute__( (used,
												 annotate("as3sig:public function createCascadShadowMapRender( \
														  light:uint, \
														  splitCamerasPointer:uint, \
														  splitTexturesPointer:uint, \
														  depthSegmentPointer:uint, \
														  splitNum:uint = 4, \
														  frustumScale:Number = 1.0):uint"),
												 annotate("as3package:alchemy") ) );

void CascadShadowMapRenderAddShadowCaster() __attribute__( (used,
														  annotate("as3sig:public function cascadShadowMapRenderAddShadowCaster(cascadShadowMapRender:uint, sceneNode:uint, renderPass:uint):uint"),
														  annotate("as3package:alchemy") ) );

void CreateEffectCode() __attribute__( (used,
									  annotate("as3sig:public function createEffectCode(compiler:uint):uint"),
									  annotate("as3package:alchemy") ) );

void EffectCodeAddColorKey() __attribute__( (used,
										   annotate("as3sig:public function effectCodeAddColorKey(effectCode:uint, type:int = 16, resultColor:Boolean = false):uint"),
										   annotate("as3package:alchemy") ) );

void EffectCodeAddInfo() __attribute__( (used,
									   annotate("as3sig:public function effectCodeAddInfo(effectCode:uint, info:uint, blendMethod:uint = 0, inputColorKey:uint = 0, outputColorKey:uint = 0, sourceColorKey:Boolean = false):void"),
									   annotate("as3package:alchemy") ) );

void EffectCodeEndDefine() __attribute__( (used,
										 annotate("as3sig:public function effectCodeEndDefine(effectCode:uint):uint"),
										 annotate("as3package:alchemy") ) );

void CreatePositionEffect() __attribute__( (used,
										  annotate("as3sig:public function createPositionEffect():uint"),
										  annotate("as3package:alchemy") ) );

void CreatePixelColorEffect() __attribute__( (used,
											annotate("as3sig:public function createPixelColorEffect(colorPointer:uint):uint"),
											annotate("as3package:alchemy") ) );

void CreateTextureEffect() __attribute__( (used,
										 annotate("as3sig:public function createTextureEffect(samplerIndex:uint = 0, texcoordIndex:uint = 0, samplerDeclarationPointer:uint = 0):uint"),
										 annotate("as3package:alchemy") ) );

void CreateEmptyEffect() __attribute__( (used,
									   annotate("as3sig:public function createEmptyEffect():uint"),
									   annotate("as3package:alchemy") ) );

void CreateEnvironmentalEffect()  __attribute__( (used,
												annotate("as3sig:public function createEnvironmentalEffect():uint"),
												annotate("as3package:alchemy") ) );

void CreateNormalMapEffect()   __attribute__( (used,
											 annotate("as3sig:public function createNormalMapEffect(normalMapUIIndex:uint, normalMapIndex:uint, numLights:uint, lightIndicesPointer:uint):uint"),
											 annotate("as3package:alchemy") ) );

void CreateVertexColorEffect() __attribute__( (used,
											 annotate("as3sig:public function createVertexColorEffect(index:uint):uint"),
											 annotate("as3package:alchemy") ) );

void CreateCascadShadowDisplayEffect()  __attribute__( (used,
													  annotate("as3sig:public function createCascadShadowDisplayEffect( \
															   samplerIndicesPointer:uint, \
															   depthSegmentPointer:uint, \
															   spilitCameraIndicesPointer:uint, \
															   samplerCount:uint, \
															   lightIndex:uint):uint"),
															   annotate("as3package:alchemy") ) );

void CreateSkyPositionEffect() __attribute__( (used,
											 annotate("as3sig:public function createSkyPositionEffect():uint"),
											 annotate("as3package:alchemy") ) );

void CreateInstancePositionEffect() __attribute__( (used,
												  annotate("as3sig:public function createInstancePositionEffect(instanceCount:uint, matricesPointer:uint = 0):uint"),
												  annotate("as3package:alchemy") ) );

void CreateInstancePosition3Scale1Effect() __attribute__( (used,
														 annotate("as3sig:public function createInstancePosition3Scale1Effect(instanceCount:uint, transformPointer:uint = 0):uint"),
														 annotate("as3package:alchemy") ) );

void CreateEffectInfoAddition() __attribute__( (used,
											  annotate("as3sig:public function createEffectInfoAddition(blendFlag:int = 2):uint"),
											  annotate("as3package:alchemy") ) );

void CreateEffectInfoModulation() __attribute__( (used,
												annotate("as3sig:public function createEffectInfoModulation(blendFlag:int = 2):uint"),
												annotate("as3package:alchemy") ) );

void CreateAlphatTestBlendMethod() __attribute__( (used,
												 annotate("as3sig:public function createAlphatTestBlendMethod(refPointer:uint = 0):uint"),
												 annotate("as3package:alchemy") ) );

void CreatePixelFogBlendMethod() __attribute__( (used,
											   annotate("as3sig:public function createPixelFogBlendMethod(fogType:int = 0, endPointer:uint = 0, distancePointer:uint = 0, densityPointer:uint = 0):uint"),
											   annotate("as3package:alchemy") ) );

void CreateAxisAlignedBoundingBox() __attribute__( (used,
												  annotate("as3sig:public function createAxisAlignedBoundingBox():uint"),
												  annotate("as3package:alchemy") ) );

void GetDataPointer()
{
	AS3_Return(g_pData);
}

void Cast()
{
	void* pPointer;

	{
		AS3_GetScalarFromVar(pPointer, pointer);
	}

	if(pPointer)
	{
		INT nFrom, nTo;

		{
			AS3_GetScalarFromVar(nFrom, from);
		}

		{
			AS3_GetScalarFromVar(nTo, to);
		}

		switch(nFrom)
		{
		case CAST_SCENE_NODE:
			switch(nTo)
			{
			case CAST_TRANSFORM:
				pPointer = static_cast<CTransform*>( reinterpret_cast<CSceneNode*>(pPointer) );
				break;
			case CAST_SCENE_NODE:
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;

		case CAST_BONE:
			switch(nTo)
			{
			case CAST_TRANSFORM:
				pPointer = static_cast<CTransform*>( reinterpret_cast<CBone*>(pPointer) );
				break;
			case CAST_SCENE_NODE:
				pPointer = static_cast<CSceneNode*>( reinterpret_cast<CBone*>(pPointer) );
				break;
			case CAST_BONE:
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;

		case CAST_SCENE_NODE_UPDATER:
			switch(nTo)
			{
			case CAST_TRANSFORM:
				pPointer = static_cast<CTransform*>( reinterpret_cast<CSceneNodeUpdater*>(pPointer) );
				break;
			case CAST_SCENE_NODE:
				pPointer = static_cast<CSceneNode*>( reinterpret_cast<CSceneNodeUpdater*>(pPointer) );
				break;
			case CAST_SCENE_NODE_UPDATER:
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;

		case CAST_SCENE_OBJECT:
			switch(nTo)
			{
			case CAST_TRANSFORM:
				pPointer = static_cast<CTransform*>( reinterpret_cast<CStaticObject*>(pPointer) );
				break;
			case CAST_SCENE_NODE:
				pPointer = static_cast<CSceneNode*>( reinterpret_cast<CStaticObject*>(pPointer) );
				break;
			case CAST_SCENE_OBJECT:
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;

		case CAST_CAMERA:
			switch(nTo)
			{
			case CAST_TRANSFORM:
				pPointer = static_cast<CTransform*>( reinterpret_cast<CCamera*>(pPointer) );
				break;
			case CAST_SCENE_NODE:
				pPointer = static_cast<CSceneNode*>( reinterpret_cast<CCamera*>(pPointer) );
				break;
			case CAST_CAMERA:
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;

		case CAST_SKY_DOME:
			switch(nTo)
			{
			case CAST_TRANSFORM:
				pPointer = static_cast<CTransform*>( reinterpret_cast<CSkyDome*>(pPointer) );
				break;
			case CAST_SCENE_OBJECT:
				pPointer = static_cast<CStaticObject*>( reinterpret_cast<CSkyDome*>(pPointer) );
				break;
			case CAST_SCENE_NODE:
				pPointer = static_cast<CSceneNode*>( reinterpret_cast<CSkyDome*>(pPointer) );
				break;
			case CAST_SKY_DOME:
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;

		case CAST_GRASS_SYSTEM:
			switch(nTo)
			{
			case CAST_TRANSFORM:
				pPointer = static_cast<CTransform*>( reinterpret_cast<CGrassSystem*>(pPointer) );
				break;
			case CAST_SCENE_NODE:
				pPointer = static_cast<CSceneNode*>( reinterpret_cast<CGrassSystem*>(pPointer) );
				break;
			case CAST_GRASS_SYSTEM:
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;

		case CAST_GEOMETRY_INSTANCE:
			switch(nTo)
			{
			case CAST_TRANSFORM:
				pPointer = static_cast<CTransform*>( reinterpret_cast<CGeometryInstance*>(pPointer) );
				break;
			case CAST_SCENE_OBJECT:
				pPointer = static_cast<CStaticObject*>( reinterpret_cast<CGeometryInstance*>(pPointer) );
				break;
			case CAST_SCENE_NODE:
				pPointer = static_cast<CSceneNode*>( reinterpret_cast<CGeometryInstance*>(pPointer) );
				break;
			case CAST_GEOMETRY_INSTANCE:
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;

		case CAST_MESH_PROCESSOR:
			switch(nTo)
			{
			case CAST_SKIN_MESH_PROCESSOR:
				pPointer = dynamic_cast<CSkinMeshProcessor*>( reinterpret_cast<IMeshProcessor*>(pPointer) );
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;

		case CAST_SKIN_MESH_PROCESSOR:
			switch(nTo)
			{
			case CAST_UPDATER:
				pPointer = static_cast<IUpdater*>( reinterpret_cast<CSkinMeshProcessor*>(pPointer) );
				break;
			case CAST_MESH_PROCESSOR:
				pPointer = static_cast<IMeshProcessor*>( reinterpret_cast<CSkinMeshProcessor*>(pPointer) );
				break;
			case CAST_SKIN_MESH_PROCESSOR:
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;

		case CAST_ANIMATION_CONTROLLER:
			switch(nTo)
			{
			case CAST_UPDATER:
				pPointer = static_cast<IUpdater*>( reinterpret_cast<CAnimationController*>(pPointer) );
				break;
			case CAST_ANIMATION_CONTROLLER:
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;

		case CAST_CASCAD_SHADOW_MAP_RENDER:
			switch(nTo)
			{
			case CAST_RENDER_GROUP:
				pPointer = static_cast<IRenderGroup*>( reinterpret_cast<CCascadShadowMapRender*>(pPointer) );
				break;
			case CAST_CASCAD_SHADOW_MAP_RENDER:
				break;
			default:
				pPointer = ALCHEMY_NULL;
				break;
			}
			break;
		}
	}

	AS3_Return(pPointer);
}

void GraphicsManagerCreateRenderSystem()
{
	double x, y, dbWidth, dbHeight;

	{
		AS3_GetScalarFromVar(x, x);
	}

	{
		AS3_GetScalarFromVar(y, y);
	}

	{
		AS3_GetScalarFromVar(dbWidth, width);
	}

	{
		AS3_GetScalarFromVar(dbHeight, height);
	}

	CGraphicsManager& GraphicsManager = ALCHEMY_GRAPHICS_MANAGER;

	UINT uType;
	if( ALCHEMY_GET_EVENT_TYPE(uType, RENDER_SYSTEM_CREATED, CGraphicsManager) )
	{
		IWindow* pWindow = GraphicsManager.CreateWindow(x, y, dbWidth, dbHeight);
		if(pWindow)
		{
			pWindow->AttachToScreen(true);

			GraphicsManager.AddEventListener(uType, g_RenderSystemHandler);
			GraphicsManager.CreateRenderSystem(pWindow);
		}
	}
}

void GraphicsManagerCreateSceneManager()
{
	CRenderSystem* pRenderSystem;

	{
		AS3_GetScalarFromVar(pRenderSystem, renderSystem);
	}

	if(pRenderSystem)
	{
		AS3_Return( ALCHEMY_GRAPHICS_MANAGER.CreateSceneManager(*pRenderSystem) );
	}

	AS3_Return(0);
}

void MeshManagerCreateMeshDataPlane()
{
	double dbWidth, dbHeight;

	{
		AS3_GetScalarFromVar(dbWidth, width);
	}

	{
		AS3_GetScalarFromVar(dbHeight, height);
	}

	if(dbWidth && dbHeight)
	{
		UINT uWidthSegment;

		{
			AS3_GetScalarFromVar(uWidthSegment, widthSegment);
		}

		UINT uHeightSegment;

		{
			AS3_GetScalarFromVar(uHeightSegment, heightSegment);
		}

		UINT uWidthStep;

		{
			AS3_GetScalarFromVar(uWidthStep, widthStep);
		}

		UINT uHeightStep;

		{
			AS3_GetScalarFromVar(uHeightStep, heightStep);
		}

		bool bIsYUp;

		{
			AS3_GetScalarFromVar(bIsYUp, yUp);
		}

		CVertexDeclaration::VERTEXELEMENT* pVertexElements;

		{
			AS3_GetScalarFromVar(pVertexElements, vertexElementsPointer);
		}

		UINT uElementCount;

		{
			AS3_GetScalarFromVar(uElementCount, elementCount);
		}

		LPFLOAT3 pPositionOffset;
		{
			AS3_GetScalarFromVar(pPositionOffset, positionOffset);
		}

		PUINT puVertexUsage;

		{
			AS3_GetScalarFromVar(puVertexUsage, vertexUsagePointer);
		}

		UINT uIndexUsage;

		{
			AS3_GetScalarFromVar(uIndexUsage, indexUsage);
		}

		AS3_Return( ALCHEMY_MESH_MANAGER.CreatePlane(dbWidth, dbHeight, uWidthSegment, uHeightSegment, uWidthStep, uHeightStep, bIsYUp, pVertexElements, uElementCount, pPositionOffset, puVertexUsage, uIndexUsage) );
	}
}

void MeshManagerAddMeshData()
{
	CMeshManager::MESHDATA* pMeshData;

	{
		AS3_GetScalarFromVar(pMeshData, meshDataPointer);
	}

	if(pMeshData)
		AS3_Return( static_cast<UINT>( ALCHEMY_MESH_MANAGER.AddMeshData(*pMeshData) ) );

	AS3_Return(0);
}

void RenderSystemGetCompiler()
{
	CRenderSystem* pRenderSystem;

	{
		AS3_GetScalarFromVar(pRenderSystem, renderSystem);
	}

	AS3_Return( &pRenderSystem->GetCompiler() );
}

void RenderSystemCreateEffectFromEffectCode()
{
	CRenderSystem* pRenderSystem;

	{
		AS3_GetScalarFromVar(pRenderSystem, renderSystem);
	}

	if(pRenderSystem)
	{
		CEffectCode::CPassDefiner* pPassDefiner;

		{
			AS3_GetScalarFromVar(pPassDefiner, effectCode);
		}

		if(pPassDefiner)
			AS3_Return( pRenderSystem->CreateEffect( pPassDefiner->GetParent() ) );
	}

	AS3_Return(0);
}

void RenderSystemCreateTexture()
{
	CRenderSystem* pRenderSystem;

	{
		AS3_GetScalarFromVar(pRenderSystem, renderSystem);
	}

	if(pRenderSystem)
	{
		UINT uWidth, uHeight, uUsage, uMipLevel;
		CTexture::FORMAT Format;
		void* pData;

		{
			AS3_GetScalarFromVar(uWidth, width);
		}

		{
			AS3_GetScalarFromVar(uHeight, height);
		}

		{
			AS3_GetScalarFromVar(uUsage, usage);
		}

		{
			AS3_GetScalarFromVar(Format, format);
		}

		{
			AS3_GetScalarFromVar(uMipLevel, mipLevel);
		}

		{
			AS3_GetScalarFromVar(pData, dataPointer);
		}

		AS3_Return( pRenderSystem->CreateTexture(uWidth, uHeight, uUsage, Format, uMipLevel, pData) );
	}

	AS3_Return(0);
}

void RenderSystemCreateCubeTexture()
{
	CRenderSystem* pRenderSystem;

	{
		AS3_GetScalarFromVar(pRenderSystem, renderSystem);
	}

	if(pRenderSystem)
	{
		UINT uSize, uUsage, uMipLevel;
		CTexture::FORMAT Format;
		const void* pPositiveXData, * pNegativeXData, * pPositiveYData, * pNegativeYData, * pPositiveZData, * pNegativeZData;

		{
			AS3_GetScalarFromVar(uSize, size);
		}

		{
			AS3_GetScalarFromVar(uUsage, usage);
		}

		{
			AS3_GetScalarFromVar(Format, format);
		}

		{
			AS3_GetScalarFromVar(uMipLevel, mipLevel);
		}

		{
			AS3_GetScalarFromVar(pPositiveXData, positiveXDataPointer);
		}

		{
			AS3_GetScalarFromVar(pNegativeXData, negativeXDataPointer);
		}

		{
			AS3_GetScalarFromVar(pPositiveYData, positiveYDataPointer);
		}

		{
			AS3_GetScalarFromVar(pNegativeYData, negativeYDataPointer);
		}

		{
			AS3_GetScalarFromVar(pPositiveZData, positiveZDataPointer);
		}

		{
			AS3_GetScalarFromVar(pNegativeZData, negativeZDataPointer);
		}

		AS3_Return( pRenderSystem->CreateCubeTexture(uSize, uUsage, Format, uMipLevel, pPositiveXData, pNegativeXData, pPositiveYData, pNegativeYData, pPositiveZData, pNegativeZData) );
	}

	AS3_Return(0);
}

void RenderSystemSetViewport()
{
	CRenderSystem* pRenderSystem;

	{
		AS3_GetScalarFromVar(pRenderSystem, renderSystem);
	}

	if(pRenderSystem)
	{
		UINT uWidth, uHeight;
		INT nAntiAlias;

		{
			AS3_GetScalarFromVar(uWidth, width);
		}

		{
			AS3_GetScalarFromVar(uHeight, height);
		}

		{
			AS3_GetScalarFromVar(nAntiAlias, antiAlias);
		}

		static_cast<CFlasccContext3D*>(pRenderSystem)->SetViewport(uWidth, uHeight, nAntiAlias);
	}
}

void RenderSystemClear()
{
	CRenderSystem* pRenderSystem;
	UINT uColor;
	UINT uFlag;

	{
		AS3_GetScalarFromVar(pRenderSystem, renderSystem);
	}

	if(pRenderSystem)
	{
		{
			AS3_GetScalarFromVar(uColor, color);
		}
		
		{
			AS3_GetScalarFromVar(uFlag, flag);
		}

		pRenderSystem->Clear(uColor, uFlag);
	}
}

void RenderSystemPresent()
{
	CRenderSystem* pRenderSystem;

	{
		AS3_GetScalarFromVar(pRenderSystem, renderSystem);
	}

	if(pRenderSystem)
		pRenderSystem->Present();
}

void SceneManagerGetRenderSystem()
{
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	if(pSceneManager)
		AS3_Return( &pSceneManager->GetRenderSystem() );

	AS3_Return(0);
}

void SceneManagerGetSceneCamera()
{
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	if(pSceneManager)
		AS3_Return( &pSceneManager->GetSceneCamera() );

	AS3_Return(0);
}

void SceneManagerSetBackgroundColor()
{
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	if(pSceneManager)
	{
		UINT uColor;

		{
			AS3_GetScalarFromVar(uColor, color);
		}

		pSceneManager->SetBackgroundColor(uColor);
	}
}

void SceneManagerCreateMesh()
{
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	if(pSceneManager)
	{
		UINT uMeshData;
		{
			AS3_GetScalarFromVar(uMeshData, meshData);
		}

		AS3_Return( pSceneManager->CreateMesh(uMeshData) );
	}

	AS3_Return(0);
}

void SceneManagerCreateMaterial()
{
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	if(pSceneManager)
	{
		UINT uMaterialData;
		CMap<CMeshManager::HANDLE, CGraphicsResource::HANDLE>* pTextureCache;
		CVector<CSceneManager::RESOURCEEINFO>* pTextureInfo;

		{
			AS3_GetScalarFromVar(uMaterialData, materialData);
		}

		{
			AS3_GetScalarFromVar(pTextureCache, textureCachePointer);
		}

		{
			AS3_GetScalarFromVar(pTextureInfo, textureInfoPointer);
		}

		AS3_Return( pSceneManager->CreateMaterial(uMaterialData, pTextureCache, pTextureInfo) );
	}

	AS3_Return(0);
}

void SceneManagerCreateLight()
{
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	if(pSceneManager)
		AS3_Return( pSceneManager->CreateLight() );

	AS3_Return(0);
}

void SceneManagerCreateRenderMethodFromEffectCode()
{
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	if(pSceneManager)
	{
		CEffectCode::CPassDefiner* pPassDefiner;

		{
			AS3_GetScalarFromVar(pPassDefiner, effectCode);
		}

		if(pPassDefiner)
		{
			CEffectCode::CGlobalInputHandler* pShaderParametersHandler;

			{
				AS3_GetScalarFromVar(pShaderParametersHandler, shaderParametersHandler);
			}

			CRenderMethod::IRenderStateHandler* pRenderStateHandler;

			{
				AS3_GetScalarFromVar(pRenderStateHandler, renderStateHandler);
			}

			AS3_Return( static_cast<IRenderMethod*>( pSceneManager->CreateRenderMethod(pPassDefiner->GetParent(), pShaderParametersHandler, pRenderStateHandler) ) );
		}
	}

	AS3_Return(0);
}

void SceneManagerAddChild()
{
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	if(pSceneManager)
	{
		CSceneNode* pSceneNode;

		{
			AS3_GetScalarFromVar(pSceneNode, sceneNode);
		}

		if(pSceneNode)
			pSceneManager->AddChild(*pSceneNode);
	}
}

void SceneManagerAddRenderGroup()
{
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	if(pSceneManager)
	{
		IRenderGroup* pRenderGroup;

		{
			AS3_GetScalarFromVar(pRenderGroup, renderGroup);
		}

		if(pRenderGroup)
			pSceneManager->AddRenderGroup(*pRenderGroup);
	}
}

void SceneManagerUpdate()
{
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	if(pSceneManager)
	{
		double dbElapseTime;

		{
			AS3_GetScalarFromVar(dbElapseTime, elapseTime);
		}

		pSceneManager->Update( static_cast<FLOAT>(dbElapseTime) );
	}
}

void SceneManagerRender()
{
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	if(pSceneManager)
		pSceneManager->Render();
}

void MeshComputeTangent()
{
	CMesh* pMesh;

	{
		AS3_GetScalarFromVar(pMesh, mesh);
	}

	if(pMesh)
	{
		UINT uTexcoordIndex;

		{
			AS3_GetScalarFromVar(uTexcoordIndex, texcoordIndex);
		}

		pMesh->ComputeTangent(uTexcoordIndex);
	}
}

void MaterialSetAmibient()
{
	CMaterial* pMaterial;

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	if(pMaterial)
	{
		double r, g, b, a;

		{
			AS3_GetScalarFromVar(r, r);
		}

		{
			AS3_GetScalarFromVar(g, g);
		}

		{
			AS3_GetScalarFromVar(b, b);
		}

		{
			AS3_GetScalarFromVar(a, a);
		}

		pMaterial->SetAmbient(r, g, b, a);
	}
}

void MaterialSetDiffuse()
{
	CMaterial* pMaterial;

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	if(pMaterial)
	{
		double r, g, b;

		{
			AS3_GetScalarFromVar(r, r);
		}

		{
			AS3_GetScalarFromVar(g, g);
		}

		{
			AS3_GetScalarFromVar(b, b);
		}

		pMaterial->SetDiffuse(r, g, b);
	}
}

void MaterialSetSpecular()
{
	CMaterial* pMaterial;

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	if(pMaterial)
	{
		double r, g, b;

		{
			AS3_GetScalarFromVar(r, r);
		}

		{
			AS3_GetScalarFromVar(g, g);
		}

		{
			AS3_GetScalarFromVar(b, b);
		}

		pMaterial->SetSpecular(r, g, b);
	}
}

void MaterialSetEmissive()
{
	CMaterial* pMaterial;

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	if(pMaterial)
	{
		double r, g, b;

		{
			AS3_GetScalarFromVar(r, r);
		}

		{
			AS3_GetScalarFromVar(g, g);
		}

		{
			AS3_GetScalarFromVar(b, b);
		}

		pMaterial->SetEmissive(r, g, b);
	}
}

void MaterialSetPower()
{
	CMaterial* pMaterial;

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	if(pMaterial)
	{
		double dbPower;

		{
			AS3_GetScalarFromVar(dbPower, power);
		}

		pMaterial->SetPower(dbPower);
	}
}

void MaterialAddTexture()
{
	CMaterial* pMaterial;

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	if(pMaterial)
	{
		LPTEXTURE pTexture;

		{
			AS3_GetScalarFromVar(pTexture, texture);
		}

		AS3_Return( pMaterial->AddTexture( pTexture->GetResourceHandle() ) );
	}

	AS3_Return(0);
}

void MaterialAddLight()
{
	CMaterial* pMaterial;
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pMaterial && pLight)
	{
		CLightManager::SHADEMODE ShadeMode;

		{
			AS3_GetScalarFromVar(ShadeMode, shadeMode);
		}

		AS3_Return( pMaterial->AddLight(pLight->GetResourceHandle(), ShadeMode) );
	}

	AS3_Return(0);
}

void MaterialAddCamera()
{
	CMaterial* pMaterial;
	CCamera* pCamera;

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	{
		AS3_GetScalarFromVar(pCamera, camera);
	}

	if(pMaterial && pCamera)
		AS3_Return( pMaterial->AddCamera(*pCamera) );

	AS3_Return(0);
}

void MaterialAddLightEffectInfoToPass()
{
	CMaterial* pMaterial;
	CEffectCode::CPassDefiner* pPassDefiner;

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	{
		AS3_GetScalarFromVar(pPassDefiner, passDefiner);
	}

	if(pMaterial && pPassDefiner)
	{
		IEffectInfoBlendMethod* pEffectInfoBlendMethod;

		{
			AS3_GetScalarFromVar(pEffectInfoBlendMethod, effectInfoBlendMethod);
		}

		IEffectInfo* pSpecularEffect;

		{
			AS3_GetScalarFromVar(pSpecularEffect, specularEffect);
		}

		IEffectInfo* pNormalMapEffect;

		{
			AS3_GetScalarFromVar(pNormalMapEffect, normalMapEffect);
		}

		UINT uOutputKey;

		{
			AS3_GetScalarFromVar(uOutputKey, outputKey);
		}

		UINT uInputKey;

		{
			AS3_GetScalarFromVar(uInputKey, inputKey);
		}

		pMaterial->AddLightEffectInfoToPass(*pPassDefiner, pEffectInfoBlendMethod, pSpecularEffect, pNormalMapEffect, uOutputKey, uInputKey);
	}
}

void LightSetType()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		LIGHTTYPE Type;

		AS3_GetScalarFromVar(Type, type);

		pLight->Type = Type;
	}
}

void LightSetAmibient()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double r, g, b, a;

		{
			AS3_GetScalarFromVar(r, r);
		}

		{
			AS3_GetScalarFromVar(g, g);
		}

		{
			AS3_GetScalarFromVar(b, b);
		}

		{
			AS3_GetScalarFromVar(a, a);
		}

		pLight->Ambient.r = r;
		pLight->Ambient.g = g;
		pLight->Ambient.b = b;
		pLight->Ambient.a = a;
	}
}

void LightSetDiffuse()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double r, g, b;

		{
			AS3_GetScalarFromVar(r, r);
		}

		{
			AS3_GetScalarFromVar(g, g);
		}

		{
			AS3_GetScalarFromVar(b, b);
		}

		pLight->Diffuse.r = r;
		pLight->Diffuse.g = g;
		pLight->Diffuse.b = b;
	}
}

void LightSetSpecular()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double r, g, b;

		{
			AS3_GetScalarFromVar(r, r);
		}

		{
			AS3_GetScalarFromVar(g, g);
		}

		{
			AS3_GetScalarFromVar(b, b);
		}

		pLight->Specular.r = r;
		pLight->Specular.g = g;
		pLight->Specular.b = b;
	}
}

void LightSetPosition()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double x, y, z;

		{
			AS3_GetScalarFromVar(x, x);
		}

		{
			AS3_GetScalarFromVar(y, y);
		}

		{
			AS3_GetScalarFromVar(z, z);
		}

		pLight->Position.x = x;
		pLight->Position.y = y;
		pLight->Position.z = z;
	}
}

void LightSetDirection()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double x, y, z;

		{
			AS3_GetScalarFromVar(x, x);
		}

		{
			AS3_GetScalarFromVar(y, y);
		}

		{
			AS3_GetScalarFromVar(z, z);
		}

		pLight->Direction.x = x;
		pLight->Direction.y = y;
		pLight->Direction.z = z;
	}
}

void LightSetRange()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double dbRange;

		{
			AS3_GetScalarFromVar(dbRange, range);
		}

		pLight->fRange = dbRange;
	}
}

void LightSetFalloff()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double dbFalloff;

		{
			AS3_GetScalarFromVar(dbFalloff, falloff);
		}

		pLight->fFalloff = dbFalloff;
	}
}

void LightSetAttenuation0()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double dbAttenuation0;

		{
			AS3_GetScalarFromVar(dbAttenuation0, attenuation0);
		}

		pLight->fAttenuation0 = dbAttenuation0;
	}
}

void LightSetAttenuation1()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double dbAttenuation1;

		{
			AS3_GetScalarFromVar(dbAttenuation1, attenuation1);
		}

		pLight->fAttenuation1 = dbAttenuation1;
	}
}

void LightSetAttenuation2()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double dbAttenuation2;

		{
			AS3_GetScalarFromVar(dbAttenuation2, attenuation2);
		}

		pLight->fAttenuation2 = dbAttenuation2;
	}
}

void LightSetTheta()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double dbTheta;

		{
			AS3_GetScalarFromVar(dbTheta, theta);
		}

		pLight->fTheta = dbTheta;
	}
}

void LightSetPhi()
{
	CLight* pLight;

	{
		AS3_GetScalarFromVar(pLight, light);
	}

	if(pLight)
	{
		double dbPhi;

		{
			AS3_GetScalarFromVar(dbPhi, phi);
		}

		pLight->fPhi = dbPhi;
	}
}

void CreateRenderStateHandler()
{
	IRenderState::RENDERSTATEINFO* pRenderStateInfo;

	{
		AS3_GetScalarFromVar(pRenderStateInfo, renderStateInfosPointer);
	}

	UINT uCount;

	{
		AS3_GetScalarFromVar(uCount, count);
	}

	if(pRenderStateInfo && uCount)
	{
		CRenderMethod::CRenderStateHandler* pRenderStateHandler;
		ALCHEMY_DEBUG_NEW(pRenderStateHandler, CRenderMethod::CRenderStateHandler);

		if( pRenderStateHandler->Create(pRenderStateInfo, uCount) )
			AS3_Return( static_cast<CRenderMethod::IRenderStateHandler*>(pRenderStateHandler) );
	}

	AS3_Return(0);
}

void TransformGetRotation()
{
	CTransform* pTransform;

	{
		AS3_GetScalarFromVar(pTransform, transform);
	}

	if(pTransform)
		AS3_Return( &pTransform->GetRotation() );

	AS3_Return(0);
}

void TransformGetTranslation()
{
	CTransform* pTransform;

	{
		AS3_GetScalarFromVar(pTransform, transform);
	}

	if(pTransform)
		AS3_Return( &pTransform->GetTranslation() );

	AS3_Return(0);
}

void TransformSetScale()
{
	CTransform* pTransform;

	{
		AS3_GetScalarFromVar(pTransform, transform);
	}

	if(pTransform)
	{
		double x, y, z;

		{
			AS3_GetScalarFromVar(x, x);
		}

		{
			AS3_GetScalarFromVar(y, y);
		}

		{
			AS3_GetScalarFromVar(z, z);
		}

		pTransform->SetScale(x, y, z);
	}
}

void TransformSetRotation()
{
	CTransform* pTransform;

	{
		AS3_GetScalarFromVar(pTransform, transform);
	}

	if(pTransform)
	{
		double x, y, z;

		{
			AS3_GetScalarFromVar(x, x);
		}

		{
			AS3_GetScalarFromVar(y, y);
		}

		{
			AS3_GetScalarFromVar(z, z);
		}

		pTransform->SetRotation(x, y, z);
	}
}

void TransformSetTranslation()
{
	CTransform* pTransform;

	{
		AS3_GetScalarFromVar(pTransform, transform);
	}

	if(pTransform)
	{
		double x, y, z;

		{
			AS3_GetScalarFromVar(x, x);
		}

		{
			AS3_GetScalarFromVar(y, y);
		}

		{
			AS3_GetScalarFromVar(z, z);
		}

		pTransform->SetTranslation(x, y, z);
	}
}

void TransformSetDirection()
{
	CTransform* pTransform;

	{
		AS3_GetScalarFromVar(pTransform, transform);
	}

	if(pTransform)
	{
		double x, y, z;

		{
			AS3_GetScalarFromVar(x, x);
		}

		{
			AS3_GetScalarFromVar(y, y);
		}

		{
			AS3_GetScalarFromVar(z, z);
		}

		pTransform->SetDirection(x, y, z);
	}
}

void TransformForward()
{
	CTransform* pTransform;

	{
		AS3_GetScalarFromVar(pTransform, transform);
	}

	if(pTransform)
	{
		double dbNumSteps;

		{
			AS3_GetScalarFromVar(dbNumSteps, numSteps);
		}

		pTransform->Forward(dbNumSteps);
	}
}

void TransformUp()
{
	CTransform* pTransform;

	{
		AS3_GetScalarFromVar(pTransform, transform);
	}

	if(pTransform)
	{
		double dbNumSteps;

		{
			AS3_GetScalarFromVar(dbNumSteps, numSteps);
		}

		pTransform->Up(dbNumSteps);
	}
}

void TransformRight()
{
	CTransform* pTransform;

	{
		AS3_GetScalarFromVar(pTransform, transform);
	}

	if(pTransform)
	{
		double dbNumSteps;

		{
			AS3_GetScalarFromVar(dbNumSteps, numSteps);
		}

		pTransform->Right(dbNumSteps);
	}
}


void TransformSetMatrix()
{
	CTransform* pTransform;

	{
		AS3_GetScalarFromVar(pTransform, transform);
	}

	MATRIX4X4* pMatrix;

	{
		AS3_GetScalarFromVar(pMatrix, matrix);
	}

	if(pTransform && pMatrix)
		pTransform->SetMatrix(*pMatrix);
}

void CreateSceneNode()
{
	CSceneNode* pSceneNode;

	ALCHEMY_DEBUG_NEW(pSceneNode, CSceneNode);

	AS3_Return(pSceneNode);
}

void SceneNodeSetName()
{
	CSceneNode* pSceneNode;

	{
		AS3_GetScalarFromVar(pSceneNode, sceneNode);
	}

	CHAR* pcName;

	{
		AS3_GetScalarFromVar(pcName, name);
	}

	if(pSceneNode && pcName)
		pSceneNode->SetName(pcName);
}

void SceneNodeSetVisible()
{
	CSceneNode* pSceneNode;

	{
		AS3_GetScalarFromVar(pSceneNode, sceneNode);
	}

	if(pSceneNode)
	{
		bool bValue;

		{
			AS3_GetScalarFromVar(bValue, value);
		}

		pSceneNode->SetVisible(bValue);
	}
}

void SceneNodeGetLocalMatrix()
{
	CSceneNode* pSceneNode;

	{
		AS3_GetScalarFromVar(pSceneNode, sceneNode);
	}

	if(pSceneNode)
		AS3_Return( &pSceneNode->GetLocalMatrix() );

	AS3_Return(0);
}

void SceneNodeGetChild()
{
	CSceneNode* pSceneNode;

	{
		AS3_GetScalarFromVar(pSceneNode, sceneNode);
	}

	const CHAR* pcName;

	{
		AS3_GetScalarFromVar(pcName, name);
	}

	if(pSceneNode && pcName)
		AS3_Return( pSceneNode->GetChild(pcName) );

	AS3_Return(0);
}

void SceneNodeAddChild()
{
	CSceneNode* pSceneNode;

	{
		AS3_GetScalarFromVar(pSceneNode, sceneNode);
	}

	CSceneNode* pChild;

	{
		AS3_GetScalarFromVar(pChild, child);
	}

	if(pSceneNode && pChild)
		pSceneNode->AddChild(*pChild);
}

void SceneNodeIntersectTree()
{
	CSceneNode* pSceneNode;

	{
		AS3_GetScalarFromVar(pSceneNode, sceneNode);
	}

	LPRAY pRay;
	{
		AS3_GetScalarFromVar(pRay, ray);
	}

	if(pSceneNode && pRay)
	{
		CMesh::LPINTERSECTINFO pIntersectInfos;

		{
			AS3_GetScalarFromVar(pIntersectInfos, intersectInfosPointer);
		}

		PUINT puIntersectInfoCount;

		{
			AS3_GetScalarFromVar(puIntersectInfoCount, intersectInfoCountPointer);
		}

		bool bIsCheckHitOn, bHitOnMask, bIsCheckFront, bFrontMask;
		LPFLOAT3 pHitPoints, pfFaceNormals;

		{
			AS3_GetScalarFromVar(bIsCheckHitOn, checkHitOn);
		}

		{
			AS3_GetScalarFromVar(bHitOnMask, hitOnMask);
		}

		{
			AS3_GetScalarFromVar(bIsCheckFront, checkFront);
		}

		{
			AS3_GetScalarFromVar(bFrontMask, frontMask);
		}

		{
			AS3_GetScalarFromVar(pHitPoints, hitPointsPointer);
		}

		{
			AS3_GetScalarFromVar(pfFaceNormals, faceNormalsPointer);
		}

		AS3_Return( pSceneNode->IntersectTree(*pRay, pIntersectInfos, puIntersectInfoCount, bIsCheckHitOn, bHitOnMask, bIsCheckFront, bFrontMask, pHitPoints, pfFaceNormals) );
	}

	AS3_Return(0);
}

void SceneNodeCloneTree()
{
	CSceneNode* pSceneNode;

	{
		AS3_GetScalarFromVar(pSceneNode, sceneNode);
	}

	if(pSceneNode)
		AS3_Return( &pSceneNode->CloneTree(g_SceneNodeFactory) );

	AS3_Return(0);
}

void SceneNodeUpdate()
{
	CSceneNode* pSceneNode;

	{
		AS3_GetScalarFromVar(pSceneNode, sceneNode);
	}

	if(pSceneNode)
		pSceneNode->Update();
}

void CreateBone()
{
	CBone* pBone;

	ALCHEMY_DEBUG_NEW(pBone, CBone);

	AS3_Return(pBone);
}

void BoneRegisterAnimationController()
{
	CBone* pBone;
	CAnimationController* pAnimationController;

	{
		AS3_GetScalarFromVar(pBone, bone);
	}

	{
		AS3_GetScalarFromVar(pAnimationController, animationController);
	}

	if(pBone && pAnimationController)
		pBone->RegisterAnimationController(*pAnimationController);
}

void CreateSceneNodeUpdater()
{
	CSceneNodeUpdater* pSceneNodeUpdater;

	ALCHEMY_DEBUG_NEW(pSceneNodeUpdater, CSceneNodeUpdater);

	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	pSceneNodeUpdater->Create(pSceneManager);

	AS3_Return(pSceneNodeUpdater);
}

void SceneNodeUpdaterAddUpdater()
{
	CSceneNodeUpdater* pSceneNodeUpdater;

	{
		AS3_GetScalarFromVar(pSceneNodeUpdater, sceneNodeUpdater);
	}

	IUpdater* pUpdater;

	{
		AS3_GetScalarFromVar(pUpdater, updater);
	}

	if(pSceneNodeUpdater && pUpdater)
		pSceneNodeUpdater->AddUpdater(*pUpdater);
}

void CreateCamera()
{
	CCamera* pCamera;
	ALCHEMY_DEBUG_NEW(pCamera, CCamera);

	AS3_Return(pCamera);
}

void CameraSetProjection()
{
	CCamera* pCamera;

	{
		AS3_GetScalarFromVar(pCamera, camera);
	}

	if(pCamera)
	{
		double dbFov, dbNearPlane, dbFarPlane, dbAspect;
		bool bIsLeftHand;

		{
			AS3_GetScalarFromVar(dbFov, fov);
		}

		{
			AS3_GetScalarFromVar(dbNearPlane, nearPlane);
		}

		{
			AS3_GetScalarFromVar(dbFarPlane, farPlane);
		}

		{
			AS3_GetScalarFromVar(dbAspect, aspect);
		}

		{
			AS3_GetScalarFromVar(bIsLeftHand, leftHand);
		}

		pCamera->SetProjection(dbFov, dbNearPlane, dbFarPlane, dbAspect, bIsLeftHand);
	}
}

void CreateSceneObject()
{
	CStaticObject* pSceneObject;

	{
		AS3_GetScalarFromVar(pSceneObject, sceneObject);
	}

	if(!pSceneObject)
	{
		ALCHEMY_DEBUG_NEW(pSceneObject, CSceneObject);
	}

	CMesh* pMesh;
	UINT uSubset;
	CEffect* pEffect;
	IRenderMethod* pRenderMethod;
	UINT uRenderPass;
	CMaterial* pMaterial;
	CSceneManager* pSceneManager;

	{
		AS3_GetScalarFromVar(pMesh, mesh);
	}

	{
		AS3_GetScalarFromVar(pEffect, effect);
	}

	{
		AS3_GetScalarFromVar(pRenderMethod, renderMethod);
	}

	{
		AS3_GetScalarFromVar(uSubset, subset);
	}

	{
		AS3_GetScalarFromVar(uRenderPass, renderPass);
	}

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	{
		AS3_GetScalarFromVar(pSceneManager, sceneManager);
	}

	pSceneObject->Create(pMesh ? pMesh->GetResourceHandle() : 0, uSubset, pEffect ? pEffect->GetResourceHandle() : 0, pRenderMethod, uRenderPass, pMaterial ? pMaterial->GetResourceHandle() : 0, pSceneManager);

	AS3_Return(pSceneObject);
}

void SceneObjectGetMeshProcessor()
{
	CStaticObject* pSceneObject;

	{
		AS3_GetScalarFromVar(pSceneObject, sceneObject);
	}

	if(pSceneObject)
		AS3_Return( static_cast<CSceneObject*>(pSceneObject)->GetMeshProcessor() );

	AS3_Return(0);
}

void SceneObjectSetMesh()
{
	CStaticObject* pSceneObject;

	{
		AS3_GetScalarFromVar(pSceneObject, sceneObject);
	}

	CMesh* pMesh;

	{
		AS3_GetScalarFromVar(pMesh, mesh);
	}

	if(pSceneObject && pMesh)
		static_cast<CSceneObject*>(pSceneObject)->SetMesh( pMesh->GetResourceHandle() );
}

void SceneObjectSetSubset()
{
	CStaticObject* pSceneObject;

	{
		AS3_GetScalarFromVar(pSceneObject, sceneObject);
	}

	if(pSceneObject)
	{
		UINT uSubset;

		{
			AS3_GetScalarFromVar(uSubset, subset);
		}

		pSceneObject->SetMeshSubset(uSubset);
	}
}

void SceneObjectSetEffect()
{
	CStaticObject* pSceneObject;

	{
		AS3_GetScalarFromVar(pSceneObject, sceneObject);
	}

	CEffect* pEffect;

	{
		AS3_GetScalarFromVar(pEffect, effect);
	}

	if(pSceneObject && pEffect)
		pSceneObject->SetEffect( pEffect->GetResourceHandle() );
}

void SceneObjectSetRenderMethod()
{
	CStaticObject* pSceneObject;

	{
		AS3_GetScalarFromVar(pSceneObject, sceneObject);
	}

	IRenderMethod* pRenderMethod;

	{
		AS3_GetScalarFromVar(pRenderMethod, renderMethod);
	}

	if(pSceneObject && pRenderMethod)
		pSceneObject->SetRenderMethod(pRenderMethod);
}

void SceneObjectSetRenderPass()
{
	CStaticObject* pSceneObject;

	{
		AS3_GetScalarFromVar(pSceneObject, sceneObject);
	}

	if(pSceneObject)
	{
		UINT uRenderPass;

		{
			AS3_GetScalarFromVar(uRenderPass, renderPass);
		}

		pSceneObject->SetRenderPass(uRenderPass);
	}
}

void SceneObjectSetMaterial()
{
	CStaticObject* pSceneObject;

	{
		AS3_GetScalarFromVar(pSceneObject, sceneObject);
	}

	CMaterial* pMaterial;

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	if(pSceneObject && pMaterial)
		pSceneObject->SetMaterial( pMaterial->GetResourceHandle() );
}

void SceneObjectSetMeshProcessor()
{
	CStaticObject* pSceneObject;

	{
		AS3_GetScalarFromVar(pSceneObject, sceneObject);
	}

	IMeshProcessor* pMeshProcessor;
	{
		AS3_GetScalarFromVar(pMeshProcessor, meshProcessor);
	}

	if(pSceneObject && pMeshProcessor)
		static_cast<CSceneObject*>(pSceneObject)->SetMeshProcessor(pMeshProcessor);
}

void SceneObjectSetBoundingVolume()
{
	CStaticObject* pSceneObject;

	{
		AS3_GetScalarFromVar(pSceneObject, sceneObject);
	}

	IBoundingVolume* pBoundingVolume;

	{
		AS3_GetScalarFromVar(pBoundingVolume, boundingVolume);
	}

	if(pSceneObject && pBoundingVolume)
		pSceneObject->SetBoundingVolume(pBoundingVolume);
}

void CreateSkyDome()
{
	CSkyDome* pSkyDome;

	ALCHEMY_DEBUG_NEW(pSkyDome, CSkyDome);

	FLOAT fRadius;
	FLOAT fPhiDelta;
	FLOAT fThetaDelta;
	CMaterial* pMaterial;

	{
		AS3_GetScalarFromVar(fRadius, radius);
	}

	{
		AS3_GetScalarFromVar(fPhiDelta, phiDelta);
	}

	{
		AS3_GetScalarFromVar(fThetaDelta, thetaDelta);
	}

	{
		AS3_GetScalarFromVar(pMaterial, material);
	}

	if(fRadius && fPhiDelta && fThetaDelta && pMaterial)
	{
		UINT uMainTextureIndex;
		PUINT pOffsetTextureIndices;
		UINT uOffsetNum;

		{
			AS3_GetScalarFromVar(uMainTextureIndex, mainTextureIndex);
		}

		{
			AS3_GetScalarFromVar(pOffsetTextureIndices, offsetTextureIndicesPointer);
		}

		{
			AS3_GetScalarFromVar(uOffsetNum, offsetNum);
		}

		pSkyDome->Create(fRadius, fPhiDelta, fThetaDelta, pMaterial->GetResourceHandle(), uMainTextureIndex, pOffsetTextureIndices, uOffsetNum);
	}

	AS3_Return(pSkyDome);
}

void CreateGrassSystem()
{
	UINT uGrassMesh;

	{
		AS3_GetScalarFromVar(uGrassMesh, grassMesh);
	}

	CGrassSystem::GRASSMAPINFO* pGrassMapInfo;

	{
		AS3_GetScalarFromVar(pGrassMapInfo, grassMapInfoPointer);
	}

	if(uGrassMesh && pGrassMapInfo)
	{
		UINT uSubset;

		{
			AS3_GetScalarFromVar(uSubset, subset);
		}

		CGrassSystem::ITerrain* pTerrain;

		{
			AS3_GetScalarFromVar(pTerrain, terrain);
		}

		CGrassSystem* pGrassSystem;
		ALCHEMY_DEBUG_NEW(pGrassSystem, CGrassSystem);

		if( pGrassSystem->Create(uGrassMesh, uSubset, *pGrassMapInfo, pTerrain) )
			AS3_Return(pGrassSystem);

		ALCHEMY_DEBUG_DELETE(pGrassSystem);
	}

	AS3_Return(0);
}

void CreateGrassSystemMeshTerrain()
{
	CGrassSystem::CMeshTerrain::LPTERRAINDATA pTerrainData;

	{
		AS3_GetScalarFromVar(pTerrainData, terrainDataPointer);
	}

	UINT uTerrainDataCount;

	{
		AS3_GetScalarFromVar(uTerrainDataCount, terrainDataCount);
	}

	if(pTerrainData && uTerrainDataCount)
	{
		CGrassSystem::CMeshTerrain* pMeshTerrain;

		ALCHEMY_DEBUG_NEW(pMeshTerrain, CGrassSystem::CMeshTerrain);

		if( pMeshTerrain->Create(pTerrainData, uTerrainDataCount) )
			AS3_Return(pMeshTerrain);

		ALCHEMY_DEBUG_DELETE(pMeshTerrain);
	}

	AS3_Return(0);
}

void CreateGeometryInstance()
{
	CGeometryInstance* pGeometryInstance;
	ALCHEMY_DEBUG_NEW(pGeometryInstance, CGeometryInstance);

	AS3_Return(pGeometryInstance);
}

void GeometryInstanceSetInstances()
{
	CGeometryInstance* pGeometryInstance;
	FLOAT fInstanceCount;
	UINT uTransformCount;

	{
		AS3_GetScalarFromVar(pGeometryInstance, geometryInstance);
	}

	{
		AS3_GetScalarFromVar(fInstanceCount, instanceCount);
	}

	{
		AS3_GetScalarFromVar(uTransformCount, transformCount);
	}

	if(pGeometryInstance && fInstanceCount && uTransformCount)
	{
		CGeometryInstance::LPTRANSFORMINFO pTransformInfos;

		{
			AS3_GetScalarFromVar(pTransformInfos, pTransformInfosPointer);
		}

		UINT uTransformInfoCount;

		{
			AS3_GetScalarFromVar(uTransformInfoCount, transformInfoCount);
		}

		pGeometryInstance->SetInstances(fInstanceCount, uTransformCount, pTransformInfos, uTransformInfoCount);
	}
}

void GeometryInstanceSetTransform()
{
	CGeometryInstance* pGeometryInstance;
	PFLOAT pfValues;

	{
		AS3_GetScalarFromVar(pGeometryInstance, geometryInstance);
	}

	{
		AS3_GetScalarFromVar(pfValues, valuesPointer);
	}

	if(pGeometryInstance && pfValues)
	{
		UINT uTransformIndex;

		{
			AS3_GetScalarFromVar(uTransformIndex, transformIndex);
		}

		UINT uTransformInfoIndex;

		{
			AS3_GetScalarFromVar(uTransformInfoIndex, transformInfoIndex);
		}

		bool bIsKeep;

		{
			AS3_GetScalarFromVar(bIsKeep, keep);
		}

		pGeometryInstance->SetTransform(uTransformIndex, uTransformInfoIndex, pfValues, bIsKeep);
	}
}

void CreateSkinMeshProcessor()
{
	CSkinMeshController::SKINTYPE SkinType;
	CSkinInfo* pSkinInfo;
	CBone* pBone;

	{
		AS3_GetScalarFromVar(SkinType, skinType);
	}

	{
		AS3_GetScalarFromVar(pSkinInfo, skinInfo);
	}

	{
		AS3_GetScalarFromVar(pBone, bone);
	}

	if(SkinType && pSkinInfo && pBone)
	{
		CEffectCode::LPELEMENTDECLARATION pSkinParameters;
		UINT uElementCount;
		UINT uMaxBoneInfluence;

		{
			AS3_GetScalarFromVar(pSkinParameters, skinParamtersPointer);
		}

		{
			AS3_GetScalarFromVar(uElementCount, elementCount);
		}

		{
			AS3_GetScalarFromVar(uMaxBoneInfluence, maxBoneInfluence);
		}

		CSkinMeshProcessor* pSkinMeshProcessor;
		ALCHEMY_DEBUG_NEW( pSkinMeshProcessor, CSkinMeshProcessor(SkinType, *pSkinInfo, *pBone, pSkinParameters, uElementCount, uMaxBoneInfluence) );

		AS3_Return(pSkinMeshProcessor);
	}

	AS3_Return(0);
}

void SkinMeshProcessorGetSkinMeshController()
{
	CSkinMeshProcessor* pSkinMeshProcessor;

	{
		AS3_GetScalarFromVar(pSkinMeshProcessor, skinMeshProcessor);
	}

	if(pSkinMeshProcessor)
		AS3_Return( &pSkinMeshProcessor->GetSkinMeshController() );

	AS3_Return(0);
}

void SkinMeshControllerGetRootBone()
{
	CSkinMeshController* pSkinMeshController;

	{
		AS3_GetScalarFromVar(pSkinMeshController, skinMeshController);
	}

	if(pSkinMeshController)
		AS3_Return( pSkinMeshController->GetRootBone() );

	AS3_Return(0);
}

void SkinMeshControllerGetHardwareSkinEffect()
{
	CSkinMeshController* pSkinMeshController;

	{
		AS3_GetScalarFromVar(pSkinMeshController, skinMeshController);
	}

	if(pSkinMeshController)
		AS3_Return( pSkinMeshController->GetHardwareSkinEffect() );

	AS3_Return(0);
}

void CreateSkinInfo()
{
	CSkinInfo::LPBONE pBone;
	UINT uBoneCount;

	{
		AS3_GetScalarFromVar(pBone, boneDataPointer);
	}

	{
		AS3_GetScalarFromVar(uBoneCount, boneCount);
	}

	if(pBone && uBoneCount)
	{
		CSkinInfo* pSkinInfo;

		ALCHEMY_DEBUG_NEW(pSkinInfo, CSkinInfo);

		pSkinInfo->Create(pBone, uBoneCount);

		AS3_Return(pSkinInfo);
	}

	AS3_Return(0);
}

void CreateAnimationSet()
{
	CAnimationSet::LPANIMATION pAnimations;
	UINT uAnimationCount;

	{
		AS3_GetScalarFromVar(pAnimations, animationDataPointer);
	}

	{
		AS3_GetScalarFromVar(uAnimationCount, animationCount);
	}

	if(pAnimations && uAnimationCount)
	{
		CAnimationSet* pAnimationSet;
		ALCHEMY_DEBUG_NEW(pAnimationSet, CAnimationSet);

		pAnimationSet->Create(pAnimations, uAnimationCount);

		AS3_Return(pAnimationSet);
	}

	AS3_Return(0);
}

void CreateAnimationController()
{
	CAnimationController* pAnimationController;
	ALCHEMY_DEBUG_NEW(pAnimationController, CAnimationController);

	AS3_Return(pAnimationController);
}

void AnimationControllerGetMaxPlayTime()
{
	CAnimationController* pAnimationController;

	{
		AS3_GetScalarFromVar(pAnimationController, animationController);
	}

	if(pAnimationController)
		AS3_Return( pAnimationController->GetMaxPlayTime() );

	AS3_Return(0.0);
}

void AnimationControllerSetPlayTime()
{
	CAnimationController* pAnimationController;

	{
		AS3_GetScalarFromVar(pAnimationController, animationController);
	}

	if(pAnimationController)
	{
		double dbPlayTime;

		{
			AS3_GetScalarFromVar(dbPlayTime, playTime);
		}

		pAnimationController->SetPlayTime(dbPlayTime);
	}
}

void AnimationControllerSetIsReplay()
{
	CAnimationController* pAnimationController;

	{
		AS3_GetScalarFromVar(pAnimationController, animationController);
	}

	if(pAnimationController)
	{
		bool bIsReplay;

		{
			AS3_GetScalarFromVar(bIsReplay, isReplay);
		}

		pAnimationController->SetIsReplay(bIsReplay);
	}
}

void AnimationControllerRegisterAnimationSet()
{
	CAnimationController* pAnimationController;
	CAnimationSet* pAnimationSet;

	{
		AS3_GetScalarFromVar(pAnimationController, animationController);
	}

	{
		AS3_GetScalarFromVar(pAnimationSet, animationSet);
	}

	if(pAnimationController && pAnimationSet)
	{
		double dbWeight, dbSpeed;

		{
			AS3_GetScalarFromVar(dbWeight, weight);
		}

		{
			AS3_GetScalarFromVar(dbSpeed, speed);
		}

		pAnimationController->RegisterAnimationSet(*pAnimationSet, dbWeight, dbSpeed);
	}
}

void AnimationControllerAdvanceTime()
{
	CAnimationController* pAnimationController;

	{
		AS3_GetScalarFromVar(pAnimationController, animationController);
	}

	if(pAnimationController)
	{
		double dbTime;

		{
			AS3_GetScalarFromVar(dbTime, time);
		}

		pAnimationController->AdvanceTime(dbTime, g_Timer);
	}
}

void AnimationControllerUpdate()
{
	CAnimationController* pAnimationController;

	{
		AS3_GetScalarFromVar(pAnimationController, animationController);
	}

	if(pAnimationController)
	{
		double dbElapseTime;

		{
			AS3_GetScalarFromVar(dbElapseTime, elapseTime);
		}

		pAnimationController->Update(dbElapseTime);
	}
}

void CreateCascadShadowMapRender()
{
	UINT uLight;
	CCamera** ppSplitCameras;
	CGraphicsResource::HANDLE* pSplitTextures;
	FLOAT* pDepthSegment;

	{
		AS3_GetScalarFromVar(uLight, light);
	}

	{
		AS3_GetScalarFromVar(ppSplitCameras, splitCamerasPointer);
	}

	{
		AS3_GetScalarFromVar(pSplitTextures, splitTexturesPointer);
	}

	{
		AS3_GetScalarFromVar(pDepthSegment, depthSegmentPointer);
	}

	if(uLight && ppSplitCameras && pSplitTextures && pDepthSegment)
	{
		UINT uSplitNum;

		{
			AS3_GetScalarFromVar(uSplitNum, splitNum);
		}

		FLOAT fFrustumScale;

		{
			AS3_GetScalarFromVar(fFrustumScale, frustumScale);
		}

		CCascadShadowMapRender* pCascadShadowMapRender;

		ALCHEMY_DEBUG_NEW(pCascadShadowMapRender, CCascadShadowMapRender);

		if( pCascadShadowMapRender->Create(uLight, ppSplitCameras, pSplitTextures, pDepthSegment, uSplitNum, fFrustumScale) )
			AS3_Return(pCascadShadowMapRender);

		ALCHEMY_DEBUG_DELETE(pCascadShadowMapRender);
	}

	AS3_Return(0);
}

void CascadShadowMapRenderAddShadowCaster()
{
	CCascadShadowMapRender* pCascadShadowMapRender;

	{
		AS3_GetScalarFromVar(pCascadShadowMapRender, cascadShadowMapRender);
	}

	if(pCascadShadowMapRender)
	{
		CSceneNode* pSceneNode;

		{
			AS3_GetScalarFromVar(pSceneNode, sceneNode);
		}

		if(pSceneNode)
		{
			UINT uRenderPass;

			{
				AS3_GetScalarFromVar(uRenderPass, renderPass);
			}

			AS3_Return( pCascadShadowMapRender->AddShadowCaster(*pSceneNode, uRenderPass) );
		}
	}

	AS3_Return(0);
}

void CreateEffectCode()
{
	ICompiler* pCompiler;

	{
		AS3_GetScalarFromVar(pCompiler, compiler);
	}

	if(pCompiler)
	{
		CEffectCode* pEffectCode;

		ALCHEMY_DEBUG_NEW( pEffectCode, CEffectCode(*pCompiler) );

		CEffectCode::CPassDefiner* pPassDefiner;

		ALCHEMY_DEBUG_NEW( pPassDefiner, CEffectCode::CPassDefiner(*pEffectCode) );

		AS3_Return(pPassDefiner);
	}

	AS3_Return(0);
}

void EffectCodeAddColorKey()
{
	CEffectCode::CPassDefiner* pPassDefiner;

	{
		AS3_GetScalarFromVar(pPassDefiner, effectCode);
	}

	if(pPassDefiner)
	{
		CVertexDeclaration::ELEMENTTYPE Type;
		bool bIsResultColor;

		{
			AS3_GetScalarFromVar(Type, type);
		}

		{
			AS3_GetScalarFromVar(bIsResultColor, resultColor);
		}

		pPassDefiner->AddColorKey(Type, bIsResultColor);
	}
}

void EffectCodeAddInfo()
{
	CEffectCode::CPassDefiner* pPassDefiner;

	{
		AS3_GetScalarFromVar(pPassDefiner, effectCode);
	}

	if(pPassDefiner)
	{
		void* pEffectInfo;

		{
			AS3_GetScalarFromVar(pEffectInfo, info);
		}

		if(pEffectInfo)
		{
			IEffectInfoBlendMethod* pBlendMethod;

			{
				AS3_GetScalarFromVar(pBlendMethod, blendMethod);
			}

			UINT uInputColorKey;

			{
				AS3_GetScalarFromVar(uInputColorKey, inputColorKey);
			}

			UINT uOutputColorKey;

			{
				AS3_GetScalarFromVar(uOutputColorKey, outputColorKey);
			}

			bool bIsInputColorKey;

			{
				AS3_GetScalarFromVar(bIsInputColorKey, sourceColorKey);
			}
		
			if(bIsInputColorKey)
				pPassDefiner->AddInfo(reinterpret_cast<UINT>(pEffectInfo), pBlendMethod, uInputColorKey, uOutputColorKey);
			else
				pPassDefiner->AddInfo(*reinterpret_cast<IEffectInfo*>(pEffectInfo), pBlendMethod, uInputColorKey, uOutputColorKey);
		}
	}
}

void EffectCodeEndDefine()
{
	CEffectCode::CPassDefiner* pPassDefiner;

	{
		AS3_GetScalarFromVar(pPassDefiner, effectCode);
	}

	if(pPassDefiner)
		AS3_Return( pPassDefiner->EndDefine() );

	AS3_Return(0);
}

void CreatePositionEffect()
{
	CPositionEffect* pPositionEffect;
	ALCHEMY_DEBUG_NEW(pPositionEffect, CPositionEffect);

	AS3_Return( static_cast<IEffectInfo*>(pPositionEffect) );
}

void CreatePixelColorEffect()
{
	LPFLOAT4 pColor;

	{
		AS3_GetScalarFromVar(pColor, colorPointer);
	}

	CPixelColorEffect* pPiexelColorEffect;
	ALCHEMY_DEBUG_NEW( pPiexelColorEffect, CPixelColorEffect(pColor) );

	AS3_Return( static_cast<IEffectInfo*>(pPiexelColorEffect) );
}

void CreateTextureEffect()
{
	UINT uSamplerIndex, uTexcoordIndex;
	IEffect::LPSAMPLERDECLARATION pSamplerDeclaration;

	 {
		 AS3_GetScalarFromVar(uSamplerIndex, samplerIndex);
	 }

	 {
		 AS3_GetScalarFromVar(uTexcoordIndex, texcoordIndex);
	 }

	 {
		 AS3_GetScalarFromVar(pSamplerDeclaration, samplerDeclarationPointer);
	 }

	CTextureEffect* pTextureEffect;
	ALCHEMY_DEBUG_NEW( pTextureEffect, CTextureEffect(uSamplerIndex, uTexcoordIndex, pSamplerDeclaration) );

	AS3_Return( static_cast<IEffectInfo*>(pTextureEffect) );
}

void CreateEmptyEffect()
{
	CEmptyEffect* pEmptyEffect;
	ALCHEMY_DEBUG_NEW(pEmptyEffect, CEmptyEffect);

	AS3_Return( static_cast<IEffectInfo*>(pEmptyEffect) );
}

void CreateEnvironmentalEffect()
{
	CEnvironmentalEffect* pEnvironmentalEffect;
	ALCHEMY_DEBUG_NEW(pEnvironmentalEffect, CEnvironmentalEffect);

	AS3_Return( static_cast<IEffectInfo*>(pEnvironmentalEffect) );
}

void CreateNormalMapEffect()
{
	UINT uNormalMapUIIndex, uNormalMapIndex, uNumLights;
	PUINT puLightIndices;

	{
		AS3_GetScalarFromVar(uNormalMapUIIndex, normalMapUIIndex);
	}

	{
		AS3_GetScalarFromVar(uNormalMapIndex, normalMapIndex);
	}

	{
		AS3_GetScalarFromVar(uNumLights, numLights);
	}

	{
		AS3_GetScalarFromVar(puLightIndices, lightIndicesPointer);
	}

	CNormalMapEffect* pNormalMapEffect;
	ALCHEMY_DEBUG_NEW( pNormalMapEffect, CNormalMapEffect(uNormalMapUIIndex, uNormalMapIndex, uNumLights, puLightIndices) );

	AS3_Return( static_cast<IEffectInfo*>(pNormalMapEffect) );
}

void CreateVertexColorEffect()
{
	UINT uIndex;

	{
		AS3_GetScalarFromVar(uIndex, index);
	}

	CVertexColorEffect* pVertexColorEffect;
	ALCHEMY_DEBUG_NEW( pVertexColorEffect, CVertexColorEffect(uIndex) );

	AS3_Return( static_cast<IEffectInfo*>(pVertexColorEffect) );
}

void CreateCascadShadowDisplayEffect()
{
	PUINT puSamplerIndices, puSpilitCameraIndices;
	PFLOAT pfDepthSegment;
	UINT uSamplerCount, uLightIndex;

	{
		AS3_GetScalarFromVar(puSamplerIndices, samplerIndicesPointer);
	}

	{
		AS3_GetScalarFromVar(pfDepthSegment, depthSegmentPointer);
	}

	{
		AS3_GetScalarFromVar(puSpilitCameraIndices, spilitCameraIndicesPointer);
	}

	{
		AS3_GetScalarFromVar(uSamplerCount, samplerCount);
	}

	{
		AS3_GetScalarFromVar(uLightIndex, lightIndex);
	}

	CCascadShadowDisplayEffect* pCascadShadowDisplayEffect;
	ALCHEMY_DEBUG_NEW( pCascadShadowDisplayEffect, CCascadShadowDisplayEffect(puSamplerIndices, pfDepthSegment, puSpilitCameraIndices, uSamplerCount, uLightIndex) );
}

void CreateSkyPositionEffect()
{
	CSkyPositionEffect* pSkyPositionEffect;

	ALCHEMY_DEBUG_NEW( pSkyPositionEffect, CSkyPositionEffect );

	AS3_Return( static_cast<IEffectInfo*>(pSkyPositionEffect) );

}

void CreateInstancePositionEffect()
{
	UINT uInstanceCount;

	{
		AS3_GetScalarFromVar(uInstanceCount, instanceCount);
	}

	if(uInstanceCount)
	{
		MATRIX4X4* pMatrices;
		{
			AS3_GetScalarFromVar(pMatrices, matricesPointer);
		}

		CInstancePositionEffect* pInstancePositionEffect;

		ALCHEMY_DEBUG_NEW( pInstancePositionEffect, CInstancePositionEffect(uInstanceCount, pMatrices) );

		AS3_Return( static_cast<IEffectInfo*>(pInstancePositionEffect) );
	}

	AS3_Return(0);
}

void CreateInstancePosition3Scale1Effect()
{
	UINT uInstanceCount;

	{
		AS3_GetScalarFromVar(uInstanceCount, instanceCount);
	}

	if(uInstanceCount)
	{
		LPFLOAT4 pTransform;

		{
			AS3_GetScalarFromVar(pTransform, transformPointer);
		}

		CInstancePosition3Scale1Effect* pInstancePosition3Scale1Effect;

		ALCHEMY_DEBUG_NEW( pInstancePosition3Scale1Effect, CInstancePosition3Scale1Effect(uInstanceCount, pTransform) );

		AS3_Return( static_cast<IEffectInfo*>(pInstancePosition3Scale1Effect) );
	}

	AS3_Return(0);
}

void CreateEffectInfoAddition()
{
	CEffectInfoAddition::BLENDFLAG BlendFlag;

	{
		AS3_GetScalarFromVar(BlendFlag, blendFlag);
	}

	CEffectInfoAddition* pEffectInfoAddition;
	ALCHEMY_DEBUG_NEW( pEffectInfoAddition, CEffectInfoAddition(BlendFlag) );

	AS3_Return( static_cast<IEffectInfoBlendMethod*>(pEffectInfoAddition) );
}

void CreateEffectInfoModulation()
{
	CEffectInfoModulation::BLENDFLAG BlendFlag;

	{
		AS3_GetScalarFromVar(BlendFlag, blendFlag);
	}

	CEffectInfoModulation* pEffectInfoModulation;
	ALCHEMY_DEBUG_NEW( pEffectInfoModulation, CEffectInfoModulation(BlendFlag) );

	AS3_Return( static_cast<IEffectInfoBlendMethod*>(pEffectInfoModulation) );
}

void CreateAlphatTestBlendMethod()
{
	PFLOAT pfRef;

	{
		AS3_GetScalarFromVar(pfRef, refPointer);
	}

	CAlphaTestBlendMethod* pAlphaTestBlendMethod;
	ALCHEMY_DEBUG_NEW( pAlphaTestBlendMethod, CAlphaTestBlendMethod(pfRef) );

	AS3_Return( static_cast<IEffectInfoBlendMethod*>(pAlphaTestBlendMethod) );
}

void CreatePixelFogBlendMethod()
{
	CPixelFogBlendMethod::FOG_TYPE FogType;

	{
		AS3_GetScalarFromVar(FogType, fogType);
	}

	CPixelFogBlendMethod* pPixelFogBlendMethod;
	ALCHEMY_DEBUG_NEW( pPixelFogBlendMethod, CPixelFogBlendMethod(FogType) );

	PFLOAT pfEnd, pfDistance, pfDensity;

	{
		AS3_GetScalarFromVar(pfEnd, endPointer);
	}

	{
		AS3_GetScalarFromVar(pfDistance, distancePointer);
	}

	{
		AS3_GetScalarFromVar(pfDensity, densityPointer);
	}

	pPixelFogBlendMethod->Create(pfEnd, pfDistance, pfDensity);

	AS3_Return( static_cast<IEffectInfoBlendMethod*>(pPixelFogBlendMethod) );
}

void CreateAxisAlignedBoundingBox()
{
	CAxisAlignedBoundingBox* pAxisAlignedBoundingBox;
	ALCHEMY_DEBUG_NEW(pAxisAlignedBoundingBox, CAxisAlignedBoundingBox);

	AS3_Return( static_cast<IBoundingVolume*>(pAxisAlignedBoundingBox) );
}

int main()
{
	CFlasccGraphicsManager* pGraphicsManager;
	ALCHEMY_DEBUG_NEW(pGraphicsManager, CFlasccGraphicsManager);

	CMeshManager* pMeshManager;
	ALCHEMY_DEBUG_NEW(pMeshManager, CMeshManager);

	AS3_GoAsync();
}