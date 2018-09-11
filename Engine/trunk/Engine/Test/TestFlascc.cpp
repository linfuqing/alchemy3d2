#include "../Flascc/AlchemyFlasccGraphicsManager.h"

#include "../Graphics/AlchemyHardwareSkinEffect.h"
#include "../Graphics/AlchemyPositionEffect.h"
#include "../Graphics/AlchemyTextureEffect.h"
#include "../Graphics/AlchemyPixelColorEffect.h"
#include "../Graphics/AlchemyEmptyEffect.h"
#include "../Graphics/AlchemyAlphaTestBlendMethod.h"
#include "../Graphics/AlchemySceneObject.h"
#include "../Graphics/AlchemyBone.h"
#include "../Graphics/AlchemyKeyListener.h"

#include "../Loader/AlchemyDAEParser.h"

#include <AS3/AS3.H>

using namespace alchemy;

alchemy::CRenderSystem* g_pRenderSystem = NULL;
alchemy::CSceneManager* g_pSceneManager = NULL;

alchemy::CAnimationController* g_pAnimationController;

alchemy::CKeyListener g_KeyListener;

class CRenderLoop :
	public alchemy::CSmartClass<alchemy::IWindow::IDisplayLink>
{
public:
	CRenderLoop() {}
	~CRenderLoop() {}

	void Run(FLOAT fTime, FLOAT fElapseTime)
	{
		if(g_pRenderSystem && g_pSceneManager)
		{
			//g_pRenderSystem->Clear( 0xffff0000, ALCHEMY_FLAG(alchemy::CRenderSystem::TARGET) | ALCHEMY_FLAG(alchemy::CRenderSystem::ZBUFFER) | ALCHEMY_FLAG(alchemy::CRenderSystem::STENCIL) );

			alchemy::CCamera& Camera = g_pSceneManager->GetSceneCamera();

			//static FLOAT z = 0;
			/*g_pSceneManager->GetSceneCamera().Right(0.01f);

			alchemy::CVector3 Position = g_pSceneManager->GetSceneCamera().GetTranslation();
			Position.Normalize();
			Position.Negate();
			g_pSceneManager->GetSceneCamera().SetDirection(Position);*/

			const alchemy::FLOAT3& Position = Camera.GetTranslation();

			if( g_KeyListener.CheckKeyDown(alchemy::IWindow::KEY_W) )
				Camera.SetTranslation(Position.x, Position.y, Position.z + 1.0f);

			if( g_KeyListener.CheckKeyDown(alchemy::IWindow::KEY_S) )
				Camera.SetTranslation(Position.x, Position.y, Position.z - 1.0f);

			if( g_KeyListener.CheckKeyDown(alchemy::IWindow::KEY_A) )
				Camera.SetTranslation(Position.x - 1.0f, Position.y, Position.z);

			if( g_KeyListener.CheckKeyDown(alchemy::IWindow::KEY_D) )
				Camera.SetTranslation(Position.x + 1.0f, Position.y, Position.z);

			if(g_pAnimationController)
				g_pAnimationController->Update(0.001f);

			g_pSceneManager->Update(fElapseTime);
			g_pSceneManager->Render();

			g_pRenderSystem->Present();
		}
	}
};

CRenderLoop g_RenderLoop;

alchemy::CDAEParser* g_pPaser;

class CRenderSystemCreatedListener : 
	public alchemy::CSmartClass<alchemy::IEventDispatcher::IListener>
{
public:
	CRenderSystemCreatedListener()
	{

	}

	~CRenderSystemCreatedListener()
	{

	}

	void Handler(const alchemy::IEventDispatcher::CEvent& Event)
	{
		g_pRenderSystem = &( static_cast<const CGraphicsManager::CRenderSystemEvent&>(Event).GetRenderSystem() );

		inline_as3(
			"import alchemy.utils.AlchemyFPS;\n"
			"import alchemy.utils.AlchemyRenderInfo;\n"
			"import alchemy.core.AlchemyRenderSystem;\n"
			"import alchemy.core.AlchemyGraphicsManager;\n"
			"import flash.display.DisplayObjectContainer;\n"
			"var root = CModule.rootSprite as DisplayObjectContainer;\n"
			"var renderInfo = null;\n"
			"var renderSystem = AlchemyGraphicsManager.instance.getRenderSystem(0);"
			"if(renderSystem) \n"
			"{ \n"
			"	renderInfo = new AlchemyRenderInfo;\n"
			"	renderSystem.renderInfo = renderInfo;\n"
			"} \n"
			"var fps = new AlchemyFPS(renderInfo);\n"
			"fps.x = root.stage.stageWidth - fps.width;\n"
			"root.addChild(fps);\n"
		: : );

		g_pSceneManager = alchemy::ALCHEMY_GRAPHICS_MANAGER.CreateSceneManager(*g_pRenderSystem);
		if(g_pSceneManager)
		{
			if(g_pPaser)
			{
				alchemy::CVector<alchemy::CSceneObject*> SceneObjects;
				alchemy::CVector<alchemy::CBone*> Bones;
				//alchemy::CVector<alchemy::CSceneManager::RESOURCEEINFO> MaterialInfos;
				alchemy::CSceneNode* pSceneNode = g_pSceneManager->CreateScene(alchemy::ALCHEMY_MESH_MANAGER.AddSceneData( g_pPaser->GetSceneData() ), alchemy::CSkinMeshController::SKIN_SOFTWARE, &SceneObjects, ALCHEMY_NULL, ALCHEMY_NULL, ALCHEMY_NULL, &Bones);
				if(pSceneNode)
				{
					g_pSceneManager->AddChild(*pSceneNode);

					alchemy::CEffectCode Code( g_pRenderSystem->GetCompiler() );

					alchemy::CEffectCode::CPassDefiner PassDefiner(Code);

					//alchemy::CHardwareSkinEffect HardwareSkinEffect;
					alchemy::CPositionEffect PositionEffect;
					alchemy::CTextureEffect TextureEffect;
					//alchemy::CFloat4 Color(0.0, 1.0, 0.0, 1.0);
					//alchemy::CPixelColorEffect PixelColorEffect(&Color);
					alchemy::CEmptyEffect EmptyEffect;
					alchemy::CAlphaTestBlendMethod AlphaTestBlendMethod;

					/*alchemy::CEffectCode::ELEMENTDECLARATION ElementDeclaration[] = 
					{
						{
							alchemy::CVertexDeclaration::FLOAT4, 
								alchemy::CVertexDeclaration::POSITION, 
								0
						}
					};*/

					//HardwareSkinEffect.Create(16, 4, ElementDeclaration, 1);
					//PassDefiner.AddInfo(HardwareSkinEffect, NULL);
					PassDefiner.AddInfo(PositionEffect, ALCHEMY_NULL);
					PassDefiner.AddInfo(TextureEffect, ALCHEMY_NULL);
					//PassDefiner.AddInfo(PixelColorEffect, NULL);
					//PassDefiner.AddInfo(EmptyEffect, &AlphaTestBlendMethod);
					PassDefiner.EndDefine();

					alchemy::CEffect* pEffect = g_pRenderSystem->CreateEffect(Code);

					alchemy::CRenderMethod* pRenderMethod = g_pSceneManager->CreateRenderMethod(Code);

					if(pEffect && pRenderMethod)
					{
						alchemy::CSceneObject* pSceneObject;
						alchemy::CVector<alchemy::CSceneObject*>::CIterator Iterator = SceneObjects.GetIterator(0);
						while( alchemy::CVector<alchemy::CSceneObject*>::IsValuable(Iterator) )
						{
							pSceneObject = *Iterator;
							if(pSceneObject)
							{
								pSceneObject->ApplyForRender();
								pSceneObject->SetEffect( pEffect->GetResourceHandle() );
								pSceneObject->SetRenderMethod(pRenderMethod);
							}

							++ Iterator;
						}
					}

					g_pAnimationController = g_pPaser->GetAnimationController();
					if(g_pAnimationController)
					{
						alchemy::CBone* pBone;
						alchemy::CVector<alchemy::CBone*>::CIterator BoneIterator = Bones.GetIterator(0);
						while( alchemy::CVector<alchemy::CBone*>::IsValuable(BoneIterator) )
						{
							pBone = *BoneIterator;
							if(pBone)
							{
								pBone->RegisterAnimationController(*g_pAnimationController);

								break;
							}

							++ BoneIterator;
						}
					}
				}

				ALCHEMY_DEBUG_DELETE(g_pPaser);
			}

			g_pSceneManager->SetBackgroundColor(0xffff0000);
			g_pSceneManager->GetSceneCamera().SetProjection(45, 1.0, 3000.0);

			g_pSceneManager->GetSceneCamera().SetTranslation(0.0, 0.0, - 20.0f);
		}

		alchemy::ALCHEMY_GRAPHICS_MANAGER.MainLoop();
	}
};

CRenderSystemCreatedListener g_RenderSystemCreatedListener;

class CLoaderListener : 
	public alchemy::CSmartClass<alchemy::IEventDispatcher::IListener>
{
public:
	CLoaderListener()
	{

	}

	~CLoaderListener()
	{

	}

	void Handler(const alchemy::IEventDispatcher::CEvent& Event)
	{
		UINT uWindow = alchemy::ALCHEMY_GRAPHICS_MANAGER.CreateWindow(0.0, 0.0, 0.0, 0.0);
		alchemy::IWindow* pWindow = alchemy::ALCHEMY_GRAPHICS_MANAGER.GetWindow(uWindow);

		if(pWindow)
		{
			pWindow->AttachToScreen(true);

			pWindow->AddRunLoop(g_RenderLoop);

			g_KeyListener.AddEventListenerTo(*pWindow);
		}

		const alchemy::CGraphicsManager::CLoadAllEvent& LoadAllEvent = static_cast<const alchemy::CGraphicsManager::CLoadAllEvent&>(Event);
		alchemy::IEventDispatcher* pEventDispatcher = LoadAllEvent.GetTarget();
		if(pEventDispatcher)
		{
			const UINT* puDataIndices = LoadAllEvent.GetDataIndices();
			UINT uDataCount = LoadAllEvent.GetDataCount() - 1;
			const alchemy::CGraphicsManager::DATA* pData;
			alchemy::CMeshManager::TEXTUREDATA TextureData;
			for(UINT i = 0; i < uDataCount; ++ i)
			{
				pData = alchemy::ALCHEMY_GRAPHICS_MANAGER.GetData(puDataIndices[i]);
				if(pData)
				{
					TextureData.pData = pData->pData;
					TextureData.uWidth = pData->uAlpha;
					TextureData.uHeight = pData->uBeta;
					TextureData.Format = alchemy::CTexture::DEFAULT;
					TextureData.uUsage = 0;
					TextureData.uMipLevel = 0;
					TextureData.pcFileName = ALCHEMY_NULL;

					alchemy::ALCHEMY_MESH_MANAGER.AddTextureData(TextureData);
				}
			}

			pData = alchemy::ALCHEMY_GRAPHICS_MANAGER.GetData(puDataIndices[uDataCount]);
			if(pData)
			{
				alchemy::CDAEParser::TEXTUREDATARES TextureDataRes[] = 
				{
					/*{ALCHEMY_TEXT("Map__36-image")       , 1}, 
					{ALCHEMY_TEXT("Map__36_ncl1_1-image"), 2}, 
					{ALCHEMY_TEXT("Map__38-image")       , 3}, 
					{ALCHEMY_TEXT("Map__48-image")       , 4}, 
					{ALCHEMY_TEXT("Map__39-image")       , 5}, 
					{ALCHEMY_TEXT("Map__37-image")       , 6}, */
					{ALCHEMY_TEXT("Map__23-image")       , 1}, 
					{ALCHEMY_TEXT("Map__19-image")       , 2}, 
					{ALCHEMY_TEXT("Map__21-image")       , 3}, 
					{ALCHEMY_TEXT("Map__20-image")       , 4}, 
					{ALCHEMY_TEXT("Map__22-image")       , 5}, 
					{ALCHEMY_TEXT("Map__27-image")       , 6}, 
				};

				alchemy::CDAEParser::STREAMUSAGE StreamUsage[] = 
				{
					{alchemy::CVertexDeclaration::FLOAT4, alchemy::CVertexDeclaration::POSITION, 0, 0},
					{alchemy::CVertexDeclaration::FLOAT3, alchemy::CVertexDeclaration::NORMAL, 0, 0},
				};

				ALCHEMY_DEBUG_NEW( g_pPaser, CDAEParser(TextureDataRes, sizeof(TextureDataRes) / sizeof(alchemy::CDAEParser::TEXTUREDATARES), StreamUsage, sizeof(StreamUsage) / sizeof(alchemy::CDAEParser::STREAMUSAGE), 0) );
				g_pPaser->Parse(pData->pData);
			}
		}

		UINT uType;
		if( ALCHEMY_GET_EVENT_TYPE(uType, RENDER_SYSTEM_CREATED, CGraphicsManager) )
			alchemy::ALCHEMY_GRAPHICS_MANAGER.AddEventListener(uType, g_RenderSystemCreatedListener);

		alchemy::ALCHEMY_GRAPHICS_MANAGER.CreateRenderSystem(uWindow, 0, 0, 0, 0);
	}
};

CLoaderListener g_LoaderListener;

extern "C" void FlasccMain()
{
	alchemy::CFlasccGraphicsManager* pGraphicsManager;

	ALCHEMY_DEBUG_NEW(pGraphicsManager, CFlasccGraphicsManager);

	UINT uType;
	if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_ALL_COMPLETE, alchemy::CGraphicsManager) )
		alchemy::ALCHEMY_GRAPHICS_MANAGER.AddEventListener(uType, g_LoaderListener);

	static const alchemy::CHAR* pcPATH[] = 
	{
		/*ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_01.png"), 
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_02.png"), 
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_03.png"), 
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_04.png"), 
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_05.png"), 
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_06.png"), 
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all.xml")*/

		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_feet_diff.jpg"),
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_head_spec.jpg"),
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_leg_diff.jpg"),
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_body_spec.jpg"),
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_hand_diff.jpg"),
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_head_diff.jpg"),
		ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_ban_01.xml"),
	};

	alchemy::ALCHEMY_GRAPHICS_MANAGER.LoadAll( pcPATH, sizeof(pcPATH) / sizeof(const alchemy::CHAR*) );
}

int main()
{
	AS3_GoAsync();

	return 0;
}