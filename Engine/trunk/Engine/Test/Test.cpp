
#include "stdafx.h"

#include "../Flascc/AlchemyFlasccGraphicsManager.h"
#include "../Graphics/AlchemyGeometryInstance.h"

#include "../Loader/AlchemyJSONResourceParser.h"
#include "../Loader/AlchemyJSONConfigParser.h"
#include "../Loader/AlchemySkinMeshRes.h"
#include "../Loader/AlchemyFileSystem.h"

#include "../Loader/AlchemyLoaderUtils.h"

#include "../Graphics/AlchemyInstancePositionEffect.h"
#include "../Graphics/AlchemyPositionEffect.h"
#include "../Graphics/AlchemyTextureEffect.h"

#undef CreateWindow

using namespace alchemy;

int _tmain(/*int argc, _TCHAR* argv[]*/)
{
	CMeshManager MeshManager;
	CFlasccGraphicsManager GraphicsManager;
	IWindow* pWindow = GraphicsManager.CreateWindow(0.0f, 0.0f, 0.0f, 0.0f);
	CRenderSystem* pRenderSystem = GraphicsManager.CreateRenderSystem(pWindow);
	CSceneManager* pSceneManager = GraphicsManager.CreateSceneManager(*pRenderSystem);

	CJSONResourceParser Paraser(pSceneManager);

	alchemy::CHAR* pBuffer = ALCHEMY_TEXT("json\\resource.astep");

	CLoaderResourceManager ResourceManager;

	char szBuffer[4096 * 10] = {0};

	CFileSystem* pFileSystem = new CFileSystem;
	pFileSystem->OpenFile( pBuffer );
	pFileSystem->ReadFile(szBuffer, sizeof(char), 4096 * 10);
	pFileSystem->CloseFile();

	BASICSTRING strTemp = szBuffer;
	STRING strOut;
	ALCHEMY_BASICSTRING_TO_STRING(strOut, g_uUTF8, strTemp);

	Paraser.Parse( strOut, ALCHEMY_TEXT("json\\") );

	pBuffer =  ALCHEMY_TEXT("json\\fuck");

	CJSONConfigParaser ConfigParser(pSceneManager->GetSceneNodeFactory(), pSceneManager);

	pFileSystem->OpenFile( pBuffer );
	pFileSystem->ReadFile(szBuffer, sizeof(char), 4096 * 10);
	pFileSystem->CloseFile();
	delete pFileSystem;

	strTemp = szBuffer;
	ALCHEMY_BASICSTRING_TO_STRING(strOut, g_uUTF8, strTemp);

	ConfigParser.Parse(strOut);

	CSkinMeshRes* pLoaderResource = static_cast<CSkinMeshRes*>( CLoaderResourceManager::GetInstance().GetLoaderResource(ALCHEMY_TEXT("astep1"), CLoaderResource::SKIN_MESH_RES) );

	CGeometryInstance GeomtryInstance;

	if(pLoaderResource->GetCreateFlag() == CLoaderResource::UNCREATE)
	{
		if( pLoaderResource->Create() )
		{
			CEffectCode EffectCode( pRenderSystem->GetCompiler() );
			CEffectCode::CPassDefiner PassDefiner(EffectCode);
			CInstancePositionEffect InstancePositionEffect(10);
			CPositionEffect PositionEffect;
			CTextureEffect TextureEffect;
			PassDefiner.AddInfo(InstancePositionEffect, ALCHEMY_NULL);
			PassDefiner.AddInfo(PositionEffect,ALCHEMY_NULL);
			PassDefiner.AddInfo(TextureEffect, ALCHEMY_NULL);
			PassDefiner.EndDefine();

			CEffect* pEffect = pRenderSystem->CreateEffect(EffectCode);
			CRenderMethod* pRenderMethod = pSceneManager->CreateRenderMethod(EffectCode);
			GeomtryInstance.Create(pLoaderResource->GetEngineResource()->GetResourceHandle(), 0,  pEffect->GetResourceHandle(), pRenderMethod, 0);
			CGeometryInstance::TRANSFORMINFO TransformInfo = {CGeometryInstance::MATRIX, 0};
			GeomtryInstance.SetInstances(100, 10, &TransformInfo, 1);
			CMatrix4x4 Matrix;
			Matrix.Identity();
			Matrix.SetPosition(0.0, 2.0, 0.0);
			GeomtryInstance.SetTransform(50, 0, Matrix.fValue, false);
			pSceneManager->AddChild(GeomtryInstance);
		}
	}

	pSceneManager->Update(0.0);
	pSceneManager->Render();

	return 0;
}
