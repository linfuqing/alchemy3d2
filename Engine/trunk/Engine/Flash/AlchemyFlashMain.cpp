#include "AlchemyFlashStdafx.h"

//#include "../Core/AlchemyDataType.h"

//#include "AlchemyFlashObjectInterface.h"

#include "AlchemyFlashContext3D.h"
#include "AlchemyFlashCustomSceneNode.h"
#include "AlchemyFlashEffect.h"
#include "AlchemyFlashIndexBuffer.h"
#include "AlchemyFlashLight.h"
#include "AlchemyFlashMaterial.h"
#include "AlchemyFlashMesh.h"
#include "AlchemyFlashProgram3D.h"
#include "AlchemyFlashSceneManager.h"
#include "AlchemyFlashSceneNodeFactory.h"
#include "AlchemyFlashSceneNode.h"
#include "AlchemyFlashCamera.h"
#include "AlchemyFlashSceneObject.h"
#include "AlchemyFlashBone.h"
#include "AlchemyFlashTexture.h"
#include "AlchemyFlashVertexBuffer.h"
#include "AlchemyFlashVertexDeclaration.h"
#include "AlchemyFlashRenderMethod.h"
#include "AlchemyFlashAGALAssembler.h"
#include "AlchemyFlashSkyPlane.h"
#include "AlchemyFlashSkyBox.h"
#include "AlchemyFlashMirror.h"
#include "AlchemyFlashWater.h"
#include "AlchemyFlashBillboardSystem.h"
#include "AlchemyFlashSkyDome.h"
#include "AlchemyFlashParticleShowSystem.h"
#include "AlchemyFlashParticleSystem.h"
#include "AlchemyFlashVolumeLightShow.h"
#include "AlchemyFlashStaticMeshInstance.h"
#include "AlchemyFlashGrassSystem.h"

#include "../Graphics/AlchemyPositionEffect.h"
#include "../Graphics/AlchemyPosition2DEffect.h"
#include "../Graphics/AlchemyTextureEffect.h"
#include "../Graphics/AlchemyForceTextureEffect.h"
#include "../Graphics/AlchemyPixelAmbientLightEffect.h"
#include "../Graphics/AlchemyCartoonEffect.h"
#include "../Graphics/AlchemyOutLineEffect.h"
#include "../Graphics/AlchemyPixelColorEffect.h"
#include "../Graphics/AlchemyEnvironmentalEffect.h"
#include "../Graphics/AlchemyNormalMapEffect.h"
#include "../Graphics/AlchemyDepthEffect.h"
#include "../Graphics/AlchemyDistanceInfoEffect.h"
#include "../Graphics/AlchemyBlinnPhongEffect.h"
#include "../Graphics/AlchemyEffectInfoAddition.h"
#include "../Graphics/AlchemyEffectInfoModulation.h"
#include "../Graphics/AlchemyFresnelBlendMethod.h"
#include "../Graphics/AlchemyFacingEffect.h"
#include "../Graphics/AlchemyDepthFieldEffect.h"
#include "../Graphics/AlchemyPixelFogBlendMethod.h"
#include "../Graphics/AlchemyContortEffect.h"
#include "../Graphics/AlchemyDepthEffect.h"
#include "../Graphics/AlchemyBillboardEffect.h"
#include "../Graphics/AlchemyPSMoveEffect.h"
#include "../Graphics/AlchemyFadeOutEffect.h"
#include "../Graphics/AlchemyEmptyEffect.h"
#include "../Graphics/AlchemyVertexColorEffect.h"
#include "../Graphics/AlchemyShadowMapDisplayEffect.h"
#include "../Graphics/AlchemyMaskTextureEffect.h"
#include "../Graphics/AlchemySpecularTextureEffect.h"
#include "../Graphics/AlchemyTextureModulusEffect.h"
#include "../Graphics/AlchemyBackgroundEffect.h"
#include "../Graphics/AlchemyEffectInfoSub.h"
#include "../Graphics/AlchemyBidirectionalBlendMethod.h"
#include "../Graphics/AlchemyDirectionalShadowDisplayEffect.h"
#include "../Graphics/AlchemyGrassAnimationEffect.h"
#include "../Graphics/AlchemyCascadShadowDisplayEffect.h"
#include "../Graphics/AlchemyTextureBlurEffect.h"
#include "../Graphics/AlchemyShadowMapRender.h"
#include "../Graphics/AlchemyPostProcessRender.h"
#include "../Graphics/AlchemyProgramRenderManager.h"

#include "../Graphics/AlchemyAlphaTestBlendMethod.h"
#include "../Graphics/AlchemySilhouetteProgram.h"
#include "../Graphics/AlchemyEnvironmentRender.h"
#include "../Graphics/AlchemyDeferredLightRenderer.h"
#include "../Graphics/AlchemyVolumeLightShow.h"
#include "../Graphics/AlchemyCascadShadowMapRender.h"

#include "../Graphics/AlchemyNormalMeshProcessor.h"
#include "../Graphics/AlchemySkinMeshProcessor.h"

#include "../Graphics/AlchemySkinMeshController.h"

#include "../Graphics/AlchemyMeshFiller.h"
#include "../Graphics/AlchemyPSLiveTimeAttribute.h"
#include "../Graphics/AlchemyPSParticleInfoAttribute.h"
#include "../Graphics/AlchemyPSPositionAttribute.h"
#include "../Graphics/AlchemyPSTextureAttribute.h"
#include "../Graphics/AlchemyPSVelocityAttribute.h"

#include "../Graphics/AlchemyParticleDataCode.h"
#include "../Graphics/AlchemyParticleComputeInfoInterface.h"
#include "../Graphics/AlchemySnowParticleCompute.h"
#include "../Graphics/AlchemyAccelerationVC.h"
#include "../Graphics/AlchemyVelocityVC.h"

#include "../Graphics/AlchemyMulityEffectProgram.h"
#include "../Graphics/AlchemySkyPassDefiner.h"

#include "../Graphics/AlchemyRenderStateInterface.h"

#include "../Loader/AlchemyDAEParser.h"

using namespace alchemy;

//////////////////////////////////////////////////////////////////////////
///Init
//////////////////////////////////////////////////////////////////////////
AS3_Val InitObject(void* pSelf, AS3_Val Args)
{
	CFlashObjectInterface* pObject;

	AS3_ArrayValue(Args, "PtrType", &pObject);

	if(pObject)
		return pObject->Init();

	return 0;
}

AS3_Val InitParticleShowSystem( void* pSelf, AS3_Val Args )
{
	CFlashParticleShowSystem* pParticleShowSytem;
	IMeshFiller*	  pMeshFiller;
	INT nParticleCount, nEffect, nMaterial;
	CFlashRenderMethod* pRenderMethod;

	AS3_ArrayValue(Args,"PtrType, IntType, PtrType, IntType, PtrType, IntType", &pParticleShowSytem, &nParticleCount, &pMeshFiller, &nEffect, &pRenderMethod, &nMaterial);


	if(pParticleShowSytem)
		return AS3_Int( pParticleShowSytem->InitPSSystem(nParticleCount, pMeshFiller, (CGraphicsResource::HANDLE)nEffect, *pRenderMethod, (CGraphicsResource::HANDLE)nMaterial ) );

	return 0;
}

//////////////////////////////////////////////////////////////////////// //
///Create
//////////////////////////////////////////////////////////////////////////
AS3_Val CreateBuffer(void* pSelf, AS3_Val Args)
{
	PUINT8 puBuffer;
	UINT uSize;

	uSize = AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW(puBuffer, UINT8[uSize]);

	return AS3_Ptr(puBuffer);
}

AS3_Val CreateAGALAssembler(void* pSelf, AS3_Val Args)
{
	CFlashAGALAssembler* pAssembler;

	ALCHEMY_DEBUG_NEW(pAssembler, CFlashAGALAssembler);

	return AS3_Array( "PtrType, PtrType, PtrType",pAssembler, static_cast<CFlashObjectInterface*>(pAssembler), static_cast<ICompiler*>(pAssembler) );
}

AS3_Val CreateRenderSystem(void* pSelf, AS3_Val Args)
{
	CFlashAGALAssembler* pAGALssembler;
	UINT uTotalRenderDataCount;

	AS3_ArrayValue(Args, "PtrType, IntType", &pAGALssembler, &uTotalRenderDataCount);
	CFlashContext3D* pContext3D;
	
	if(pAGALssembler)
	{
		ALCHEMY_DEBUG_NEW( pContext3D, CFlashContext3D(*pAGALssembler, uTotalRenderDataCount) );

		return AS3_Array("PtrType, PtrType",pContext3D, static_cast<CFlashObjectInterface*>(pContext3D));
	}

	return 0;
}

AS3_Val CreateSceneNodeFactory(void* pSelf, AS3_Val Args)
{
	CFlashSceneNodeFactory* pSceneNodeFactory;

	ALCHEMY_DEBUG_NEW( pSceneNodeFactory, CFlashSceneNodeFactory() );

	return AS3_Array("PtrType, PtrType",pSceneNodeFactory, static_cast<CFlashObjectInterface*>(pSceneNodeFactory));
}

AS3_Val CreateMeshManager(void* pSelf, AS3_Val Args)
{
	CMeshManager* pMeshManager;

	ALCHEMY_DEBUG_NEW(pMeshManager, CMeshManager);

	return AS3_Ptr(pMeshManager);
}

AS3_Val CreateSceneManager(void* pSelf, AS3_Val Args)
{
	CFlashContext3D* pContext3D;

	AS3_ArrayValue(Args, "PtrType", &pContext3D);

	CFlashSceneManager* pSceneManager;

	ALCHEMY_DEBUG_NEW( pSceneManager, CFlashSceneManager(*pContext3D) );

	return AS3_Array( "PtrType, PtrType",pSceneManager, static_cast<CFlashObjectInterface*>(pSceneManager) );
}

AS3_Val CreateMeshData(void* pSelf, AS3_Val Args)
{
	CMeshManager::LPMESHDATA pMeshData =  reinterpret_cast<CMeshManager::LPMESHDATA>( AS3_PtrValue(Args) );

	if(pMeshData)
	{
		if(pMeshData->VertexElementData.pVertexElement)
		{
			UINT uVertexBufferSize = CVertexDeclaration::GetVertexElementMaxinumStream(pMeshData->VertexElementData.pVertexElement, pMeshData->VertexElementData.uElementCount) + 1;

			ALCHEMY_DEBUG_NEW(pMeshData->ppVertexData, PUINT8[uVertexBufferSize]);

			for(UINT i = 0; i < uVertexBufferSize; ++ i)
				ALCHEMY_DEBUG_NEW(pMeshData->ppVertexData[i], UINT8[CVertexDeclaration::GetVertexElementStreamStrideOf(i, pMeshData->VertexElementData.pVertexElement, pMeshData->VertexElementData.uElementCount) * pMeshData->uNumVertices]);

			return 0;
		}

		ALCHEMY_DEBUG_NEW(pMeshData->VertexElementData.pVertexElement , CVertexDeclaration::VERTEXELEMENT[pMeshData->VertexElementData.uElementCount]);
		ALCHEMY_DEBUG_NEW(pMeshData->pAttributeRange, CMesh::ATTRIBUTERANGE[     pMeshData->uAttributeRangeCount]);

		ALCHEMY_DEBUG_NEW(pMeshData->pIndexData     , UINT8[pMeshData->uNumIndices << pMeshData->IndexFormat]);

		return 0;
	}
	
	ALCHEMY_DEBUG_NEW(pMeshData, CMeshManager::MESHDATA);

	return AS3_Ptr(pMeshData);
}

AS3_Val CreateMesh(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	//CMeshManager::MESHDATA* pMeshData;
	CMeshManager::HANDLE hMeshHandle;

	AS3_ArrayValue(Args, "PtrType, IntType", &pSceneManager, &hMeshHandle);

	if(pSceneManager && hMeshHandle)
	{
		CFlashMesh* pMesh =  static_cast<CFlashMesh*>( pSceneManager->CreateMesh(hMeshHandle) );

		if(pMesh)
			return AS3_Int( pMesh->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreatePlaneFromMeshManager(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	double dWidth; 
	double dHeight;
	unsigned int uWidthSegment; 
	unsigned int uHeightSegment;
	unsigned int uWidthStep;
	unsigned int uHeightStep;

	AS3_ArrayValue(Args, "PtrType, DoubleType, DoubleType, IntType, IntType, IntType, IntType", &pSceneManager, &dWidth, &dHeight, &uWidthSegment, &uHeightSegment, &uWidthStep, &uHeightStep);

	if(pSceneManager)
	{
		CVertexDeclaration::VERTEXELEMENT VertexElement[2];
		VertexElement[0].uStream = 0;
		VertexElement[0].uOffset = 0;
		VertexElement[0].Type = CVertexDeclaration::FLOAT4;
		VertexElement[0].Usage = CVertexDeclaration::POSITION;

		VertexElement[1].uStream = 0;
		VertexElement[1].uOffset = 16;
		VertexElement[1].Type = CVertexDeclaration::FLOAT2;
		VertexElement[1].Usage = CVertexDeclaration::TEXCOORD;
		UINT uHandle = ALCHEMY_MESH_MANAGER.CreatePlane(static_cast<FLOAT>(dWidth), static_cast<FLOAT>(dHeight), uWidthSegment, uHeightSegment, uWidthStep, uHeightStep, false, VertexElement, 2);
		if(uHandle)
		{
			//const CMeshManager::MESHDATA* pMeshData = ALCHEMY_MESH_MANAGER.GetMeshData(uHandle);

			if(uHandle)
			{
				CFlashMesh* pMesh =  static_cast<CFlashMesh*>( pSceneManager->CreateMesh(uHandle) );

				if(pMesh)
					return AS3_Int( pMesh->GetResourceHandle() );
			}
		}
	}

	return 0;
}

AS3_Val CreateBillboardMeshData(void* pSelf, AS3_Val Args)
{
	double dWidth, dHeight;
	INT nBillboardNum;
	CVertexDeclaration::VERTEXELEMENT* pVertexElements;
	INT nElementNum, nIndexUsage;
	UINT* pVertexUsage;

	AS3_ArrayValue(Args, "DoubleType, DoubleType, IntType, PtrType, IntType, IntType, PtrType", &dWidth, &dHeight, &nBillboardNum, &pVertexElements, &nElementNum, &nIndexUsage, &pVertexUsage);

	return AS3_Int( ALCHEMY_MESH_MANAGER.CreateBillboardMeshData( (FLOAT)dWidth, (FLOAT)dHeight, (UINT)nBillboardNum, pVertexElements, pVertexUsage, (UINT)nElementNum, (UINT)nIndexUsage ) );
}

AS3_Val CreateMeshFromEffect(void* pSelf, AS3_Val Args)
{
	INT nEffectHandle, nNumVertices, nNumIndices, nElementUsageCount, IndexUsage;
	CMeshManager::ELEMENTUSAGEDATA* pUsageDatas;

	AS3_ArrayValue(Args, "IntType, IntType, IntType, PtrType, IntType, IntType", &nEffectHandle, &nNumVertices, &nNumIndices, &pUsageDatas, &nElementUsageCount, &IndexUsage);

	CEffect* pEffect = static_cast<CEffect*>( CFlashSceneManager::GetInstance()->GetRenderSystem().GetResource(nEffectHandle, CGraphicsResource::EFFECT) );

	if(!pEffect)
		return 0;

	UINT uMeshData = ALCHEMY_MESH_MANAGER.CreateMeshDataFromEffect(pEffect, nNumVertices, nNumIndices, pUsageDatas, nElementUsageCount, IndexUsage);

	if(!uMeshData)
		return 0;

	CFlashMesh* pMesh = static_cast<CFlashMesh*>( CFlashSceneManager::GetInstance()->CreateMesh( uMeshData ) );

	if(!pMesh)
		return 0;

	return AS3_Int( pMesh->GetResourceHandle() );
}

AS3_Val CreateMaterialData(void* pSelf, AS3_Val Args)
{
	INT nTextureNum = (INT) ( AS3_IntValue(Args) );

	CMeshManager::LPMATERIALDATA pMaterialData;
	ALCHEMY_DEBUG_NEW(pMaterialData, CMeshManager::MATERIALDATA);
	pMaterialData->pTextureDataHandles = ALCHEMY_NULL;
	pMaterialData->uTextureCount = nTextureNum;

	if(nTextureNum)
	{
		ALCHEMY_DEBUG_NEW(pMaterialData->pTextureDataHandles, CMeshManager::HANDLE[nTextureNum]);
	}

	return AS3_Array("PtrType, PtrType", pMaterialData, pMaterialData->pTextureDataHandles);
}

AS3_Val CreateTextureData(void* pSelf, AS3_Val Args)
{
	CMeshManager::TEXTUREDATA* pTextureData;
	ALCHEMY_DEBUG_NEW(pTextureData, CMeshManager::TEXTUREDATA);
	pTextureData->pData = ALCHEMY_NULL;

	INT nBytesLen = (INT) ( AS3_IntValue(Args) );
	if(nBytesLen > 0)
	{
		PUINT8 pByteData;
		ALCHEMY_DEBUG_NEW(pByteData, UINT8[nBytesLen]);
		pTextureData->pData = reinterpret_cast<PVOID>(pByteData);
	}

	return AS3_Array("PtrType, PtrType", pTextureData, pTextureData->pData);
}

AS3_Val CreateSceneData(void* pSelf, AS3_Val Args)
{
	CMeshManager::SCENEDATA* pSceneData;
	ALCHEMY_DEBUG_NEW(pSceneData, CMeshManager::SCENEDATA);
	pSceneData->pSceneNodeData = ALCHEMY_NULL;

	INT nNodeLen = (INT)( AS3_IntValue(Args) );
	pSceneData->uSceneDataCount = nNodeLen;
	if(nNodeLen)
	{
		ALCHEMY_DEBUG_NEW(pSceneData->pSceneNodeData, CMeshManager::SCENENODEDATA[nNodeLen]);
	}

	return AS3_Array("PtrType, PtrType", pSceneData, pSceneData->pSceneNodeData);
}

AS3_Val CreateMaterial(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	//CMeshManager::LPMATERIALDATA pMaterialData;
	CMeshManager::HANDLE hMaterialDataHandle;

	AS3_ArrayValue(Args, "PtrType, IntType", &pSceneManager, &hMaterialDataHandle);

	if(pSceneManager)
	{
		CFlashMaterial* pMaterial = static_cast<CFlashMaterial*>(pSceneManager->CreateMaterial(hMaterialDataHandle));
		
		if(pMaterial)
			return AS3_Int( pMaterial->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateLight(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;

	pSceneManager = static_cast<CFlashSceneManager*>( AS3_PtrValue(Args) );

	if(pSceneManager)
	{
		CLight* pLight = static_cast<CFlashLight*>( pSceneManager->CreateLight() );

		if(pLight)
			return AS3_Int( pLight->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateParameterList(void* pSelf, AS3_Val Args)
{
	CRenderMethod::LPPARAMETERLIST pParameterList;
	UINT uLength;
	AS3_ArrayValue(Args, "PtrType, IntType", &pParameterList, &uLength);

	if(pParameterList)
	{
		if(!pParameterList->pCameraParameters)
			ALCHEMY_DEBUG_NEW(pParameterList->pCameraParameters, CRenderMethod::PARAMETERINFO[pParameterList->uCameraCount][CRenderMethod::TOTAL_CAMERA_PARAMETERS]);

		if(!pParameterList->pLightParameters)
			ALCHEMY_DEBUG_NEW(pParameterList->pLightParameters, CRenderMethod::PARAMETERINFO[pParameterList->uLightCount][CRenderMethod::TOTAL_LIGHT_PARAMETERS]);

		if(!pParameterList->pTextureParameters)
			ALCHEMY_DEBUG_NEW(pParameterList->pTextureParameters, CRenderMethod::PARAMETERINFO[pParameterList->uTextureCount][CRenderMethod::TOTAL_TEXTURE_PARAMETERS]);

		if(!pParameterList->pTextures)
			ALCHEMY_DEBUG_NEW(pParameterList->pTextures, CRenderMethod::PARAMETERINFO[pParameterList->uTextureCount]);

		if(!pParameterList->puCameraParameterFlag)
			ALCHEMY_DEBUG_NEW(pParameterList->puCameraParameterFlag, UINT[pParameterList->uCameraCount]);

		if(!pParameterList->puLightParameterFlag)
			ALCHEMY_DEBUG_NEW(pParameterList->puLightParameterFlag, UINT[pParameterList->uLightCount]);

		if(!pParameterList->puTextureParameterFlag)
			ALCHEMY_DEBUG_NEW(pParameterList->puTextureParameterFlag, UINT[pParameterList->uTextureCount]);

		return AS3_Ptr( pParameterList + sizeof(CRenderMethod::PARAMETERLIST) );
	}

	ALCHEMY_DEBUG_NEW(pParameterList, CRenderMethod::PARAMETERLIST[uLength]);

	return AS3_Ptr(pParameterList);
}

AS3_Val CreateRenderMethod(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	UINT uPassCount, uEffect;
	PUINT puRenderTargetIndexCount;
	CRenderMethod::LPPARAMETERLIST pParameterList;
	CRenderMethod::CRenderStateHandler* pRenderStateHandler;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType, PtrType, PtrType", &pSceneManager, &pParameterList, &uPassCount, &puRenderTargetIndexCount, &pRenderStateHandler);

	if(pSceneManager && pParameterList)
	{
		CFlashRenderMethod* pRenderMethod = static_cast<CFlashRenderMethod*>( pSceneManager->CreateRenderMethod(pParameterList, uPassCount, puRenderTargetIndexCount, ALCHEMY_NULL, pRenderStateHandler) );

		if(pRenderMethod)
			return AS3_Int( pRenderMethod->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateRenderMethodFromEffectCode(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	CEffectCode* pEffectCode;
	UINT uEffect;
	CRenderMethod::CRenderStateHandler* pRenderStateHandler;

	AS3_ArrayValue(Args, "PtrType, PtrType, PtrType", &pSceneManager, &pEffectCode, &pRenderStateHandler);

	if(pSceneManager && pEffectCode)
	{
		CFlashRenderMethod* pRenderMethod = static_cast<CFlashRenderMethod*>( pSceneManager->CreateRenderMethod(*pEffectCode, ALCHEMY_NULL, pRenderStateHandler) );

		if(pRenderMethod)
			return AS3_Int( pRenderMethod->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateVertexBuffer(void* pSelf, AS3_Val Args)
{
	CFlashContext3D* pRenderSys;
	UINT cnt,stride,usage;
	PVOID pData = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, IntType, IntType, IntType, PtrType", &pRenderSys, &cnt, &stride, &usage, &pData);

	if(pRenderSys)
	{
		CFlashVertexBuffer* pVertexBuf = static_cast<CFlashVertexBuffer*>(pRenderSys->CreateVertexBuffer(cnt,stride,usage,pData));
		return AS3_Int( pVertexBuf->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateIndexBuffer(void* pSelf, AS3_Val Args)
{
	CFlashContext3D* pRenderSys;
	UINT cnt,usage;
	PVOID pData = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, IntType, IntType, PtrType", &pRenderSys, &cnt, &usage, &pData);

	if(pRenderSys)
	{
		CFlashIndexBuffer* pIndexBuf = static_cast<CFlashIndexBuffer*>(pRenderSys->CreateIndexBuffer(cnt,usage,CIndexBuffer::BIT16, pData));
		return AS3_Int( pIndexBuf->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateProgram3D(void* pSelf, AS3_Val Args)
{
	CFlashContext3D* pRenderSys;
	ICompiler* pCompiler;
	PCHAR pVertexCode, pFragmentCode;

	AS3_ArrayValue(Args, "PtrType, PtrType, StrType, StrType", &pRenderSys, &pCompiler, &pVertexCode, &pFragmentCode);

	if(pRenderSys && pCompiler)
	{
		CFlashProgram3D* pProgram = static_cast<CFlashProgram3D*>(pRenderSys->CreateShader(IEffect::PROGRAM, *pCompiler, pVertexCode, pFragmentCode));

		if(pProgram)
			return AS3_Int( pProgram->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateTexture(void* pSelf, AS3_Val Args)
{
	CFlashContext3D* pRenderSys;
	UINT uWidth,uHeight,uUsage, uMipLevel;
	CTexture::FORMAT Format;
	void** ppData = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, IntType, IntType, IntType, IntType, IntType, PtrType", &pRenderSys, &uWidth, &uHeight, &uUsage, &Format, &uMipLevel, &ppData);

	if(pRenderSys)
	{
		LPTEXTURE pTexture = pRenderSys->CreateTexture(uWidth, uHeight, uUsage, Format, uMipLevel, ppData);
		return AS3_Int( pTexture->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateCubeTexture(void* pSelf, AS3_Val Args)
{
	CFlashContext3D* pRenderSys;
	unsigned int uSize,uUsage, uMipLevel;
	CTexture::FORMAT Format;
	const void** ppData = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, IntType, IntType, IntType, IntType, PtrType", &pRenderSys, &uSize, &uUsage, &Format, &uMipLevel, &ppData);

	if(pRenderSys)
	{
		LPTEXTURE pTexture = pRenderSys->CreateCubeTexture(uSize, uUsage, Format, uMipLevel, ppData[0], ppData[1], ppData[2], ppData[3], ppData[4], ppData[5]);
		return AS3_Int( pTexture->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateEffect(void* pSelf, AS3_Val Args)
{
	CFlashContext3D* pRenderSystem;
	PUINT puRenderTargetIndexCount;
	const CHAR** ppcCodes, **ppcAdditionalCodes;
	CEffect::LPPASSINFO pPassInfos;
	UINT uPassCount;

	AS3_ArrayValue(Args, "PtrType, IntType, PtrType, PtrType, PtrType, PtrType", &pRenderSystem, &uPassCount, &puRenderTargetIndexCount, &ppcCodes, &ppcAdditionalCodes, &pPassInfos);

	if(pRenderSystem)
	{
		CEffect* pEffect = pRenderSystem->CreateEffect(uPassCount, puRenderTargetIndexCount, reinterpret_cast<const CHAR**>(ppcCodes), reinterpret_cast<const CHAR**>(ppcAdditionalCodes), pPassInfos);

		if(pEffect)
			return AS3_Int( pEffect->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateEffectFromEffectCode(void* pSelf, AS3_Val Args)
{
	CFlashContext3D* pRenderSystem;
	CEffectCode* pEffectCode;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pRenderSystem, &pEffectCode);

	if(pRenderSystem && pEffectCode)
	{
		CEffect* pEffect = pRenderSystem->CreateEffect(*pEffectCode);

		if(pEffect)
			return AS3_Int( pEffect->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateBone(void* pSelf, AS3_Val Args)
{
	CFlashBone* pBone = new CFlashBone;

	return AS3_Array( "PtrType, PtrType, PtrType", pBone, static_cast<CFlashObjectInterface*>(pBone), static_cast<CSceneNode*>(pBone) );
}

AS3_Val CreateSceneNode(void* pSelf, AS3_Val Args)
{
	CFlashSceneNode* pSceneNode = new CFlashSceneNode;

	return AS3_Array( "PtrType, PtrType, PtrType", pSceneNode, static_cast<CFlashObjectInterface*>(pSceneNode), static_cast<CSceneNode*>(pSceneNode) );
}

AS3_Val CreateSkyPlane(void* pSelf, AS3_Val Args)
{
	double dLength, dWidth, dHeight;
	int nTexture, nIsAmbientLight, nIsEmissive, nIsUVOffset;

	AS3_ArrayValue(Args, "DoubleType, DoubleType, DoubleType, IntType, IntType, IntType, IntType", &dLength, &dWidth, &dHeight, &nTexture, &nIsAmbientLight, &nIsEmissive, &nIsUVOffset);

	CFlashSkyPlane* pSkyPlane;
	ALCHEMY_DEBUG_NEW(pSkyPlane, CFlashSkyPlane);

	pSkyPlane->Create(static_cast<FLOAT>(dLength), static_cast<FLOAT>(dWidth), static_cast<FLOAT>(dHeight), nTexture, static_cast<bool>(nIsAmbientLight), static_cast<bool>(nIsEmissive), static_cast<bool>(nIsUVOffset) );

	return AS3_Array( "PtrType, PtrType, PtrType", pSkyPlane, static_cast<CFlashObjectInterface*>(pSkyPlane), static_cast<CSceneNode*>(pSkyPlane) );
}

AS3_Val CreateSkyBox(void* pSelf, AS3_Val Args)
{
	double width,height,depth;
	int nTexture;

	AS3_ArrayValue(Args,"DoubleType, DoubleType, DoubleType, IntType",&width, &height, &depth, &nTexture);

	CFlashSkyBox* pSkyBox;
	ALCHEMY_DEBUG_NEW(pSkyBox, CFlashSkyBox);

	bool b = pSkyBox->Create(static_cast<FLOAT>(width), static_cast<FLOAT>(height), static_cast<FLOAT>(depth), nTexture );

	return AS3_Array( "PtrType, PtrType, PtrType", pSkyBox, static_cast<CFlashObjectInterface*>(pSkyBox), static_cast<CSceneNode*>(pSkyBox) );

	return 0;
}

AS3_Val CreateRenderStateSet(void* pSelf, AS3_Val Args)
{
	IRenderState::RENDERSTATEINFO* pRenderInfo;
	INT nNum;
	CRenderMethod::CRenderStateHandler* pRenderStateHandle;

	AS3_ArrayValue(Args, "PtrType, IntType", &pRenderInfo, &nNum);

	ALCHEMY_DEBUG_NEW(pRenderStateHandle, CRenderMethod::CRenderStateHandler);

	if( pRenderStateHandle->Create(pRenderInfo, nNum) )
		return AS3_Ptr(pRenderStateHandle);

	return 0;
}

AS3_Val CreateBillboardSystem(void* pSelf, AS3_Val Args)
{
	INT nMaxBillboardNum, nMaterial, nEffect, isDynamic, nMesh, nSubset;
	CFlashRenderMethod* pRenderMethod = ALCHEMY_NULL;

	AS3_ArrayValue(Args,"IntType, IntType, PtrType, IntType, IntType, IntType, IntType", &nMaxBillboardNum, &nEffect, &pRenderMethod, &nMaterial, &isDynamic, &nMesh, &nSubset);

	CFlashBillboardSystem* pBillboard;
	ALCHEMY_DEBUG_NEW(pBillboard, CFlashBillboardSystem);
	CBillboardSet* pBillboardSet;
	ALCHEMY_DEBUG_NEW(pBillboardSet, CBillboardSet );

	pBillboardSet->Create( (UINT)nMaxBillboardNum, (bool)isDynamic);

	bool bRet = pBillboard->Create( (CGraphicsResource::HANDLE)nMesh, nSubset, nEffect, *pRenderMethod, nMaterial, pBillboardSet );
	if(bRet)
		return AS3_Array( "PtrType, PtrType, PtrType, PtrType", pBillboard, static_cast<CFlashObjectInterface*>(pBillboard), static_cast<CSceneNode*>(pBillboard), pBillboardSet );
	else
		return 0;
}

AS3_Val CreateSkyDome(void* pSelf, AS3_Val Args)
{
	double radius,phiDelta, thetaDelta;
	int nTexture;
	int* pTextures, nCount;

	AS3_ArrayValue(Args,"DoubleType, DoubleType, DoubleType, IntType, PtrType, IntType",&radius, &phiDelta, &thetaDelta, &nTexture, &pTextures, &nCount);

	CFlashSkyDome* pSkyDome;
	ALCHEMY_DEBUG_NEW(pSkyDome, CFlashSkyDome);

	CGraphicsResource::HANDLE* pTextureHandles = ALCHEMY_NULL;
	if(pTextures != ALCHEMY_NULL && nCount > 0)
	{
		ALCHEMY_DEBUG_NEW(pTextureHandles, CGraphicsResource::HANDLE[nCount]);
		for (UINT i=0; i<nCount; ++i)
			pTextureHandles[i] = pTextures[i];
	}

	pSkyDome->Create(static_cast<FLOAT>(radius), static_cast<FLOAT>(phiDelta), 
		static_cast<FLOAT>(thetaDelta), (CGraphicsResource::HANDLE)nTexture, pTextureHandles, (UINT)nCount);

	ALCHEMY_DEBUG_DELETE_ARRAY(pTextureHandles);

	return AS3_Array( "PtrType, PtrType, PtrType", pSkyDome, static_cast<CFlashObjectInterface*>(pSkyDome), static_cast<CSceneNode*>(pSkyDome) );
}

AS3_Val CreateSkyDomeWithMesh(void* pSelf, AS3_Val Args)
{
	INT nMeshHandle;
	INT nTexture;
	INT* pTextures, nCount;

	AS3_ArrayValue(Args,"IntType, IntType, PtrType, IntType",&nMeshHandle, &nTexture, &pTextures, &nCount);

	CFlashSkyDome* pSkyDome;
	ALCHEMY_DEBUG_NEW(pSkyDome, CFlashSkyDome);
	CGraphicsResource::HANDLE* pTextureHandles = ALCHEMY_NULL;
	if(pTextures != ALCHEMY_NULL && nCount > 0)
	{
		ALCHEMY_DEBUG_NEW(pTextureHandles, CGraphicsResource::HANDLE[nCount]);
		for (UINT i=0; i<nCount; ++i)
			pTextureHandles[i] = pTextures[i];
	}

	pSkyDome->Create( (CGraphicsResource::HANDLE)nMeshHandle, (CGraphicsResource::HANDLE)nTexture, pTextureHandles, (UINT)nCount);

	ALCHEMY_DEBUG_DELETE_ARRAY(pTextureHandles);

	return AS3_Array( "PtrType, PtrType, PtrType", pSkyDome, static_cast<CFlashObjectInterface*>(pSkyDome), static_cast<CSceneNode*>(pSkyDome) );
}

AS3_Val CreateGrassSystem(void* pSelf, AS3_Val Args)
{
	int nGrassMesh, nSubset, nTerrainCount;
	CGrassSystem::LPGRASSMAPINFO pGrassMapInfo;
	CGrassSystem::CMeshTerrain::LPTERRAINDATA pTerrain;

	AS3_ArrayValue(Args,"IntType, IntType, PtrType, PtrType, IntType",&nGrassMesh, &nSubset, &pGrassMapInfo, &pTerrain, &nTerrainCount);

	CFlashGrassSystem* pGrassSystem;
	ALCHEMY_DEBUG_NEW(pGrassSystem, CFlashGrassSystem);
	CGrassSystem::CMeshTerrain MeshTerrain;
	MeshTerrain.Create(pTerrain, (UINT)nTerrainCount);

	pGrassSystem->Create( (CGraphicsResource::HANDLE)nGrassMesh, (UINT)nSubset, *pGrassMapInfo, &MeshTerrain);

	return AS3_Array( "PtrType, PtrType, PtrType", pGrassSystem, static_cast<CFlashObjectInterface*>(pGrassSystem), static_cast<CSceneNode*>(pGrassSystem) );
}

AS3_Val CreateCascadShadowMap(void* pSelf, AS3_Val Args)
{
	CCascadShadowMapRender* pCascalShadowMap;
	INT nLightIndex, nSize, nSplitNum;
	AS3_ArrayValue(Args, "IntType, IntType, IntType", &nLightIndex, &nSize, &nSplitNum);

	ALCHEMY_DEBUG_NEW(pCascalShadowMap, CCascadShadowMapRender);

	if( !pCascalShadowMap->Create(nLightIndex, nSize, nSplitNum) )
	{
		ALCHEMY_DEBUG_DELETE(pCascalShadowMap);
		return 0;
	}

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType", pCascalShadowMap, static_cast<CAbstractProgramRender*>(pCascalShadowMap), static_cast<CRenderer*>(pCascalShadowMap), pCascalShadowMap->GetSplitTextures() );
}

AS3_Val CreateProgramRenderManager(void* pSelf, AS3_Val Args)
{
	CProgramRenderManager* pManager;
	ALCHEMY_DEBUG_NEW(pManager, CProgramRenderManager);

	return AS3_Ptr(pManager);
}

AS3_Val CreateMeshFiller(void* pSelf, AS3_Val Args)
{
	CMeshFiller* pMeshFiller;
	ALCHEMY_DEBUG_NEW(pMeshFiller, CMeshFiller);

	return AS3_Array("PtrType, PtrType", pMeshFiller, static_cast<IMeshFiller*>(pMeshFiller) );
}

AS3_Val CreateParticleShowSystem( void*pSelf, AS3_Val Args )
{
	CFlashParticleShowSystem* pPSSystem;

	ALCHEMY_DEBUG_NEW(pPSSystem, CFlashParticleShowSystem);

	return AS3_Array( "PtrType, PtrType, PtrType", pPSSystem, static_cast<CFlashObjectInterface*>(pPSSystem), static_cast<CSceneNode*>(pPSSystem) );
}

AS3_Val CreatePSLiveTimeAttribute( void* pSelf, AS3_Val Args )
{
	CPSLiveTimeAttribute*	pLiveTimeAttribute;
	double fMin, fMax;

	AS3_ArrayValue( Args, "DoubleType, DoubleType", &fMin, &fMax);

	ALCHEMY_DEBUG_NEW(pLiveTimeAttribute, CPSLiveTimeAttribute( (FLOAT)fMin, (FLOAT)fMax ) );

	return AS3_Array( "PtrType, PtrType", pLiveTimeAttribute, static_cast<IMeshAttribute*>(pLiveTimeAttribute) );
}

AS3_Val CreatePSParticleInfoAttribute( void* pSelf, AS3_Val Args )
{
	CPSParticleInfoAttribute*	pAttribute;
	double fWidth, fHeight;

	AS3_ArrayValue( Args, "DoubleType, DoubleType", &fWidth, &fHeight);

	ALCHEMY_DEBUG_NEW(pAttribute, CPSParticleInfoAttribute( (FLOAT)fWidth, (FLOAT)fHeight ) );

	return AS3_Array( "PtrType, PtrType", pAttribute, static_cast<IMeshAttribute*>(pAttribute) );
}

AS3_Val CreatePSPositionAttribute( void* pSelf, AS3_Val Args )
{
	CPSPositionAttribute*	pAttribute;
	double x,y,z;

	AS3_ArrayValue( Args, "DoubleType, DoubleType, DoubleType", &x, &y, &z);

	FLOAT3 Position;
	Position.x = x;
	Position.y = y;
	Position.z = z;

	ALCHEMY_DEBUG_NEW(pAttribute, CPSPositionAttribute( Position ) );

	return AS3_Array( "PtrType, PtrType", pAttribute, static_cast<IMeshAttribute*>(pAttribute) );
}

AS3_Val CreatePSTextureAttribute( void* pSelf, AS3_Val Args )
{
	CPSTextureAttribute*	pAttribute;

	ALCHEMY_DEBUG_NEW(pAttribute, CPSTextureAttribute );

	return AS3_Array( "PtrType, PtrType", pAttribute, static_cast<IMeshAttribute*>(pAttribute) );
}

AS3_Val CreatePSVelocityAttribute( void* pSelf, AS3_Val Args )
{
	CPSVelocityAttribute*	pAttribute;
	double fMin, fMax;

	AS3_ArrayValue( Args, "DoubleType, DoubleType", &fMin, &fMax );

	ALCHEMY_DEBUG_NEW(pAttribute, CPSVelocityAttribute( (FLOAT)fMin, (FLOAT)fMax ) );

	return AS3_Array( "PtrType, PtrType", pAttribute, static_cast<IMeshAttribute*>(pAttribute) );
}

AS3_Val CreateMirror(void* pSelf, AS3_Val Args)
{
	double dbWidth, dbHeight;
	int nNormalMap, nMaterial, nIsYUp;
	int nReflectionMapShift, nRefractionMapShift;
	unsigned int uWidthSegment, uHeightSegment, uEffectInfoCount = 0;
	ICompiler* pCompiler;
	const CEffectCode::CPassDefiner::EFFECTINFO* pEffectInfos = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "DoubleType, DoubleType, IntType, IntType, IntType, IntType, IntType, PtrType, IntType, IntType",  
		&dbWidth, &dbHeight, &uWidthSegment, &uHeightSegment, &nReflectionMapShift, &nNormalMap, &nMaterial, &pEffectInfos, &uEffectInfoCount, &nIsYUp);

	if(pCompiler)
	{
		CFlashMirror* pMirror;
		ALCHEMY_DEBUG_NEW(pMirror, CFlashMirror);

		pMirror->Create(
			static_cast<FLOAT>(dbWidth), 
			static_cast<FLOAT>(dbHeight),
			uWidthSegment, 
			uHeightSegment, 
			nReflectionMapShift, 
			nNormalMap,
			static_cast<CGraphicsResource::HANDLE>(nMaterial),
			pEffectInfos, 
			uEffectInfoCount,
			static_cast<bool>(nIsYUp) );

		return AS3_Array( "PtrType, PtrType, PtrType", pMirror, static_cast<CFlashObjectInterface*>(pMirror), static_cast<CSceneNode*>(pMirror) );
	}

	return 0;
}

AS3_Val CreateStaticMeshInstance(void* pSelf, AS3_Val Args)
{
	CFlashStaticMeshInstance* pStaticMeshInstance;
	INT nEffect;
	CFlashRenderMethod* pRenderMethod;

	AS3_ArrayValue(Args,"IntType, PtrType", &nEffect, &pRenderMethod);

	ALCHEMY_DEBUG_NEW(pStaticMeshInstance, CFlashStaticMeshInstance);

	if(pStaticMeshInstance)
	{
		if( pStaticMeshInstance->Create( (CGraphicsResource::HANDLE)nEffect, pRenderMethod ) )
			return AS3_Array( "PtrType, PtrType, PtrType", pStaticMeshInstance, static_cast<CFlashObjectInterface*>(pStaticMeshInstance), static_cast<CSceneNode*>(pStaticMeshInstance) );
	}

	return 0;
}

AS3_Val CreateWater(void* pSelf, AS3_Val Args)
{
	double dbWidth, dbHeight;
	int nMaterial, nIsYUp;
	int nReflectionMapShift, nRefractionMapShift;
	unsigned int uNormalMap, uWidthSegment, uHeightSegment, uEffectInfoCount = 0;
	ICompiler* pCompiler;
	const CEffectCode::CPassDefiner::EFFECTINFO* pEffectInfos = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "DoubleType, DoubleType, IntType, IntType, IntType, IntType, IntType, IntType, PtrType, IntType",  
		&dbWidth, &dbHeight, &uWidthSegment, &uHeightSegment, &nReflectionMapShift, &nRefractionMapShift, &uNormalMap, &nMaterial, &pEffectInfos, &uEffectInfoCount);

	if(pCompiler)
	{
		CFlashWater* pWater;
		ALCHEMY_DEBUG_NEW(pWater, CFlashWater);

		pWater->Create(
			static_cast<FLOAT>(dbWidth), 
			static_cast<FLOAT>(dbHeight),
			uWidthSegment,
			uHeightSegment,
			nReflectionMapShift, 
			nRefractionMapShift, 
			uNormalMap,
			static_cast<CGraphicsResource::HANDLE>(nMaterial),
			pEffectInfos, 
			uEffectInfoCount);

		return AS3_Array( "PtrType, PtrType, PtrType", pWater, static_cast<CFlashObjectInterface*>(pWater), static_cast<CSceneNode*>(pWater) );
	}

	return 0;
}

AS3_Val CreateCamera(void* pSelf, AS3_Val Args)
{
	CFlashCamera* pCamera;
	ALCHEMY_DEBUG_NEW(pCamera, CFlashCamera);

	return AS3_Array( "PtrType, PtrType, PtrType", pCamera, static_cast<CFlashObjectInterface*>(pCamera), static_cast<CSceneNode*>(pCamera) );
}

AS3_Val CreateScene(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	CMeshManager::HANDLE hSceneData;
	INT nSkinType, nFlag;
	CFlashSceneManager::RESOURCEEINFO* pMaterialInfo = ALCHEMY_NULL;
	CFlashSceneManager::RESOURCEEINFO* pTextureInfo = ALCHEMY_NULL;
	CFlashSceneManager::RESOURCEEINFO* pMeshInfo = ALCHEMY_NULL;

	static const UINT MATERIAL_BIT = 0;
	static const UINT TEXTURE_BIT = 1;
	static const UINT MESH_BIT = 2;

	AS3_ArrayValue(Args,"PtrType, IntType, IntType, PtrType, PtrType, PtrType", &pSceneManager, &hSceneData, &nSkinType, &nFlag, &pMaterialInfo, &pTextureInfo, &pMeshInfo);

	static CVector<CFlashSceneManager::RESOURCEEINFO> MaterialInfo;
	static CVector<CFlashSceneManager::RESOURCEEINFO> TextureInfo;
	static CVector<CFlashSceneManager::RESOURCEEINFO> MeshInfo;

	if(!pMaterialInfo && !pTextureInfo && !pMeshInfo && pSceneManager)
	{
		MaterialInfo.Clear();
		TextureInfo.Clear();
		MeshInfo.Clear();
		CFlashSceneNode* pSceneNode = static_cast<CFlashSceneNode*>( pSceneManager->CreateScene(hSceneData, (CSkinMeshController::SKINTYPE)nSkinType,
			ALCHEMY_NULL, ALCHEMY_TEST_BIT(nFlag, MATERIAL_BIT) ? &MaterialInfo : ALCHEMY_NULL, 
			ALCHEMY_TEST_BIT(nFlag, TEXTURE_BIT) ? &TextureInfo : ALCHEMY_NULL,
			ALCHEMY_TEST_BIT(nFlag, MESH_BIT) ? &MeshInfo : ALCHEMY_NULL) 
			);

		return AS3_Array( "AS3ValType, IntType, IntType, IntType",  static_cast<const AS3TYPE>(*pSceneNode), MaterialInfo.GetLength(), TextureInfo.GetLength(), MeshInfo.GetLength() );
	}
	else
	{
		UINT uLen;
		UINT i;

		if(pMaterialInfo)
		{
			uLen = MaterialInfo.GetLength();
			for (i=0; i<uLen; ++i)
			{
				pMaterialInfo[i].DataHandle = MaterialInfo[i].DataHandle;
				pMaterialInfo[i].ResourceHandle = MaterialInfo[i].ResourceHandle;
			}
		}

		if(pTextureInfo)
		{
			uLen = TextureInfo.GetLength();
			for (i=0; i<uLen; ++i)
			{
				pTextureInfo[i].DataHandle = TextureInfo[i].DataHandle;
				pTextureInfo[i].ResourceHandle = TextureInfo[i].ResourceHandle;
			}
		}

		if(pMeshInfo)
		{
			uLen = MeshInfo.GetLength();
			for (i=0; i<uLen; ++i)
			{
				pMeshInfo[i].DataHandle = MeshInfo[i].DataHandle;
				pMeshInfo[i].ResourceHandle = MeshInfo[i].ResourceHandle;
			}
		}
	}

	return 0;
}

AS3_Val CreateSceneObject(void* pSelf, AS3_Val Args)
{
	//CFlashSceneManager* pSceneManager;
	CFlashSceneObject* pSceneObject;
	UINT uMesh, uMaterial, uSubset;

	AS3_ArrayValue(Args, "IntType, IntType, IntType, PtrType", &uMesh, &uSubset, &uMaterial, &pSceneObject);

	if(!uMesh && !uMaterial && !uSubset && !pSceneObject)
	{
		ALCHEMY_DEBUG_NEW(pSceneObject, CFlashSceneObject);
		return AS3_Array( "PtrType, PtrType, PtrType", pSceneObject, static_cast<CFlashObjectInterface*>(pSceneObject), static_cast<CSceneNode*>(pSceneObject) );
	}
	else
	{
		if( pSceneObject->Create(uMesh, uSubset, 0, ALCHEMY_NULL, uMaterial) )
			return AS3_Int(1);

		return 0;
	}


	/*if(pSceneManager)
	{
		pSceneObject = static_cast<CFlashSceneObject*>( pSceneManager->CreateSceneObject(uMesh, uSubset, uMaterial, uRenderMethod, pSceneObject) );

		if(pSceneObject)
			return static_cast<const AS3TYPE>(*pSceneObject);

		return 0;
	}

	if(!pSceneObject)
	{
		ALCHEMY_DEBUG_NEW(pSceneObject, CFlashSceneObject);

		return AS3_Array( "PtrType, PtrType, PtrType", pSceneObject, static_cast<CFlashObjectInterface*>(pSceneObject), static_cast<CSceneNode*>(pSceneObject) );
	}

	return static_cast<const AS3TYPE>(*pSceneObject);*/

	return static_cast<const AS3TYPE>(*pSceneObject);
}

AS3_Val CreateParticleSystem(void* pSelf, AS3_Val Args)
{
	CFlashParticleSystem* pParticleSystem;
	CParticleController* pParticleController;
	INT nMaxParticleCount, nEffectHandle, nMaterialHandle;
	CFlashRenderMethod* pRenderMethod;
	IMeshFiller* pMeshFiller;

	AS3_ArrayValue(Args,"PtrType, IntType, PtrType, IntType, PtrType, IntType", &pParticleController, &nMaxParticleCount, &pMeshFiller, &nEffectHandle, &pRenderMethod, &nMaterialHandle);

	ALCHEMY_DEBUG_NEW(pParticleSystem, CFlashParticleSystem);

	pParticleSystem->Create(*pParticleController, nMaxParticleCount, pMeshFiller, (CGraphicsResource::HANDLE)nEffectHandle, 
				*pRenderMethod, (CGraphicsResource::HANDLE)nMaterialHandle);


	return AS3_Array( "PtrType, PtrType, PtrType", pParticleSystem, static_cast<CFlashObjectInterface*>(pParticleSystem), static_cast<CSceneNode*>(pParticleSystem) );
}

AS3_Val CreateBoneData(void* pSelf, AS3_Val Args)
{
	CSkinInfo::BONE* pBoneData;
	UINT nFluence = 0;
	UINT nameLen = 0;

	AS3_ArrayValue(Args,"PtrType, IntType, IntType",&pBoneData, &nFluence,&nameLen);

	ALCHEMY_DEBUG_NEW(pBoneData->pcName,CHAR[nameLen]);

	ALCHEMY_DEBUG_NEW(pBoneData->pfWeights,FLOAT[nFluence]);
	ALCHEMY_DEBUG_NEW(pBoneData->puIndices,UINT[nFluence]);

	pBoneData->uInfluenceCount = nFluence;

	return AS3_Array("PtrType, PtrType, PtrType, PtrType", pBoneData->pcName, pBoneData->pfWeights,pBoneData->puIndices,&(pBoneData->OffsetMatrix));
}

AS3_Val CreateSkinInfoData(void* pSelf, AS3_Val Args)
{
	INT len = AS3_IntValue(Args);
	CSkinInfo::BONE* pBoneData;

	ALCHEMY_DEBUG_NEW(pBoneData,CSkinInfo::BONE[len]);

	return AS3_Ptr(pBoneData);
}

AS3_Val CreateSkinInfo(void* pSelf, AS3_Val Args)
{
	CSkinInfo* pSkinInfo;

	ALCHEMY_DEBUG_NEW(pSkinInfo,CSkinInfo);

	return AS3_Ptr(pSkinInfo);
}

AS3_Val CreateAniamtionSet(void* pSelf, AS3_Val Args)
{
	CAnimationSet* pAnimSet = new CAnimationSet();

	return AS3_Ptr(pAnimSet);
}

AS3_Val CreateAnimationController(void* pSelf, AS3_Val Args)
{
	CAnimationController* pAnimController = new CAnimationController();
	UINT maxOutputCnt,maxAnimationSetCnt;

	AS3_ArrayValue(Args,"IntType, IntType",&maxOutputCnt, &maxAnimationSetCnt);

	pAnimController->Create(maxOutputCnt,maxAnimationSetCnt);

	return AS3_Ptr(pAnimController);
}

AS3_Val CreateAnimationTracker(void* pSelf, AS3_Val Args)
{
	UINT nameLen, frameLen;
	CAnimationSet::LPANIMATION pAnimData;

	AS3_ArrayValue(Args,"PtrType, IntType, IntType",&pAnimData, &nameLen, &frameLen);

	ALCHEMY_DEBUG_NEW(pAnimData->pcName,CHAR[nameLen]);
	ALCHEMY_DEBUG_NEW(pAnimData->pfTime,FLOAT[frameLen]);
	ALCHEMY_DEBUG_NEW(pAnimData->pTransformMatrices,MATRIX4X4[frameLen]);

	pAnimData->uFrameCount = frameLen;

	return AS3_Array("PtrType, PtrType, PtrType", pAnimData->pcName, pAnimData->pfTime, pAnimData->pTransformMatrices);
}

AS3_Val CreateAnimationSetData(void* pSelf, AS3_Val Args)
{
	UINT trackCount;
	CAnimationSet::LPANIMATION pAnimAry;

	trackCount = static_cast<UINT>( AS3_IntValue(Args) );

	ALCHEMY_DEBUG_NEW(pAnimAry,CAnimationSet::ANIMATION[trackCount])

	return AS3_Ptr(pAnimAry);
}

AS3_Val CreateSkinMeshController(void* pSelf, AS3_Val Args)
{
	CSkinMeshController* pSkinMeshController;
	CSkinInfo* pSkinInfo;
	CBone* pRoot;
	CFlashMesh* pMesh;
	INT skinType;

	AS3_ArrayValue(Args,"PtrType, IntType, PtrType, PtrType",&pMesh, &skinType, &pSkinInfo, &pRoot);

	ALCHEMY_DEBUG_NEW(pSkinMeshController, CSkinMeshController);

	bool bRet = pSkinMeshController->Create(*pMesh, (CSkinMeshController::SKINTYPE)skinType, *pSkinInfo, *pRoot, 30);

	return bRet == true ? AS3_Ptr( pSkinMeshController ) : 0;
}

AS3_Val CreateDeferredLightRenderer(void* pSelf, AS3_Val Args)
{
	CDeferredLightRenderer* pDeferredLightRenderer;
	int nAORandomMap, nAONumDirectionMap, nAOStep, nOutputRenderTargetMap;

	AS3_ArrayValue(Args, "IntType, IntType, IntType, IntType", &nAORandomMap, &nAONumDirectionMap, &nAOStep, &nOutputRenderTargetMap);

	ALCHEMY_DEBUG_NEW( pDeferredLightRenderer, CSmartClass<CDeferredLightRenderer> );
	pDeferredLightRenderer->Create(nAORandomMap, nAONumDirectionMap, nAOStep, nOutputRenderTargetMap);

	return AS3_Ptr(pDeferredLightRenderer);
}

AS3_Val CreateEffectCode(void* pSelf, AS3_Val Args)
{
	ICompiler* pCompiler;
	CEffectCode* pEffectCode;
	CEffectCode::CPassDefiner* pPassDefiner;

	AS3_ArrayValue(Args, "PtrType", &pCompiler);

	if(pCompiler)
	{
		ALCHEMY_DEBUG_NEW( pEffectCode, CEffectCode(*pCompiler) );
		ALCHEMY_DEBUG_NEW( pPassDefiner, CEffectCode::CPassDefiner(*pEffectCode) );

		return AS3_Array("PtrType, PtrType", pEffectCode, pPassDefiner);
	}

	return 0;
}

AS3_Val CreateParticleDataCode(void* pSelf, AS3_Val Args)
{
	CParticleDataCode* pCode;
	CParticleDataCode::CParticleLogicDefiner* pDefiner;

	ALCHEMY_DEBUG_NEW(pCode, CParticleDataCode);
	ALCHEMY_DEBUG_NEW(pDefiner, CParticleDataCode::CParticleLogicDefiner(*pCode));

	return AS3_Array("PtrType, PtrType", pCode, pDefiner);
}

AS3_Val CreateEffectCodeFromDeferredRenderingPipeline(void* pSelf, AS3_Val Args)
{
	ICompiler* pCompiler;
	CEffectCode* pEffectCode;
	CEffectCode::CPassDefiner* pPassDefiner;
	CFlashSceneManager* pSceneManager;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pCompiler, &pSceneManager);

	if(pCompiler)
	{
		ALCHEMY_DEBUG_NEW( pEffectCode, CEffectCode(*pCompiler) );
		if(pSceneManager)
			pPassDefiner = &pSceneManager->GetDeferredRenderingPipeline().CreatePassDefiner(*pEffectCode);
		else
			ALCHEMY_DEBUG_NEW( pPassDefiner, CEffectCode::CPassDefiner(*pEffectCode) );

		return AS3_Array("PtrType, PtrType", pEffectCode, pPassDefiner);
	}

	return 0;
}

AS3_Val CreatePositionEffect(void* pSelf, AS3_Val Args)
{
	CPositionEffect* pPositionEffect;

	ALCHEMY_DEBUG_NEW(pPositionEffect, CPositionEffect);

	return AS3_Array( "PtrType, PtrType", pPositionEffect, static_cast<IEffectInfo*>(pPositionEffect) );
}

AS3_Val CreatePosition2DEffect(void* pSelf, AS3_Val Args)
{
	CPosition2DEffect* pPositionEffect;

	ALCHEMY_DEBUG_NEW(pPositionEffect, CPosition2DEffect);

	return AS3_Array( "PtrType, PtrType", pPositionEffect, static_cast<IEffectInfo*>(pPositionEffect) );
}

AS3_Val CreateTextureEffect(void* pSelf, AS3_Val Args)
{
	CTextureEffect* pTextureEffect;
	INT nSamplerIndex, nTexcoordIndex;
	IEffect::SAMPLERDECLARATION* pSamplerData;

	AS3_ArrayValue(Args,"IntType, IntType, PtrType",&nSamplerIndex, &nTexcoordIndex, &pSamplerData);

	ALCHEMY_DEBUG_NEW( pTextureEffect, CTextureEffect(nSamplerIndex,nTexcoordIndex,pSamplerData) );

	return AS3_Array( "PtrType, PtrType", pTextureEffect, static_cast<IEffectInfo*>(pTextureEffect) );

	return 0;
}

AS3_Val CreateForceTextureEffect(void* pSelf, AS3_Val Args)
{
	CForceTextureEffect* pTextureEffect;

	int nSamplerIndex = AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW( pTextureEffect, CForceTextureEffect(nSamplerIndex) );

	return AS3_Array( "PtrType, PtrType", pTextureEffect, static_cast<IEffectInfo*>(pTextureEffect) );

	return 0;
}

AS3_Val CreatePixelAmbientLightEffect(void* pSelf, AS3_Val Args)
{
	CPixelAmbientLightEffect* pPixelAmbientLightEffect;

	ALCHEMY_DEBUG_NEW( pPixelAmbientLightEffect, CPixelAmbientLightEffect );

	return AS3_Array( "PtrType, PtrType", pPixelAmbientLightEffect, static_cast<IEffectInfo*>(pPixelAmbientLightEffect) );

	return 0;
}

AS3_Val CreateCartoonEffect(void* pSelf, AS3_Val Args)
{
	CCartoonEffect* pCartoonEffect;

	int nSamplerIndex = AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW( pCartoonEffect, CCartoonEffect(nSamplerIndex) );

	return AS3_Array( "PtrType, PtrType" , pCartoonEffect, static_cast<IEffectInfo*>(pCartoonEffect));
}

AS3_Val CreateOutlineEffect(void* pSelf, AS3_Val Args)
{
	COutLineEffect* pOutlineEffect;

	ALCHEMY_DEBUG_NEW( pOutlineEffect, COutLineEffect );

	return AS3_Array("PtrType, PtrType", pOutlineEffect, static_cast<IEffectInfo*>(pOutlineEffect));
}

AS3_Val CreateFacingEffect(void* pSelf, AS3_Val Args)
{
	CFacingEffect* pFacingEffect;

	int nIsVertexShader = AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW( pFacingEffect, CFacingEffect(nIsVertexShader ? true :false) );

	return AS3_Array( "PtrType, PtrType", pFacingEffect, static_cast<IEffectInfo*>(pFacingEffect) );
}

AS3_Val CreatePixelColorEffect(void* pSelf, AS3_Val Args)
{
	CPixelColorEffect* pPixelEffect;

	FLOAT4* pColor = (FLOAT4*)AS3_PtrValue(Args);

	ALCHEMY_DEBUG_NEW(pPixelEffect, CPixelColorEffect( pColor ) );

	return AS3_Array( "PtrType, PtrType", pPixelEffect, static_cast<IEffectInfo*>(pPixelEffect) );
}

AS3_Val CreateEmptyEffect(void* pSelf, AS3_Val Args)
{
	CEmptyEffect* pEffect;

	ALCHEMY_DEBUG_NEW(pEffect, CEmptyEffect);

	return AS3_Array( "PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect) );
}

AS3_Val CreateVertexColorEffect(void* pSelf, AS3_Val Args)
{
	CVertexColorEffect* pEffect;
	INT nIndex;

	AS3_ArrayValue(Args, "IntType", &nIndex);

	ALCHEMY_DEBUG_NEW(pEffect, CVertexColorEffect( (UINT)nIndex) );

	return AS3_Array( "PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect) );
}

AS3_Val CreateDirectionalShdowDisplayEffect(void* pSelf, AS3_Val Args)
{
	CDirectionalShadowDisplayEffect* pEffect;
	INT nShadowMapIndex, nLightIndex, nCameraIndex;

	AS3_ArrayValue(Args, "IntType, IntType, IntType", &nShadowMapIndex, &nLightIndex, &nCameraIndex);

	ALCHEMY_DEBUG_NEW(pEffect, CDirectionalShadowDisplayEffect(nShadowMapIndex, nLightIndex, nCameraIndex) );

	return AS3_Array( "PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect) );
}

AS3_Val CreateAlphaTestBlend(void* pSelf, AS3_Val Args)
{
	CAlphaTestBlendMethod* pBlend;

	double fAlphaRef;

	AS3_ArrayValue(Args,"DoubleType", &fAlphaRef);

	ALCHEMY_DEBUG_NEW( pBlend, CAlphaTestBlendMethod(  ) );

	return AS3_Array( "PtrType, PtrType", pBlend, static_cast<IEffectInfoBlendMethod*>(pBlend) );
}

AS3_Val CreateDepthEffect(void* pSelf, AS3_Val Args)
{
	CDepthEffect* pEffect;
	INT nType = (INT)AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW(pEffect, CDepthEffect( (CDepthEffect::PROJECTIONTYPE)nType, true ) );

	return AS3_Array( "PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect) );
}

AS3_Val CreateDistanceInfoEffect(void* pSelf, AS3_Val Args)
{
	CDistanceInfoEffect* pEffect;
	double dFocusDis;
	
	AS3_ArrayValue(Args,"DoubleType", &dFocusDis);

	ALCHEMY_DEBUG_NEW(pEffect, CDistanceInfoEffect( FLOAT(dFocusDis) )  );


	return AS3_Array( "PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect) );
}

AS3_Val CreateShadowMapDisplayEffect(void* pSelf, AS3_Val Args)
{
	CShadowMapDisplayEffect* pDisplayEffect;

	INT nShadowIndex, nLightIndex, nCameraIndex;

	AS3_ArrayValue(Args,"IntType, IntType, IntType", &nShadowIndex, &nLightIndex, &nCameraIndex);

	ALCHEMY_DEBUG_NEW(pDisplayEffect, CShadowMapDisplayEffect(nShadowIndex,nLightIndex, nCameraIndex));

	CSceneManager* pSceneManager = CSceneManager::GetInstance();
	const CLight* pLight;
	if( !pSceneManager || ! (pLight = pSceneManager->GetLight(nLightIndex)) )
		return AS3_Array( "PtrType, PtrType, IntType", pDisplayEffect, static_cast<CShadowMapDisplayEffect*>(pDisplayEffect), 0 );

	return AS3_Array( "PtrType, PtrType, IntType", pDisplayEffect, static_cast<CShadowMapDisplayEffect*>(pDisplayEffect), pLight->GetShadowMap()->GetResourceHandle() );
}

AS3_Val CreatePixelFogBlend(void* pSelf, AS3_Val Args)
{
	CPixelFogBlendMethod* pFogBlend;

	INT nType;
	FLOAT* pFogEnd, *pDistance, *pFogDensity;

	AS3_ArrayValue(Args, "IntType, PtrType, PtrType, PtrType", &nType, &pFogEnd, &pDistance, &pFogDensity);

	ALCHEMY_DEBUG_NEW(pFogBlend, CPixelFogBlendMethod( (CPixelFogBlendMethod::FOG_TYPE)nType ) );

	pFogBlend->Create( pFogEnd, pDistance, pFogDensity );

	return AS3_Array( "PtrType, PtrType",  pFogBlend, static_cast<IEffectInfoBlendMethod*>(pFogBlend) );
}

AS3_Val CreateEnvironmentDisplayEffect(void* pSelf, AS3_Val Args)
{
	CEnvironmentalEffect* pEffect;

	ALCHEMY_DEBUG_NEW(pEffect,CEnvironmentalEffect );

	return AS3_Array( "PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect) );
}

AS3_Val CreateNormalMapEffect(void* pSelf, AS3_Val Args)
{
	CNormalMapEffect* pEffect;
	INT nUVIndex, nNoramlIndex, nNumLights;
	PUINT pLightsIndex;

	AS3_ArrayValue(Args, "IntType, IntType, IntType, PtrType", &nUVIndex, &nNoramlIndex, &nNumLights, &pLightsIndex);

	ALCHEMY_DEBUG_NEW(pEffect,CNormalMapEffect(nUVIndex, nNoramlIndex, nNumLights, pLightsIndex) );

	return AS3_Array("PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect));
}

AS3_Val CreateBillboardEffect(void* pSelf, AS3_Val Args)
{
	CBillboardEffect* pEffect;

	ALCHEMY_DEBUG_NEW(pEffect, CBillboardEffect);

	return AS3_Array("PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect));
}

AS3_Val CreateMaskTextureEffect(void* pSelf, AS3_Val Args)
{
	CMaskTextureEffect* pEffect;
	INT nChannelIndex, nSamplerIndex, nTexCoordIndex;
	IEffect::SAMPLERDECLARATION* pSamplerData;
	double dMaskReference;

	AS3_ArrayValue(Args,"IntType, DoubleType, IntType, IntType, PtrType", &nChannelIndex, &dMaskReference, &nSamplerIndex, &nTexCoordIndex, &pSamplerData);

	ALCHEMY_DEBUG_NEW(pEffect, CMaskTextureEffect( (UINT)nChannelIndex, (FLOAT)dMaskReference, (UINT)nSamplerIndex, (UINT)nTexCoordIndex, pSamplerData ) );

	return AS3_Array("PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect));
}

AS3_Val CreateSpecularTextureEffect(void* pSelf, AS3_Val Args)
{
	CSpecularTextureEffect* pEffect;
	INT nChannelIndex, nSamplerIndex, nTexCoordIndex;
	IEffect::SAMPLERDECLARATION* pSamplerData;

	AS3_ArrayValue(Args,"IntType, IntType, IntType, PtrType", &nChannelIndex, &nSamplerIndex, &nTexCoordIndex, &pSamplerData);

	ALCHEMY_DEBUG_NEW(pEffect, CSpecularTextureEffect( (UINT)nChannelIndex, (UINT)nSamplerIndex, (UINT)nTexCoordIndex, pSamplerData ) );

	return AS3_Array("PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect));
}

AS3_Val CreateCascadShadowDisplayEffect(void* pSelf, AS3_Val Args)
{
	CCascadShadowDisplay* pEffect;
	CCascadShadowMapRender* pShadowMap;
	INT nLightIndex;
	UINT* pSamplerIndices;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pShadowMap, &pSamplerIndices, &nLightIndex);

	if(pShadowMap && pSamplerIndices)
	{
		UINT uSamplerCount;
		const FLOAT* pDepthSegment = pShadowMap->GetDepthSegment(&uSamplerCount);

		ALCHEMY_DEBUG_NEW(pEffect, CCascadShadowDisplay(pSamplerIndices, pDepthSegment, pShadowMap->GetSplitCameraIndices(), uSamplerCount, (UINT)nLightIndex, pShadowMap->GetMainCameraIndex()) );
		return AS3_Array("PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect));
	}

	return 0;
}

AS3_Val CreateTextureBlurEffect(void* pSelf, AS3_Val Args)
{
	FLOAT2* pSamplerOffsets;
	FLOAT*	pSamplerWeights;
	INT	nSamplerAttributeNum, nSamplerIndex, nTexcoordIndex;
	IEffect::SAMPLERDECLARATION* pSamplerDeclaration;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType, IntType, IntType, PtrType", &pSamplerOffsets, &pSamplerWeights, &nSamplerAttributeNum, &nSamplerIndex, &nTexcoordIndex, &pSamplerDeclaration);

	CTextureBlurEffect* pBlurEffect;
	
	ALCHEMY_DEBUG_NEW(pBlurEffect, CTextureBlurEffect(pSamplerOffsets, pSamplerWeights, (UINT)nSamplerAttributeNum, (UINT)nSamplerIndex, (UINT)nTexcoordIndex, pSamplerDeclaration));

	return AS3_Array("PtrType, PtrType", pBlurEffect, static_cast<IEffectInfo*>(pBlurEffect));
}

AS3_Val CreateTextureModulusEffect(void* pSelf, AS3_Val Args)
{
	CTextureModulusEffect* pEffect;
	INT nChannelIndex, nSamplerIndex, nTexCoordIndex;
	IEffect::SAMPLERDECLARATION* pSamplerData;

	AS3_ArrayValue(Args,"IntType, IntType, IntType, PtrType", &nChannelIndex, &nSamplerIndex, &nTexCoordIndex, &pSamplerData);

	ALCHEMY_DEBUG_NEW(pEffect, CTextureModulusEffect( (UINT)nChannelIndex, (UINT)nSamplerIndex, (UINT)nTexCoordIndex, pSamplerData ) );

	return AS3_Array("PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect));
}

AS3_Val CreateBackgroundEffect(void* pSelf, AS3_Val Args)
{
	CBackgroundEffect* pEffect;
	ALCHEMY_DEBUG_NEW(pEffect, CBackgroundEffect);
	return AS3_Array("PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect));
}

AS3_Val CreateEffectInfoSub(void* pSelf, AS3_Val Args)
{
	CEffectInfoSub* pBlendMethod;
	INT nBlendFlag = (INT)AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW(pBlendMethod, CEffectInfoSub( CEffectInfoSub::BLENDFLAG(nBlendFlag) ) );

	return AS3_Array("PtrType, PtrType", pBlendMethod, static_cast<IEffectInfoBlendMethod*>(pBlendMethod) );
}

AS3_Val CreateBidirectionalBlendMethod(void* pSelf, AS3_Val Args)
{
	CBidirectionalBlendMethod* pBlendMethod;

	INT nBlendMethod = (INT)AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW(pBlendMethod, CBidirectionalBlendMethod( CBidirectionalBlendMethod::BLENDMETHOD(nBlendMethod) ) );

	return AS3_Array("PtrType, PtrType", pBlendMethod, static_cast<IEffectInfoBlendMethod*>(pBlendMethod) );
}

AS3_Val CreatePSMoveEffect(void* pSelf, AS3_Val Args)
{
	CPSMoveEffect* pEffect;

	ALCHEMY_DEBUG_NEW(pEffect, CPSMoveEffect);

	return AS3_Array("PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect));
}

AS3_Val CreateGrassAnimationEffect(void* pSelf, AS3_Val Args)
{
	CGrassAnimationEffect* pEffect;

	FLOAT* pOffset = reinterpret_cast<FLOAT*>( AS3_PtrValue(Args) );

	ALCHEMY_DEBUG_NEW(pEffect, CGrassAnimationEffect(pOffset) );

	return AS3_Array("PtrType, PtrType", pEffect, static_cast<IEffectInfo*>(pEffect));
}

AS3_Val CreateEffectInfoAddition(void* pSelf, AS3_Val Args)
{
	CEffectInfoAddition* pEffectInfoAddition;
	INT nBlendAlpha = (INT)AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW( pEffectInfoAddition, CEffectInfoAddition( (CEffectInfoAddition::BLENDFLAG)nBlendAlpha ) );

	return AS3_Array( "PtrType, PtrType" , pEffectInfoAddition, static_cast<IEffectInfoBlendMethod*>(pEffectInfoAddition) );
}

AS3_Val CreateEffectInfoModulation(void* pSelf, AS3_Val Args)
{
	CEffectInfoModulation* pEffectInfoModulation;
	INT nBlendAlpha = (INT)AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW( pEffectInfoModulation, CEffectInfoModulation( (CEffectInfoModulation::BLENDFLAG)nBlendAlpha ) );

	return AS3_Array("PtrType, PtrType", pEffectInfoModulation, static_cast<IEffectInfoBlendMethod*>(pEffectInfoModulation));
}

AS3_Val CreateFresnelBlendMethod(void* pSelf, AS3_Val Args)
{
	CFresnelBlendMethod* pFresnelBlendMethod;

	ALCHEMY_DEBUG_NEW(pFresnelBlendMethod, CFresnelBlendMethod);

	return AS3_Array("PtrType, PtrType", pFresnelBlendMethod, static_cast<IEffectInfoBlendMethod*>(pFresnelBlendMethod));
}

AS3_Val CreateMulityEffectProgram(void* pSelf, AS3_Val Args)
{
	CMulityEffectProgram* pProgram;
	INT nClearColor, nClearFlag, hRenderTarget, uCameraIndex;

	AS3_ArrayValue(Args, "IntType, IntType, IntType, IntType", &nClearColor, &nClearFlag, &hRenderTarget, &uCameraIndex);

	ALCHEMY_DEBUG_NEW(pProgram, CMulityEffectProgram);
	if( pProgram && pProgram->Create( (ARGBCOLOR)nClearColor, (UINT)nClearFlag, (CGraphicsResource::HANDLE)hRenderTarget, uCameraIndex ) )
		return AS3_Array("PtrType, PtrType", pProgram, static_cast<CRenderer*>(pProgram));

	return 0;
}

AS3_Val CreateSkyPassDefiner(void* pSelf, AS3_Val Args)
{
	CEffectCode* pCode;
	CSkyPassDefiner* pSkyPassDefiner;
	ICompiler* pCompiler;
	FLOAT* pOffset;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pCompiler, &pOffset);

	if(pCompiler)
	{
		ALCHEMY_DEBUG_NEW(pCode, CEffectCode(*pCompiler) );
		ALCHEMY_DEBUG_NEW(pSkyPassDefiner, CSkyPassDefiner(*pCode, pOffset) );

		return AS3_Array("PtrType, PtrType", pCode, pSkyPassDefiner);
	}

	return 0;
}

AS3_Val CreateShadowMapRender(void* pSelf, AS3_Val Args)
{
	CShadowMapRender* pRender;
	INT nLightIndex, nShadowType;
	UINT uCameraIndex;

	AS3_ArrayValue(Args, "IntType, IntType", &nLightIndex, &nShadowType);

	ALCHEMY_DEBUG_NEW(pRender, CShadowMapRender);

	uCameraIndex = pRender->Create( (UINT)nLightIndex, (CShadowMapRender::SHADOWLIGHTTYPE)nShadowType );
	
	if(uCameraIndex)
		return AS3_Array( "PtrType, PtrType, PtrType, IntType", pRender, static_cast<CAbstractProgramRender*>(pRender), static_cast<CRenderer*>(pRender), uCameraIndex );

	ALCHEMY_DEBUG_DELETE(pRender);

	return 0;
}

AS3_Val CreateBeforePostProcessRender(void* pSelf, AS3_Val Args)
{
	CPostProcessRender* pRender;
	INT nMaterial, nEffect, nRenderPass, nOutputRenderTarget, nSceneMesh, nRenderQueue;
	CFlashRenderMethod* pRenderMethod;

	AS3_ArrayValue(Args, "IntType, PtrType, IntType, IntType, IntType, IntType, IntType", &nMaterial, &pRenderMethod, &nEffect, &nRenderPass, &nOutputRenderTarget, &nSceneMesh, &nRenderQueue);

	ALCHEMY_DEBUG_NEW(pRender, CPostProcessRender);
	if( pRender->Create( (CGraphicsResource::HANDLE)nMaterial, *pRenderMethod, (CGraphicsResource::HANDLE)nEffect, (UINT)nRenderPass, (CGraphicsResource::HANDLE)nOutputRenderTarget, (CGraphicsResource::HANDLE)nSceneMesh, (CRenderQueue::RENDERENTRYTYPE)nRenderQueue ) )
		return AS3_Array("PtrType, PtrType", pRender, static_cast<CRenderer*>(pRender));

	return 0;
}

AS3_Val CreateScenePostProcessRender(void* pSelf, AS3_Val Args)
{
	CPostProcessRender* pRender;
	INT nMaterial, nRenderTargetIndex, nEffect, nRenderPass, nOutputRenderTarget, nSceneMesh, nRenderQueue;
	CFlashRenderMethod* pRenderMethod;

	AS3_ArrayValue(Args, "IntType, IntType, PtrType, IntType, IntType, IntType, IntType, IntType", &nMaterial, &nRenderTargetIndex, &pRenderMethod, &nEffect, &nRenderPass, &nOutputRenderTarget, &nSceneMesh, &nRenderQueue);

	ALCHEMY_DEBUG_NEW(pRender, CPostProcessRender);
	if( pRender->Create( (CGraphicsResource::HANDLE)nMaterial, (UINT)nRenderTargetIndex, *pRenderMethod, (CGraphicsResource::HANDLE)nEffect, (UINT)nRenderPass, (CGraphicsResource::HANDLE)nOutputRenderTarget, (CGraphicsResource::HANDLE)nSceneMesh, (CRenderQueue::RENDERENTRYTYPE)nRenderQueue ) )
		return AS3_Array("PtrType, PtrType", pRender, static_cast<CRenderer*>(pRender));

	return 0;
}

AS3_Val CreateEnvironmentRender(void* pSelf, AS3_Val Args)
{
	CEnvironmentRender* pRender;
	CSceneNode* pNode;
	INT uEnvironmentSize;

	AS3_ArrayValue(Args, "IntType, PtrType", &uEnvironmentSize, &pNode);

	ALCHEMY_DEBUG_NEW(pRender, ENVIRONMENTRENDER);

	if( pRender->Create( uEnvironmentSize, pNode->GetWorldMatrix() ) )
		return AS3_Array( "PtrType, PtrType", pRender, static_cast<CRenderer*>(pRender) );

	return 0;
}

AS3_Val CreateNormalMeshProcessor(void* pSelf, AS3_Val Args)
{
	CNormalMeshProcessor* pMeshProcessor;

	ALCHEMY_DEBUG_NEW(pMeshProcessor, CNormalMeshProcessor() );

	return AS3_Array("PtrType, PtrType", pMeshProcessor, static_cast<IMeshProcessor*>(pMeshProcessor) );
}

AS3_Val CreateSkinMeshProcessor(void* pSelf, AS3_Val Args)
{
	CSkinMeshProcessor* pMeshProcessor;

	INT skinType;
	CSkinInfo* pSkinInfo;
	CFlashBone* pRoot;

	AS3_ArrayValue(Args,"IntType, PtrType, PtrType",&skinType, &pSkinInfo, &pRoot);

	ALCHEMY_DEBUG_NEW(pMeshProcessor, CSkinMeshProcessor( (CSkinMeshController::SKINTYPE)skinType, *pSkinInfo,  *pRoot, ALCHEMY_NULL, 0, 30) );

	return AS3_Array("PtrType, PtrType", pMeshProcessor, static_cast<IMeshProcessor*>(pMeshProcessor) );
}

AS3_Val CreateSilhoutteProgram(void* pSelf, AS3_Val Args)
{
	CSilhouetteProgram* pProgram;
	//CRenderInfo* pRenderInfo;
	CSceneNode* pNode;
	int isUpdate, nMesh;
	FLOAT fWidthModulus;
	CARGBColor* pColor;

	AS3_ArrayValue(Args, "PtrType, IntType, DoubleType, PtrType, IntType", &pNode, &nMesh, &fWidthModulus, &pColor, &isUpdate);

	ALCHEMY_DEBUG_NEW(pProgram, SILHOUETTEPROGRAM);

	if( pProgram->Create( &pNode->GetWorldMatrix(), (CGraphicsResource::HANDLE)nMesh,  *pColor, (FLOAT)fWidthModulus, (bool)isUpdate ) )
		return AS3_Array( "PtrType", pProgram );

	return 0;
}

AS3_Val CreateDepthFieldEffect(void* pSelf, AS3_Val Args)
{
	CDepthFieldEffect* pEffect;
	ALCHEMY_DEBUG_NEW(pEffect, CDepthFieldEffect);

	double fDistance, fBlue;

	AS3_ArrayValue(Args, "DoubleType, DoubleType", &fDistance, &fBlue);

	bool bRet = pEffect->Create(fDistance, fBlue);

	return bRet == true ? AS3_Array( "PtrType", pEffect) : 0; 
}

AS3_Val CreateContorEffect(void* pSelf, AS3_Val Args)
{
	CContortEffect* pEffect;
	ALCHEMY_DEBUG_NEW(pEffect, CContortEffect);

	double fRadius, fScale;

	AS3_ArrayValue(Args, "DoubleType, DoubleType", &fRadius, &fScale);

	bool bRet = pEffect->Create(fScale, fRadius);

	return bRet == true ? AS3_Array( "PtrType", pEffect) : 0; 
}

AS3_Val CreateSnowParticleCompute(void* pSelf, AS3_Val Args)
{
	CSnowParticleCompute* pSnowCompute;
	ALCHEMY_DEBUG_NEW(pSnowCompute, CSnowParticleCompute);

	FLOAT3* pMinEdge, *pMaxEdge;
	double dMinLiveTime, dMaxLiveTime, dGroundPos;

	AS3_ArrayValue(Args,"PtrType, PtrType, DoubleType, DoubleType, DoubleType,", &pMinEdge, &pMaxEdge, &dMinLiveTime, &dMaxLiveTime,
		&dGroundPos);

	pSnowCompute->Create(*pMinEdge, *pMaxEdge, (FLOAT)dMinLiveTime, (FLOAT)dMaxLiveTime, (FLOAT)dGroundPos );

	return AS3_Array("PtrType, PtrType", pSnowCompute, static_cast<CParticleComputeInfoInterface*>(pSnowCompute) );
}

AS3_Val CreateAccelerationVelocityVC(void* pSelf, AS3_Val Args)
{
	CAccelerationVC* pVC;
	FLOAT3* pAccelerateVelocity;
	double dPhi;

	AS3_ArrayValue(Args, "PtrType, DoubleType", &pAccelerateVelocity, &dPhi);

	ALCHEMY_DEBUG_NEW(pVC, CAccelerationVC(*pAccelerateVelocity, dPhi));

	return AS3_Array("PtrType, PtrType", pVC, static_cast<CParticleVariableComputeInfoInterface*>(pVC) );
}

AS3_Val CreateVelocityVC(void* pSelf, AS3_Val Args)
{
	CVelocityVC* pVC;
	double dMaxVelocity, dMinVelocity;

	AS3_ArrayValue(Args, "DoubleType, DoubleType", &dMinVelocity, &dMaxVelocity);

	ALCHEMY_DEBUG_NEW(pVC, CVelocityVC(dMinVelocity, dMaxVelocity));

	return AS3_Array("PtrType, PtrType", pVC, static_cast<CParticleVariableComputeInfoInterface*>(pVC) );
}

AS3_Val CreateVolumeLightShow(void* pSelf, AS3_Val Args)
{
	CFlashVolumeLightShow* pVolumenLight;
	INT uLightIndex, uNumSamples, nOCCTexture;
	double dDensity, dExposure, dWeight, dDecy;

	AS3_ArrayValue(Args, "IntType, IntType, IntType, DoubleType, DoubleType, DoubleType, DoubleType",&nOCCTexture, &uLightIndex, &uNumSamples, &dDensity, &dExposure, &dDecy, &dWeight);

	ALCHEMY_DEBUG_NEW(pVolumenLight, CFlashVolumeLightShow);

	if( pVolumenLight->Create((CGraphicsResource::HANDLE)nOCCTexture, uLightIndex, uNumSamples, (FLOAT)dDensity, (FLOAT)dExposure, (FLOAT)dDecy, (FLOAT)dWeight) )
		return AS3_Array("PtrType, PtrType, PtrType", pVolumenLight, static_cast<CRenderer*>(pVolumenLight), static_cast<CFlashObjectInterface*>(pVolumenLight) );

	return 0;
}

//////////////////////////////////////////////////////////////////////////
///Destroy
//////////////////////////////////////////////////////////////////////////
AS3_Val DestroyBuffer(void* pSelf, AS3_Val Args)
{
	PUINT8 puBuffer = reinterpret_cast<PUINT8>( AS3_PtrValue(Args) );

	ALCHEMY_DEBUG_DELETE_ARRAY(puBuffer);

	return 0;
}

AS3_Val DestroyMeshData(void* pSelf, AS3_Val Args)
{
	CMeshManager::MESHDATA* pMeshData =  reinterpret_cast<CMeshManager::MESHDATA*>(AS3_PtrValue(Args));

	if(pMeshData)
	{
		if(pMeshData->ppVertexData)
		{
			UINT uVertexBufferSize = CVertexDeclaration::GetVertexElementMaxinumStream(pMeshData->VertexElementData.pVertexElement, pMeshData->VertexElementData.uElementCount) + 1;

			for(UINT i = 0; i < uVertexBufferSize; ++ i)
				ALCHEMY_DEBUG_DELETE_ARRAY(pMeshData->ppVertexData[i]);

			ALCHEMY_DEBUG_DELETE_ARRAY(pMeshData->ppVertexData);
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(pMeshData->VertexElementData.pVertexElement);
		ALCHEMY_DEBUG_DELETE_ARRAY(pMeshData->pAttributeRange);
		ALCHEMY_DEBUG_DELETE_ARRAY(pMeshData->pIndexData);

		ALCHEMY_DEBUG_DELETE(pMeshData);
	}

	return 0;
}

AS3_Val DestroyTextureData(void* pSelf, AS3_Val Args)
{
	CMeshManager::TEXTUREDATA* pTextureData = reinterpret_cast<CMeshManager::TEXTUREDATA*>( AS3_PtrValue(Args) );
	if(pTextureData)
	{
		if(pTextureData->pData)
		{
			PUINT8 pBytes = reinterpret_cast<PUINT8>(pTextureData->pData);
			ALCHEMY_DEBUG_DELETE_ARRAY(pBytes);
		}

		ALCHEMY_DEBUG_DELETE(pTextureData);
	}

	return 0;
}

AS3_Val DestroySceneData(void* pSelf, AS3_Val Args)
{
	CMeshManager::SCENEDATA* pSceneData = reinterpret_cast<CMeshManager::SCENEDATA*>( AS3_PtrValue(Args) );
	if(pSceneData)
	{
		if(pSceneData->uSceneDataCount > 1)
		{
			for (UINT i=0; i<pSceneData->uSceneDataCount; ++i)
			{
				if(pSceneData->pSceneNodeData[i].uNameLen)
				{
					ALCHEMY_DEBUG_DELETE_ARRAY( pSceneData->pSceneNodeData[i].pcName );
					pSceneData->pSceneNodeData[i].uNameLen = 0;
				}

				/*if(pSceneData->pSceneNodeData[i].pAnimationController)
					pSceneData->pSceneNodeData[i].pAnimationController->Release();

				if(pSceneData->pSceneNodeData[i].pSkinInfo)
					pSceneData->pSceneNodeData[i].pSkinInfo->Release();*/
			}

			ALCHEMY_DEBUG_DELETE_ARRAY(pSceneData->pSceneNodeData);
		}
		else
			ALCHEMY_DEBUG_DELETE(pSceneData->pSceneNodeData);

		pSceneData->uSceneDataCount = 0;
	}

	return 0;
}

AS3_Val DestroyMaterialData(void* pSelf, AS3_Val Args)
{
	CMeshManager::LPMATERIALDATA pMaterialData =  reinterpret_cast<CMeshManager::LPMATERIALDATA>(AS3_PtrValue(Args));

	PUINT8 pDeleteData;

	if(pMaterialData)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(pMaterialData->pTextureDataHandles);
		ALCHEMY_DEBUG_DELETE_ARRAY(pMaterialData);
	}

	return 0;
}

AS3_Val DestroyParameterList(void* pSelf, AS3_Val Args)
{
	CRenderMethod::LPPARAMETERLIST pParameterList, pCurrentParameter;
	UINT uLength;
	AS3_ArrayValue(Args, "PtrType, IntType", &pParameterList, &uLength);

	//if(pParameterList)
	//{
	//	for(UINT i = 0; i < uLength; ++ i)
	//	{
	//		pCurrentParameter = &pParameterList[i];

	//		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->pLightParameters);

	//		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->pTextureParameters);

	//		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->pTextures);

	//		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->puLightParameterFlag);

	//		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->puTextureParameterFlag);

	//		//ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->pVertexElements);
	//	}

	//	ALCHEMY_DEBUG_DELETE_ARRAY(pParameterList);
	//}
	CRenderMethod::DestroyParameterList(pParameterList, uLength);

	return 0;
}

AS3_Val DestroySkinInfoData(void* pSelf, AS3_Val Args)
{
	CSkinInfo::LPBONE pBoneData = static_cast<CSkinInfo::LPBONE>( AS3_PtrValue(Args) );

	ALCHEMY_DEBUG_DELETE_ARRAY(pBoneData);

	return 0;
}

AS3_Val DestroySkinInfo(void* pSelf, AS3_Val Args)
{
	CSkinInfo* pSkinInfo = static_cast<CSkinInfo*>( AS3_PtrValue(Args) );

	pSkinInfo->Destroy();
	ALCHEMY_DEBUG_DELETE(pSkinInfo);
	
	return 0;
}

AS3_Val DestroyBoneData(void* pSelf, AS3_Val Args)
{
	CSkinInfo::LPBONE pBoneData = static_cast<CSkinInfo::LPBONE>( AS3_PtrValue(Args) );

	ALCHEMY_DEBUG_DELETE_ARRAY(pBoneData->pcName);
	ALCHEMY_DEBUG_DELETE_ARRAY(pBoneData->pfWeights);
	ALCHEMY_DEBUG_DELETE_ARRAY(pBoneData->puIndices);

	return 0;
}

AS3_Val DestroyAnimTrack(void* pSelf, AS3_Val Args)
{
	CAnimationSet::LPANIMATION pAnimData = static_cast<CAnimationSet::LPANIMATION>( AS3_PtrValue(Args) );
	
	ALCHEMY_DEBUG_DELETE_ARRAY(pAnimData->pcName);
	ALCHEMY_DEBUG_DELETE_ARRAY(pAnimData->pfTime);
	ALCHEMY_DEBUG_DELETE_ARRAY(pAnimData->pTransformMatrices);

	return 0;
}

AS3_Val DestroyAnimSetData(void* pSelf, AS3_Val Args)
{
	CAnimationSet::LPANIMATION pAnimData = static_cast<CAnimationSet::LPANIMATION>( AS3_PtrValue(Args) );

	ALCHEMY_DEBUG_DELETE_ARRAY(pAnimData);

	return 0;
}

AS3_Val DestroyAnimSet(void* pSelf, AS3_Val Args)
{
	CAnimationSet* pAnimSet = static_cast<CAnimationSet*>( AS3_PtrValue(Args) );
	pAnimSet->Destroy();

	return 0;
}

AS3_Val DestroyAnimController(void* pSelf, AS3_Val Args)
{
	CAnimationController* pAnimController = static_cast<CAnimationController*>( AS3_PtrValue(Args) );
	pAnimController->Destroy();

	return 0;
}


AS3_Val DestroyStreamBuffer(void* pSelf, AS3_Val Args)
{
	UINT* pBuffer = static_cast<UINT*>( AS3_PtrValue(Args) );
	delete [] pBuffer;

	return 0;
}

AS3_Val DestroyEffectCode(void* pSelf, AS3_Val Args)
{
	CEffectCode* pEffectCode;
	CEffectCode::CPassDefiner* pPassDefiner;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pEffectCode, &pPassDefiner);

	ALCHEMY_DEBUG_DELETE(pEffectCode);
	ALCHEMY_DEBUG_DELETE(pPassDefiner);

	return 0;
}

AS3_Val DestroyTextureEffect(void* pSelf, AS3_Val Args)
{
	CTextureEffect* pTextureEffect;

	pTextureEffect = static_cast<CTextureEffect*>( AS3_PtrValue(Args) );

	ALCHEMY_DEBUG_DELETE(pTextureEffect);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
///Fill
//////////////////////////////////////////////////////////////////////////
AS3_Val FillObject(void* pSelf, AS3_Val Args)
{
	CFlashObject* pObject;
	AS3_Val Object;
	AS3_Val MethodArray;

	AS3_ArrayValue(Args, "PtrType, AS3ValType, AS3ValType", &pObject, &Object, &MethodArray);

	if(pObject)
		pObject->Fill(Object, MethodArray);

	return 0;
}
//////////////////////////////////////////////////////////////////////////
///Add
//////////////////////////////////////////////////////////////////////////
AS3_Val AddTextureToMaterial(void* pSelf, AS3_Val Args)
{
	CFlashMaterial* pMaterial = ALCHEMY_NULL;
	CFlashTexture* pTexture = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pMaterial, &pTexture);

	if(pMaterial && pTexture)
		return  AS3_Int( pMaterial->AddTexture( pTexture->GetResourceHandle() ) );

	return 0;
}

AS3_Val AddAttributeToMesh(void* pSelf, AS3_Val Args)
{
	CFlashMesh* pMesh = ALCHEMY_NULL;
	CFlashMesh::LPATTRIBUTERANGE pAttribute = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pMesh, &pAttribute);

	return AS3_Int( pMesh->GetAttributeTable().Add(*pAttribute) );
}

AS3_Val AddChildToSceneNode(void* pSelf, AS3_Val Args)
{
	CSceneNode* pSceneNode = ALCHEMY_NULL;
	CSceneNode* pChild     = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pSceneNode, &pChild);

	if(pSceneNode && pChild)
		pSceneNode->AddChild(*pChild);

	return 0;
}

AS3_Val AddLightToSceneManager(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager = ALCHEMY_NULL;
	int                 nLight    ;
	int                 nShadeType;

	AS3_ArrayValue(Args, "PtrType, IntType, IntType", &pSceneManager, &nLight, &nShadeType);

	if(pSceneManager && nLight)
		pSceneManager->AddLight( nLight, static_cast<CLightManager::SHADEMODE>(nShadeType) );

	return 0;
}

AS3_Val AddChildToSceneManager(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager = ALCHEMY_NULL;
	CSceneNode        * pChild        = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pSceneManager, &pChild);

	if(pSceneManager && pChild)
		pSceneManager->AddChild(*pChild);

	return 0;
}

AS3_Val AddCameraToSceneManager(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager = ALCHEMY_NULL;
	CFlashCamera* pCamera = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pSceneManager, &pCamera);

	if(pSceneManager && pCamera)
		return AS3_Int( pSceneManager->AddCamera( *pCamera ) );

	return 0;
}

AS3_Val AddInfoToPassDefiner(void* pSelf, AS3_Val Args)
{
	CEffectCode::CPassDefiner* pPassDefiner;
	IEffectInfo* pEffectInfo;
	int nIndex, nBlendType;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pPassDefiner, &pEffectInfo, &nBlendType);

	if(pPassDefiner)
		pPassDefiner->AddInfo( *pEffectInfo, static_cast<CEffectCode::CPassDefiner::BLENDMETHOD>(nBlendType) );

	return 0;
}

AS3_Val AddInfoForBlendMethodToPassDefiner(void* pSelf, AS3_Val Args)
{
	CEffectCode::CPassDefiner* pPassDefiner;
	IEffectInfo* pEffectInfo;
	IEffectInfoBlendMethod* pBlendMethod;

	AS3_ArrayValue(Args, "PtrType, PtrType, PtrType", &pPassDefiner, &pEffectInfo, &pBlendMethod );

	pPassDefiner->AddInfo(*pEffectInfo, pBlendMethod);

	return 0;
}

AS3_Val AddColorKeyAndInfoToPassDefiner(void* pSelf, AS3_Val Args)
{
	CEffectCode::CPassDefiner* pPassDefiner;
	IEffectInfoBlendMethod* pBlendMethod;
	IEffectInfo* pEffectInfo;
	INT nOutputColorKey, nInputColorKey;

	AS3_ArrayValue(Args, "PtrType, PtrType, PtrType, IntType, IntType", &pPassDefiner, &pEffectInfo, &pBlendMethod, &nInputColorKey, &nOutputColorKey);

	if(pPassDefiner)
		pPassDefiner->AddInfo(*pEffectInfo, pBlendMethod, nInputColorKey, nOutputColorKey);

	return 0;
}

AS3_Val AddColorKeyInfoToPassDefiner(void* pSelf, AS3_Val Args)
{
	CEffectCode::CPassDefiner* pPassDefiner;
	IEffectInfoBlendMethod* pBlendMethod;
	INT nSourceColorKey, nOutputColorKey, nInputColorKey;

	AS3_ArrayValue(Args, "PtrType, IntType, PtrType, IntType, IntType", &pPassDefiner, &nSourceColorKey, &pBlendMethod, &nInputColorKey, &nOutputColorKey);

	if(pPassDefiner)
		pPassDefiner->AddInfo(nSourceColorKey, pBlendMethod, nInputColorKey, nOutputColorKey);

	return 0;
}

AS3_Val AddColorKeyToPassDefiner(void* pSelf, AS3_Val Args)
{
	CEffectCode::CPassDefiner* pPassDefiner;
	INT nType, nResult;

	AS3_ArrayValue(Args, "PtrType, IntType, IntType", &pPassDefiner, &nType, &nResult);

	if(pPassDefiner)
		return AS3_Int( pPassDefiner->AddColorKey( (CVertexDeclaration::ELEMENTTYPE)nType, (nResult != 0) ) );

	return 0;
}

AS3_Val AddAttributeToMeshFiller(void* pSelf, AS3_Val Args)
{
	CMeshFiller* pFiller;
	IMeshAttribute* pAttribute;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pFiller, &pAttribute);

	if(pFiller && pAttribute)
		pFiller->AddMeshAttribute( *pAttribute );

	return 0;
}

AS3_Val FillMeshFromMeshFiller(void* pSelf, AS3_Val Args)
{
	CMeshFiller* pFiller;
	INT nMesh;

	AS3_ArrayValue(Args, "PtrType, IntType", &pFiller, &nMesh);

	if(pFiller)
		return AS3_Int( pFiller->FillMesh( (CGraphicsResource::HANDLE)nMesh ) );

	return 0;
}

AS3_Val AddParticleComputeInfoToDataCode(void* pSelf, AS3_Val Args)
{
	CParticleDataCode::CParticleLogicDefiner* pDefiner;
	CParticleComputeInfoInterface* pComputeInfo;

	AS3_ArrayValue(Args,"PtrType, PtrType", &pDefiner, &pComputeInfo);

	pDefiner->AppendInfo( *pComputeInfo );

	return 0;
}

AS3_Val AddParticleVariableInfoToDataCode(void* pSelf, AS3_Val Args)
{
	CParticleDataCode::CParticleLogicDefiner* pDefiner;
	CParticleVariableComputeInfoInterface* pComputeVariableInfo;

	AS3_ArrayValue(Args,"PtrType, PtrType", &pDefiner, &pComputeVariableInfo);

	pDefiner->AppendInfo( *pComputeVariableInfo );

	return 0;
}

AS3_Val BuildParticleController(void* pSelf, AS3_Val Args)
{
	CParticleDataCode::CParticleLogicDefiner* pDefiner = reinterpret_cast<CParticleDataCode::CParticleLogicDefiner*>( AS3_PtrValue(Args) );
 
	CParticleController* pController = pDefiner->Build();

	return AS3_Array("PtrType, PtrType", pController, static_cast<IInterface*>(pController));
}

//AS3_Val AddEnvironmentRenderAble( void* pSelf, AS3_Val Args )
//{
//	CEnvironmentRender* pRender;
//	CFlashSceneNode* pRenderAble;
//
//	AS3_ArrayValue(Args, "PtrType, PtrType", &pRender, &pRenderAble);
//
//	pRender->AddEnvironmentAble( *static_cast<CRenderQueue::ICallBackToRender*>(pRenderAble) );
//}

AS3_Val AddProgramRenderToManager( void* pSelf, AS3_Val Args )
{
	CAbstractProgramRender* pProgramRender =  static_cast<CAbstractProgramRender*>( AS3_PtrValue(Args) );

	if(pProgramRender)
		ALCHEMY_PROGRAM_MANAGER.AddProgramRender(*pProgramRender);

	return 0;
}

AS3_Val AddRenderObjectToProgram(void* pSelf, AS3_Val Args)
{
	CMulityEffectProgram* pProgram;
	CFlashSceneNode* pSceneNode;
	INT nEffect, nRenderPass, nMaterial;
	CFlashRenderMethod* pRenderMethod;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType, PtrType, IntType, IntType", &pProgram, &pSceneNode, &nEffect, &pRenderMethod, &nRenderPass, &nMaterial);

	if(pProgram)
		pProgram->AddRenderObject(*pSceneNode, (CGraphicsResource::HANDLE)nEffect, *pRenderMethod, (UINT)nRenderPass, (CGraphicsResource::HANDLE)nMaterial );

	return 0;
}

AS3_Val AddRenderObjectSelfToProgram(void* pSelf, AS3_Val Args)
{
	CMulityEffectProgram* pProgram;
	CFlashSceneNode* pSceneNode;
	INT nRenderPass;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pProgram, &pSceneNode, &nRenderPass);

	if(pProgram && pSceneNode)
		pProgram->AddRenderObject(*pSceneNode, nRenderPass);

	return 0;
}

AS3_Val AddSkinDataToMeshManager(void* pSelf, AS3_Val Args)
{
	CMeshManager::SKINDATA* pSkinData = reinterpret_cast<CMeshManager::SKINDATA*>( AS3_PtrValue(Args) );
	if(pSkinData)
		return AS3_Int( ALCHEMY_MESH_MANAGER.AddSkinData(*pSkinData) );

	return 0;
}

AS3_Val AddTextureDataToMeshManager(void* pSelf, AS3_Val Args)
{
	CMeshManager::TEXTUREDATA* pTextureData = reinterpret_cast<CMeshManager::TEXTUREDATA*>( AS3_PtrValue(Args) );
	if(pTextureData)
		return AS3_Int( ALCHEMY_MESH_MANAGER.AddTextureData(*pTextureData) );

	return 0;
}

AS3_Val AddMaterialDataToMeshManager(void* pSelf, AS3_Val Args)
{
	CMeshManager::MATERIALDATA* pMaterialData = reinterpret_cast<CMeshManager::MATERIALDATA*>( AS3_PtrValue(Args) );
	if(pMaterialData)
		return AS3_Int( ALCHEMY_MESH_MANAGER.AddMaterialData(*pMaterialData) );

	return 0;
}

AS3_Val AddMeshDataToMeshManager(void* pSelf, AS3_Val Args)
{
	CMeshManager::MESHDATA* pMeshData = reinterpret_cast<CMeshManager::MESHDATA*>( AS3_PtrValue(Args) );
	if(pMeshData)
		return AS3_Int( ALCHEMY_MESH_MANAGER.AddMeshData(*pMeshData) );

	return 0;
}

AS3_Val AddSceneDataToMeshManager(void* pSelf, AS3_Val Args)
{
	CMeshManager::SCENEDATA* pSceneData = reinterpret_cast<CMeshManager::SCENEDATA*>( AS3_PtrValue(Args) );
	return pSceneData ? AS3_Int( ALCHEMY_MESH_MANAGER.AddSceneData(*pSceneData) ) : 0;
}

AS3_Val AddNodeToBillboardSystem(void* pSelf, AS3_Val Args)
{
	FLOAT3* pPos;
	double dWidth, dHeight;

	CBillboardSet* pBillboardSet;

	AS3_ArrayValue(Args, "PtrType, PtrType, DoubleType, DoubleType", &pBillboardSet, &pPos, &dWidth, &dHeight);

	if(pBillboardSet && pPos)
		return AS3_Int( pBillboardSet->AddBillboard(*pPos, (FLOAT)dWidth, (FLOAT)dHeight ) );

	return 0;
}

AS3_Val AddShadowCasterToCascadShadowMap(void* pSelf, AS3_Val Args)
{
	CCascadShadowMapRender* pShadowMap;
	CFlashSceneNode* pSceneNode;
	INT nEffect, nRenderPass;
	CFlashRenderMethod* pRenderMethod;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType, IntType, PtrType", &pShadowMap, &pSceneNode, &nEffect, &nRenderPass, &pRenderMethod);

	if(pShadowMap && pSceneNode && pRenderMethod)
		pShadowMap->AddShadowCaster(*pSceneNode, (CGraphicsResource::HANDLE)nEffect, (UINT)nRenderPass, *pRenderMethod );

	return 0;
}

AS3_Val AddShadowCasterToShadowMap(void* pSelf, AS3_Val Args)
{
	CShadowMapRender* pRender;
	CFlashSceneNode* pSceneNode;
	INT nEffect, nRenderPass;
	CFlashRenderMethod* pRenderMethod;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType, IntType, PtrType", &pRender, &pSceneNode, &nEffect, &nRenderPass, &pRenderMethod);

	if(pRender && pSceneNode && pRenderMethod)
		pRender->AddShadowCaster(*pSceneNode, (CGraphicsResource::HANDLE)nEffect, (UINT)nRenderPass, *pRenderMethod );

	return 0;
}

//////////////////////////////////////////////////////////////////////////
///Remove
//////////////////////////////////////////////////////////////////////////
AS3_Val RemoveTextureFromMaterial(void* pSelf, AS3_Val Args)
{
	CFlashMaterial* pMaterial = ALCHEMY_NULL;
	UINT TextureIndex;

	AS3_ArrayValue(Args, "PtrType, IntType", &pMaterial, &TextureIndex);

	pMaterial->RemoveTexture(TextureIndex);

	return  0;
}

AS3_Val RemoveAttributeFromMesh(void* pSelf, AS3_Val Args)
{
	/*CFlashMesh* pMesh = ALCHEMY_NULL;
	CFlashMesh::LPATTRIBUTERANGE pAttribute = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pMesh, &pAttribute);

	UINT length = pMesh->GetAttributeTable().GetLength();
	CFlashMesh::ATTRIBUTERANGE tmp = ALCHEMY_NULL;
	for(int i=0; i<length; ++i)
	{
		pTmp =  pMesh->GetAttributeTable()[i];
		if(pTmp == pAttribute)
		{
			pMesh->GetAttributeTable().Remove(i,1);
			break;
		}
	}*/

	return 0;
}

AS3_Val RemoveChildFromSceneNode(void* pSelf, AS3_Val Args)
{
	CSceneNode* pSceneNode = ALCHEMY_NULL;
	CSceneNode* pChild     = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pSceneNode, &pChild);

	if(pSceneNode && pChild)
		pSceneNode->RemoveChild(*pChild);

	return 0;
}

AS3_Val RemoveChildFromSceneManager(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager = ALCHEMY_NULL;
	CSceneNode        * pChild        = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pSceneManager, &pChild);

	if(pSceneManager && pChild)
		pSceneManager->RemoveChild(*pChild);

	return 0;
}

//AS3_Val RemoveRenderAbleFromEnvironmentRender(void* pSelf, AS3_Val Args)
//{
//	CEnvironmentRender* pRender;
//	INT uIndex;
//
//	AS3_ArrayValue(Args, "PtrType, IntType", &pRender, &uIndex);
//
//	if(pRender)
//		pRender->RemoveEnvironmentAble( uIndex );
//
//	return 0;
//}

AS3_Val RemoveNodeFromBillboardSystem(void* pSelf, AS3_Val Args)
{
	CBillboardSet* pBillboardSet;
	INT nNodeHandle;

	AS3_ArrayValue(Args, "PtrType, IntType", &pBillboardSet, &nNodeHandle);

	if(pBillboardSet)
		pBillboardSet->RemoveBillboard( (UINT)nNodeHandle );

	return 0;
}

//////////////////////////////////////////////////////////////////////////
///Get Set
//////////////////////////////////////////////////////////////////////////


AS3_Val LockTexture(void* pSelf, AS3_Val Args)
{
	CFlashTexture* pTexture;
	INT nSurface, nFlag;
	PUINT puRowPitch, puSlicePitch;

	AS3_ArrayValue(Args,"PtrType, IntType, IntType, PtrType, PtrType", &pTexture, &nSurface, &nFlag, &puRowPitch, &puSlicePitch);

	return  AS3_Ptr( pTexture->Lock( nSurface, nFlag, puRowPitch, puSlicePitch ) );
}

AS3_Val UnlockTexture(void* pSelf, AS3_Val Args)
{
	CFlashTexture* pTexture =  reinterpret_cast< CFlashTexture* >( AS3_PtrValue( Args ) ) ;
	pTexture->Unlock();
	return 0;
}

AS3_Val SetViewportToContext3D(void* pSelf, AS3_Val Args)
{
	CFlashContext3D* pContext3D;
	int nWidth, nHeight;
	AS3_ArrayValue(Args, "PtrType, IntType, IntType", &pContext3D, &nWidth, &nHeight);

	if(pContext3D)
	{
		pContext3D->SetViewport(nWidth, nHeight);
	}

	return 0;
}

AS3_Val GetAttributeDataFromMesh(void* pSelf, AS3_Val Args)
{
	CFlashMesh* pMesh = ALCHEMY_NULL;
	UINT attributeIndex;
	AS3_ArrayValue(Args, "PtrType, IntType", &pMesh, &attributeIndex);

	const CFlashMesh::LPATTRIBUTERANGE pAttribute = pMesh->GetAttributeTable().Get(attributeIndex);
	if(pAttribute)
		return AS3_Array("IntType, IntType",pAttribute->uStartIndex,pAttribute->uIndexCount);

	return 0;
}

AS3_Val SetAttributeDataFromMesh(void* pSelf, AS3_Val Args)
{
	CFlashMesh* pMesh = ALCHEMY_NULL;
	UINT attributeIndex;
	CFlashMesh::ATTRIBUTERANGE attribute = {0};

	AS3_ArrayValue(Args, "PtrType, IntType, IntType, IntType", &pMesh, &attributeIndex, &attribute.uStartIndex, &attribute.uIndexCount);

	pMesh->GetAttributeTable()[attributeIndex] = attribute;

	return 0;
}

AS3_Val GetMeshFromProcessor(void* pSelf, AS3_Val Args)
{
	IMeshProcessor* pProcessor = static_cast<IMeshProcessor*>( AS3_PtrValue(Args) );
	if(pProcessor)
	{
		CMesh* pMesh = pProcessor->GetMesh();
		return pMesh ? AS3_Int( pMesh->GetResourceHandle() ) : 0;
	}

	return 0;
}

AS3_Val GetHardwareSkinEffect(void* pSelf, AS3_Val Args)
{
	CSkinMeshController* pSkinMeshController = static_cast<CSkinMeshController*>( AS3_PtrValue(Args) );
	if(pSkinMeshController)
		return AS3_Ptr( pSkinMeshController->GetHardwareSkinEffect() );

	return 0;
}

/*AS3_Val SetPassToEffect(void* pSelf, AS3_Val Args)
{
	CFlashEffect* pEffect = ALCHEMY_NULL;
	CFlashProgram3D* pPass = ALCHEMY_NULL;
	UINT uIndex;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pEffect, &pPass, &uIndex);

	pEffect->SetPass(pPass, uIndex);

	return 0;
}*/

AS3_Val SetProjectionToCamera(void* pSelf, AS3_Val Args)
{
	CFlashCamera* pCamera = ALCHEMY_NULL;
	double FOV, NearPlane, FarPlane, Aspect;

	AS3_ArrayValue(Args, "PtrType, DoubleType, DoubleType, DoubleType, DoubleType", &pCamera, &FOV, &NearPlane, &FarPlane, &Aspect);

	if(pCamera)
		pCamera->SetProjection( static_cast<FLOAT>(FOV), static_cast<FLOAT>(NearPlane), static_cast<FLOAT>(FarPlane), static_cast<FLOAT>(Aspect));

	return 0;
}

//AS3_Val GetEffectInfoFromShadowRender(void* pSelf, AS3_Val Args)
//{
//	LPSHADOWMAPRENDER pRender = static_cast< LPSHADOWMAPRENDER >( AS3_PtrValue(Args) );
//
//	if(pRender)
//		return AS3_Ptr( &pRender->GetShadowMapEffectInfo() );
//
//	return 0;
//}
//
//AS3_Val GetEffectInfoFromDistanceInfoRender(void* pSelf, AS3_Val Args)
//{
//	LPDISTANCEINFORENDER pRender = static_cast< LPDISTANCEINFORENDER >( AS3_PtrValue(Args) );
//
//	if(pRender)
//		return AS3_Ptr( &pRender->GetDistanceEffectInfo() );
//
//	return 0;
//}

AS3_Val RenderScene(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	double fEleapseTime;

	AS3_ArrayValue(Args, "PtrType, DoubleType", &pSceneManager, &fEleapseTime);

	pSceneManager->Update( static_cast<FLOAT>(fEleapseTime) );
	pSceneManager->Render();

	return 0;
}

AS3_Val UpdateAnimation(void* pSelf, AS3_Val Args)
{
	CAnimationController* pAnimation;
	double fElapseTime;

	AS3_ArrayValue(Args, "PtrType, DoubleType", &pAnimation, &fElapseTime);

	if(pAnimation)
		pAnimation->Update( (FLOAT)fElapseTime );

	return 0;
}

AS3_Val UpdateProgramRenderManager(void* pSelf, AS3_Val Args)
{
	ALCHEMY_PROGRAM_MANAGER.Update();

	return 0;
}

AS3_Val SetBonesDataToSkinInfo(void* pSelf, AS3_Val Args)
{
	CSkinInfo* pSkinInfo;
	CSkinInfo::LPBONE pBoneData;
	UINT len;

	AS3_ArrayValue(Args,"PtrType, PtrType, IntType", &pSkinInfo, &pBoneData, &len);

	pSkinInfo->Create(pBoneData,len);

	return 0;
}

AS3_Val CloneSceneNode(void* pSelf, AS3_Val Args)
{
	CSceneNode* pSceneNode;

	AS3_ArrayValue(Args,"PtrType",&pSceneNode);

	if(pSceneNode)
	{
		CSceneNode& CloneSceneNode = pSceneNode->CloneTree(ALCHEMY_FLASH_SCENE_NODE_FACTORY);
		CFlashAbstractSceneNode* pCloneFlashSceneNode = dynamic_cast<CFlashAbstractSceneNode*>( static_cast<ISceneNode*>(&CloneSceneNode) );
		if(pCloneFlashSceneNode)
			return static_cast<const AS3TYPE>(*pCloneFlashSceneNode);
	}

	return 0;
}

AS3_Val SetNodeName(void* pSelf, AS3_Val Args)
{
	CSceneNode* pNode;
	PCHAR name;

	AS3_ArrayValue(Args,"PtrType, PtrType",&pNode, &name);

	pNode->SetName(name);

	return 0;
}

//AS3_Val SetSkinMeshControllerToSceneObject(void* pSelf, AS3_Val Args)
//{
//	CFlashSceneObject* pSceneObject;
//	CSkinMeshController* pSkinMeshController;
//	INT isUpdate;
//
//	AS3_ArrayValue(Args,"PtrType, PtrType, PtrType", &pSceneObject, &pSkinMeshController, &isUpdate);
//
//	pSceneObject->SetSkinMeshController( *pSkinMeshController, (bool) isUpdate );
//
//	return 0;
//}

AS3_Val SetMeshProcessorToSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject* pSceneObject;
	IMeshProcessor* pProcessor;
	INT nNeedUpdate;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pSceneObject, &pProcessor, &nNeedUpdate);

	pSceneObject->SetMeshProcessor(*pProcessor, (bool)nNeedUpdate);

	return 0;
}

AS3_Val SetEffectToSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject * pSceneObject;
	INT Effect;

	AS3_ArrayValue(Args,"PtrType, IntType",&pSceneObject, &Effect);

	return AS3_Int( (int)( pSceneObject->SetEffect(Effect) ) );
}

AS3_Val SetRenderMethodToSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject * pSceneObject;
	CFlashRenderMethod*	pRenderMethod;

	AS3_ArrayValue(Args,"PtrType, PtrType",&pSceneObject, &pRenderMethod);

	return AS3_Int( (int)( pSceneObject->SetRenderMethod( *pRenderMethod ) ) );
}

AS3_Val SetEffectToGrassSystem(void* pSelf, AS3_Val Args)
{
	CFlashGrassSystem * pGrassSystem;
	INT Effect;

	AS3_ArrayValue(Args,"PtrType, IntType",&pGrassSystem, &Effect);

	return AS3_Int( (int)( pGrassSystem->SetEffect(Effect) ) );
}

AS3_Val SetRenderMethodToGrassSystem(void* pSelf, AS3_Val Args)
{
	CFlashGrassSystem * pGrassSystem;
	CFlashRenderMethod*	pRenderMethod;

	AS3_ArrayValue(Args,"PtrType, PtrType",&pGrassSystem, &pRenderMethod);

	return AS3_Int( (int)( pGrassSystem->SetRenderMethod( *pRenderMethod ) ) );
}


AS3_Val SetSceneDataToStaticMeshInstance(void* pSelf, AS3_Val Args)
{
	CFlashStaticMeshInstance* pInstance;
	INT nSceneDataHandle;

	AS3_ArrayValue(Args, "PtrType, IntType", &pInstance, &nSceneDataHandle);

	if(pInstance)
		return AS3_Int( pInstance->SetSceneData( (CMeshManager::HANDLE) nSceneDataHandle) );

	return 0;
}

AS3_Val SetEffectToStaticMeshInstance(void* pSelf, AS3_Val Args)
{
	CFlashStaticMeshInstance * pInstance;
	INT Effect;

	AS3_ArrayValue(Args,"PtrType, IntType",&pInstance, &Effect);

	pInstance->SetEffect(Effect);

	return 0;
}

AS3_Val SetRenderMethodToStaticMeshInstance(void* pSelf, AS3_Val Args)
{
	CFlashStaticMeshInstance * pInstance;
	CFlashRenderMethod*	 pRenderMethod;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pInstance, &pRenderMethod);

	pInstance->SetRenderMethod( pRenderMethod );

	return 0;
}

//AS3_Val SetAniamtionController(void* pSelf, AS3_Val Args)
//{
//	CFlashSceneObject* pSceneObject;
//	CAnimationController* pAnimController;
//
//	AS3_ArrayValue(Args,"PtrType, PtrType",&pSceneObject,&pAnimController);
//
//	pSceneObject->SetAnimationController( *pAnimController );
//
//	return 0;
//}

AS3_Val SetEffectRenderToSkyDome(void* pSelf, AS3_Val Args)
{
	CFlashSkyDome* pSkyDome;
	INT nEffect;
	CFlashRenderMethod* pRenderMethod;

	AS3_ArrayValue(Args, "PtrType, IntType, PtrType", &pSkyDome, &nEffect, &pRenderMethod);

	if(pSkyDome)
		pSkyDome->SetEffectRender( (CGraphicsResource::HANDLE)nEffect, pRenderMethod );

	return 0;
}

AS3_Val SetUVSpeedToSkyDome(void* pSelf, AS3_Val Args)
{
	CFlashSkyDome* pSkyDome;
	double uSpeed, vSpeed;

	AS3_ArrayValue(Args,"PtrType, DoubleType, DoubleType",&pSkyDome, &uSpeed, &vSpeed);

	pSkyDome->SetUVSpeed( static_cast<FLOAT>(uSpeed), static_cast<FLOAT>(vSpeed) );

	return 0;
}

AS3_Val SetMaterialToGrassSystem(void* pSelf, AS3_Val Args)
{
	CFlashGrassSystem* pGrassSystem;
	INT nMaterial;

	AS3_ArrayValue(Args, "PtrType, IntType", &pGrassSystem, &nMaterial);

	if(pGrassSystem)
		pGrassSystem->SetMaterial( (CGraphicsResource::HANDLE)nMaterial );

	return 0;
}

AS3_Val SetOrghMatrixToCamera(void* pSelf, AS3_Val Args)
{
	CFlashCamera* pCamera;
	SPHERE* pSphere;
	FLOAT3* pDirectional;

	AS3_ArrayValue(Args, "PtrType, PtrType, PtrType", &pCamera, &pSphere, &pDirectional);

	if(pCamera && pSphere && pDirectional)
		pCamera->ExtractFrom(*pSphere, *pDirectional);

	return 0;
}

AS3_Val SetVisibleToSceneNode(void* pSelf, AS3_Val Args)
{
	CSceneNode* pSceneNode;
	INT nVal;

	AS3_ArrayValue(Args, "PtrType, IntType", &pSceneNode, &nVal);

	if(pSceneNode)
		pSceneNode->SetVisible( (bool)nVal );
}

//AS3_Val SetRenderInfoFromRender(void* pSelf, AS3_Val Args)
//{
//	CEffectRender* pEffectRender;
//	CFlashRenderMethod* pRenderMethod;
//	INT nEffectHandle,nPassIndex;
//
//	AS3_ArrayValue(Args, "PtrType, PtrType, IntType, IntType", &pEffectRender, &pRenderMethod, &nEffectHandle, &nPassIndex);
//
//	if(pEffectRender)
//		pEffectRender->SetRenderInfo(*pRenderMethod, (CGraphicsResource::HANDLE)nEffectHandle, nPassIndex );
//}

/*AS3_Val SetRenderMethodFromGPUParticleSystem(void* pSelf, AS3_Val Args)
{
	CFlashGPUParticleSystem* pParticleSystem;
	CFlashRenderMethod* pRenderMethod;

	AS3_ArrayValue(Args,"PtrType, PtrType", &pParticleSystem, &pRenderMethod);

	if(pParticleSystem && pRenderMethod)
		pParticleSystem->SetRenderMethod( pRenderMethod->GetResourceHandle() );

	return 0;
}*/

AS3_Val GetDestinationMeshFromSceneObject(void* pSelf, AS3_Val Args)
{
	/*CFlashSceneObject* pSceneObject = static_cast<CFlashSceneObject*>( AS3_PtrValue(Args) );
	CMesh* pMesh;

	if(pSceneObject)
	{
		pMesh = pSceneObject->GetDestMesh();
		if(pMesh)
			return AS3_Int( pMesh->GetResourceHandle() );
	}*/

	return 0;
}

//AS3_Val SetAnimControllerToProcessor(void* pSelf, AS3_Val Args)
//{
//	CSkinMeshProcessor* pProcessor;
//	CAnimationController* pAnimController;
//
//	AS3_ArrayValue(Args, "PtrType, PtrType", &pProcessor, &pAnimController );
//
//	if(pProcessor && pAnimController)
//		pProcessor->SetAnimationController( *pAnimController );
//
//	return 0;
//}

AS3_Val SetFrameEmitToParticleSystem(void* pSelf, AS3_Val Args)
{
	CFlashParticleSystem* pParticleSystem;
	INT nFrameEmitted;
	
	AS3_ArrayValue(Args,"PtrType, IntType", &pParticleSystem, &nFrameEmitted);
	
	if(pParticleSystem)
		pParticleSystem->SetFrameEmitted( nFrameEmitted );

	return 0;
}

AS3_Val SetVisibleToRender(void* pSelf, AS3_Val Args)
{
	CRenderer* pRender;
	INT nVisible;

	AS3_ArrayValue(Args,"PtrType, IntType", &pRender, &nVisible);

	if(pRender)
		pRender->SetVisible( (bool)nVisible );

	return 0;
}

AS3_Val GetSkinControllerInfo(void* pSelf, AS3_Val Args)
{
	CSkinMeshController* pSkinMeshController = static_cast<CSkinMeshController*>( AS3_PtrValue(Args) );

	if(pSkinMeshController)
	{
		CFlashBone* pRootBone = static_cast<CFlashBone*>( pSkinMeshController->GetRootBone() );
		const CSkinInfo* pSkinInfo = pSkinMeshController->GetSkinInfo();

		CSkinMeshController::SKINTYPE SkinType = pSkinMeshController->GetSkinType();

		return SkinType == CSkinMeshController::SKIN_SOFTWARE ? AS3_Array("AS3ValType, PtrType, IntType", static_cast<const AS3TYPE>( static_cast<const CFlashBone&>(*pRootBone) ), pSkinInfo, SkinType) : AS3_Array("AS3ValType, PtrType, IntType, PtrType", static_cast<const AS3TYPE>( static_cast<const CFlashBone&>(*pRootBone) ), pSkinInfo, SkinType, pSkinMeshController->GetHardwareSkinEffect() );
	}

	return 0;
}

AS3_Val GetSkinProcessorInfo(void* pSelf, AS3_Val Args)
{
	CSkinMeshProcessor* pProcessor = static_cast<CSkinMeshProcessor*>( AS3_PtrValue(Args) );
	if(pProcessor)
		return AS3_Ptr( &pProcessor->GetSkinMeshController() );

	return 0;
}

AS3_Val GetMeshProcessorFromSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject* pSceneObject = static_cast<CFlashSceneObject*>( AS3_PtrValue(Args) );
	if(pSceneObject)
	{
		IMeshProcessor* pMeshProcessor = pSceneObject->GetMeshProcessor();
		CSkinMeshProcessor* pSkinProcessor = dynamic_cast<CSkinMeshProcessor*>( pMeshProcessor );
		CNormalMeshProcessor* pNormalProcessor = dynamic_cast<CNormalMeshProcessor*>( pMeshProcessor );

		if(pSkinProcessor)
			return AS3_Array("PtrType, PtrType, IntType", pSkinProcessor, static_cast<IMeshProcessor*>(pSkinProcessor), 2);
		else if(pNormalProcessor)
			return AS3_Array("PtrType, PtrType, IntType", pNormalProcessor, static_cast<IMeshProcessor*>(pNormalProcessor), 1);
	}

	return 0;
}

AS3_Val GetAnimControllerFromProcessor(void* pSelf, AS3_Val Args)
{
	CSkinMeshProcessor* pSkinMeshProcessor = static_cast<CSkinMeshProcessor*>( AS3_PtrValue(Args) );
	return pSkinMeshProcessor ? AS3_Ptr( &pSkinMeshProcessor->GetSkinMeshController() ) : 0 ;
}

AS3_Val GetMaterialFromSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject* pSceneObject = static_cast<CFlashSceneObject*>( AS3_PtrValue(Args) );

	CMaterial* pMaterial = pSceneObject ? pSceneObject->GetMaterial() : ALCHEMY_NULL;
	return pMaterial ? AS3_Int( pMaterial->GetResourceHandle() ) : 0;
}

AS3_Val GetEffectFromSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject* pSceneObject = static_cast<CFlashSceneObject*>( AS3_PtrValue(Args) );
	return pSceneObject ? AS3_Int( pSceneObject->GetEffect() ) : 0;
}

AS3_Val GetRenderMethodFromSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject* pSceneObject = static_cast<CFlashSceneObject*>( AS3_PtrValue(Args) );
	CFlashRenderMethod* pRenderMethod = static_cast<CFlashRenderMethod*>( pSceneObject->GetRenderMethod() );
	return pRenderMethod ? AS3_Int( pRenderMethod->GetResourceHandle() ) : 0;
}

AS3_Val GetAnimationSetFromController(void* pSelf, AS3_Val Args)
{
	CAnimationController* pAnimController;
	INT nIndex;

	AS3_ArrayValue(Args, "PtrType, IntType", &pAnimController, &nIndex);
	
	return pAnimController ? AS3_Ptr( pAnimController->GetAnimationSetByIndex(nIndex) ) : 0;
}

AS3_Val GetPlayTimeFromAnimationSet(void* pSelf, AS3_Val Args)
{
	CAnimationSet* pAnimSet = static_cast<CAnimationSet*>( AS3_PtrValue(Args) );

	if(pAnimSet)
	{
		const CAnimationSet::ANIMATION* pAnimation = pAnimSet->GetAnimations();
		UINT uAnimationNum = pAnimSet->GetAnimationCount();

		if(!pAnimation || !uAnimationNum)
			return 0;

		pAnimation = &pAnimation[ uAnimationNum-1 ];

		return AS3_Number( pAnimation->pfTime[ pAnimation->uFrameCount-1 ] );
	}

	return 0;
}

AS3_Val GetMaterialNumFromStaticMeshInstance(void* pSelf, AS3_Val Args)
{
	CFlashStaticMeshInstance* pStaticInstance = static_cast<CFlashStaticMeshInstance*>( AS3_PtrValue(Args) );
	if(pStaticInstance)
		return AS3_Int( pStaticInstance->GetMaterialNum() );

	return 0;
}

AS3_Val GetMaterialFromStaticMeshInstance(void* pSelf, AS3_Val Args)
{
	CFlashStaticMeshInstance* pStaticInstance;
	INT nMaterialIndex;

	AS3_ArrayValue(Args, "PtrType, IntType", &pStaticInstance, &nMaterialIndex);

	if(pStaticInstance)
		return AS3_Int( pStaticInstance->GetMaterialByIndex( (UINT)nMaterialIndex ) );

	return 0;
}

AS3_Val GetSceneNodeName(void* pSelf, AS3_Val Args)
{
	CFlashSceneNode* pSceneNode = reinterpret_cast<CFlashSceneNode*>( AS3_PtrValue(Args) );
	const CHAR* pNodeName = pSceneNode->GetName();
	UINT uLen = StringLength(pNodeName);

	return AS3_Array("PtrType, IntType", pNodeName, uLen *sizeof(CHAR) );
}

AS3_Val GetSphereFromCamera(void* pSelf, AS3_Val Args)
{
	CFlashCamera* pCamera = reinterpret_cast< CFlashCamera* >( AS3_PtrValue(Args) );
	SPHERE Sphere;

	pCamera->GetFrustum().GetSphere(Sphere);

	return AS3_Array("DoubleType, DoubleType, DoubleType, DoubleType", Sphere.Origin.x, Sphere.Origin.y, Sphere.Origin.z, Sphere.fRadius);
}

//AS3_Val GetRenderInfoNumFromStaticMeshInstance(void* pSelf, AS3_Val Args)
//{
//	CFlashStaticMeshInstance* pStaticInstance = static_cast<CFlashStaticMeshInstance*>( AS3_PtrValue(Args) );
//	if(pStaticInstance)
//		return AS3_Int( pStaticInstance->GetRenderInfoNum() );
//
//	return 0;
//}
//
//AS3_Val GetRenderInfoFromStaticMeshInstance(void* pSelf, AS3_Val Args)
//{
//	CFlashStaticMeshInstance* pStaticInstance;
//	INT nRenderInfoIndex;
//
//	AS3_ArrayValue(Args, "PtrType, IntType", &pStaticInstance, &nRenderInfoIndex);
//
//	if(pStaticInstance)
//		return AS3_Ptr( pStaticInstance->GetRenderInfoByIndex( (UINT)nRenderInfoIndex ) );
//}

/*AS3_Val EffectApplyForRender(void* pSelf, AS3_Val Args)
{
	CAbstractPostProcessEffect* pEffect = (CAbstractPostProcessEffect*)AS3_PtrValue(Args);
	if(pEffect)
		 pEffect->ApplyForRender();

	return 0;
}*/

AS3_Val EndDefinePassDefiner(void* pSelf, AS3_Val Args)
{
	CEffectCode::CPassDefiner* pPassDefiner = static_cast<CEffectCode::CPassDefiner*>( AS3_PtrValue(Args) );

	if(pPassDefiner)
		return AS3_Int( pPassDefiner->EndDefine() );

	return 0;
}

AS3_Val BuildShadowMapFromLight(void* pSelf, AS3_Val Args)
{
	CFlashLight* pLight;
	int uSize;
	double FOV, Near, Far;

	AS3_ArrayValue(Args, "PtrType, IntType, DoubleType, DoubleType, DoubleType", &pLight, &uSize, &FOV, &Near, &Far);

	if(pLight)
		pLight->BuildShadowMap( uSize, static_cast<FLOAT>(FOV), static_cast<FLOAT>(Near), static_cast<FLOAT>(Far) );

	return AS3_Int( pLight->GetShadowMap()->GetResourceHandle() );
}

AS3_Val AddEffectInfoToPassFromSceneManager(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	CEffectCode::CPassDefiner* pPassDefiner;
	IEffectInfoBlendMethod* pEffectInfoBlendMethod;
	IEffectInfo* pNormalMapEffect, *pSpecularEffect;
	INT nOutputKey, nIntputKey;
	
	AS3_ArrayValue(Args, "PtrType, PtrType, PtrType, PtrType, PtrType, IntType, IntType", &pSceneManager, &pPassDefiner, &pEffectInfoBlendMethod, &pSpecularEffect, &pNormalMapEffect, &nOutputKey, &nIntputKey);

	if(pSceneManager && pPassDefiner)
		pSceneManager->AddLightEffectInfoToPass(*pPassDefiner, pEffectInfoBlendMethod, pSpecularEffect, pNormalMapEffect);

	return 0;
}

AS3_Val ResetAnimationController(void* pSelf, AS3_Val Args)
{
	CAnimationController* pAnimController = reinterpret_cast<CAnimationController*>( AS3_PtrValue(Args) );

	pAnimController->ResetTime();

	return 0;
}

AS3_Val RegisterAniamtionSetToController(void* pSelf, AS3_Val Args)
{
	CAnimationController* pAnimController;
	CAnimationSet* pAnimSet;
	double weight,speed;

	AS3_ArrayValue(Args,"PtrType, PtrType, DoubleType, DoubleType",&pAnimController,&pAnimSet, &weight, &speed);

	pAnimController->RegisterAnimationSet(*pAnimSet, static_cast<FLOAT>(weight), static_cast<FLOAT>(speed));

	return 0;
}

AS3_Val UnregisterAnimationSetFromController(void* pSelf, AS3_Val Args)
{
	CAnimationController* pAnimController;
	CAnimationSet* pAnimSet;

	AS3_ArrayValue(Args,"PtrType, PtrType",&pAnimController,&pAnimSet);

	pAnimController->UnregisterAnimationSet(*pAnimSet);

	return 0;
}

AS3_Val RegisterBoneToAnimationController(void* pSelf, AS3_Val Args)
{
	CAnimationController* pAnimationController;
	CFlashBone* pBone;

	AS3_ArrayValue(Args,"PtrType, PtrType",&pAnimationController,&pBone);

	if(pBone && pAnimationController)
		pBone->RegisterAnimationController(*pAnimationController);

	return 0;
}

AS3_Val SetTrackersToAnimationSet(void* pSelf, AS3_Val Args)
{
	CAnimationSet* pAnimSet;
	CAnimationSet::LPANIMATION pAnimData;
	UINT len;

	AS3_ArrayValue(Args,"PtrType, PtrType, IntType",&pAnimSet,&pAnimData, &len);

	pAnimSet->Create(pAnimData,len);

	return 0;
}

AS3_Val ComputeMeshTangent(void* pSelf, AS3_Val Args)
{
	CFlashMesh* pMesh;
	INT nIndex;

	AS3_ArrayValue(Args,"PtrType, IntType",&pMesh, &nIndex);

	if(pMesh)
		return AS3_Int( pMesh->ComputeTangent(nIndex) );

	return 0;
}

AS3_Val ComputeMeshNormal(void* pSelf, AS3_Val Args)
{
	CFlashMesh* pMesh = reinterpret_cast<CFlashMesh*>( AS3_PtrValue(Args) );

	if(pMesh)
		return AS3_Int( pMesh->ComputeNormal() );

	return 0;
}

AS3_Val WriteDataToVertexBuffer(void* pSelf, AS3_Val Args)
{
	CFlashVertexBuffer* pVertexBuffer;
	PVOID pData;
	UINT length;
	
	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pVertexBuffer, &pData, &length);

	PVOID pVertexData = (PVOID)pVertexBuffer->Lock(0);

	ALCHEMY_MEMORY_COPY(pVertexData,pData,length);

	pVertexBuffer->Unlock();

	return 0;
}

AS3_Val ApplyForRenderFromDeferredLightRenderer(void* pSelf, AS3_Val Args)
{
	CDeferredLightRenderer* pDeferredLightRenderer = static_cast<CDeferredLightRenderer*>( AS3_PtrValue(Args) );
	pDeferredLightRenderer->ApplyForRender();

	return AS3_Ptr(pDeferredLightRenderer);
}

AS3_Val GetTextureByIndexFromMaterial(void* pSelf, AS3_Val Args)
{
	INT nIndex;
	CFlashMaterial* pMaterial;

	AS3_ArrayValue(Args,"PtrType, IntType", &pMaterial, &nIndex);

	if(pMaterial)
	{
		const LPTEXTURE* ppTexture = pMaterial->GetTextures().Get(nIndex);
		return ppTexture ? AS3_Int( (*ppTexture)->GetResourceHandle() ) : 0;
	}

	return 0;
}

AS3_Val GetTextureNumFromMaterial(void* pSelf, AS3_Val Args)
{
	CFlashMaterial* pMaterial = static_cast<CFlashMaterial*>( AS3_PtrValue(Args) );
	return pMaterial ? AS3_Int( pMaterial->GetTextures().GetLength() ) : 0;
}

AS3_Val GetNodeName(void* pSelf, AS3_Val Args)
{
	CSceneNode* pNode = static_cast<CSceneNode*>( AS3_PtrValue(Args) );
	const CHAR* pNamePtr = pNode->GetName();

	return pNode ? AS3_Array("PtrType, IntType", pNamePtr, StringLength(pNamePtr)) : 0;
}

//////////////////////////////////////////////////////////////////////////
///test
//////////////////////////////////////////////////////////////////////////
AS3_Val OnCreateScene(void* pSelf, AS3_Val Args)
{
	const UINT8* pData;
	CSceneParser::TEXTUREDATARES* pTextureDataReses;
	UINT uNum;

	AS3_ArrayValue(Args,"PtrType, PtrType, IntType", &pData, &pTextureDataReses, &uNum);

	CDAEParser* pParaser;
	ALCHEMY_DEBUG_NEW( pParaser, CDAEParser(pTextureDataReses, uNum) );
	if( !pParaser->Parse( (const void*)pData ) )
		return 0;

	CMeshManager::HANDLE hHandle = ALCHEMY_MESH_MANAGER.AddSceneData( pParaser->GetSceneData() );

	ALCHEMY_DEBUG_DELETE(pParaser);

	return AS3_Int(hHandle);
}


//////////////////////////////////////////////////////////////////////////
///main
//////////////////////////////////////////////////////////////////////////

int main(/*int argc, _TCHAR* argv[]*/)
{
	AS3_Val InitObjectMethod = AS3_Function( ALCHEMY_NULL, InitObject );
	AS3_Val InitParticleShowSystemMethod = AS3_Function( ALCHEMY_NULL, InitParticleShowSystem );
	
	AS3_Val CreateAGALAssemblerMethod = AS3_Function( ALCHEMY_NULL, CreateAGALAssembler );
	AS3_Val CreateBufferMethod = AS3_Function( ALCHEMY_NULL, CreateBuffer );
	AS3_Val CreateRenderSystemMethod = AS3_Function( ALCHEMY_NULL, CreateRenderSystem );
	AS3_Val CreateSceneNodeFactoryMethod = AS3_Function( ALCHEMY_NULL, CreateSceneNodeFactory );
	AS3_Val CreateMeshManagerMethod = AS3_Function(ALCHEMY_NULL, CreateMeshManager);
	AS3_Val CreateSceneManagerMethod = AS3_Function( ALCHEMY_NULL, CreateSceneManager );
	AS3_Val CreateMeshDataMethod = AS3_Function( ALCHEMY_NULL, CreateMeshData );
	AS3_Val CreateMeshMethod = AS3_Function( ALCHEMY_NULL, CreateMesh );
	AS3_Val CreatePlaneFromMeshManagerMethod = AS3_Function(ALCHEMY_NULL, CreatePlaneFromMeshManager);
	AS3_Val CreateBillboardMeshDataMethod = AS3_Function(ALCHEMY_NULL, CreateBillboardMeshData);
	
	AS3_Val CreateMeshFromEffectMethod = AS3_Function(ALCHEMY_NULL, CreateMeshFromEffect);
	
	AS3_Val CreateMaterialDataMethod = AS3_Function( ALCHEMY_NULL, CreateMaterialData );
	AS3_Val CreateTextureDataMethod = AS3_Function( ALCHEMY_NULL, CreateTextureData );
	AS3_Val CreateSceneDataMethod = AS3_Function( ALCHEMY_NULL, CreateSceneData );
	
	AS3_Val CreateMaterialMethod = AS3_Function( ALCHEMY_NULL, CreateMaterial );
	AS3_Val CreateLightMethod = AS3_Function( ALCHEMY_NULL, CreateLight );
	AS3_Val CreateParameterListMethod = AS3_Function( ALCHEMY_NULL, CreateParameterList );
	AS3_Val CreateRenderMethodMethod = AS3_Function( ALCHEMY_NULL, CreateRenderMethod );
	AS3_Val CreateRenderMethodFromEffectCodeMethod = AS3_Function( ALCHEMY_NULL, CreateRenderMethodFromEffectCode);
	AS3_Val CreateVertexBufferMethod = AS3_Function( ALCHEMY_NULL, CreateVertexBuffer );
	AS3_Val CreateIndexBufferMethod = AS3_Function( ALCHEMY_NULL, CreateIndexBuffer );
	AS3_Val CreateProgram3DMethod = AS3_Function( ALCHEMY_NULL, CreateProgram3D );
	AS3_Val CreateEffectMethod = AS3_Function( ALCHEMY_NULL, CreateEffect );
	AS3_Val CreateEffectFromEffectCodeMethod = AS3_Function( ALCHEMY_NULL, CreateEffectFromEffectCode );
	AS3_Val CreateTextureMethod = AS3_Function( ALCHEMY_NULL, CreateTexture );
	AS3_Val CreateCubeTextureMethod = AS3_Function( ALCHEMY_NULL, CreateCubeTexture );
	AS3_Val CreateSceneNodeMethod = AS3_Function( ALCHEMY_NULL, CreateSceneNode );
	AS3_Val CreateSkyPlaneMethod = AS3_Function( ALCHEMY_NULL, CreateSkyPlane );
	AS3_Val CreateSkyBoxMethod = AS3_Function( ALCHEMY_NULL, CreateSkyBox );
	AS3_Val CreateRenderStateSetMethod = AS3_Function( ALCHEMY_NULL, CreateRenderStateSet );

	
	AS3_Val CreateMirrorMethod = AS3_Function( ALCHEMY_NULL, CreateMirror );
	AS3_Val CreateWaterMethod = AS3_Function( ALCHEMY_NULL, CreateWater );
	AS3_Val CreateStaticMeshInstanceMethod = AS3_Function( ALCHEMY_NULL, CreateStaticMeshInstance );
	AS3_Val CreateBillboardSystemMethod = AS3_Function( ALCHEMY_NULL, CreateBillboardSystem );
	AS3_Val CreateSkyDomeMethod = AS3_Function( ALCHEMY_NULL, CreateSkyDome );
	AS3_Val CreateSkyDomeWithMeshMethod = AS3_Function( ALCHEMY_NULL, CreateSkyDomeWithMesh );
	AS3_Val CreateGrassSystemMethod = AS3_Function( ALCHEMY_NULL, CreateGrassSystem );
	AS3_Val CreateCascadShadowMapMethod = AS3_Function( ALCHEMY_NULL, CreateCascadShadowMap );
	AS3_Val CreateProgramRenderManagerMethod = AS3_Function( ALCHEMY_NULL, CreateProgramRenderManager );
	

	AS3_Val CreateParticleShowSystemMethod = AS3_Function( ALCHEMY_NULL, CreateParticleShowSystem );
	AS3_Val CreateMeshFillerMethod = AS3_Function( ALCHEMY_NULL, CreateMeshFiller );
	AS3_Val CreatePSLiveTimeAttributeMethod = AS3_Function( ALCHEMY_NULL, CreatePSLiveTimeAttribute );
	AS3_Val CreatePSParticleInfoAttributeMethod = AS3_Function( ALCHEMY_NULL, CreatePSParticleInfoAttribute );
	AS3_Val CreatePSPositionAttributeMethod = AS3_Function( ALCHEMY_NULL, CreatePSPositionAttribute );
	AS3_Val CreatePSTextureAttributeMethod = AS3_Function( ALCHEMY_NULL, CreatePSTextureAttribute );
	AS3_Val CreatePSVelocityAttributeMethod = AS3_Function( ALCHEMY_NULL, CreatePSVelocityAttribute );

	
	AS3_Val CreateNormalMeshProcessorMethod = AS3_Function( ALCHEMY_NULL, CreateNormalMeshProcessor);
	AS3_Val CreateSkinMeshProcessorMethod = AS3_Function( ALCHEMY_NULL, CreateSkinMeshProcessor);
	
	
	AS3_Val CreateShadowMapRenderMethod = AS3_Function( ALCHEMY_NULL, CreateShadowMapRender );
	AS3_Val CreateBeforePostProcessRenderMethod = AS3_Function( ALCHEMY_NULL, CreateBeforePostProcessRender );
	AS3_Val CreateScenePostProcessRenderMethod = AS3_Function( ALCHEMY_NULL, CreateScenePostProcessRender );

	AS3_Val CreateEnvironmentRenderMethod = AS3_Function( ALCHEMY_NULL, CreateEnvironmentRender );
	AS3_Val CreateSilhoutteProgramMethod = AS3_Function( ALCHEMY_NULL, CreateSilhoutteProgram );
	AS3_Val CreateShadowMapDisplayEffectMethod = AS3_Function( ALCHEMY_NULL, CreateShadowMapDisplayEffect );
	AS3_Val CreateDepthFieldEffectMethod = AS3_Function( ALCHEMY_NULL, CreateDepthFieldEffect );
	AS3_Val CreateContorEffectMethod = AS3_Function( ALCHEMY_NULL, CreateContorEffect );
	
	AS3_Val CreateSnowParticleComputeMethod = AS3_Function(ALCHEMY_NULL, CreateSnowParticleCompute );
	AS3_Val CreateVelocityVCMethod = AS3_Function(ALCHEMY_NULL, CreateVelocityVC );
	AS3_Val CreateVolumeLightShowMethod = AS3_Function(ALCHEMY_NULL, CreateVolumeLightShow );
	
	AS3_Val CreateAccelerationVelocityVCMethod = AS3_Function(ALCHEMY_NULL, CreateAccelerationVelocityVC );

	AS3_Val CreateCameraMethod = AS3_Function( ALCHEMY_NULL, CreateCamera );
	AS3_Val CreateSceneMethod = AS3_Function( ALCHEMY_NULL, CreateScene );
	
	AS3_Val CreateSceneObjectMethod = AS3_Function( ALCHEMY_NULL, CreateSceneObject );
	AS3_Val CreateParticleSystemMethod = AS3_Function( ALCHEMY_NULL, CreateParticleSystem );
	AS3_Val CreateBoneMethod = AS3_Function( ALCHEMY_NULL, CreateBone );
	AS3_Val CreateSkinInfoDataMethod = AS3_Function( ALCHEMY_NULL, CreateSkinInfoData );
	AS3_Val CreateSkinInfoMethod = AS3_Function( ALCHEMY_NULL, CreateSkinInfo );
	AS3_Val CreateBoneDataMethod = AS3_Function( ALCHEMY_NULL, CreateBoneData );
	AS3_Val CreateAniamtionSetMethod = AS3_Function( ALCHEMY_NULL, CreateAniamtionSet );
	AS3_Val CreateAnimationControllerMethod = AS3_Function( ALCHEMY_NULL, CreateAnimationController );
	AS3_Val CreateAnimationTrackerMethod = AS3_Function( ALCHEMY_NULL, CreateAnimationTracker );
	AS3_Val CreateAnimationSetDataMethod = AS3_Function( ALCHEMY_NULL, CreateAnimationSetData );
	AS3_Val CreateSkinMeshControllerMethod = AS3_Function( ALCHEMY_NULL, CreateSkinMeshController );

	
	AS3_Val CreateDeferredLightRendererMethod = AS3_Function(ALCHEMY_NULL, CreateDeferredLightRenderer);
	AS3_Val CreateEffectCodeMethod = AS3_Function(ALCHEMY_NULL, CreateEffectCode);
	AS3_Val CreateParticleDataCodeMethod = AS3_Function(ALCHEMY_NULL, CreateParticleDataCode);

	
	AS3_Val CreateEffectCodeFromDeferredRenderingPipelineMethod = AS3_Function(ALCHEMY_NULL, CreateEffectCodeFromDeferredRenderingPipeline);
	AS3_Val CreatePositionEffectMethod = AS3_Function( ALCHEMY_NULL, CreatePositionEffect );
	AS3_Val CreatePosition2DEffectMethod = AS3_Function( ALCHEMY_NULL, CreatePosition2DEffect );
	
	AS3_Val CreateTextureEffectMethod = AS3_Function(ALCHEMY_NULL, CreateTextureEffect);
	AS3_Val CreateForceTextureEffectMethod = AS3_Function(ALCHEMY_NULL, CreateForceTextureEffect);
	AS3_Val CreatePixelAmbientLightEffectMethod = AS3_Function(ALCHEMY_NULL, CreatePixelAmbientLightEffect);
	AS3_Val CreateFacingEffectMethod = AS3_Function(ALCHEMY_NULL, CreateFacingEffect);
	AS3_Val CreatePixelColorEffectMethod = AS3_Function(ALCHEMY_NULL, CreatePixelColorEffect);

	AS3_Val CreateEmptyEffectMethod = AS3_Function(ALCHEMY_NULL, CreateEmptyEffect);
	AS3_Val CreateVertexColorEffectMethod = AS3_Function(ALCHEMY_NULL, CreateVertexColorEffect);
	AS3_Val CreateDirectionalShdowDisplayEffectMethod = AS3_Function(ALCHEMY_NULL, CreateDirectionalShdowDisplayEffect);

	
	AS3_Val CreateAlphaTestBlendMethod = AS3_Function(ALCHEMY_NULL, CreateAlphaTestBlend);

	AS3_Val CreateDepthEffectMethod = AS3_Function(ALCHEMY_NULL, CreateDepthEffect);
	AS3_Val CreateDistanceInfoEffectMethod = AS3_Function(ALCHEMY_NULL, CreateDistanceInfoEffect);

	AS3_Val CreatePixelFogBlendMethod = AS3_Function(ALCHEMY_NULL, CreatePixelFogBlend);
	AS3_Val CreateEnvironmentDisplayEffectMethod = AS3_Function( ALCHEMY_NULL, CreateEnvironmentDisplayEffect );
	AS3_Val CreateNormalMapEffectMethod = AS3_Function( ALCHEMY_NULL, CreateNormalMapEffect );
	AS3_Val CreateBillboardEffectMethod = AS3_Function( ALCHEMY_NULL, CreateBillboardEffect );
	AS3_Val CreateMaskTextureEffectMethod = AS3_Function( ALCHEMY_NULL, CreateMaskTextureEffect );
	AS3_Val CreateSpecularTextureEffectMethod = AS3_Function( ALCHEMY_NULL, CreateSpecularTextureEffect );
	AS3_Val CreateCascadShadowDisplayEffectMethod = AS3_Function( ALCHEMY_NULL, CreateCascadShadowDisplayEffect );
	AS3_Val CreateTextureBlurEffectMethod = AS3_Function( ALCHEMY_NULL, CreateTextureBlurEffect );

	
	AS3_Val CreateTextureModulusEffectMethod = AS3_Function( ALCHEMY_NULL, CreateTextureModulusEffect );
	AS3_Val CreateBackgroundEffectMethod = AS3_Function( ALCHEMY_NULL, CreateBackgroundEffect );
	
	AS3_Val CreateEffectInfoSubMethod = AS3_Function( ALCHEMY_NULL, CreateEffectInfoSub );
	AS3_Val CreateBidirectionalBlendMethodMethod = AS3_Function( ALCHEMY_NULL, CreateBidirectionalBlendMethod );

	
	AS3_Val CreatePSMoveEffectMethod = AS3_Function( ALCHEMY_NULL, CreatePSMoveEffect );
	AS3_Val CreateGrassAnimationEffectMethod = AS3_Function( ALCHEMY_NULL, CreateGrassAnimationEffect );
	
	AS3_Val CreateEffectInfoAdditionMethod = AS3_Function(ALCHEMY_NULL, CreateEffectInfoAddition);
	AS3_Val CreateEffectInfoModulationMethod = AS3_Function(ALCHEMY_NULL, CreateEffectInfoModulation);

	AS3_Val CreateFresnelBlendMethodMethod = AS3_Function(ALCHEMY_NULL, CreateFresnelBlendMethod);
	AS3_Val CreateMulityEffectProgramMethod = AS3_Function(ALCHEMY_NULL, CreateMulityEffectProgram);
	AS3_Val CreateSkyPassDefinerMethod = AS3_Function(ALCHEMY_NULL, CreateSkyPassDefiner);

	


	AS3_Val DestroyBufferMethod = AS3_Function( ALCHEMY_NULL, DestroyBuffer );
	AS3_Val DestroyMeshDataMethod = AS3_Function( ALCHEMY_NULL, DestroyMeshData );
	AS3_Val DestroyMaterialDataMethod = AS3_Function( ALCHEMY_NULL, DestroyMaterialData );
	AS3_Val DestroyParameterListMethod = AS3_Function( ALCHEMY_NULL, DestroyParameterList );
	AS3_Val DestroySkinInfoDataMethod = AS3_Function( ALCHEMY_NULL, DestroySkinInfoData );
	AS3_Val DestroySkinInfoMethod = AS3_Function( ALCHEMY_NULL, DestroySkinInfo );

	AS3_Val DestroyBoneDataMethod = AS3_Function( ALCHEMY_NULL, DestroyBoneData );
	AS3_Val DestroyAnimTrackMethod = AS3_Function( ALCHEMY_NULL, DestroyAnimTrack );
	AS3_Val DestroyAnimSetDataMethod = AS3_Function( ALCHEMY_NULL, DestroyAnimSetData );
	AS3_Val DestroyAnimSetMethod = AS3_Function( ALCHEMY_NULL, DestroyAnimSet );
	AS3_Val DestroyAnimControllerMethod = AS3_Function( ALCHEMY_NULL, DestroyAnimController );
	AS3_Val DestroyStreamBufferMethod = AS3_Function( ALCHEMY_NULL, DestroyStreamBuffer );
	AS3_Val DestroyEffectCodeMethod = AS3_Function( ALCHEMY_NULL, DestroyEffectCode );
	AS3_Val DestroyTextureEffectMethod = AS3_Function( ALCHEMY_NULL, DestroyTextureEffect );

	AS3_Val DestroyTextureDataMethod = AS3_Function( ALCHEMY_NULL, DestroyTextureData );
	AS3_Val DestroySceneDataMethod = AS3_Function( ALCHEMY_NULL, DestroySceneData );
	
	AS3_Val FillObjectMethod = AS3_Function( ALCHEMY_NULL, FillObject );
	AS3_Val AddTextureToMaterialMethod = AS3_Function( ALCHEMY_NULL, AddTextureToMaterial );
	AS3_Val AddAttributeToMeshMethod = AS3_Function( ALCHEMY_NULL, AddAttributeToMesh );
	AS3_Val AddChildToSceneNodeMethod = AS3_Function( ALCHEMY_NULL, AddChildToSceneNode );
	AS3_Val AddLightToSceneManagerMethod =  AS3_Function( ALCHEMY_NULL, AddLightToSceneManager );
	AS3_Val AddChildToSceneManagerMethod = AS3_Function( ALCHEMY_NULL, AddChildToSceneManager );
	AS3_Val AddCameraToSceneManagerMethod = AS3_Function( ALCHEMY_NULL, AddCameraToSceneManager );

	
	AS3_Val AddInfoToPassDefinerMethod = AS3_Function(ALCHEMY_NULL, AddInfoToPassDefiner);

	

	AS3_Val AddSkinDataToMeshManagerMethod = AS3_Function(ALCHEMY_NULL, AddSkinDataToMeshManager);
	AS3_Val AddTextureDataToMeshManagerMethod = AS3_Function(ALCHEMY_NULL, AddTextureDataToMeshManager);
	AS3_Val AddMaterialDataToMeshManagerMethod = AS3_Function(ALCHEMY_NULL, AddMaterialDataToMeshManager);
	AS3_Val AddMeshDataToMeshManagerMethod = AS3_Function(ALCHEMY_NULL, AddMeshDataToMeshManager);
	AS3_Val AddSceneDataToMeshManagerMethod = AS3_Function(ALCHEMY_NULL, AddSceneDataToMeshManager);
	AS3_Val AddNodeToBillboardSystemMethod = AS3_Function(ALCHEMY_NULL, AddNodeToBillboardSystem);
	AS3_Val AddShadowCasterToCascadShadowMapMethod = AS3_Function(ALCHEMY_NULL, AddShadowCasterToCascadShadowMap);
	AS3_Val AddShadowCasterToShadowMapMethod = AS3_Function(ALCHEMY_NULL, AddShadowCasterToShadowMap);

	

	AS3_Val AddInfoForBlendMethodToPassDefinerMethod = AS3_Function(ALCHEMY_NULL, AddInfoForBlendMethodToPassDefiner);
	AS3_Val AddColorKeyAndInfoToPassDefinerMethod = AS3_Function(ALCHEMY_NULL, AddColorKeyAndInfoToPassDefiner);
	AS3_Val AddColorKeyInfoToPassDefinerMethod = AS3_Function(ALCHEMY_NULL, AddColorKeyInfoToPassDefiner);
	AS3_Val AddColorKeyToPassDefinerMethod = AS3_Function(ALCHEMY_NULL, AddColorKeyToPassDefiner);

	AS3_Val AddAttributeToMeshFillerMethod = AS3_Function(ALCHEMY_NULL, AddAttributeToMeshFiller);
	AS3_Val FillMeshFromMeshFillerMethod = AS3_Function(ALCHEMY_NULL, FillMeshFromMeshFiller);
	
	AS3_Val AddParticleComputeInfoToDataCodeMethod = AS3_Function(ALCHEMY_NULL, AddParticleComputeInfoToDataCode);
	AS3_Val AddParticleVariableInfoToDataCodeMethod = AS3_Function(ALCHEMY_NULL, AddParticleVariableInfoToDataCode);
	AS3_Val BuildParticleControllerMethod = AS3_Function(ALCHEMY_NULL, BuildParticleController);

	
	AS3_Val AddProgramRenderToManagerMethod = AS3_Function(ALCHEMY_NULL, AddProgramRenderToManager);
	AS3_Val AddRenderObjectToProgramMethod = AS3_Function(ALCHEMY_NULL, AddRenderObjectToProgram);
	AS3_Val AddRenderObjectSelfToProgramMethod = AS3_Function(ALCHEMY_NULL, AddRenderObjectSelfToProgram);
	

	AS3_Val RemoveTextureFromMaterialMethod = AS3_Function( ALCHEMY_NULL, RemoveTextureFromMaterial );
	AS3_Val RemoveAttributeFromMeshMethod = AS3_Function( ALCHEMY_NULL, RemoveAttributeFromMesh );
	AS3_Val RemoveChildFromSceneNodeMethod = AS3_Function( ALCHEMY_NULL, RemoveChildFromSceneNode );
	AS3_Val RemoveChildFromSceneManagerMethod = AS3_Function( ALCHEMY_NULL, RemoveChildFromSceneManager );

	AS3_Val RemoveNodeFromBillboardSystemMethod = AS3_Function( ALCHEMY_NULL, RemoveNodeFromBillboardSystem );

	AS3_Val LockTextureMethod = AS3_Function( ALCHEMY_NULL, LockTexture );
	AS3_Val UnlockMethod = AS3_Function( ALCHEMY_NULL, UnlockTexture );
	
	AS3_Val SetViewportToContext3DMethod = AS3_Function( ALCHEMY_NULL, SetViewportToContext3D );
	AS3_Val GetAttributeDataFromMeshMethod = AS3_Function( ALCHEMY_NULL, GetAttributeDataFromMesh );
	AS3_Val SetAttributeDataFromMeshMethod = AS3_Function( ALCHEMY_NULL, SetAttributeDataFromMesh );

	AS3_Val GetMeshFromProcessorMethod = AS3_Function( ALCHEMY_NULL, GetMeshFromProcessor );
	
	AS3_Val GetHardwareSkinEffectMethod = AS3_Function( ALCHEMY_NULL, GetHardwareSkinEffect );


	AS3_Val SetProjectionToCameraMethod = AS3_Function( ALCHEMY_NULL, SetProjectionToCamera );
	

	AS3_Val RenderSceneMethod = AS3_Function( ALCHEMY_NULL, RenderScene );
	AS3_Val UpdateAnimationMethod = AS3_Function( ALCHEMY_NULL, UpdateAnimation );
	
	AS3_Val UpdateProgramRenderManagerMethod = AS3_Function( ALCHEMY_NULL, UpdateProgramRenderManager );
	
	AS3_Val SetBonesDataToSkinInfoMethod = AS3_Function( ALCHEMY_NULL, SetBonesDataToSkinInfo );
	AS3_Val CloneSceneNodeMethod = AS3_Function(ALCHEMY_NULL, CloneSceneNode);
	AS3_Val SetNodeNameMethod = AS3_Function( ALCHEMY_NULL, SetNodeName );
	AS3_Val SetMeshProcessorToSceneObjectMethod = AS3_Function( ALCHEMY_NULL, SetMeshProcessorToSceneObject );
	


	AS3_Val SetEffectToSceneObjectMethod = AS3_Function( ALCHEMY_NULL, SetEffectToSceneObject );
	AS3_Val SetRenderMethodToSceneObjectMethod = AS3_Function( ALCHEMY_NULL, SetRenderMethodToSceneObject );
	AS3_Val SetEffectToGrassSystemMethod = AS3_Function(ALCHEMY_NULL, SetEffectToGrassSystem);
	AS3_Val SetRenderMethodToGrassSystemMethod = AS3_Function(ALCHEMY_NULL, SetRenderMethodToGrassSystem);
	AS3_Val SetSceneDataToStaticMeshInstanceMethod = AS3_Function( ALCHEMY_NULL, SetSceneDataToStaticMeshInstance );

	AS3_Val SetEffectToStaticMeshInstanceMethod = AS3_Function( ALCHEMY_NULL, SetEffectToStaticMeshInstance );
	AS3_Val SetRenderMethodToStaticMeshInstanceMethod = AS3_Function( ALCHEMY_NULL, SetRenderMethodToStaticMeshInstance );
	AS3_Val SetEffectRenderToSkyDomeMethod = AS3_Function( ALCHEMY_NULL, SetEffectRenderToSkyDome );

	

	
	AS3_Val SetUVSpeedToSkyDomeMethod = AS3_Function( ALCHEMY_NULL, SetUVSpeedToSkyDome );
	AS3_Val SetMaterialToGrassSystemMethod = AS3_Function( ALCHEMY_NULL, SetMaterialToGrassSystem );
	AS3_Val SetOrghMatrixToCameraMethod = AS3_Function( ALCHEMY_NULL, SetOrghMatrixToCamera );
	AS3_Val SetVisibleToSceneNodeMethod = AS3_Function( ALCHEMY_NULL, SetVisibleToSceneNode );

	
	
	/*AS3_Val SetRenderInfoFromRenderMethod = AS3_Function( ALCHEMY_NULL, SetRenderInfoFromRender );*/

	
	AS3_Val GetDestinationMeshFromSceneObjectMehod = AS3_Function(ALCHEMY_NULL, GetDestinationMeshFromSceneObject);
	//AS3_Val SetAnimControllerToProcessorMehod = AS3_Function(ALCHEMY_NULL, SetAnimControllerToProcessor);
	AS3_Val SetFrameEmitToParticleSystemMehod = AS3_Function(ALCHEMY_NULL, SetFrameEmitToParticleSystem);

	AS3_Val SetVisibleToRenderMehod = AS3_Function(ALCHEMY_NULL, SetVisibleToRender);

	AS3_Val GetSkinControllerInfoMehod = AS3_Function(ALCHEMY_NULL, GetSkinControllerInfo);
	AS3_Val GetSkinProcessorInfoMehod = AS3_Function(ALCHEMY_NULL, GetSkinProcessorInfo);
	AS3_Val GetMeshProcessorFromSceneObjectMehod = AS3_Function(ALCHEMY_NULL, GetMeshProcessorFromSceneObject);
	AS3_Val GetAnimControllerFromProcessorMehod = AS3_Function(ALCHEMY_NULL, GetAnimControllerFromProcessor);
	AS3_Val GetMaterialFromSceneObjectMehod = AS3_Function(ALCHEMY_NULL, GetMaterialFromSceneObject);
	AS3_Val GetEffectFromSceneObjectMehod = AS3_Function(ALCHEMY_NULL, GetEffectFromSceneObject);
	AS3_Val GetRenderMethodFromSceneObjectMehod = AS3_Function(ALCHEMY_NULL, GetRenderMethodFromSceneObject);

	AS3_Val GetAnimationSetFromControllerMehod = AS3_Function(ALCHEMY_NULL, GetAnimationSetFromController);
	AS3_Val GetPlayTimeFromAnimationSetMehod = AS3_Function(ALCHEMY_NULL, GetPlayTimeFromAnimationSet);

	AS3_Val GetMaterialNumFromStaticMeshInstanceMehod = AS3_Function(ALCHEMY_NULL, GetMaterialNumFromStaticMeshInstance);
	AS3_Val GetMaterialFromStaticMeshInstanceMehod = AS3_Function(ALCHEMY_NULL, GetMaterialFromStaticMeshInstance);
	AS3_Val GetSceneNodeNameMehod = AS3_Function(ALCHEMY_NULL, GetSceneNodeName);
	AS3_Val GetSphereFromCameraMehod = AS3_Function(ALCHEMY_NULL, GetSphereFromCamera);
	

	AS3_Val EndDefinePassDefinerMethod = AS3_Function( ALCHEMY_NULL, EndDefinePassDefiner );
	AS3_Val BuildShadowMapFromLightMethod = AS3_Function( ALCHEMY_NULL, BuildShadowMapFromLight );
	AS3_Val AddEffectInfoToPassFromSceneManagerMethod = AS3_Function( ALCHEMY_NULL, AddEffectInfoToPassFromSceneManager );
	AS3_Val ResetAnimationControllerMethod = AS3_Function( ALCHEMY_NULL, ResetAnimationController );

	
	AS3_Val RegisterAniamtionSetToControllerMethod = AS3_Function( ALCHEMY_NULL, RegisterAniamtionSetToController );
	AS3_Val UnregisterAnimationSetFromControllerMethod = AS3_Function( ALCHEMY_NULL, UnregisterAnimationSetFromController );
	AS3_Val RegisterBoneToAnimationControllerMethod = AS3_Function( ALCHEMY_NULL, RegisterBoneToAnimationController );
	
	AS3_Val SetTrackersToAnimationSetMethod = AS3_Function( ALCHEMY_NULL, SetTrackersToAnimationSet );
	AS3_Val ComputeMeshTangentMethod = AS3_Function( ALCHEMY_NULL, ComputeMeshTangent );
	AS3_Val ComputeMeshNormalMethod = AS3_Function( ALCHEMY_NULL, ComputeMeshNormal );
	
	AS3_Val WriteDataToVertexBufferMethod = AS3_Function( ALCHEMY_NULL, WriteDataToVertexBuffer );
	AS3_Val ApplyForRenderFromDeferredLightRendererMethod = AS3_Function(ALCHEMY_NULL, ApplyForRenderFromDeferredLightRenderer);
	//AS3_Val ApplyForRenderFromRenderMethod = AS3_Function( ALCHEMY_NULL, ApplyForRenderFromRender );

	//AS3_Val GetRenderTargetFromRenderMethod = AS3_Function( ALCHEMY_NULL, GetRenderTargetFromRender );
	AS3_Val GetTextureByIndexFromMaterialMethod = AS3_Function( ALCHEMY_NULL, GetTextureByIndexFromMaterial );
	AS3_Val GetTextureNumFromMaterialMethod = AS3_Function( ALCHEMY_NULL, GetTextureNumFromMaterial );
	AS3_Val GetNodeNameMethod = AS3_Function( ALCHEMY_NULL, GetNodeName );

	AS3_Val CreateCartoonEffectMethod = AS3_Function( ALCHEMY_NULL, CreateCartoonEffect );

	AS3_Val OnCreateSceneMethod = AS3_Function( ALCHEMY_NULL, OnCreateScene );
	
	AS3_Val CreateOutlineEffectMethod = AS3_Function( ALCHEMY_NULL, CreateOutlineEffect );
		
	AS3_Val Result = AS3_Object("InitObject:AS3ValType,\
								InitParticleShowSystem:AS3ValType,\
								CreateBuffer:AS3ValType,\
								CreateAGALAssembler:AS3ValType,\
								CreateRenderSystem:AS3ValType,\
								CreateSceneNodeFactory:AS3ValType,\
								CreateMeshManager:AS3ValType,\
								CreateSceneManager:AS3ValType,\
								CreateMeshData:AS3ValType,\
								CreateMesh:AS3ValType,\
								CreatePlaneFromMeshManager:AS3ValType,\
								CreateBillboardMeshData:AS3ValType,\
								CreateMeshFromEffect:AS3ValType,\
								CreateMaterialData:AS3ValType,\
								CreateTextureData:AS3ValType,\
								CreateSceneData:AS3ValType,\
								CreateMaterial:AS3ValType,\
								CreateLight:AS3ValType,\
								CreateParameterList:AS3ValType,\
								CreateRenderMethod:AS3ValType,\
								CreateRenderMethodFromEffectCode:AS3ValType,\
								CreateVertexBuffer:AS3ValType,\
								CreateIndexBuffer:AS3ValType,\
								CreateProgram3D:AS3ValType,\
								CreateTexture:AS3ValType,\
								CreateCubeTexture:AS3ValType,\
								CreateEffect:AS3ValType,\
								CreateEffectFromEffectCode:AS3ValType,\
								CreateSceneNode:AS3ValType,\
								CreateSkyPlane:AS3ValType,\
								CreateSkyBox:AS3ValType,\
								CreateRenderStateSet:AS3ValType,\
								CreateMirror:AS3ValType,\
								CreateWater:AS3ValType,\
								CreateStaticMeshInstance:AS3ValType,\
								CreateBillboardSystem:AS3ValType,\
								CreateSkyDome:AS3ValType,\
								CreateSkyDomeWithMesh:AS3ValType,\
								CreateGrassSystem:AS3ValType,\
								CreateCascadShadowMap:AS3ValType,\
								CreateProgramRenderManager:AS3ValType,\
								CreateParticleShowSystem:AS3ValType,\
								CreateMeshFiller:AS3ValType,\
								CreatePSLiveTimeAttribute:AS3ValType,\
								CreatePSParticleInfoAttribute:AS3ValType,\
								CreatePSPositionAttribute:AS3ValType,\
								CreatePSTextureAttribute:AS3ValType,\
								CreatePSVelocityAttribute:AS3ValType,\
								CreateNormalMeshProcessor:AS3ValType,\
								CreateSkinMeshProcessor:AS3ValType,\
								CreateShadowMapRender:AS3ValType,\
								CreateBeforePostProcessRender:AS3ValType,\
								CreateScenePostProcessRender:AS3ValType,\
								CreateEnvironmentRender:AS3ValType,\
								CreateSilhoutteProgram:AS3ValType,\
								CreateCamera:AS3ValType,\
								CreateScene:AS3ValType,\
								CreateSceneObject:AS3ValType,\
								CreateParticleSystem:AS3ValType,\
								CreateBone:AS3ValType,\
								CreateSkinInfoData:AS3ValType,\
								CreateSkinInfo:AS3ValType,\
								CreateBoneData:AS3ValType,\
								CreateAniamtionSet:AS3ValType,\
								CreateAnimationController:AS3ValType,\
								CreateAnimationTracker:AS3ValType,\
								CreateAnimationSetData:AS3ValType,\
								CreateSkinMeshController:AS3ValType,\
								CreateDeferredLightRenderer:AS3ValType,\
								CreateEffectCode:AS3ValType,\
								CreateParticleDataCode:AS3ValType,\
								CreateEffectCodeFromDeferredRenderingPipeline:AS3ValType,\
								CreatePositionEffect:AS3ValType,\
								CreatePosition2DEffect:AS3ValType,\
								CreateTextureEffect:AS3ValType,\
								CreateForceTextureEffect:AS3ValType,\
								CreatePixelAmbientLightEffect:AS3ValType,\
								CreateFacingEffect:AS3ValType,\
								CreatePixelColorEffect:AS3ValType,\
								CreateEmptyEffect:AS3ValType,\
								CreateVertexColorEffect:AS3ValType,\
								CreateDirectionalShdowDisplayEffect:AS3ValType,\
								CreateAlphaTestBlend:AS3ValType,\
								CreateDepthEffect:AS3ValType,\
								CreateDistanceInfoEffect:AS3ValType,\
								CreatePixelFogBlend:AS3ValType,\
								CreateEnvironmentDisplayEffect:AS3ValType,\
								CreateNormalMapEffect:AS3ValType,\
								CreateBillboardEffect:AS3ValType,\
								CreateMaskTextureEffect:AS3ValType,\
								CreateSpecularTextureEffect:AS3ValType,\
								CreateCascadShadowDisplayEffect:AS3ValType,\
								CreateTextureBlurEffect:AS3ValType,\
								CreateTextureModulusEffect:AS3ValType,\
								CreateBackgroundEffect:AS3ValType,\
								CreateEffectInfoSub:AS3ValType,\
								CreateBidirectionalBlendMethod:AS3ValType,\
								CreatePSMoveEffect:AS3ValType,\
								CreateGrassAnimationEffect:AS3ValType,\
								CreateEffectInfoAddition:AS3ValType,\
								CreateEffectInfoModulation:AS3ValType,\
								CreateFresnelBlendMethod:AS3ValType,\
								CreateMulityEffectProgram:AS3ValType,\
								CreateSkyPassDefiner:AS3ValType,\
								CreateShadowMapDisplayEffect:AS3ValType,\
								CreateDepthFieldEffect:AS3ValType,\
								CreateContorEffect:AS3ValType,\
								CreateSnowParticleCompute:AS3ValType,\
								CreateVelocityVC:AS3ValType,\
								CreateVolumeLightShow:AS3ValType,\
								CreateAccelerationVelocityVC:AS3ValType,\
								DestroyBuffer:AS3ValType,\
								DestroyMeshData:AS3ValType,\
								DestroyMaterialData:AS3ValType,\
								DestroyParameterList:AS3ValType,\
								DestroySkinInfoData:AS3ValType,\
								DestroySkinInfo:AS3ValType,\
								DestroyBoneData:AS3ValType,\
								DestroyAnimTrack:AS3ValType,\
								DestroyAnimSetData:AS3ValType,\
								DestroyAnimSet:AS3ValType,\
								DestroyAnimController:AS3ValType,\
								DestroyStreamBuffer:AS3ValType,\
								DestroyEffectCode:AS3ValType,\
								DestroyTextureEffect:AS3ValType,\
								DestroyTextureData:AS3ValType,\
								DestroySceneData:AS3ValType,\
								FillObject:AS3ValType,\
								AddTextureToMaterial:AS3ValType,\
								AddAttributeToMesh:AS3ValType,\
								AddChildToSceneNode:AS3ValType,\
								AddLightToSceneManager:AS3ValType,\
								AddChildToSceneManager:AS3ValType,\
								AddCameraToSceneManager:AS3ValType,\
								AddInfoToPassDefiner:AS3ValType,\
								AddSkinDataToMeshManager:AS3ValType,\
								AddTextureDataToMeshManager:AS3ValType,\
								AddMaterialDataToMeshManager:AS3ValType,\
								AddMeshDataToMeshManager:AS3ValType,\
								AddSceneDataToMeshManager:AS3ValType,\
								AddNodeToBillboardSystem:AS3ValType,\
								AddShadowCasterToCascadShadowMap:AS3ValType,\
								AddShadowCasterToShadowMap:AS3ValType,\
								AddInfoForBlendMethodToPassDefiner:AS3ValType,\
								AddColorKeyAndInfoToPassDefiner:AS3ValType,\
								AddColorKeyInfoToPassDefiner:AS3ValType,\
								AddColorKeyToPassDefiner:AS3ValType,\
								AddAttributeToMeshFiller:AS3ValType,\
								FillMeshFromMeshFiller:AS3ValType,\
								AddParticleComputeInfoToDataCode:AS3ValType,\
								AddParticleVariableInfoToDataCode:AS3ValType,\
								BuildParticleController:AS3ValType,\
								AddProgramRenderToManager:AS3ValType,\
								AddRenderObjectToProgram:AS3ValType,\
								AddRenderObjectSelfToProgram:AS3ValType,\
								RemoveTextureFromMaterial:AS3ValType,\
								RemoveAttributeFromMesh:AS3ValType,\
								RemoveChildFromSceneNode:AS3ValType,\
								RemoveChildFromSceneManager:AS3ValType,\
								RemoveNodeFromBillboardSystem:AS3ValType,\
								LockTexture:AS3ValType,\
								UnlockTexture:AS3ValType,\
								SetViewportToContext3D:AS3ValType,\
								GetAttributeDataFromMesh:AS3ValType,\
								SetAttributeDataFromMesh:AS3ValType,\
								GetMeshFromProcessor:AS3ValType,\
								GetHardwareSkinEffect:AS3ValType,\
								SetProjectionToCamera:AS3ValType,\
								RenderScene:AS3ValType,\
								UpdateAnimation:AS3ValType,\
								UpdateProgramRenderManager:AS3ValType,\
								SetBonesDataToSkinInfo:AS3ValType,\
								CloneSceneNode:AS3ValType, \
								SetNodeName:AS3ValType,\
								EndDefinePassDefiner:AS3ValType,\
								BuildShadowMapFromLight:AS3ValType,\
								AddEffectInfoToPassFromSceneManager:AS3ValType,\
								ResetAnimationController:AS3ValType,\
								RegisterAniamtionSetToController:AS3ValType,\
								UnregisterAnimationSetFromController:AS3ValType,\
								RegisterBoneToAnimationController:AS3ValType,\
								SetTrackersToAnimationSet:AS3ValType,\
								ComputeMeshTangent:AS3ValType,\
								ComputeMeshNormal:AS3ValType,\
								SetMeshProcessorToSceneObject:AS3ValType,\
								SetEffectToSceneObject:AS3ValType,\
								SetRenderMethodToSceneObject:AS3ValType,\
								SetEffectToGrassSystem:AS3ValType, \
								SetRenderMethodToGrassSystem:AS3ValType, \
								SetSceneDataToStaticMeshInstance:AS3ValType,\
								SetEffectToStaticMeshInstance:AS3ValType,\
								SetRenderMethodToStaticMeshInstance:AS3ValType,\
								SetEffectRenderToSkyDome:AS3ValType,\
								SetUVSpeedToSkyDome:AS3ValType,\
								SetMaterialToGrassSystem:AS3ValType,\
								SetOrghMatrixToCamera:AS3ValType,\
								SetVisibleToSceneNode:AS3ValType,\
								GetDestinationMeshFromSceneObject:AS3ValType,\
								SetFrameEmitToParticleSystem:AS3ValType,\
								SetVisibleToRender:AS3ValType,\
								GetSkinControllerInfo:AS3ValType,\
								GetSkinProcessorInfo:AS3ValType,\
								GetMeshProcessorFromSceneObject:AS3ValType,\
								GetAnimControllerFromProcessor:AS3ValType,\
								GetMaterialFromSceneObject:AS3ValType,\
								GetEffectFromSceneObject:AS3ValType,\
								GetRenderMethodFromSceneObject:AS3ValType,\
								GetAnimationSetFromController:AS3ValType,\
								GetPlayTimeFromAnimationSet:AS3ValType,\
								GetMaterialNumFromStaticMeshInstance:AS3ValType,\
								GetMaterialFromStaticMeshInstance:AS3ValType,\
								GetSceneNodeName:AS3ValType,\
								GetSphereFromCamera:AS3ValType,\
								WriteDataToVertexBuffer:AS3ValType,\
								ApplyForRenderFromDeferredLightRenderer:AS3ValType,\
								GetTextureByIndexFromMaterial:AS3ValType,\
								GetTextureNumFromMaterial:AS3ValType,\
								GetNodeName:AS3ValType,\
								CreateCartoonEffect:AS3ValType,\
								OnCreateScene:AS3ValType,\
								CreateOutlineEffect:AS3ValType",
								InitObjectMethod,
								InitParticleShowSystemMethod,
								CreateBufferMethod,
								CreateAGALAssemblerMethod,
								CreateRenderSystemMethod,
								CreateSceneNodeFactoryMethod,
								CreateMeshManagerMethod,
								CreateSceneManagerMethod,
								CreateMeshDataMethod,
								CreateMeshMethod,
								CreatePlaneFromMeshManagerMethod,
								CreateBillboardMeshDataMethod,
								CreateMeshFromEffectMethod,
								CreateMaterialDataMethod,
								CreateTextureDataMethod,
								CreateSceneDataMethod,
								CreateMaterialMethod,
								CreateLightMethod,
								CreateParameterListMethod,
								CreateRenderMethodMethod,
								CreateRenderMethodFromEffectCodeMethod,
								CreateVertexBufferMethod,
								CreateIndexBufferMethod,
								CreateProgram3DMethod,
								CreateTextureMethod,
								CreateCubeTextureMethod,
								CreateEffectMethod,
								CreateEffectFromEffectCodeMethod,
								CreateSceneNodeMethod,
								CreateSkyPlaneMethod,
								CreateSkyBoxMethod,
								CreateRenderStateSetMethod,
								CreateMirrorMethod,
								CreateWaterMethod,
								CreateStaticMeshInstanceMethod,
								CreateBillboardSystemMethod,
								CreateSkyDomeMethod,
								CreateSkyDomeWithMeshMethod,
								CreateGrassSystemMethod, 
								CreateCascadShadowMapMethod,
								CreateProgramRenderManagerMethod,
								CreateParticleShowSystemMethod,
								CreateMeshFillerMethod,
								CreatePSLiveTimeAttributeMethod,
								CreatePSParticleInfoAttributeMethod,
								CreatePSPositionAttributeMethod,
								CreatePSTextureAttributeMethod,
								CreatePSVelocityAttributeMethod,
								CreateNormalMeshProcessorMethod,
								CreateSkinMeshProcessorMethod,
								CreateShadowMapRenderMethod,
								CreateBeforePostProcessRenderMethod,
								CreateScenePostProcessRenderMethod,
								CreateEnvironmentRenderMethod,
								CreateSilhoutteProgramMethod,
								CreateCameraMethod,
								CreateSceneMethod,
								CreateSceneObjectMethod,
								CreateParticleSystemMethod,
								CreateBoneMethod,
								CreateSkinInfoDataMethod,
								CreateSkinInfoMethod,
								CreateBoneDataMethod,
								CreateAniamtionSetMethod,
								CreateAnimationControllerMethod,
								CreateAnimationTrackerMethod,
								CreateAnimationSetDataMethod,
								CreateSkinMeshControllerMethod,
								CreateDeferredLightRendererMethod,
								CreateEffectCodeMethod,
								CreateParticleDataCodeMethod,
								CreateEffectCodeFromDeferredRenderingPipelineMethod,
								CreatePositionEffectMethod,
								CreatePosition2DEffectMethod,
								CreateTextureEffectMethod,
								CreateForceTextureEffectMethod,
								CreatePixelAmbientLightEffectMethod,
								CreateFacingEffectMethod,
								CreatePixelColorEffectMethod,
								CreateEmptyEffectMethod,
								CreateVertexColorEffectMethod,
								CreateDirectionalShdowDisplayEffectMethod,
								CreateAlphaTestBlendMethod,
								CreateDepthEffectMethod,
								CreateDistanceInfoEffectMethod,
								CreatePixelFogBlendMethod,
								CreateEnvironmentDisplayEffectMethod,
								CreateNormalMapEffectMethod,
								CreateBillboardEffectMethod,
								CreateMaskTextureEffectMethod,
								CreateSpecularTextureEffectMethod,
								CreateCascadShadowDisplayEffectMethod,
								CreateTextureBlurEffectMethod,
								CreateTextureModulusEffectMethod,
								CreateBackgroundEffectMethod,
								CreateEffectInfoSubMethod,
								CreateBidirectionalBlendMethodMethod,
								CreatePSMoveEffectMethod,
								CreateGrassAnimationEffectMethod,
								CreateEffectInfoAdditionMethod,
								CreateEffectInfoModulationMethod,
								CreateFresnelBlendMethodMethod,
								CreateMulityEffectProgramMethod,
								CreateSkyPassDefinerMethod,
								CreateShadowMapDisplayEffectMethod,
								CreateDepthFieldEffectMethod,
								CreateContorEffectMethod,
								CreateSnowParticleComputeMethod,
								CreateVelocityVCMethod,
								CreateVolumeLightShowMethod,
								CreateAccelerationVelocityVCMethod,
								DestroyBufferMethod,
								DestroyMeshDataMethod,
								DestroyMaterialDataMethod,
								DestroyParameterListMethod,
								DestroySkinInfoDataMethod,
								DestroySkinInfoMethod,
								DestroyBoneDataMethod,
								DestroyAnimTrackMethod,
								DestroyAnimSetDataMethod,
								DestroyAnimSetMethod,
								DestroyAnimControllerMethod,
								DestroyStreamBufferMethod,
								DestroyEffectCodeMethod,
								DestroyTextureEffectMethod,
								DestroyTextureDataMethod,
								DestroySceneDataMethod,
								FillObjectMethod,
								AddTextureToMaterialMethod,
								AddAttributeToMeshMethod,
								AddChildToSceneNodeMethod,
								AddLightToSceneManagerMethod,
								AddChildToSceneManagerMethod,
								AddCameraToSceneManagerMethod,
								AddInfoToPassDefinerMethod,
								AddSkinDataToMeshManagerMethod,
								AddTextureDataToMeshManagerMethod,
								AddMaterialDataToMeshManagerMethod,
								AddMeshDataToMeshManagerMethod,
								AddSceneDataToMeshManagerMethod,
								AddNodeToBillboardSystemMethod,
								AddShadowCasterToCascadShadowMapMethod,
								AddShadowCasterToShadowMapMethod,
								AddInfoForBlendMethodToPassDefinerMethod,
								AddColorKeyAndInfoToPassDefinerMethod,
								AddColorKeyInfoToPassDefinerMethod,
								AddColorKeyToPassDefinerMethod,
								AddAttributeToMeshFillerMethod,
								FillMeshFromMeshFillerMethod,
								AddParticleComputeInfoToDataCodeMethod,
								AddParticleVariableInfoToDataCodeMethod,
								BuildParticleControllerMethod,
								AddProgramRenderToManagerMethod,
								AddRenderObjectToProgramMethod,
								AddRenderObjectSelfToProgramMethod,
								RemoveTextureFromMaterialMethod,
								RemoveAttributeFromMeshMethod,
								RemoveChildFromSceneNodeMethod,
								RemoveChildFromSceneManagerMethod,
								RemoveNodeFromBillboardSystemMethod,
								LockTextureMethod,
								UnlockMethod,
								SetViewportToContext3DMethod, 
								GetAttributeDataFromMeshMethod,
								SetAttributeDataFromMeshMethod,
								GetMeshFromProcessorMethod,
								GetHardwareSkinEffectMethod,
								SetProjectionToCameraMethod,
								RenderSceneMethod,
								UpdateAnimationMethod,
								UpdateProgramRenderManagerMethod,
								SetBonesDataToSkinInfoMethod,
								CloneSceneNodeMethod, 
								SetNodeNameMethod,
								EndDefinePassDefinerMethod,
								BuildShadowMapFromLightMethod, 
								AddEffectInfoToPassFromSceneManagerMethod,
								ResetAnimationControllerMethod,
								RegisterAniamtionSetToControllerMethod,
								UnregisterAnimationSetFromControllerMethod,
								RegisterBoneToAnimationControllerMethod,
								SetTrackersToAnimationSetMethod,
								ComputeMeshTangentMethod,
								ComputeMeshNormalMethod,
								SetMeshProcessorToSceneObjectMethod,
								SetEffectToSceneObjectMethod,
								SetRenderMethodToSceneObjectMethod,
								SetEffectToGrassSystemMethod,
								SetRenderMethodToGrassSystemMethod, 
								SetSceneDataToStaticMeshInstanceMethod,
								SetEffectToStaticMeshInstanceMethod,
								SetRenderMethodToStaticMeshInstanceMethod,
								SetEffectRenderToSkyDomeMethod,
								SetUVSpeedToSkyDomeMethod,
								SetMaterialToGrassSystemMethod,
								SetOrghMatrixToCameraMethod,
								SetVisibleToSceneNodeMethod,
								GetDestinationMeshFromSceneObjectMehod,
								SetFrameEmitToParticleSystemMehod,
								SetVisibleToRenderMehod,
								GetSkinControllerInfoMehod,
								GetSkinProcessorInfoMehod,
								GetMeshProcessorFromSceneObjectMehod,
								GetAnimControllerFromProcessorMehod,
								GetMaterialFromSceneObjectMehod,
								GetEffectFromSceneObjectMehod,
								GetRenderMethodFromSceneObjectMehod,
								GetAnimationSetFromControllerMehod,
								GetPlayTimeFromAnimationSetMehod,
								GetMaterialNumFromStaticMeshInstanceMehod,
								GetMaterialFromStaticMeshInstanceMehod,
								GetSceneNodeNameMehod,
								GetSphereFromCameraMehod,
								WriteDataToVertexBufferMethod,
								ApplyForRenderFromDeferredLightRendererMethod,
								GetTextureByIndexFromMaterialMethod,
								GetTextureNumFromMaterialMethod,
								GetNodeNameMethod,
								CreateCartoonEffectMethod,
								OnCreateSceneMethod,
								CreateOutlineEffectMethod
								);

	AS3_Release(InitObjectMethod);
	AS3_Release(InitParticleShowSystemMethod);
	AS3_Release(CreateBufferMethod);
	AS3_Release(CreateAGALAssemblerMethod);
	AS3_Release(CreateRenderSystemMethod);
	AS3_Release(CreateSceneNodeFactoryMethod);
	AS3_Release(CreateMeshManagerMethod);
	AS3_Release(CreateSceneManagerMethod);
	AS3_Release(CreateMeshDataMethod);
	AS3_Release(CreateMeshMethod);
	AS3_Release(CreatePlaneFromMeshManagerMethod);
	AS3_Release(CreateBillboardMeshDataMethod);
	
	AS3_Release(CreateMeshFromEffectMethod);
	AS3_Release(CreateMaterialDataMethod);
	AS3_Release(CreateTextureDataMethod);
	AS3_Release(CreateSceneDataMethod);
	AS3_Release(CreateMaterialMethod);
	AS3_Release(CreateLightMethod);
	AS3_Release(CreateParameterListMethod);
	AS3_Release(CreateRenderMethodMethod);
	AS3_Release(CreateRenderMethodFromEffectCodeMethod);
	AS3_Release(CreateVertexBufferMethod);
	AS3_Release(CreateIndexBufferMethod);
	AS3_Release(CreateProgram3DMethod);
	AS3_Release(CreateTextureMethod);
	AS3_Release(CreateCubeTextureMethod);
	AS3_Release(CreateEffectMethod);
	AS3_Release(CreateEffectFromEffectCodeMethod);
	AS3_Release(CreateSceneNodeMethod);
	AS3_Release(CreateSkyPlaneMethod);
	AS3_Release(CreateSkyBoxMethod);
	AS3_Release(CreateRenderStateSetMethod);
	AS3_Release(CreateMirrorMethod);
	AS3_Release(CreateWaterMethod);
	AS3_Release(CreateStaticMeshInstanceMethod);
	AS3_Release(CreateBillboardSystemMethod);
	AS3_Release(CreateSkyDomeMethod);
	AS3_Release(CreateSkyDomeWithMeshMethod);
	AS3_Release(CreateGrassSystemMethod);
	AS3_Release(CreateCascadShadowMapMethod);
	AS3_Release(CreateProgramRenderManagerMethod);
	

	AS3_Release(CreateParticleShowSystemMethod);
	AS3_Release(CreateMeshFillerMethod);
	AS3_Release(CreatePSLiveTimeAttributeMethod);
	AS3_Release(CreatePSParticleInfoAttributeMethod);
	AS3_Release(CreatePSPositionAttributeMethod);
	AS3_Release(CreatePSTextureAttributeMethod);
	AS3_Release(CreatePSVelocityAttributeMethod);

	AS3_Release(CreateNormalMeshProcessorMethod);
	AS3_Release(CreateSkinMeshProcessorMethod);
	AS3_Release(CreateShadowMapRenderMethod);
	AS3_Release(CreateBeforePostProcessRenderMethod);
	AS3_Release(CreateScenePostProcessRenderMethod);
	
	AS3_Release(CreateEnvironmentRenderMethod);
	AS3_Release(CreateSilhoutteProgramMethod);

	AS3_Release(CreateCameraMethod);
	AS3_Release(CreateSceneMethod);
	
	AS3_Release(CreateSceneObjectMethod);

	AS3_Release(CreateParticleSystemMethod);

	AS3_Release(CreateBoneMethod);
	AS3_Release(CreateSkinInfoDataMethod);
	AS3_Release(CreateSkinInfoMethod);
	AS3_Release(CreateBoneDataMethod);

	AS3_Release(CreateAniamtionSetMethod);
	AS3_Release(CreateAnimationControllerMethod);
	AS3_Release(CreateAnimationTrackerMethod);
	AS3_Release(CreateAnimationSetDataMethod);
	AS3_Release(CreateSkinMeshControllerMethod);
	AS3_Release(CreateDeferredLightRendererMethod);
	AS3_Release(CreateEffectCodeMethod);
	AS3_Release(CreateParticleDataCodeMethod);
	AS3_Release(CreateEffectCodeFromDeferredRenderingPipelineMethod);
	AS3_Release(CreatePositionEffectMethod);
	AS3_Release(CreatePosition2DEffectMethod);
	
	AS3_Release(CreateTextureEffectMethod);
	AS3_Release(CreateForceTextureEffectMethod);
	AS3_Release(CreatePixelAmbientLightEffectMethod);
	AS3_Release(CreateFacingEffectMethod);
	AS3_Release(CreatePixelColorEffectMethod);

	AS3_Release(CreateEmptyEffectMethod);
	AS3_Release(CreateVertexColorEffectMethod);
	AS3_Release(CreateDirectionalShdowDisplayEffectMethod);

	
	AS3_Release(CreateAlphaTestBlendMethod);
	AS3_Release(CreateDepthEffectMethod);
	AS3_Release(CreateDistanceInfoEffectMethod);

	AS3_Release(CreatePixelFogBlendMethod);
	AS3_Release(CreateEnvironmentDisplayEffectMethod);
	AS3_Release(CreateNormalMapEffectMethod);
	AS3_Release(CreateBillboardEffectMethod);

	AS3_Release(CreateMaskTextureEffectMethod);
	AS3_Release(CreateSpecularTextureEffectMethod);
	AS3_Release(CreateCascadShadowDisplayEffectMethod);
	AS3_Release(CreateTextureBlurEffectMethod);
	
	AS3_Release(CreateTextureModulusEffectMethod);
	AS3_Release(CreateBackgroundEffectMethod);
	
	AS3_Release(CreateEffectInfoSubMethod);
	AS3_Release(CreateBidirectionalBlendMethodMethod);
	
	AS3_Release(CreatePSMoveEffectMethod);
	AS3_Release(CreateGrassAnimationEffectMethod);
	
	AS3_Release(CreateEffectInfoAdditionMethod);
	AS3_Release(CreateFresnelBlendMethodMethod);
	AS3_Release(CreateMulityEffectProgramMethod);
	AS3_Release(CreateSkyPassDefinerMethod);
	

	AS3_Release(CreateShadowMapDisplayEffectMethod);
	AS3_Release(CreateDepthFieldEffectMethod);
	AS3_Release(CreateContorEffectMethod);
	AS3_Release(CreateSnowParticleComputeMethod);
	AS3_Release(CreateVelocityVCMethod);
	AS3_Release(CreateVolumeLightShowMethod);
	
	AS3_Release(CreateAccelerationVelocityVCMethod);

	AS3_Release(CreateEffectInfoModulationMethod);

	AS3_Release(DestroyBufferMethod);
	AS3_Release(DestroyMeshDataMethod);
	AS3_Release(DestroyMaterialDataMethod);
	AS3_Release(DestroyParameterListMethod);
	AS3_Release(DestroySkinInfoDataMethod);
	AS3_Release(DestroySkinInfoMethod);
	AS3_Release(DestroyBoneDataMethod);

	AS3_Release(DestroyAnimTrackMethod);
	AS3_Release(DestroyAnimSetDataMethod);
	AS3_Release(DestroyAnimSetMethod);
	AS3_Release(DestroyAnimControllerMethod);
	AS3_Release(DestroyStreamBufferMethod);
	AS3_Release(DestroyEffectCodeMethod);
	AS3_Release(DestroyTextureEffectMethod);

	AS3_Release(DestroyTextureDataMethod);
	AS3_Release(DestroySceneDataMethod);

	AS3_Release(FillObjectMethod);
	AS3_Release(AddTextureToMaterialMethod);
	AS3_Release(AddAttributeToMeshMethod);
	AS3_Release(AddChildToSceneNodeMethod);
	AS3_Release(AddLightToSceneManagerMethod);
	AS3_Release(AddChildToSceneManagerMethod);
	AS3_Release(AddCameraToSceneManagerMethod);
	AS3_Release(AddInfoToPassDefinerMethod);
	AS3_Release(AddSkinDataToMeshManagerMethod);
	

	AS3_Release(AddTextureDataToMeshManagerMethod);
	AS3_Release(AddMaterialDataToMeshManagerMethod);
	AS3_Release(AddMeshDataToMeshManagerMethod);
	AS3_Release(AddSceneDataToMeshManagerMethod);
	AS3_Release(AddNodeToBillboardSystemMethod);
	AS3_Release(AddShadowCasterToCascadShadowMapMethod);
	AS3_Release(AddShadowCasterToShadowMapMethod);

	AS3_Release(AddInfoForBlendMethodToPassDefinerMethod);
	AS3_Release(AddColorKeyAndInfoToPassDefinerMethod);
	AS3_Release(AddColorKeyInfoToPassDefinerMethod);
	AS3_Release(AddColorKeyToPassDefinerMethod);

	AS3_Release(AddAttributeToMeshFillerMethod);
	AS3_Release(FillMeshFromMeshFillerMethod);
	

	AS3_Release(AddParticleComputeInfoToDataCodeMethod);
	AS3_Release(AddParticleVariableInfoToDataCodeMethod);
	AS3_Release(BuildParticleControllerMethod);
	AS3_Release(AddProgramRenderToManagerMethod);

	AS3_Release(AddRenderObjectToProgramMethod);
	AS3_Release(AddRenderObjectSelfToProgramMethod);

	AS3_Release(RemoveTextureFromMaterialMethod);
	AS3_Release(RemoveAttributeFromMeshMethod);
	AS3_Release(RemoveChildFromSceneNodeMethod);
	AS3_Release(RemoveChildFromSceneManagerMethod);

	AS3_Release(RemoveNodeFromBillboardSystemMethod);
	

	AS3_Release(LockTextureMethod);
	AS3_Release(UnlockMethod);

	AS3_Release(SetViewportToContext3DMethod);
	AS3_Release(GetAttributeDataFromMeshMethod);
	AS3_Release(SetAttributeDataFromMeshMethod);
	AS3_Release(GetMeshFromProcessorMethod);
	AS3_Release(GetHardwareSkinEffectMethod);
	AS3_Release(SetProjectionToCameraMethod);
	
	AS3_Release(RenderSceneMethod);
	AS3_Release(UpdateAnimationMethod);
	
	AS3_Release(UpdateProgramRenderManagerMethod);
	
	AS3_Release(SetBonesDataToSkinInfoMethod );
	AS3_Release(CloneSceneNodeMethod);
	AS3_Release(SetNodeNameMethod );
	

	AS3_Release(EndDefinePassDefinerMethod);
	AS3_Release(BuildShadowMapFromLightMethod);

	AS3_Release(AddEffectInfoToPassFromSceneManagerMethod);

	AS3_Release(ResetAnimationControllerMethod);
	
	AS3_Release(RegisterAniamtionSetToControllerMethod);
	AS3_Release(UnregisterAnimationSetFromControllerMethod);
	AS3_Release(RegisterBoneToAnimationControllerMethod);
	
	AS3_Release(SetTrackersToAnimationSetMethod);
	AS3_Release(ComputeMeshTangentMethod);
	AS3_Release(ComputeMeshNormalMethod);
	
	AS3_Release(SetMeshProcessorToSceneObjectMethod);
	
	AS3_Release(SetEffectToSceneObjectMethod);
	AS3_Release(SetRenderMethodToSceneObjectMethod );
	AS3_Release(SetEffectToGrassSystemMethod);
	AS3_Release(SetRenderMethodToGrassSystemMethod);
	AS3_Release(SetSceneDataToStaticMeshInstanceMethod );
	AS3_Release(SetEffectToStaticMeshInstanceMethod );
	AS3_Release(SetRenderMethodToStaticMeshInstanceMethod );
	AS3_Release(SetEffectRenderToSkyDomeMethod );
	
	
	AS3_Release(SetUVSpeedToSkyDomeMethod);
	AS3_Release(SetMaterialToGrassSystemMethod);
	AS3_Release(SetOrghMatrixToCameraMethod);
	AS3_Release(SetVisibleToSceneNodeMethod);
	
	
	AS3_Release(GetDestinationMeshFromSceneObjectMehod);

	AS3_Release(SetFrameEmitToParticleSystemMehod);
	AS3_Release(SetVisibleToRenderMehod);

	AS3_Release(GetSkinControllerInfoMehod);
	AS3_Release(GetSkinProcessorInfoMehod);
	AS3_Release(GetMeshProcessorFromSceneObjectMehod);
	AS3_Release(GetAnimControllerFromProcessorMehod);
	AS3_Release(GetMaterialFromSceneObjectMehod);
	AS3_Release(GetEffectFromSceneObjectMehod);
	AS3_Release(GetRenderMethodFromSceneObjectMehod);
	AS3_Release(GetAnimationSetFromControllerMehod);
	AS3_Release(GetPlayTimeFromAnimationSetMehod);

	AS3_Release(GetMaterialNumFromStaticMeshInstanceMehod);
	AS3_Release(GetMaterialFromStaticMeshInstanceMehod);
	AS3_Release(GetSceneNodeNameMehod);
	AS3_Release(GetSphereFromCameraMehod);
	

	AS3_Release(WriteDataToVertexBufferMethod );

	AS3_Release(ApplyForRenderFromDeferredLightRendererMethod);

	AS3_Release(GetTextureByIndexFromMaterialMethod );
	AS3_Release(GetTextureNumFromMaterialMethod );
	AS3_Release(GetNodeNameMethod);

	AS3_Release(CreateCartoonEffectMethod);

	AS3_Release(OnCreateSceneMethod);

	AS3_Release( CreateOutlineEffectMethod );

	AS3_LibInit(Result);

	return 0;
}
