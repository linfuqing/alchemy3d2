#pragma once

#include "../Core/AlchemySingleton.h"
#include "../Graphics/AlchemyRenderSystem.h"
//#include "AlchemyFlashTexture.h"
#include "AlchemyFlashGraphicsResourcePool.h"
//#include "AlchemyFlashProgram3D.h"
#include "AlchemyFlashAGALAssembler.h"

namespace alchemy
{
	class CFlashTexture;
	class CFlashContext3D :
		 public CRenderSystem, public CFlashGraphicsResourcePool
	{
		typedef enum
		{
			MAXINUM_TEXTURE_COUNT = 8,
			MAXINUM_VERTEX_INDEX_COUNT = 8,
			MAXINUM_VERTEX_PARAMETER_COUNT = 128,
			MAXINUM_FRAGMENT_PARAMETER_COUNT = 28,
			
			VERTEX_PARAMETER_SIZE = 4,
			FRAGMENT_PARAMETER_SIZE = 4
			//MAXINUM_PARAMETER_COUNT = MAXINUM_VERTEX_PARAMETER_COUNT + MAXINUM_FRAGMENT_PARAMETER_COUNT
		}CONTAINS;

		typedef enum
		{
			CLEAR_DIRTY = 0,
			RENDER_TARGET_DIRTY,
			PROGRAM_DIRTY,
			DRAW_INDEXED_TRIANGLES,
			INDEX_BUFFER_DIRTY,
			VERTEX_BUFFER_DIRTY_SHIFT,
			VERTEX_INDEX_DIRTY_SHIFT = VERTEX_BUFFER_DIRTY_SHIFT + MAXINUM_VERTEX_INDEX_COUNT,
			TEXTURE_DIRTY_SHIFT = VERTEX_INDEX_DIRTY_SHIFT/*VERTEX_BUFFER_DIRTY_SHIFT*/ + MAXINUM_VERTEX_INDEX_COUNT
		}RENDERDATAFLAG;

#pragma pack(1)
		typedef struct
		{
			UINT uRenderState[CRenderSystem::TOTAL_RENDER_STATE];

			UINT uDirtyFlag;
		}DEVICESTATEBLOCK;

		typedef struct  
		{
			ARGBCOLOR Color;
			UINT uFlag;
		}CLEARDATA;

		typedef struct  
		{
			UINT16 uVertexBuffer;
			UINT uOffsetInBytes;
		}VERTEXBUFFERDATA, * LPVERTEXBUFFERDATA;

		typedef struct
		{
			VERTEXBUFFERDATA VertexBufferData;
			CVertexDeclaration::ELEMENTTYPE Type;
		}VERTEXDATA, * LPVERTEXDATA;

		typedef struct
		{
			UINT16 uProgram;
			UINT uVertexParameterSize[MAXINUM_VERTEX_PARAMETER_COUNT];
			UINT uFragmentParameterSize[MAXINUM_FRAGMENT_PARAMETER_COUNT];
			UINT uVertexIndex[MAXINUM_VERTEX_PARAMETER_COUNT];
			UINT uFragmentIndex[MAXINUM_FRAGMENT_PARAMETER_COUNT];
			UINT uVertexParameterCount;
			UINT uFragmentParameterCount;
		}PROGRAMDATA;

		typedef struct
		{
			DEVICESTATEBLOCK DeviceStateBlock;

			CLEARDATA ClearData;

			UINT16 uRenderTarget;
			UINT uRenderSurface;

			VERTEXDATA VertexData[MAXINUM_VERTEX_INDEX_COUNT];
			UINT16 uIndexBuffer;
			PROGRAMDATA ProgramData;
			UINT uStart;
			UINT uTriangleCount;
			UINT uFlag;
		}RENDERDATA, * LPRENDERDATA;
#pragma pack()

	public:
		CFlashContext3D(CCompilerInterface& Complier, UINT uTotalRenderDataCount);
		~CFlashContext3D(void);

		void Destroy();

		CCompilerInterface& GetComplier();

		bool GetViewport(PUINT puWidth, PUINT puHeight);

		CGraphicsResource::HANDLE GetRenderTarget();

		bool Clear(ARGBCOLOR Color, UINT uFlag);

		bool RegisterRenderState();
		bool RestoreRenderState();

		bool SetRenderState(CRenderSystem::RENDERSTATE State, UINT uValue);

		bool SetRenderTarget(CGraphicsResource::HANDLE Texture, UINT uSurface, UINT uIndex);

		bool SetVertexDeclaration(CGraphicsResource::HANDLE VertexDeclaration);
		bool SetStreamSource(CGraphicsResource::HANDLE VertexBuffer, UINT uStream);
		bool SetIndices(CGraphicsResource::HANDLE IndexBuffer);
		bool SetShader(CGraphicsResource::HANDLE Shader);

		bool DrawTriangles(CRenderSystem::TRIANGLETYPE Type, UINT uStartVertex, UINT uTriangleCount);
		bool DrawIndexedTriangles(CRenderSystem::TRIANGLETYPE Type, UINT uStartIndex, UINT uTriangleCount);

		bool DrawToTexture(CGraphicsResource::HANDLE Source, CEffectCode::SAMPLERFILTER Filter);

		void SubmitRenderDataQueueToRender();

		bool PushProgramParameter(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uCount);
		bool PushProgramMatrixArray(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount);
		bool PushProgramMatrixPointerArray(CEffectInterface::EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValue, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount);

		bool SetProgramTexture(CGraphicsResource::HANDLE Texture, UINT uIndex);

		void CommitProgramTextureChange(UINT uTextureFlag);

#ifdef AS3_H
		AS3TYPE Init();
		void Fill(AS3TYPE Object, AS3TYPE MethodArray);
#endif

	protected:
		CGraphicsResource* _Create(CGraphicsResource::TYPE Type);

	private:
		void __ClearRenderData(RENDERDATA& RenderData);

		void __PushRenderData();

		RENDERDATA m_CurrentRenderData;
		RENDERDATA m_OldRenderData;
		LPRENDERDATA m_pRenderDataQueue;
		UINT m_uTotalRenderDataCount;
		UINT m_uCurrentRenderDataCount;

		PFLOAT m_pfProgramVertexParameter;
		UINT   m_uProgramVertexParameterCount;

		PFLOAT m_pfProgramFragmentParameter;
		UINT   m_uProgramFragmentParameterCount;

		PUINT16 m_puProgramTexture;

		UINT m_uTextureFlag;

		CGraphicsResource::HANDLE m_VertexBuffer[MAXINUM_VERTEX_INDEX_COUNT];
		UINT m_uVertexBufferDirtyFlag;

		CRenderSystem::RENDERSTATE m_RenderStateStack[CRenderSystem::TOTAL_RENDER_STATE];
		UINT m_RenderStateValueStack[CRenderSystem::TOTAL_RENDER_STATE];
		UINT m_uRenderStateStackCount;
		UINT m_uRenderStateFlag;

		CVertexDeclaration* m_pVertexDeclaration;

		UINT m_uViewportWidth;
		UINT m_uViewportHeight;
		bool m_bIsViewportDirty;

		CCompilerInterface& m_Complier;

#ifdef AS3_H
		bool _RemoveResource(CGraphicsResource::HANDLE Handle, CGraphicsResource::TYPE Type);
		//AS3TYPE m_CreateResourceMethod;
		AS3TYPE m_RenderMethod;
#endif
	};
}