#include "stdafx.h"
#include "../Flash/AlchemyFlashAGALAssembler.h"
#include "../Graphics/AlchemyTextureEffect.h"
#include "../Graphics/AlchemyPixelAmbientLightEffect.h"
#include "../Flash/AlchemyFlashSceneManager.h"
#include "../Graphics/AlchemyLightEffect.h"
#include "../Graphics/AlchemyVertexDeclaration.h"
#include "../Graphics/AlchemyHardwareSkinEffect.h"
#include "../Graphics/AlchemyShadowMapDisplayEffect.h"
#include "../Graphics/AlchemySkyPlane.h"
#include "../Graphics/AlchemyWater.h"
#include "../Graphics/AlchemyTexture2DEffect.h"
#include "../Graphics/AlchemyPositionEffect.h"
#include "../Graphics/AlchemyFresnelBlendMethod.h"
#include "../Graphics/AlchemyAttributeProjectionEffect.h"
#include "../Graphics/AlchemyBlinnPhongEffect.h"
#include "../Graphics/AlchemyEffectInfoAddition.h"
#include "../Graphics/AlchemyGeometryInstance.h"
//#include "../Core/AlchemySmartPointer.h"
//#include "../Flash/AlchemyFlashContext3D.h"
//#include <stdio.h>
//#include <memory.h>
//#include "contains.h"
#include <memory.h>

#include "../Core/AlchemyString.h"
#include "../Core/AlchemyList.h"
#include "../Core/AlchemyFunction.h"

#include <crtdbg.h>

using namespace alchemy;

int _tmain(/*int argc, _TCHAR* argv[]*/)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CMeshManager MeshManager;

	//ʹ��AGAL������
	CFlashAGALAssembler Complier;

	CFlashContext3D Context3D(Complier, 10);

	CFlashSceneNodeFactory Factory;

	CFlashSceneManager SceneManager(Context3D, 10);

	//����������
	CEffectCode Code(Complier);

	//PASS������
	CEffectCode::CPassDefiner Pass(Code);

	//ʹ������Ч��
	CTextureEffect TextureEffect(1);
	//CPixelAmbientLightEffect LightEffect;
	CHardwareSkinEffect HardwareSkinEffect;

	CEffectCode::ELEMENTPARAMETER SkinElementParameter[2];
	SkinElementParameter[0].Type = CVertexDeclaration::FLOAT4;
	SkinElementParameter[0].Usage = CVertexDeclaration::POSITION;
	SkinElementParameter[0].uIndex = 0;
	SkinElementParameter[1].Type = CVertexDeclaration::FLOAT3;
	SkinElementParameter[1].Usage = CVertexDeclaration::NORMAL;
	SkinElementParameter[1].uIndex = 0;
	HardwareSkinEffect.Create(30, 5, SkinElementParameter, 1);
	//�����л��

	UINT k = 0;
	CShadowMapDisplayEffect ShadowMapDisplayEffect(0, 0, k);

	//����Ϣ�����0��λ��
	//Pass.AddInfo(TextureEffect, ALCHEMY_NULL);

	//Pass.AddInfo(LightEffect, CEffectCode::CPassDefiner::MODULATE);
	//CLightEffect LightEffect;
	//LightEffect.AddLight(CLight::POINT);
	//LightEffect.BuildLightEffectPass(Pass);
	CLight* pLight = SceneManager.CreateLight();
	pLight->BuildShadowMap(512, 90, 0.5, 1000);
	pLight->Render();
	SceneManager.AddLight(pLight->GetResourceHandle(), COldLightEffect::BLINN_PHONG);
	pLight = SceneManager.CreateLight();
	SceneManager.AddLight(pLight->GetResourceHandle(), COldLightEffect::BLINN_PHONG);
	pLight = SceneManager.CreateLight();
	/*SceneManager.AddLight(pLight->GetResourceHandle(), CLightEffect::BLINN_PHONG);
	pLight = SceneManager.CreateLight();
	SceneManager.AddLight(pLight->GetResourceHandle(), CLightEffect::BLINN_PHONG);
	pLight = SceneManager.CreateLight();
	SceneManager.AddLight(pLight->GetResourceHandle(), CLightEffect::BLINN_PHONG);
	pLight = SceneManager.CreateLight();
	SceneManager.AddLight(pLight->GetResourceHandle(), CLightEffect::BLINN_PHONG);
	pLight = SceneManager.CreateLight();
	SceneManager.AddLight(pLight->GetResourceHandle(), CLightEffect::BLINN_PHONG);
	pLight = SceneManager.CreateLight();
	SceneManager.AddLight(pLight->GetResourceHandle(), CLightEffect::BLINN_PHONG);*/
	//SceneManager.BuildSceneEffectPass(Pass, 2);

	//Pass.AddInfo(HardwareSkinEffect, ALCHEMY_NULL);
	//Pass.AddInfo(ShadowMapDisplayEffect, ALCHEMY_NULL/*CEffectCode::CPassDefiner::ADD*/);
	//Pass.AddInfo(TextureEffect, CEffectCode::CPassDefiner::SELECTARG1);

	//CSceneObject::CShadowMapEffect ShadowMapEffect;
	//Pass.AddInfo(ShadowMapEffect, ALCHEMY_NULL);
	CPositionEffect PositionEffect;
	//CTexture2DEffect Texture2DEffect;
	Pass.AddInfo(PositionEffect, ALCHEMY_NULL);
	//Pass.AddInfo(ShadowMapDisplayEffect, ALCHEMY_NULL);
	//Pass.AddInfo(Texture2DEffect, ALCHEMY_NULL);
	//Pass.AddInfo(ShadowMapDisplayEffect, ALCHEMY_NULL/*CEffectCode::CPassDefiner::ADD*/);
	//CAttributeProjectionEffect AttributeProjectionEffect(CVertexDeclaration::NORMAL, true);
	//Pass.AddInfo(AttributeProjectionEffect, ALCHEMY_NULL);
	CTexture2DEffect Texture2DEffect(true);
	Pass.AddInfo(Texture2DEffect, ALCHEMY_NULL);
	CFresnelBlendMethod Fresnel;
	Pass.AddInfo(TextureEffect, &Fresnel);

	//Pass.AddInfo(TextureEffect1, &Fresnel);
	//Pass.AddInfo(TextureEffect, CEffectCode::CPassDefiner::ADD);

	//SceneManager.BuildSceneEffectPass(Pass, 6);
	//����PASS
	UINT uPass = Pass.EndDefine();

	const CEffectCode::PASS* pPass = Code.GetPass(uPass);

	STRING Out;

	//���ɴ��뵽Out��
	Code.BuildShaderFunctionCode(Out, pPass->uVertexShader);
	wprintf(ALCHEMY_TEXT("Vertex Shader:\n"));
	wprintf(static_cast<const CHAR*>(Out));

	Code.BuildShaderFunctionCode(Out, pPass->uFragmentShader);
	wprintf(ALCHEMY_TEXT("\nFragment Shader:\n"));
	wprintf(static_cast<const CHAR*>(Out));

	CRenderMethod* pRenderMethod = SceneManager.CreateRenderMethod(Code);
	pRenderMethod->Activate(0);

	CMaterial* pMaterial = SceneManager.CreateMaterial();
	//pRenderMethod->SetMaterial(*pMaterial);

	CMatrix4x4 Matrix;
	Matrix.Identity();
	/*pRenderMethod->SetWorldMatrix(Matrix);

	pRenderMethod->SetCamera( SceneManager.GetCamera() );

	pRenderMethod->CommitTransform();*/

	CVertexDeclaration::VERTEXELEMENT VertexElements[5];
	VertexElements[0].Type = CVertexDeclaration::FLOAT4;
	VertexElements[0].Usage = CVertexDeclaration::POSITION;
	VertexElements[0].uOffset = 0;
	VertexElements[0].uStream = 0;

	VertexElements[1].Type = CVertexDeclaration::FLOAT2;
	VertexElements[1].Usage = CVertexDeclaration::TEXCOORD;
	VertexElements[1].uOffset = 0;
	VertexElements[1].uStream = 0;

	VertexElements[2].Type = CVertexDeclaration::FLOAT3;
	VertexElements[2].Usage = CVertexDeclaration::NORMAL;
	VertexElements[2].uOffset = 0;
	VertexElements[2].uStream = 0;

	VertexElements[3].Type = CVertexDeclaration::FLOAT4;
	VertexElements[3].Usage = CVertexDeclaration::BLENDWEIGHT;
	VertexElements[3].uOffset = 0;
	VertexElements[3].uStream = 1;

	VertexElements[4].Type = CVertexDeclaration::FLOAT4;
	VertexElements[4].Usage = CVertexDeclaration::BLENDINDICES;
	VertexElements[4].uOffset = 0;
	VertexElements[4].uStream = 2;

	UINT uVertexUsage[5] = {0, 0, 0, 0, 0};

	CMesh* pMesh = SceneManager.CreateMesh(3, 3, 0, CIndexBuffer::BIT16, VertexElements, 5, uVertexUsage);
	PUINT16 puIndices = reinterpret_cast<PUINT16>( pMesh->GetIndexBuffer().Lock(0) );
	puIndices[0] = 0;
	puIndices[1] = 1;
	puIndices[2] = 2;
	pMesh->GetIndexBuffer().Unlock();


	CMesh::ATTRIBUTERANGE AttributeRange;

	AttributeRange.uIndexCount = 3;
	AttributeRange.uStartIndex = 0;

	pMesh->GetAttributeTable().Add(AttributeRange);
	/*CMeshManager MeshManager;

	CVertexDeclaration::VERTEXELEMENT VertexElement[2];
	VertexElement[0].uStream = 0;
	VertexElement[0].uOffset = 0;
	VertexElement[0].Type = CVertexDeclaration::FLOAT4;
	VertexElement[0].Usage = CVertexDeclaration::POSITION;

	VertexElement[1].uStream = 0;
	VertexElement[1].uOffset = 16;
	VertexElement[1].Type = CVertexDeclaration::FLOAT2;
	VertexElement[1].Usage = CVertexDeclaration::TEXCOORD;
	UINT uPlane = MeshManager.CreatePlane(1000.0f, 1000.0f, 1, 1, 1, 1, true, VertexElement, 2);
	CMesh* pMesh = SceneManager.CreateMesh(*MeshManager.GetMeshData(uPlane));*/

	CSceneObject* pSceneObject = SceneManager.CreateSceneObject(pMesh->GetResourceHandle()/*pMesh->GetResourceHandle()*/, 0, pMaterial->GetResourceHandle(), pRenderMethod->GetResourceHandle());
	//SceneManager.AddChild(*pSceneObject);

	UINT uData[] = {1, 2, 3, 4};
	CTexture* pTexture  = Context3D.CreateCubeTexture(4, 0, CTexture::A8R8G8B8, 1, uData, uData, uData, uData, uData, uData);

	CSkyPlane SkyPlane;
	SkyPlane.Create(1000, 1000, 1000, pTexture->GetResourceHandle(), Complier, true, true, true);
	SceneManager.AddChild(SkyPlane);

	//CMirror Mirror;

	//Mirror.Create(1000, 1000, 1, 1, 0, 0, ALCHEMY_NULL, 0, true);
	//Mirror.SetTranslation(CFloat3(0.0f, -200.f, 0.0f));
	//SceneManager.AddChild(Mirror);

	CBlinnPhongEffect BlinnPhongEffect(0);

	CWater Water;
	CEffectCode::CPassDefiner::EFFECTINFO EffectInfo[2];
	EffectInfo[0].pEffectInfo  = &BlinnPhongEffect;
	EffectInfo[0].pBlendMethod = ALCHEMY_NULL;

	CEffectInfoAddition Add;
	CLightEffect LightEffect;
	EffectInfo[1].pEffectInfo  = &LightEffect;
	EffectInfo[1].pBlendMethod = &Add;
	Water.Create( 1000, 1000, 1, 1, 0, 0, pTexture->GetResourceHandle(), 0, EffectInfo, 0);
	SceneManager.AddChild(Water);

	CGeometryInstance GeometryInstance;
	GeometryInstance.Create( *pRenderMethod, pMaterial->GetResourceHandle() );
	GeometryInstance.AddGeometry( pMesh->GetResourceHandle() );
	SceneManager.AddChild(GeometryInstance);

	SceneManager.GetSceneCamera().SetProjection(3.1415926f / 2, 1, 5000, 1);

	SceneManager.Update(0.0f);

	for(UINT i = 0; i < 10; ++ i)
		SceneManager.Render();

	/*CEffectCode::CShaderParametersDefiner ParametersDefiner(Code);
	//����FLOAT4�͵�Position�������
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION);
	//����FLOAT2�͵�UV�������
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD);
	//���ز���ָ��,��õĲ���Ϊstruct{Texcoord:Float2, Position:Float4}
	UINT uParameters = ParametersDefiner.EndDefine();

	//������ɫ��������
	CEffectCode::CShaderFunctionDefiner VertexDefiner(Code, true);

	//������õĲ�����Ϊ��ǰ������ɫ���Ĳ������벢��ȡ����ָ��
	UINT uVertexParameter = VertexDefiner.BeginFunction(uParameters);
	
	//�Բ������е�ֵַΪ1��Ѱַ�൱��Parameter.Position��Parameter[1]
	UINT uSourcePosition  = Code.AddAddressVariable(uVertexParameter, 0);
	//�Բ������е�ֵַΪ1��Ѱַ�൱��Parameter.Texcoord��Parameter[0]
	UINT uSourceUV        = Code.AddAddressVariable(uVertexParameter, 1);

	//����һ������ΪMATRIX4X4����ʱ�������൱��MATRIX4X4 Matrix(Ѱַ������)
	UINT uMatrix = VertexDefiner.ApplyVariable(CEffectCode::MATRIX4X4_VARIABLE);

	//����һ������ΪParameters����ʱ����
	UINT uTemp = VertexDefiner.ApplyVariable(uParameters);

	UINT uDestinationPosition = Code.AddAddressVariable(uTemp, 0);
	UINT uDestinationUV       = Code.AddAddressVariable(uTemp, 1);

	//Ѱַ����
	UINT uAddress[4] = {0,1, 2,3};
	//����mov DestinationPosition SourcePositionָ�������Ѱַ�����ǰ��������mov DestinationPosition.xyz SourcePosition.xyz
	VertexDefiner.AppendInstruction(CCompilerInterface::MOV, uDestinationPosition, uAddress    , 3, uSourcePosition, uAddress    , 3, 0, ALCHEMY_NULL, 0);
	//ͬ�ϣ�����Ѱַ�������һ������mov DestinationPosition.w SourcePosition.w
	VertexDefiner.AppendInstruction(CCompilerInterface::MOV, uDestinationPosition, &uAddress[3], 1, uSourcePosition, &uAddress[3], 1, 0, ALCHEMY_NULL, 0);

	VertexDefiner.AppendInstruction(CCompilerInterface::MOV, uDestinationUV      , ALCHEMY_NULL, 0, uSourceUV      , ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	//����ʱ������Ϊ����ֵ���൱��return temp
	if( VertexDefiner.EndFunction(uTemp) )
	{
		STRING Out;

		//���ɴ���
		Code.BuildShaderFunctionCode( Out, VertexDefiner.EndDefine() );

		//���
		wprintf( ALCHEMY_TEXT("Vertex Shader:\n") );

		wprintf( static_cast<const CHAR*>(Out) );

		//����ȫ�ֱ���
		//UINT uUV = Code.AddGlobalVariable(CEffectCode::VECTOR4_VARIABLE, 0, CEffectCode::LIGHT, 0, 0);

		//Ƭ����ɫ������������ͬ 
		CEffectCode::CShaderFunctionDefiner FragmentDefiner(Code, false);

		UINT uFragmentParameter = FragmentDefiner.BeginFunction(uParameters);

		UINT uUV = Code.AddAddressVariable(uFragmentParameter, 1);

		CEffectCode::SAMPLERDECLARATION Sampler;

		UINT uSampler = Code.AddSamplerVariable(Sampler, 0);

		uTemp = FragmentDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE);

		FragmentDefiner.AppendInstruction(CCompilerInterface::TEX, uTemp, ALCHEMY_NULL, 0, uUV, ALCHEMY_NULL, 0, uSampler, ALCHEMY_NULL, 0);

		if( FragmentDefiner.EndFunction(uTemp) )
		{
			wprintf( ALCHEMY_TEXT("\nFragment Shader:\n") );

			Code.BuildShaderFunctionCode( Out, FragmentDefiner.EndDefine() );

			wprintf( static_cast<const CHAR*>(Out) );
		}
	}*/

	/*CFlashSceneNodeFactory* pSceneNodeFactory;

	ALCHEMY_DEBUG_NEW(pSceneNodeFactory, CFlashSceneNodeFactory);

	CFlashContext3D* pContext3D;

	ALCHEMY_DEBUG_NEW(pContext3D, CFlashContext3D(10));

	CFlashSceneManager* pSceneManager;

	ALCHEMY_DEBUG_NEW(pSceneManager, CFlashSceneManager(*pContext3D, 20));

	CMaterial* Material = pSceneManager->CreateMaterial();
	CEffect* pEffect = pContext3D->CreateEffect(1);
	CRenderMethod::PARAMETERLIST Parameters;
	memset(&Parameters, 0, sizeof(Parameters));
	CRenderMethod* pRenderMethod = pSceneManager->CreateRenderMethod(&Parameters, pEffect->GetResourceHandle());
	CMeshManager::MESHDATA MeshData;
	memset(&MeshData, 0, sizeof(MeshData));
	CMesh* pMesh = pSceneManager->CreateMesh(MeshData);

	CSceneObject* pSceneObject = pSceneManager->CreateSceneObject(pMesh->GetResourceHandle(), 0, Material->GetResourceHandle(), pRenderMethod);

	pSceneManager->AddChild(*pSceneObject);

	pSceneManager->Render();*/

	return 0;
}