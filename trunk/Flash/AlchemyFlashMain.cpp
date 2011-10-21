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
#include "AlchemyFlashBillboard.h"
#include "AlchemyFlashSkyDome.h"


#include "../Graphics/AlchemyPositionEffect.h"
#include "../Graphics/AlchemyTextureEffect.h"
#include "../Graphics/AlchemyForceTextureEffect.h"
#include "../Graphics/AlchemyPixelAmbientLightEffect.h"
#include "../Graphics/AlchemyCartoonEffect.h"
#include "../Graphics/AlchemyOutLineEffect.h"
#include "../Graphics/AlchemyPixelColorEffect.h"
#include "../Graphics/AlchemyProgramInstance.h"
#include "../Graphics/AlchemyEffectInfoInstance.h"
#include "../Graphics/AlchemyEnvironmentProgram.h"
#include "../Graphics/AlchemyShadowMapProgram.h"
#include "../Graphics/AlchemyShadowMapDisplayEffectInfo.h"
#include "../Graphics/AlchemyBlinnPhongEffect.h"
#include "../Graphics/AlchemyLightEffect.h"
#include "../Graphics/AlchemyEffectInfoAddition.h"
#include "../Graphics/AlchemyEffectInfoModulation.h"
#include "../Graphics/AlchemyFresnelBlendMethod.h"
#include "../Graphics/AlchemyFacingEffect.h"
#include "../Graphics/AlchemyDepthFieldEffect.h"
#include "../Graphics/AlchemyDistanceInfoProgram.h"
#include "../Graphics/AlchemySilhouetteProgram.h"
#include "../Graphics/AlchemyEnvironmentDisplayProgram.h"
#include "../Graphics/AlchemyPixelFogBlendMethod.h"
#include "../Graphics/AlchemyContortEffect.h"

#include "../Graphics/AlchemySkinMeshController.h"

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

	return AS3_Array( "PtrType, PtrType, PtrType",pAssembler, static_cast<CFlashObjectInterface*>(pAssembler), static_cast<CCompilerInterface*>(pAssembler) );
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
	UINT uMaxRenderEntries;
	CFlashContext3D* pContext3D;

	AS3_ArrayValue(Args, "PtrType, IntType", &pContext3D, &uMaxRenderEntries);

	CFlashSceneManager* pSceneManager;

	ALCHEMY_DEBUG_NEW( pSceneManager, CFlashSceneManager(*pContext3D, uMaxRenderEntries) );

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
	CMeshManager::MESHDATA* pMeshData;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pSceneManager, &pMeshData);

	if(pSceneManager && pMeshData)
	{
		CFlashMesh* pMesh =  static_cast<CFlashMesh*>( pSceneManager->CreateMesh(*pMeshData) );

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
			const CMeshManager::MESHDATA* pMeshData = ALCHEMY_MESH_MANAGER.GetMeshData(uHandle);

			if(pMeshData)
			{
				CFlashMesh* pMesh =  static_cast<CFlashMesh*>( pSceneManager->CreateMesh(*pMeshData) );

				if(pMesh)
					return AS3_Int( pMesh->GetResourceHandle() );
			}
		}
	}

	return 0;
}

AS3_Val CreateMaterialData(void* pSelf, AS3_Val Args)
{
	CMeshManager::LPMATERIALDATA pMaterialData = reinterpret_cast<CMeshManager::LPMATERIALDATA>( AS3_PtrValue(Args) ) ;

	if(pMaterialData)
	{
		if(pMaterialData->pTextureData)
		{
			CMeshManager::LPTEXTUREDATA pTextureData;
			for(UINT i = 0; i < pMaterialData->uTextureCount; i ++)
			{
				pTextureData = &pMaterialData->pTextureData[i];

				ALCHEMY_DEBUG_NEW(pTextureData->pData, UINT8[(pTextureData->uWidth * pTextureData->uHeight) << pTextureData->Format]);
			}

			return 0;
		}

		if(pMaterialData->uTextureCount)
			ALCHEMY_DEBUG_NEW(pMaterialData->pTextureData, CMeshManager::TEXTUREDATA[pMaterialData->uTextureCount]);

		return 0;
	}

	ALCHEMY_DEBUG_NEW(pMaterialData, CMeshManager::MATERIALDATA);

	return AS3_Ptr(pMaterialData);
}

AS3_Val CreateMaterial(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	CMeshManager::LPMATERIALDATA pMaterialData;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pSceneManager, &pMaterialData);

	if(pSceneManager)
	{
		CFlashMaterial* pMaterial = static_cast<CFlashMaterial*>(pSceneManager->CreateMaterial(pMaterialData));
		
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
		if(!pParameterList->pLightParameters)
			ALCHEMY_DEBUG_NEW(pParameterList->pLightParameters, CRenderMethod::PARAMETERINFO[pParameterList->uLightCount][CRenderMethod::TOTAL_LIGHT_PARAMETERS]);

		if(!pParameterList->pTextureParameters)
			ALCHEMY_DEBUG_NEW(pParameterList->pTextureParameters, CRenderMethod::PARAMETERINFO[pParameterList->uTextureCount][CRenderMethod::TOTAL_TEXTURE_PARAMETERS]);

		if(!pParameterList->pTextures)
			ALCHEMY_DEBUG_NEW(pParameterList->pTextures, CRenderMethod::PARAMETERINFO[pParameterList->uTextureCount]);

		if(!pParameterList->puLightParameterFlag)
			ALCHEMY_DEBUG_NEW(pParameterList->puLightParameterFlag, UINT[pParameterList->uLightCount]);

		if(!pParameterList->puTextureParameterFlag)
			ALCHEMY_DEBUG_NEW(pParameterList->puTextureParameterFlag, UINT[pParameterList->uTextureCount]);

		if(!pParameterList->pVertexElementParameters)
			ALCHEMY_DEBUG_NEW(pParameterList->pVertexElementParameters, CEffectCode::ELEMENTPARAMETER[pParameterList->uVertexElementParameterCount]);

		return AS3_Ptr( pParameterList + sizeof(CRenderMethod::PARAMETERLIST) );
	}

	ALCHEMY_DEBUG_NEW(pParameterList, CRenderMethod::PARAMETERLIST[uLength]);

	return AS3_Ptr(pParameterList);
}

AS3_Val CreateRenderMethod(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	UINT uPassCount, uEffect;
	CRenderMethod::LPPARAMETERLIST pParameterList;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType, IntType", &pSceneManager, &pParameterList, &uPassCount, &uEffect);

	if(pSceneManager && pParameterList)
	{
		CFlashRenderMethod* pRenderMethod = static_cast<CFlashRenderMethod*>( 
			uEffect ? pSceneManager->CreateRenderMethod( pParameterList, static_cast<CGraphicsResource::HANDLE>(uEffect) ) : pSceneManager->CreateRenderMethod(pParameterList, uPassCount) );

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

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pSceneManager, &pEffectCode, &uEffect);

	if(pSceneManager && pEffectCode)
	{
		CFlashRenderMethod* pRenderMethod = static_cast<CFlashRenderMethod*>( pSceneManager->CreateRenderMethod( *pEffectCode, static_cast<CGraphicsResource::HANDLE>(uEffect) ) );

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
	CCompilerInterface* pCompiler;
	PCHAR pVertexCode, pFragmentCode;

	AS3_ArrayValue(Args, "PtrType, PtrType, StrType, StrType", &pRenderSys, &pCompiler, &pVertexCode, &pFragmentCode);

	if(pRenderSys && pCompiler)
	{
		CFlashProgram3D* pProgram = static_cast<CFlashProgram3D*>(pRenderSys->CreateShader(CEffectInterface::PROGRAM, *pCompiler, pVertexCode, pFragmentCode));

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
	UINT uPassCount;

	AS3_ArrayValue(Args, "PtrType, IntType", &pRenderSystem, &uPassCount);

	if(pRenderSystem)
	{
		CEffect* pEffect = pRenderSystem->CreateEffect(uPassCount);

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

		return AS3_Int( pEffect->GetResourceHandle() );
	}

	return 0;
}

AS3_Val CreateBone(void* pSelf, AS3_Val Args)
{
	CFlashBone* pBone = static_cast<CFlashBone*>( &CSmartPointer<CFlashBone>::Create() );

	return AS3_Array( "PtrType, PtrType, PtrType", pBone, static_cast<CFlashObjectInterface*>(pBone), static_cast<CSceneNode*>(pBone) );
}

AS3_Val CreateSceneNode(void* pSelf, AS3_Val Args)
{
	CFlashSceneNode* pSceneNode = static_cast<CFlashSceneNode*>( &CSmartPointer<CFlashSceneNode>::Create() );

	return AS3_Array( "PtrType, PtrType, PtrType", pSceneNode, static_cast<CFlashObjectInterface*>(pSceneNode), static_cast<CSceneNode*>(pSceneNode) );
}

AS3_Val CreateSkyPlane(void* pSelf, AS3_Val Args)
{
	double dLength, dWidth, dHeight;
	int nTexture, nIsAmbientLight, nIsEmissive, nIsUVOffset;
	CCompilerInterface* pCompiler;

	AS3_ArrayValue(Args, "DoubleType, DoubleType, DoubleType, IntType, PtrType, IntType, IntType, IntType", &dLength, &dWidth, &dHeight, &nTexture, &pCompiler, &nIsAmbientLight, &nIsEmissive, &nIsUVOffset);

	if(pCompiler)
	{
		CFlashSkyPlane* pSkyPlane = static_cast<CFlashSkyPlane*>( &CSmartPointer<CFlashSkyPlane>::Create() );

		pSkyPlane->Create(static_cast<FLOAT>(dLength), static_cast<FLOAT>(dWidth), static_cast<FLOAT>(dHeight), nTexture, *pCompiler, static_cast<bool>(nIsAmbientLight), static_cast<bool>(nIsEmissive), static_cast<bool>(nIsUVOffset) );

		return AS3_Array( "PtrType, PtrType, PtrType", pSkyPlane, static_cast<CFlashObjectInterface*>(pSkyPlane), static_cast<CSceneNode*>(pSkyPlane) );
	}

	return 0;
}

AS3_Val CreateSkyBox(void* pSelf, AS3_Val Args)
{
	double width,height,depth;
	int nTexture;

	AS3_ArrayValue(Args,"DoubleType, DoubleType, DoubleType, IntType",&width, &height, &depth, &nTexture);

	CFlashSkyBox* pSkyBox = static_cast<CFlashSkyBox*>( &CSmartPointer<CFlashSkyBox>::Create() );

	bool b = pSkyBox->Create(static_cast<FLOAT>(width), static_cast<FLOAT>(height), static_cast<FLOAT>(depth), nTexture );

	return AS3_Array( "PtrType, PtrType, PtrType", pSkyBox, static_cast<CFlashObjectInterface*>(pSkyBox), static_cast<CSceneNode*>(pSkyBox) );

	return 0;
}

AS3_Val CreateBillboard(void* pSelf, AS3_Val Args)
{
	double width,height;
	int nTexture,nNumSprites, bIsDynamic;

	AS3_ArrayValue(Args,"DoubleType, DoubleType, IntType, IntType, IntType", &width, &height, &nNumSprites, &nTexture, &bIsDynamic);

	CFlashBillboard* pBillboard = static_cast<CFlashBillboard*>( &CSmartPointer<CFlashBillboard>::Create() );

	pBillboard->Create(static_cast<FLOAT>(width), static_cast<FLOAT>(height), nNumSprites, nTexture, bIsDynamic);

	return AS3_Array( "PtrType, PtrType, PtrType", pBillboard, static_cast<CFlashObjectInterface*>(pBillboard), static_cast<CSceneNode*>(pBillboard) );
}

AS3_Val CreateSkyDome(void* pSelf, AS3_Val Args)
{
	double radius,phiDelta, thetaDelta;
	int nTexture;
	int* pTextures, nCount;

	AS3_ArrayValue(Args,"DoubleType, DoubleType, DoubleType, IntType, PtrType, IntType",&radius, &phiDelta, &thetaDelta, &nTexture, &pTextures, &nCount);

	CFlashSkyDome* pSkyDome = static_cast<CFlashSkyDome*>( &CSmartPointer<CFlashSkyDome>::Create() );

	CGraphicsResource::HANDLE* pTextureHandles;
	if(pTextures != ALCHEMY_NULL && nCount > 0)
	{
		ALCHEMY_DEBUG_NEW(pTextureHandles, CGraphicsResource::HANDLE[nCount]);
		for (UINT i=0; i<nCount; ++i)
			pTextureHandles[i] = pTextures[i];
	}

	pSkyDome->Create(static_cast<FLOAT>(radius), static_cast<FLOAT>(phiDelta), 
		static_cast<FLOAT>(thetaDelta), (CGraphicsResource::HANDLE)nTexture, pTextureHandles, (UINT)nCount);

	if(pTextureHandles)
		ALCHEMY_DEBUG_DELETE_ARRAY(pTextureHandles);

	return AS3_Array( "PtrType, PtrType, PtrType", pSkyDome, static_cast<CFlashObjectInterface*>(pSkyDome), static_cast<CSceneNode*>(pSkyDome) );
}

AS3_Val CreateMirror(void* pSelf, AS3_Val Args)
{
	double dbWidth, dbHeight;
	int nNormalMap, nMaterial, nIsYUp;
	int nReflectionMapShift, nRefractionMapShift;
	unsigned int uWidthSegment, uHeightSegment, uEffectInfoCount = 0;
	CCompilerInterface* pCompiler;
	const CEffectCode::CPassDefiner::EFFECTINFO* pEffectInfos = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "DoubleType, DoubleType, IntType, IntType, IntType, IntType, IntType, PtrType, IntType, IntType",  
		&dbWidth, &dbHeight, &uWidthSegment, &uHeightSegment, &nReflectionMapShift, &nNormalMap, &nMaterial, &pEffectInfos, &uEffectInfoCount, &nIsYUp);

	if(pCompiler)
	{
		CFlashMirror* pMirror = static_cast<CFlashMirror*>( &CSmartPointer<CFlashMirror>::Create() );

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

AS3_Val CreateWater(void* pSelf, AS3_Val Args)
{
	double dbWidth, dbHeight;
	int nMaterial, nIsYUp;
	int nReflectionMapShift, nRefractionMapShift;
	unsigned int uNormalMap, uWidthSegment, uHeightSegment, uEffectInfoCount = 0;
	CCompilerInterface* pCompiler;
	const CEffectCode::CPassDefiner::EFFECTINFO* pEffectInfos = ALCHEMY_NULL;

	AS3_ArrayValue(Args, "DoubleType, DoubleType, IntType, IntType, IntType, IntType, IntType, IntType, PtrType, IntType",  
		&dbWidth, &dbHeight, &uWidthSegment, &uHeightSegment, &nReflectionMapShift, &nRefractionMapShift, &uNormalMap, &nMaterial, &pEffectInfos, &uEffectInfoCount);

	if(pCompiler)
	{
		CFlashWater* pWater = static_cast<CFlashWater*>( &CSmartPointer<CFlashWater>::Create() );

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
	CFlashCamera* pCamera = static_cast<CFlashCamera*>( &CSmartPointer<CFlashCamera>::Create() );

	return AS3_Array( "PtrType, PtrType, PtrType", pCamera, static_cast<CFlashObjectInterface*>(pCamera), static_cast<CSceneNode*>(pCamera) );
}

AS3_Val CreateSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	CFlashSceneObject* pSceneObject;
	UINT uMesh, uMaterial, uRenderMethod,uSubset;

	AS3_ArrayValue(Args, "PtrType, IntType, IntType, IntType, IntType, PtrType", &pSceneManager, &uMesh, &uSubset, &uMaterial, &uRenderMethod, &pSceneObject);

	if(pSceneManager)
	{
		pSceneObject = static_cast<CFlashSceneObject*>( pSceneManager->CreateSceneObject(uMesh, uSubset, uMaterial, uRenderMethod, pSceneObject) );

		if(pSceneObject)
			return static_cast<const AS3TYPE>(*pSceneObject);

		return 0;
	}

	if(!pSceneObject)
	{
		pSceneObject = static_cast<CFlashSceneObject*>( &CSmartPointer<CFlashSceneObject>::Create() );

		return AS3_Array( "PtrType, PtrType, PtrType", pSceneObject, static_cast<CFlashObjectInterface*>(pSceneObject), static_cast<CSceneNode*>(pSceneObject) );
	}

	return static_cast<const AS3TYPE>(*pSceneObject);
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
	CMesh* pMesh;
	INT skinType;
	CEffectCode* pEffectCode;

	AS3_ArrayValue(Args,"PtrType, IntType, PtrType, PtrType, PtrType",&pMesh, &skinType, &pSkinInfo, &pRoot, &pEffectCode);

	ALCHEMY_DEBUG_NEW(pSkinMeshController, CSkinMeshController);

	bool bRet = pSkinMeshController->Create(*pMesh, (CSkinMeshController::SKINTYPE)skinType, *pSkinInfo, *pRoot, 30, pEffectCode);

	return bRet == true ? AS3_Ptr( pSkinMeshController ) : 0;
}

AS3_Val CreateEffectCode(void* pSelf, AS3_Val Args)
{
	CCompilerInterface* pCompiler;
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

AS3_Val CreatePositionEffect(void* pSelf, AS3_Val Args)
{
	CPositionEffect* pPositionEffect;

	ALCHEMY_DEBUG_NEW(pPositionEffect, CPositionEffect);

	return AS3_Array( "PtrType, PtrType", pPositionEffect, static_cast<CEffectInfoInterface*>(pPositionEffect) );

	return 0;
}

AS3_Val CreateTextureEffect(void* pSelf, AS3_Val Args)
{
	CTextureEffect* pTextureEffect;

	int nSamplerIndex = AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW( pTextureEffect, CTextureEffect(nSamplerIndex) );

	return AS3_Array( "PtrType, PtrType", pTextureEffect, static_cast<CEffectInfoInterface*>(pTextureEffect) );

	return 0;
}

AS3_Val CreateForceTextureEffect(void* pSelf, AS3_Val Args)
{
	CForceTextureEffect* pTextureEffect;

	int nSamplerIndex = AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW( pTextureEffect, CForceTextureEffect(nSamplerIndex) );

	return AS3_Array( "PtrType, PtrType", pTextureEffect, static_cast<CEffectInfoInterface*>(pTextureEffect) );

	return 0;
}

AS3_Val CreatePixelAmbientLightEffect(void* pSelf, AS3_Val Args)
{
	CPixelAmbientLightEffect* pPixelAmbientLightEffect;

	ALCHEMY_DEBUG_NEW( pPixelAmbientLightEffect, CPixelAmbientLightEffect );

	return AS3_Array( "PtrType, PtrType", pPixelAmbientLightEffect, static_cast<CEffectInfoInterface*>(pPixelAmbientLightEffect) );

	return 0;
}

AS3_Val CreateCartoonEffect(void* pSelf, AS3_Val Args)
{
	CCartoonEffect* pCartoonEffect;

	int nSamplerIndex = AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW( pCartoonEffect, CCartoonEffect(nSamplerIndex) );

	return AS3_Array( "PtrType, PtrType" , pCartoonEffect, static_cast<CEffectInfoInterface*>(pCartoonEffect));
}

AS3_Val CreateOutlineEffect(void* pSelf, AS3_Val Args)
{
	COutLineEffect* pOutlineEffect;

	ALCHEMY_DEBUG_NEW( pOutlineEffect, COutLineEffect );

	return AS3_Array("PtrType, PtrType", pOutlineEffect, static_cast<CEffectInfoInterface*>(pOutlineEffect));
}

AS3_Val CreateFacingEffect(void* pSelf, AS3_Val Args)
{
	CFacingEffect* pFacingEffect;

	int nIsVertexShader = AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW( pFacingEffect, CFacingEffect(nIsVertexShader ? true :false) );

	return AS3_Array( "PtrType, PtrType", pFacingEffect, static_cast<CEffectInfoInterface*>(pFacingEffect) );
}

AS3_Val CreatePixelColorEffect(void* pSelf, AS3_Val Args)
{
	CPixelColorEffect* pPixelEffect;

	FLOAT4* pColor = (FLOAT4*)AS3_PtrValue(Args);

	ALCHEMY_DEBUG_NEW(pPixelEffect, CPixelColorEffect( pColor ) );

	return AS3_Array( "PtrType, PtrType", pPixelEffect, static_cast<CEffectInfoInterface*>(pPixelEffect) );
}

AS3_Val CreatePixelFogBlend(void* pSelf, AS3_Val Args)
{
	CPixelFogBlendMethod* pFogBlend;

	INT nType;
	double fogStart, fogEnd, fogDensity;

	AS3_ArrayValue(Args, "IntType, DoubleType, DoubleType, DoubleType", &nType, &fogStart, &fogEnd, &fogDensity);

	ALCHEMY_DEBUG_NEW(pFogBlend, CPixelFogBlendMethod( (CPixelFogBlendMethod::FOG_TYPE)nType ) );

	FLOAT fFogStart = fogStart, fFogEnd = fogEnd, fFogDensity = fogDensity;

	pFogBlend->Create( &fFogStart, &fFogEnd, &fFogDensity  );

	return AS3_Array( "PtrType, PtrType",  pFogBlend, static_cast<CEffectInfoBlendMethodInterface*>(pFogBlend) );
}

AS3_Val CreateEffectInfoAddition(void* pSelf, AS3_Val Args)
{
	CEffectInfoAddition* pEffectInfoAddition;

	ALCHEMY_DEBUG_NEW(pEffectInfoAddition, CEffectInfoAddition);

	return AS3_Array( "PtrType, PtrType" , pEffectInfoAddition, static_cast<CEffectInfoBlendMethodInterface*>(pEffectInfoAddition) );
}

AS3_Val CreateEffectInfoModulation(void* pSelf, AS3_Val Args)
{
	CEffectInfoModulation* pEffectInfoModulation;

	ALCHEMY_DEBUG_NEW(pEffectInfoModulation, CEffectInfoModulation);

	return AS3_Array("PtrType, PtrType", pEffectInfoModulation, static_cast<CEffectInfoBlendMethodInterface*>(pEffectInfoModulation));
}

AS3_Val CreateFresnelBlendMethod(void* pSelf, AS3_Val Args)
{
	CFresnelBlendMethod* pFresnelBlendMethod;

	ALCHEMY_DEBUG_NEW(pFresnelBlendMethod, CFresnelBlendMethod);

	return AS3_Array("PtrType, PtrType", pFresnelBlendMethod, static_cast<CEffectInfoBlendMethodInterface*>(pFresnelBlendMethod));
}

AS3_Val CreateProgramInstance(void* pSelf, AS3_Val Args)
{
	CProgramInstance* pProgramInstance;

	//CEffectCode::CPassDefiner* pPassDefiner;

	//pPassDefiner = reinterpret_cast< CEffectCode::CPassDefiner* >( AS3_PtrValue(Args) ) ;

	CSceneManager::RENDERENTRYTYPE EffectType = (CSceneManager::RENDERENTRYTYPE)( AS3_IntValue(Args) );

	ALCHEMY_DEBUG_NEW( pProgramInstance, CProgramInstance(EffectType) );

	pProgramInstance->Create();

	//pProgramInstance->Create(*pPassDefiner);

	return AS3_Ptr(pProgramInstance);
}

AS3_Val CreateShadowMapProgram(void* pSelf, AS3_Val Args)
{
	CShadowMapProgram* pProgramInstance;

	UINT uLightIndex;

	CEffectCode::CPassDefiner* pPassDefiner;

	AS3_ArrayValue(Args, "IntType, PtrType", &uLightIndex, &pPassDefiner);

	ALCHEMY_DEBUG_NEW( pProgramInstance, CShadowMapProgram );

	pProgramInstance->Create( uLightIndex, *pPassDefiner );

	return AS3_Array( "PtrType, PtrType", pProgramInstance, static_cast<CProgramInstance*>(pProgramInstance) );
}

AS3_Val CreateEnvironmentProgram(void* pSelf, AS3_Val Args)
{
	CEnvironmentProgram* pProgramInstance;

	INT uEnvironmentSize = (INT)AS3_IntValue(Args);

	ALCHEMY_DEBUG_NEW( pProgramInstance, CEnvironmentProgram );

	pProgramInstance->Create( uEnvironmentSize);

	return AS3_Array( "PtrType, PtrType", pProgramInstance, static_cast<CProgramInstance*>(pProgramInstance) );
}

AS3_Val CreateEnvironmentDisplayProgram(void* pSelf, AS3_Val Args)
{
	CEnvironmentDisplayProgram* pProgram;
	CEnvironmentProgram* pEnvironProgram;

	INT  uSamplerIndex, BlendMethod;
	CEffectCode::CPassDefiner* pPassDefiner;

	AS3_ArrayValue(Args, "PtrType, IntType, PtrType, IntType",&pEnvironProgram, &uSamplerIndex, &pPassDefiner, &BlendMethod);

	ALCHEMY_DEBUG_NEW( pProgram, CEnvironmentDisplayProgram );

	pProgram->Create(*pEnvironProgram, uSamplerIndex, *pPassDefiner, (CEffectCode::CPassDefiner::BLENDMETHOD)BlendMethod );

	return AS3_Array( "PtrType, PtrType", pProgram, static_cast<CProgramInstance*>(pProgram) );
}

AS3_Val CreateDistanceInfoProgram(void* pSelf, AS3_Val Args)
{
	CDistanceInfoProgram* pProgram;
	CEffectCode::CPassDefiner* pPassDefiner;
	CDepthFieldEffect* pDepthEffect;

	AS3_ArrayValue(Args,"PtrType, PtrType", &pDepthEffect, &pPassDefiner);

	ALCHEMY_DEBUG_NEW(pProgram, CDistanceInfoProgram);

	bool bRet = pProgram->Create( *pDepthEffect, *pPassDefiner );

	return bRet == true ? AS3_Array( "PtrType, PtrType", pProgram, static_cast<CProgramInstance*>(pProgram) ) : 0;
}

AS3_Val CreateSilhoutteProgram(void* pSelf, AS3_Val Args)
{
	CSilhouetteProgram* pPromgram;
	int nMesh, isUpdate;
	FLOAT fWidthModulus;
	CARGBColor* pColor;

	AS3_ArrayValue(Args, "IntType, DoubleType, PtrType, IntType", &nMesh, &fWidthModulus, &pColor, &isUpdate);

	ALCHEMY_DEBUG_NEW(pPromgram,CSilhouetteProgram);

	bool bRet = pPromgram->Create( (CGraphicsResource::HANDLE)nMesh, (FLOAT)fWidthModulus, *pColor, (bool)isUpdate );

	return bRet == true ? AS3_Array( "PtrType, PtrType", pPromgram, static_cast<CProgramInstance*>(pPromgram) ) : 0;
}

AS3_Val CreateShadowMapDisplayEffectInfo(void* pSelf, AS3_Val Args)
{
	CShadowMapDisplayEffectInfo* pShadowMapDisplayEffectInfo;

	UINT uLightIndex, uSamplerIndex;
	CEffectCode::CPassDefiner* pPassDefiner;

	AS3_ArrayValue(Args, "PtrType, IntType, IntType", &pPassDefiner, &uLightIndex, &uSamplerIndex);

	ALCHEMY_DEBUG_NEW(pShadowMapDisplayEffectInfo, CShadowMapDisplayEffectInfo);

	pShadowMapDisplayEffectInfo->Create(uLightIndex, uSamplerIndex, *pPassDefiner);

	return AS3_Array( "PtrType, PtrType", pShadowMapDisplayEffectInfo, static_cast<CEffectInfoInstance*>(pShadowMapDisplayEffectInfo) );
}

AS3_Val CreateDepthFieldEffect(void* pSelf, AS3_Val Args)
{
	CDepthFieldEffect* pEffect;
	ALCHEMY_DEBUG_NEW(pEffect, CDepthFieldEffect);

	double fDistance, fBlue;

	AS3_ArrayValue(Args, "DoubleType, DoubleType", &fDistance, &fBlue);

	bool bRet = pEffect->Create(fDistance, fBlue);

	return bRet == true ? AS3_Array( "PtrType, PtrType", pEffect, static_cast<CAbstractPostProcessEffect*>(pEffect) ) : 0; 
}

AS3_Val CreateContorEffect(void* pSelf, AS3_Val Args)
{
	CContortEffect* pEffect;
	ALCHEMY_DEBUG_NEW(pEffect, CContortEffect);

	double fRadius, fScale;

	AS3_ArrayValue(Args, "DoubleType, DoubleType", &fRadius, &fScale);

	bool bRet = pEffect->Create(fScale, fRadius);

	return bRet == true ? AS3_Array( "PtrType, PtrType", pEffect, static_cast<CAbstractPostProcessEffect*>(pEffect) ) : 0; 
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

AS3_Val DestroyMaterialData(void* pSelf, AS3_Val Args)
{
	CMeshManager::LPMATERIALDATA pMaterialData =  reinterpret_cast<CMeshManager::LPMATERIALDATA>(AS3_PtrValue(Args));

	PUINT8 pDeleteData;

	if(pMaterialData)
	{
		if(pMaterialData->pTextureData)
		{
			for(UINT i = 0; i < pMaterialData->uTextureCount; i ++)
				ALCHEMY_DEBUG_DELETE_ARRAY(pMaterialData->pTextureData[i].pData);
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(pMaterialData->pTextureData);

		ALCHEMY_DEBUG_DELETE_ARRAY(pMaterialData);
	}

	return 0;
}

AS3_Val DestroyParameterList(void* pSelf, AS3_Val Args)
{
	CRenderMethod::LPPARAMETERLIST pParameterList, pCurrentParameter;
	UINT uLength;
	AS3_ArrayValue(Args, "PtrType, IntType", &pParameterList, &uLength);

	if(pParameterList)
	{
		for(UINT i = 0; i < uLength; ++ i)
		{
			pCurrentParameter = &pParameterList[i];

			ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->pLightParameters);

			ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->pTextureParameters);

			ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->pTextures);

			ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->puLightParameterFlag);

			ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->puTextureParameterFlag);

			ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameter->pVertexElementParameters);
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(pParameterList);
	}

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
//AS3_Val FillRenderSystem(void* pSelf, AS3_Val Args)
//{
//	CFlashContext3D* pContext3D;
//	AS3_Val Object;
//	AS3_Val Method[3];
//
//	AS3_ArrayValue(Args, "PtrType, AS3ValType, AS3ValType, AS3ValType, AS3ValType", &pContext3D, &Object, &Method[0], &Method[1], &Method[2]);
//
//	if(pContext3D)
//		pContext3D->Fill(Object, Method);
//
//	return 0;
//}
//
//AS3TYPE FillSceneNode(void* pSelf, AS3_Val Args)
//{
//	CFlashAbstractSceneNode* pSceneNode;
//	AS3_Val Object;
//	AS3_Val Method[2];
//
//	AS3_ArrayValue(Args, "PtrType, AS3ValType, AS3ValType, AS3ValType", &pSceneNode, &Object, &Method[0], &Method[1]);
//
//	if(pSceneNode)
//		pSceneNode->Fill(Object, Method);
//
//	return 0;
//}
//
//AS3_Val FillCustomSceneNode(void* pSelf, AS3_Val Args)
//{
//	CFlashCustomSceneNode* pSceneNode;
//	AS3_Val Object;
//	AS3_Val Method[2];
//
//	AS3_ArrayValue(Args, "PtrType, AS3ValType, AS3ValType, AS3ValType", &pSceneNode, &Object, &Method[0], &Method[1]);
//
//	if(pSceneNode)
//		pSceneNode->Fill(Object, Method);
//
//	return 0;
//}

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

	AS3_ArrayValue(Args, "PtrType, IntType, InType", &pSceneManager, &nLight, &nShadeType);

	if(pSceneManager && nLight)
		pSceneManager->AddLight( nLight, COldLightEffect::BLINN_PHONG/*static_cast<CLightEffect::SHADEMODE>(nShadeType)*/ );

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

AS3_Val AddInfoToPassDefiner(void* pSelf, AS3_Val Args)
{
	CEffectCode::CPassDefiner* pPassDefiner;
	CEffectInfoInterface* pEffectInfo;
	int nIndex, nBlendType;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pPassDefiner, &pEffectInfo, &nBlendType);

	if(pPassDefiner)
		pPassDefiner->AddInfo( *pEffectInfo, static_cast<CEffectCode::CPassDefiner::BLENDMETHOD>(nBlendType) );

	return 0;
}

AS3_Val AddEffectInfoToProgram(void* pSelf, AS3_Val Args)
{
	CProgramInstance* pProgramInstance;
	CEffectInfoInstance* pEffectInfoInstance;

	AS3_ArrayValue(Args, "PtrType, PtrType", &pProgramInstance, &pEffectInfoInstance);

	pProgramInstance->AddEffectInfoInstance( *pEffectInfoInstance );

	return 0;
}

AS3_Val AddProgramToSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject* pSceneObect;
	CProgramInstance* pProgramInstance;
	INT nPassIndex;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pSceneObect, &pProgramInstance, &nPassIndex);

	pSceneObect->AddProgramInstance( *pProgramInstance, nPassIndex);

	return 0;
}

AS3_Val AddInfoForBlendMethodToPassDefiner(void* pSelf, AS3_Val Args)
{
	CEffectCode::CPassDefiner* pPassDefiner;
	CEffectInfoInterface* pEffectInfo;
	CEffectInfoBlendMethodInterface* pBlendMethod;

	AS3_ArrayValue(Args, "PtrType, PtrType, PtrType", &pPassDefiner, &pEffectInfo, &pBlendMethod );

	pPassDefiner->AddInfo(*pEffectInfo, pBlendMethod);

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

AS3_Val GetHardwareSkinEffect(void* pSelf, AS3_Val Args)
{
	CSkinMeshController* pSkinMeshController = static_cast<CSkinMeshController*>( AS3_PtrValue(Args) );
	if(pSkinMeshController)
		return AS3_Ptr( pSkinMeshController->GetHardwareSkinEffect() );

	return 0;
}

AS3_Val SetPassToEffect(void* pSelf, AS3_Val Args)
{
	CFlashEffect* pEffect = ALCHEMY_NULL;
	CFlashProgram3D* pPass = ALCHEMY_NULL;
	UINT uIndex;

	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pEffect, &pPass, &uIndex);

	pEffect->SetPass(pPass, uIndex);

	return 0;
}

AS3_Val SetProjectionToCamera(void* pSelf, AS3_Val Args)
{
	CFlashCamera* pCamera = ALCHEMY_NULL;
	double FOV, NearPlane, FarPlane, Aspect;

	AS3_ArrayValue(Args, "PtrType, DoubleType, DoubleType, DoubleType, DoubleType", &pCamera, &FOV, &NearPlane, &FarPlane, &Aspect);

	if(pCamera)
		pCamera->SetProjection( static_cast<FLOAT>(FOV), static_cast<FLOAT>(NearPlane), static_cast<FLOAT>(FarPlane), static_cast<FLOAT>(Aspect) );

	return 0;
}

AS3_Val RenderScene(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	double fEleapseTime;

	AS3_ArrayValue(Args, "PtrType, DoubleType", &pSceneManager, &fEleapseTime);

	pSceneManager->Update( static_cast<FLOAT>(fEleapseTime) );
	pSceneManager->Render();

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

AS3_Val SetBoneName(void* pSelf, AS3_Val Args)
{
	CFlashBone* pBone;
	PCHAR name;

	AS3_ArrayValue(Args,"PtrType, StrType",&pBone,&name);

	pBone->SetName(name);

	return 0;
}

AS3_Val SetSceneObjectShadowEnble(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject* pSceneObject;
	UINT uLightIndex;
	CCompilerInterface* pCompiler;

	AS3_ArrayValue(Args,"PtrType, IntType, PtrType",&pSceneObject, &uLightIndex, &pCompiler);

	if(pSceneObject && pCompiler)
		pSceneObject->SetShadowEnble(CSceneObject::SHADOW_MAP, uLightIndex, *pCompiler);

	return 0;
}

AS3_Val SetSceneObjectShadowDisplay(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject* pSceneObject;
	UINT uLightIndex;

	AS3_ArrayValue(Args,"PtrType, IntType",&pSceneObject, &uLightIndex);

	if(pSceneObject)
		return AS3_Ptr( pSceneObject->SetShadowDisplay(uLightIndex) );

	return 0;
}

AS3_Val SetSkinInfoToSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject* pSceneObject;
	CSkinInfo*		   pSkinInfo;
	CFlashBone*		   pRootBone;
	INT					skinType;
	CEffectCode::CPassDefiner* pPass;

	AS3_ArrayValue(Args,"PtrType, IntType, PtrType, PtrType, PtrType",&pSceneObject, &skinType, &pSkinInfo, &pRootBone, &pPass);

	//CEffectCode::ELEMENTPARAMETER SkinElementParameter[] = { {CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0}, {CVertexDeclaration::NORMAL, CVertexDeclaration::FLOAT3, 0} };
	/*SkinElementParameter.Usage  = CVertexDeclaration::POSITION;
	SkinElementParameter.Type   = CVertexDeclaration::FLOAT4;
	SkinElementParameter.uIndex = 0;*/

	CMesh* pMesh = pSceneObject->SetSkinInfo( (CSceneObject::SKINTYPE)skinType, *pSkinInfo, *pRootBone, 30, pPass);

	return AS3_Int( pMesh->GetResourceHandle() );
}

AS3_Val SetSkinMeshControllerToSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject* pSceneObject;
	CSkinMeshController* pSkinMeshController;
	INT isUpdate;

	AS3_ArrayValue(Args,"PtrType, PtrType, PtrType", &pSceneObject, &pSkinMeshController, &isUpdate);

	pSceneObject->SetSkinMeshController( pSkinMeshController, (bool) isUpdate );

	return 0;
}

AS3_Val SetRenderMethodToSceneObject(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject * pSceneObject;
	CFlashRenderMethod*	pRenderMethod;

	AS3_ArrayValue(Args,"PtrType, PtrType",&pSceneObject, &pRenderMethod);

	return AS3_Int( (int)( pSceneObject->SetRenderMethod( *pRenderMethod ) ) );
}

AS3_Val SetAniamtionController(void* pSelf, AS3_Val Args)
{
	CFlashSceneObject* pSceneObject;
	CAnimationController* pAnimController;

	AS3_ArrayValue(Args,"PtrType, PtrType",&pSceneObject,&pAnimController);

	pSceneObject->SetAnimationController( *pAnimController );

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

AS3_Val EffectApplyForRender(void* pSelf, AS3_Val Args)
{
	CAbstractPostProcessEffect* pEffect = (CAbstractPostProcessEffect*)AS3_PtrValue(Args);
	if(pEffect)
		 pEffect->ApplyForRender();

	return 0;
}

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

	return 0;
}

AS3_Val BuildSceneEffectPassFromSceneManager(void* pSelf, AS3_Val Args)
{
	CFlashSceneManager* pSceneManager;
	CEffectCode::CPassDefiner* pPassDefiner;
	int nLightFlag;
	
	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pSceneManager, &pPassDefiner, &nLightFlag);

	if(pSceneManager && pPassDefiner)
		pSceneManager->BuildSceneEffectPass(*pPassDefiner, nLightFlag);

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

AS3_Val SetTrackersToAnimationSet(void* pSelf, AS3_Val Args)
{
	CAnimationSet* pAnimSet;
	CAnimationSet::LPANIMATION pAnimData;
	UINT len;

	AS3_ArrayValue(Args,"PtrType, PtrType, IntType",&pAnimSet,&pAnimData, &len);

	pAnimSet->Create(pAnimData,len);

	return 0;
}

//AS3_Val AddShareVertexDestinationWithStream(void* pSelf, AS3_Val Args)
//{
//	CSceneObject* pObject;
//	UINT* pStream;
//	UINT streamLen;
//	UINT* pStreamUsage;
//	CMesh::ATTRIBUTERANGE* pAttribute;
//
//	AS3_ArrayValue(Args,"PtrType, PtrType, IntType, PtrType, PtrType ",&pObject, &pStream, &streamLen, &pStreamUsage, &pAttribute);
//
//	if(pStream == ALCHEMY_NULL)
//	{
//		pStream = new UINT[streamLen];
//		pStreamUsage = new UINT[streamLen];
//		pAttribute = new CMesh::ATTRIBUTERANGE;
//
//		return AS3_Array("PtrType, PtrType, PtrType", pStream, pStreamUsage, pAttribute);
//	}
//	else
//	{
//		CGraphicsResource::HANDLE* pHandles = new CGraphicsResource::HANDLE[streamLen];
//		pObject->AddShareVertexDestination(pHandles,pStream,streamLen,pStreamUsage,pAttribute);
//		delete [] pStream;
//		delete [] pStreamUsage;
//		delete pAttribute;
//
//		return AS3_Array("PtrType",pHandles);
//	}
//
//	return 0;
//}

AS3_Val AddShareVertexDestinationWithDeclaration(void* pSelf, AS3_Val Args)
{
	UINT elementCnt;
	CSceneObject* pObject;
	CMesh::ATTRIBUTERANGE* pAttribute;
	CEffectCode::ELEMENTPARAMETER* pVerElement;

	AS3_ArrayValue(Args,"PtrType, PtrType, IntType, PtrType",&pObject, &pVerElement, &elementCnt, &pAttribute);

	if(pVerElement == ALCHEMY_NULL)
	{
		pAttribute = new CMesh::ATTRIBUTERANGE;
		pVerElement = new CEffectCode::ELEMENTPARAMETER[elementCnt];

		return AS3_Array("PtrType, PtrType", pVerElement, pAttribute);
	}
	else
	{
		pObject->AddShareVertexDestination(pVerElement,elementCnt,pAttribute);
		delete [] pVerElement;
		delete pAttribute;
	}

	return 0;
}

AS3_Val WriteDataToVertexBuffer(void* pSelf, AS3_Val Args)
{
	CFlashVertexBuffer* pVertexBuffer;
	PVOID pData;
	UINT length;
	
	AS3_ArrayValue(Args, "PtrType, PtrType, IntType", &pVertexBuffer, &pData, &length);

	PVOID pVertexData = pVertexBuffer->Lock(0);

	ALCHEMY_MEMORY_COPY(pVertexData,pData,length);

	pVertexBuffer->Unlock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
///main
//////////////////////////////////////////////////////////////////////////

int main(/*int argc, _TCHAR* argv[]*/)
{
	AS3_Val InitObjectMethod = AS3_Function( ALCHEMY_NULL, InitObject );
	AS3_Val CreateAGALAssemblerMethod = AS3_Function( ALCHEMY_NULL, CreateAGALAssembler );
	AS3_Val CreateBufferMethod = AS3_Function( ALCHEMY_NULL, CreateBuffer );
	AS3_Val CreateRenderSystemMethod = AS3_Function( ALCHEMY_NULL, CreateRenderSystem );
	AS3_Val CreateSceneNodeFactoryMethod = AS3_Function( ALCHEMY_NULL, CreateSceneNodeFactory );
	AS3_Val CreateMeshManagerMethod = AS3_Function(ALCHEMY_NULL, CreateMeshManager);
	AS3_Val CreateSceneManagerMethod = AS3_Function( ALCHEMY_NULL, CreateSceneManager );
	AS3_Val CreateMeshDataMethod = AS3_Function( ALCHEMY_NULL, CreateMeshData );
	AS3_Val CreateMeshMethod = AS3_Function( ALCHEMY_NULL, CreateMesh );
	AS3_Val CreatePlaneFromMeshManagerMethod = AS3_Function(ALCHEMY_NULL, CreatePlaneFromMeshManager);
	AS3_Val CreateMaterialDataMethod = AS3_Function( ALCHEMY_NULL, CreateMaterialData );
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
	AS3_Val CreateMirrorMethod = AS3_Function( ALCHEMY_NULL, CreateMirror );
	AS3_Val CreateWaterMethod = AS3_Function( ALCHEMY_NULL, CreateWater );
	AS3_Val CreateBillboardMethod = AS3_Function( ALCHEMY_NULL, CreateBillboard );
	AS3_Val CreateSkyDomeMethod = AS3_Function( ALCHEMY_NULL, CreateSkyDome );

	AS3_Val CreateProgramInstanceMethod = AS3_Function( ALCHEMY_NULL, CreateProgramInstance );
	AS3_Val CreateShadowMapProgramMethod = AS3_Function( ALCHEMY_NULL, CreateShadowMapProgram );
	AS3_Val CreateEnvironmentProgramMethod = AS3_Function( ALCHEMY_NULL, CreateEnvironmentProgram );
	AS3_Val CreateEnvironmentDisplayProgramMethod = AS3_Function( ALCHEMY_NULL, CreateEnvironmentDisplayProgram );

	
	AS3_Val CreateDistanceInfoProgramMethod = AS3_Function( ALCHEMY_NULL, CreateDistanceInfoProgram );
	

	AS3_Val CreateShadowMapDisplayEffectInfoMethod = AS3_Function( ALCHEMY_NULL, CreateShadowMapDisplayEffectInfo );
	AS3_Val CreateDepthFieldEffectMethod = AS3_Function( ALCHEMY_NULL, CreateDepthFieldEffect );
	AS3_Val CreateContorEffectMethod = AS3_Function( ALCHEMY_NULL, CreateContorEffect );
	

	

	


	AS3_Val CreateCameraMethod = AS3_Function( ALCHEMY_NULL, CreateCamera );
	AS3_Val CreateSceneObjectMethod = AS3_Function( ALCHEMY_NULL, CreateSceneObject );
	AS3_Val CreateBoneMethod = AS3_Function( ALCHEMY_NULL, CreateBone );
	AS3_Val CreateSkinInfoDataMethod = AS3_Function( ALCHEMY_NULL, CreateSkinInfoData );
	AS3_Val CreateSkinInfoMethod = AS3_Function( ALCHEMY_NULL, CreateSkinInfo );

	AS3_Val CreateBoneDataMethod = AS3_Function( ALCHEMY_NULL, CreateBoneData );

	AS3_Val CreateAniamtionSetMethod = AS3_Function( ALCHEMY_NULL, CreateAniamtionSet );
	AS3_Val CreateAnimationControllerMethod = AS3_Function( ALCHEMY_NULL, CreateAnimationController );
	AS3_Val CreateAnimationTrackerMethod = AS3_Function( ALCHEMY_NULL, CreateAnimationTracker );
	AS3_Val CreateAnimationSetDataMethod = AS3_Function( ALCHEMY_NULL, CreateAnimationSetData );
	AS3_Val CreateSkinMeshControllerMethod = AS3_Function( ALCHEMY_NULL, CreateSkinMeshController );

	

	AS3_Val CreateEffectCodeMethod = AS3_Function(ALCHEMY_NULL, CreateEffectCode);
	AS3_Val CreatePositionEffectMethod = AS3_Function( ALCHEMY_NULL, CreatePositionEffect );
	AS3_Val CreateTextureEffectMethod = AS3_Function(ALCHEMY_NULL, CreateTextureEffect);
	AS3_Val CreateForceTextureEffectMethod = AS3_Function(ALCHEMY_NULL, CreateForceTextureEffect);
	AS3_Val CreatePixelAmbientLightEffectMethod = AS3_Function(ALCHEMY_NULL, CreatePixelAmbientLightEffect);
	AS3_Val CreateFacingEffectMethod = AS3_Function(ALCHEMY_NULL, CreateFacingEffect);
	AS3_Val CreatePixelColorEffectMethod = AS3_Function(ALCHEMY_NULL, CreatePixelColorEffect);
	AS3_Val CreatePixelFogBlendMethod = AS3_Function(ALCHEMY_NULL, CreatePixelFogBlend);

	
	
	AS3_Val CreateEffectInfoAdditionMethod = AS3_Function(ALCHEMY_NULL, CreateEffectInfoAddition);
	AS3_Val CreateEffectInfoModulationMethod = AS3_Function(ALCHEMY_NULL, CreateEffectInfoModulation);
	AS3_Val CreateFresnelBlendMethodMethod = AS3_Function(ALCHEMY_NULL, CreateFresnelBlendMethod);


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
	
		

	//AS3_Val FillRenderSystemMethod = AS3_Function( ALCHEMY_NULL, FillRenderSystem );
	//AS3_Val FillSceneNodeMethod = AS3_Function( ALCHEMY_NULL, FillSceneNode );
	//AS3_Val FillCustomSceneNodeMethod = AS3_Function( ALCHEMY_NULL, FillCustomSceneNode );
	AS3_Val FillObjectMethod = AS3_Function( ALCHEMY_NULL, FillObject );
	AS3_Val AddTextureToMaterialMethod = AS3_Function( ALCHEMY_NULL, AddTextureToMaterial );
	AS3_Val AddAttributeToMeshMethod = AS3_Function( ALCHEMY_NULL, AddAttributeToMesh );
	//AS3_Val AddPassToEffectMethod = AS3_Function( ALCHEMY_NULL, AddPassToEffect );
	AS3_Val AddChildToSceneNodeMethod = AS3_Function( ALCHEMY_NULL, AddChildToSceneNode );
	AS3_Val AddLightToSceneManagerMethod =  AS3_Function( ALCHEMY_NULL, AddLightToSceneManager );
	AS3_Val AddChildToSceneManagerMethod = AS3_Function( ALCHEMY_NULL, AddChildToSceneManager );
	AS3_Val AddInfoToPassDefinerMethod = AS3_Function(ALCHEMY_NULL, AddInfoToPassDefiner);

	AS3_Val AddEffectInfoToProgramMethod = AS3_Function(ALCHEMY_NULL, AddEffectInfoToProgram);
	AS3_Val AddProgramToSceneObjectMethod = AS3_Function(ALCHEMY_NULL, AddProgramToSceneObject);
	AS3_Val AddInfoForBlendMethodToPassDefinerMethod = AS3_Function(ALCHEMY_NULL, AddInfoForBlendMethodToPassDefiner);

	


	AS3_Val RemoveTextureFromMaterialMethod = AS3_Function( ALCHEMY_NULL, RemoveTextureFromMaterial );
	AS3_Val RemoveAttributeFromMeshMethod = AS3_Function( ALCHEMY_NULL, RemoveAttributeFromMesh );
	//AS3_Val RemovePassFromEffectMethod = AS3_Function( ALCHEMY_NULL, RemovePassFromEffect );
	AS3_Val RemoveChildFromSceneNodeMethod = AS3_Function( ALCHEMY_NULL, RemoveChildFromSceneNode );
	AS3_Val RemoveChildFromSceneManagerMethod = AS3_Function( ALCHEMY_NULL, RemoveChildFromSceneManager );

	AS3_Val LockTextureMethod = AS3_Function( ALCHEMY_NULL, LockTexture );
	AS3_Val UnlockMethod = AS3_Function( ALCHEMY_NULL, UnlockTexture );
	
	AS3_Val GetAttributeDataFromMeshMethod = AS3_Function( ALCHEMY_NULL, GetAttributeDataFromMesh );
	AS3_Val SetAttributeDataFromMeshMethod = AS3_Function( ALCHEMY_NULL, SetAttributeDataFromMesh );
	AS3_Val GetHardwareSkinEffectMethod = AS3_Function( ALCHEMY_NULL, GetHardwareSkinEffect );


	AS3_Val SetPassToEffectMethod = AS3_Function( ALCHEMY_NULL, SetPassToEffect );
	AS3_Val SetProjectionToCameraMethod = AS3_Function( ALCHEMY_NULL, SetProjectionToCamera );

	//AS3_Val RebuildSceneObjectMethod = AS3_Function( ALCHEMY_NULL, RebuildSceneObject );
	AS3_Val RenderSceneMethod = AS3_Function( ALCHEMY_NULL, RenderScene );

	AS3_Val SetBonesDataToSkinInfoMethod = AS3_Function( ALCHEMY_NULL, SetBonesDataToSkinInfo );
	AS3_Val SetBoneNameMethod = AS3_Function( ALCHEMY_NULL, SetBoneName );
	AS3_Val SetSceneObjectShadowEnbleMethod = AS3_Function( ALCHEMY_NULL, SetSceneObjectShadowEnble );
	AS3_Val SetSceneObjectShadowDisplayMethod = AS3_Function( ALCHEMY_NULL, SetSceneObjectShadowDisplay );

	AS3_Val SetAniamtionControllerMethod = AS3_Function( ALCHEMY_NULL, SetAniamtionController );
	AS3_Val SetSkinInfoToSceneObjectMethod = AS3_Function( ALCHEMY_NULL, SetSkinInfoToSceneObject );
	AS3_Val SetSkinMeshControllerToSceneObjectMethod = AS3_Function( ALCHEMY_NULL, SetSkinMeshControllerToSceneObject );

	

	AS3_Val SetRenderMethodToSceneObjectMethod = AS3_Function( ALCHEMY_NULL, SetRenderMethodToSceneObject );
	AS3_Val SetUVSpeedToSkyDomeMethod = AS3_Function( ALCHEMY_NULL, SetUVSpeedToSkyDome );

	AS3_Val EffectApplyForRenderMethod = AS3_Function( ALCHEMY_NULL, EffectApplyForRender );
	

	AS3_Val EndDefinePassDefinerMethod = AS3_Function( ALCHEMY_NULL, EndDefinePassDefiner );

	AS3_Val BuildShadowMapFromLightMethod = AS3_Function( ALCHEMY_NULL, BuildShadowMapFromLight );
	AS3_Val BuildSceneEffectPassFromSceneManagerMethod = AS3_Function( ALCHEMY_NULL, BuildSceneEffectPassFromSceneManager );

	AS3_Val ResetAnimationControllerMethod = AS3_Function( ALCHEMY_NULL, ResetAnimationController );

	
	AS3_Val RegisterAniamtionSetToControllerMethod = AS3_Function( ALCHEMY_NULL, RegisterAniamtionSetToController );
	AS3_Val UnregisterAnimationSetFromControllerMethod = AS3_Function( ALCHEMY_NULL, UnregisterAnimationSetFromController );
	AS3_Val SetTrackersToAnimationSetMethod = AS3_Function( ALCHEMY_NULL, SetTrackersToAnimationSet );
	//AS3_Val AddShareVertexDestinationWithStreamMethod = AS3_Function( ALCHEMY_NULL, AddShareVertexDestinationWithStream );
	AS3_Val AddShareVertexDestinationWithDeclarationMethod = AS3_Function( ALCHEMY_NULL, AddShareVertexDestinationWithDeclaration );
	AS3_Val WriteDataToVertexBufferMethod = AS3_Function( ALCHEMY_NULL, WriteDataToVertexBuffer );

	AS3_Val CreateCartoonEffectMethod = AS3_Function( ALCHEMY_NULL, CreateCartoonEffect );
	
	AS3_Val CreateOutlineEffectMethod = AS3_Function( ALCHEMY_NULL, CreateOutlineEffect );
		


	AS3_Val Result = AS3_Object("InitObject:AS3ValType,\
								CreateBuffer:AS3ValType,\
								CreateAGALAssembler:AS3ValType,\
								CreateRenderSystem:AS3ValType,\
								CreateSceneNodeFactory:AS3ValType,\
								CreateMeshManager:AS3ValType,\
								CreateSceneManager:AS3ValType,\
								CreateMeshData:AS3ValType,\
								CreateMesh:AS3ValType,\
								CreatePlaneFromMeshManager:AS3ValType,\
								CreateMaterialData:AS3ValType,\
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
								CreateMirror:AS3ValType,\
								CreateWater:AS3ValType,\
								CreateBillboard:AS3ValType,\
								CreateSkyDome:AS3ValType,\
								CreateCamera:AS3ValType,\
								CreateSceneObject:AS3ValType,\
								CreateBone:AS3ValType,\
								CreateSkinInfoData:AS3ValType,\
								CreateSkinInfo:AS3ValType,\
								CreateBoneData:AS3ValType,\
								CreateAniamtionSet:AS3ValType,\
								CreateAnimationController:AS3ValType,\
								CreateAnimationTracker:AS3ValType,\
								CreateAnimationSetData:AS3ValType,\
								CreateSkinMeshController:AS3ValType,\
								CreateEffectCode:AS3ValType,\
								CreatePositionEffect:AS3ValType,\
								CreateTextureEffect,AS3ValType,\
								CreateForceTextureEffect,AS3ValType,\
								CreatePixelAmbientLightEffect:AS3ValType,\
								CreateFacingEffect:AS3ValType,\
								CreatePixelColorEffect:AS3ValType,\
								CreatePixelFogBlend:AS3ValType,\
								CreateEffectInfoAddition:AS3ValType,\
								CreateEffectInfoModulation:AS3ValType,\
								CreateFresnelBlendMethod:AS3ValType,\
								CreateProgramInstance:AS3ValType,\
								CreateShadowMapProgram:AS3ValType,\
								CreateEnvironmentProgram:AS3ValType,\
								CreateEnvironmentDisplayProgram:AS3ValType,\
								CreateDistanceInfoProgram:AS3ValType,\
								CreateShadowMapDisplayEffectInfo:AS3ValType,\
								CreateDepthFieldEffect:AS3ValType,\
								CreateContorEffect:AS3ValType,\
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
								FillObject:AS3ValType,\
								AddTextureToMaterial:AS3ValType,\
								AddAttributeToMesh:AS3ValType,\
								AddChildToSceneNode:AS3ValType,\
								AddLightToSceneManager:AS3ValType,\
								AddChildToSceneManager:AS3ValType,\
								AddInfoToPassDefiner:AS3ValType,\
								AddEffectInfoToProgram:AS3ValType,\
								AddProgramToSceneObject:AS3ValType,\
								AddInfoForBlendMethodToPassDefiner:AS3ValType,\
								RemoveTextureFromMaterial:AS3ValType,\
								RemoveAttributeFromMesh:AS3ValType,\
								RemoveChildFromSceneNode:AS3ValType,\
								RemoveChildFromSceneManager:AS3ValType,\
								LockTexture:AS3ValType,\
								UnlockTexture:AS3ValType,\
								GetAttributeDataFromMesh:AS3ValType,\
								SetAttributeDataFromMesh:AS3ValType,\
								GetHardwareSkinEffect:AS3ValType,\
								SetPassToEffect:AS3ValType,\
								SetProjectionToCamera:AS3ValType,\
								RenderScene:AS3ValType,\
								SetBonesDataToSkinInfo:AS3ValType,\
								SetBoneName:AS3ValType,\
								SetSceneObjectShadowEnble:AS3ValType,\
								SetSceneObjectShadowDisplay:AS3ValType,\
								SetAniamtionController:AS3ValType,\
								EndDefinePassDefiner:AS3ValType,\
								BuildShadowMapFromLight:AS3ValType,\
								BuildSceneEffectPassFromSceneManager:AS3ValType,\
								ResetAnimationController:AS3ValType,\
								RegisterAniamtionSetToController:AS3ValType,\
								UnregisterAnimationSetFromController:AS3ValType,\
								SetTrackersToAnimationSet:AS3ValType,\
								SetSkinInfoToSceneObject:AS3ValType,\
								SetSkinMeshControllerToSceneObject:AS3ValType,\
								SetRenderMethodToSceneObject:AS3ValType,\
								SetUVSpeedToSkyDome:AS3ValType,\
								EffectApplyForRender:AS3ValType,\
								AddShareVertexDestinationWithDeclaration:AS3ValType,\
								WriteDataToVertexBuffer:AS3ValType,\
								CreateCartoonEffect:AS3ValType,\
								CreateOutlineEffect:AS3ValType",
								InitObjectMethod,
								CreateBufferMethod,
								CreateAGALAssemblerMethod,
								CreateRenderSystemMethod,
								CreateSceneNodeFactoryMethod,
								CreateMeshManagerMethod,
								CreateSceneManagerMethod,
								CreateMeshDataMethod,
								CreateMeshMethod,
								CreatePlaneFromMeshManagerMethod,
								CreateMaterialDataMethod,
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
								CreateMirrorMethod,
								CreateWaterMethod,
								CreateBillboardMethod,
								CreateSkyDomeMethod,
								CreateCameraMethod,
								CreateSceneObjectMethod,
								CreateBoneMethod,
								CreateSkinInfoDataMethod,
								CreateSkinInfoMethod,
								CreateBoneDataMethod,
								CreateAniamtionSetMethod,
								CreateAnimationControllerMethod,
								CreateAnimationTrackerMethod,
								CreateAnimationSetDataMethod,
								CreateSkinMeshControllerMethod,
								CreateEffectCodeMethod,
								CreatePositionEffectMethod,
								CreateTextureEffectMethod,
								CreateForceTextureEffectMethod,
								CreatePixelAmbientLightEffectMethod,
								CreateFacingEffectMethod,
								CreatePixelColorEffectMethod,
								CreatePixelFogBlendMethod,
								CreateEffectInfoAdditionMethod,
								CreateEffectInfoModulationMethod,
								CreateFresnelBlendMethodMethod,
								CreateProgramInstanceMethod,
								CreateShadowMapProgramMethod,
								CreateEnvironmentProgramMethod,
								CreateEnvironmentDisplayProgramMethod,
								CreateDistanceInfoProgramMethod,
								CreateShadowMapDisplayEffectInfoMethod,
								CreateDepthFieldEffectMethod,
								CreateContorEffectMethod,
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
								FillObjectMethod,
								AddTextureToMaterialMethod,
								AddAttributeToMeshMethod,
								//AddPassToEffectMethod,
								AddChildToSceneNodeMethod,
								AddLightToSceneManagerMethod,
								AddChildToSceneManagerMethod,
								AddInfoToPassDefinerMethod,
								AddEffectInfoToProgramMethod,
								AddProgramToSceneObjectMethod,
								AddInfoForBlendMethodToPassDefinerMethod,
								RemoveTextureFromMaterialMethod,
								RemoveAttributeFromMeshMethod,
								//RemovePassFromEffectMethod,
								RemoveChildFromSceneNodeMethod,
								RemoveChildFromSceneManagerMethod,
								LockTextureMethod,
								UnlockMethod,
								GetAttributeDataFromMeshMethod,
								SetAttributeDataFromMeshMethod,
								GetHardwareSkinEffectMethod,
								SetPassToEffectMethod,
								SetProjectionToCameraMethod,
								//RebuildSceneObjectMethod,
								RenderSceneMethod,
								SetBonesDataToSkinInfoMethod,
								SetBoneNameMethod,
								SetSceneObjectShadowEnbleMethod,
								SetSceneObjectShadowDisplayMethod,
								SetAniamtionControllerMethod,
								EndDefinePassDefinerMethod,
								BuildShadowMapFromLightMethod, 
								BuildSceneEffectPassFromSceneManagerMethod,
								ResetAnimationControllerMethod,
								RegisterAniamtionSetToControllerMethod,
								UnregisterAnimationSetFromControllerMethod,
								SetTrackersToAnimationSetMethod,
								SetSkinInfoToSceneObjectMethod,
								SetSkinMeshControllerToSceneObjectMethod,
								SetRenderMethodToSceneObjectMethod,
								SetUVSpeedToSkyDomeMethod,
								EffectApplyForRenderMethod,
								//AddShareVertexDestinationWithStreamMethod,
								AddShareVertexDestinationWithDeclarationMethod,
								WriteDataToVertexBufferMethod,
								CreateCartoonEffectMethod,
								CreateOutlineEffectMethod
								);

	AS3_Release(InitObjectMethod);
	AS3_Release(CreateBufferMethod);
	AS3_Release(CreateAGALAssemblerMethod);
	AS3_Release(CreateRenderSystemMethod);
	AS3_Release(CreateSceneNodeFactoryMethod);
	AS3_Release(CreateMeshManagerMethod);
	AS3_Release(CreateSceneManagerMethod);
	AS3_Release(CreateMeshDataMethod);
	AS3_Release(CreateMeshMethod);
	AS3_Release(CreatePlaneFromMeshManagerMethod);
	AS3_Release(CreateMaterialDataMethod);
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
	AS3_Release(CreateMirrorMethod);
	AS3_Release(CreateWaterMethod);
	AS3_Release(CreateBillboardMethod);
	AS3_Release(CreateSkyDomeMethod);
	AS3_Release(CreateCameraMethod);
	AS3_Release(CreateSceneObjectMethod);
	AS3_Release(CreateBoneMethod);
	AS3_Release(CreateSkinInfoDataMethod);
	AS3_Release(CreateSkinInfoMethod);
	AS3_Release(CreateBoneDataMethod);

	AS3_Release(CreateAniamtionSetMethod);
	AS3_Release(CreateAnimationControllerMethod);
	AS3_Release(CreateAnimationTrackerMethod);
	AS3_Release(CreateAnimationSetDataMethod);
	AS3_Release(CreateSkinMeshControllerMethod);
	AS3_Release(CreateEffectCodeMethod);
	AS3_Release(CreatePositionEffectMethod);
	AS3_Release(CreateTextureEffectMethod);
	AS3_Release(CreateForceTextureEffectMethod);
	AS3_Release(CreatePixelAmbientLightEffectMethod);
	AS3_Release(CreateFacingEffectMethod);
	AS3_Release(CreatePixelColorEffectMethod);
	AS3_Release(CreatePixelFogBlendMethod);
	AS3_Release(CreateEffectInfoAdditionMethod);
	AS3_Release(CreateFresnelBlendMethodMethod);

	AS3_Release(CreateProgramInstanceMethod);
	AS3_Release(CreateShadowMapProgramMethod);
	AS3_Release(CreateEnvironmentProgramMethod);
	AS3_Release(CreateEnvironmentDisplayProgramMethod);
	AS3_Release(CreateDistanceInfoProgramMethod);
	AS3_Release(CreateShadowMapDisplayEffectInfoMethod);
	AS3_Release(CreateDepthFieldEffectMethod);
	AS3_Release(CreateContorEffectMethod);
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
	

	//AS3_Release(FillRenderSystemMethod);
	//AS3_Release(FillSceneNodeMethod);
	//AS3_Release(FillCustomSceneNodeMethod);
	AS3_Release(FillObjectMethod);
	AS3_Release(AddTextureToMaterialMethod);
	AS3_Release(AddAttributeToMeshMethod);
	//AS3_Release(AddPassToEffectMethod);
	AS3_Release(AddChildToSceneNodeMethod);
	AS3_Release(AddLightToSceneManagerMethod);
	AS3_Release(AddChildToSceneManagerMethod);
	AS3_Release(AddInfoToPassDefinerMethod);

	AS3_Release(AddEffectInfoToProgramMethod);
	AS3_Release(AddProgramToSceneObjectMethod);
	AS3_Release(AddInfoForBlendMethodToPassDefinerMethod);

	AS3_Release(RemoveTextureFromMaterialMethod);
	AS3_Release(RemoveAttributeFromMeshMethod);
	//AS3_Release(RemovePassFromEffectMethod);
	AS3_Release(RemoveChildFromSceneNodeMethod);
	AS3_Release(RemoveChildFromSceneManagerMethod);

	AS3_Release(LockTextureMethod);
	AS3_Release(UnlockMethod);

	AS3_Release(GetAttributeDataFromMeshMethod);
	AS3_Release(SetAttributeDataFromMeshMethod);
	AS3_Release(GetHardwareSkinEffectMethod);
	AS3_Release(SetPassToEffectMethod);
	AS3_Release(SetProjectionToCameraMethod);
	//AS3_Release(RebuildSceneObjectMethod);
	AS3_Release(RenderSceneMethod);
	AS3_Release(SetBonesDataToSkinInfoMethod );
	AS3_Release(SetBoneNameMethod );

	AS3_Release(SetSceneObjectShadowEnbleMethod);
	AS3_Release(SetSceneObjectShadowDisplayMethod);

	AS3_Release(SetAniamtionControllerMethod);
	AS3_Release(EndDefinePassDefinerMethod);

	AS3_Release(BuildShadowMapFromLightMethod);
	AS3_Release(BuildSceneEffectPassFromSceneManagerMethod);

	AS3_Release(ResetAnimationControllerMethod);
	
	AS3_Release(RegisterAniamtionSetToControllerMethod);
	AS3_Release(UnregisterAnimationSetFromControllerMethod);
	AS3_Release(SetTrackersToAnimationSetMethod);
	
	AS3_Release(SetSkinInfoToSceneObjectMethod );
	AS3_Release(SetSkinMeshControllerToSceneObjectMethod);
	AS3_Release(SetRenderMethodToSceneObjectMethod );
	AS3_Release(SetUVSpeedToSkyDomeMethod);
	AS3_Release(EffectApplyForRenderMethod);
	
	
	//AS3_Release(AddShareVertexDestinationWithStreamMethod );
	AS3_Release(AddShareVertexDestinationWithDeclarationMethod );
	AS3_Release(WriteDataToVertexBufferMethod );

	AS3_Release(CreateCartoonEffectMethod);

	AS3_Release( CreateOutlineEffectMethod );

	AS3_LibInit(Result);

	return 0;
}
