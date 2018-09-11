#pragma once

#include "../Graphics/AlchemyRenderSystem.h"

namespace alchemy
{
	class CFlasccContext3D :
		public CRenderSystem
	{
		typedef enum
		{
			MAXINUM_TEXTURE_COUNT = 8,
			MAXINUM_VERTEX_INDEX_COUNT = 8,
			MAXINUM_VERTEX_PARAMETER_COUNT = 128,
			MAXINUM_FRAGMENT_PARAMETER_COUNT = 28,

			VERTEX_PARAMETER_SIZE = 4,
			FRAGMENT_PARAMETER_SIZE = 4
		}CONTAINS;

		typedef enum
		{
			RENDER_TARGET_DIRTY = 0,
			PROGRAM_DIRTY,
			INDEX_BUFFER_DIRTY,
			VERTEX_INDEX_DIRTY_SHIFT,
			TEXTURE_DIRTY_SHIFT = VERTEX_INDEX_DIRTY_SHIFT + MAXINUM_VERTEX_INDEX_COUNT
		}RENDERDATAFLAG;

#pragma pack(1)
		typedef struct
		{
			UINT uRenderState[CRenderSystem::TOTAL_RENDER_STATE];

			UINT uDirtyFlag;
		}RENDERSTATEBLOCK;

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
			UINT uDirtyVertexParameterBufferOffset;
			UINT uDirtyVertexParameterBufferCount;
			UINT uDirtyFragmentParameterBufferOffset;
			UINT uDirtyFragmentParameterBufferCount;
		}PROGRAMDATA;

		typedef struct
		{
			RENDERSTATEBLOCK RenderStateBlock;

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

		class CViewportResetHandler :
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CViewportResetHandler(CFlasccContext3D& Parent);
			~CViewportResetHandler();

			void Handler(const CEvent& Event);

		private:
			CFlasccContext3D& m_Parent;
		};
	public:
		CFlasccContext3D(UINT uRenderSystemIndex);
		~CFlasccContext3D(void);

		UINT GetRenderSystemIndex()const;

		bool CheckFormat(FORMATTYPE Type, UINT uFormat);

		ICompiler& GetCompiler();

		bool GetViewport(PUINT puWidth, PUINT puHeight);

		bool Clear(ARGBCOLOR Color, UINT uFlag);

		bool ResetTexture();

		bool RegisterRenderState();
		bool RestoreRenderState();

		bool SetRenderState(const RENDERSTATEINFO* pRenderStateInfos, UINT uRenderStateInfoCount, bool bIsRegister);

		bool SetRenderTarget(CGraphicsResource::HANDLE Texture, UINT uSurface, UINT uIndex);

		bool SetVertexDeclaration(CGraphicsResource::HANDLE VertexDeclaration);
		bool SetStreamSource(CGraphicsResource::HANDLE VertexBuffer, UINT uStream);
		bool SetIndices(CGraphicsResource::HANDLE IndexBuffer);
		bool SetShader(CGraphicsResource::HANDLE Shader);

		bool SetShaderParameters(IEffect::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uCount);
		bool SetShaderMatrixArray(IEffect::EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount);
		bool SetShaderMatrixPointerArray(IEffect::EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount);

		bool SetShaderConstants(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE ConstantBuffer, UINT uIndex);

		bool SetTexture(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE Texture, UINT uIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration);

		bool DrawIndexedTriangles(CRenderSystem::TRIANGLETYPE Type, UINT uStartIndex, UINT uTriangleCount);

		bool Present();

		void SetViewport(UINT uWidth, UINT uHeight, INT nAntiAlias);

		bool AddEventListenerForViewportReset();
	protected:
			CGraphicsResource* _Create(CGraphicsResource::TYPE Type);

			bool _RemoveResource(CGraphicsResource::HANDLE Handle, CGraphicsResource::TYPE Type);
	private:
		void __ClearRenderData(RENDERDATA& RenderData);

		UINT m_uRenderSystemIndex;

		RENDERDATA m_CurrentRenderData;
		RENDERDATA m_OldRenderData;

		FLOAT m_fProgramVertexParameter  [MAXINUM_VERTEX_PARAMETER_COUNT   * VERTEX_PARAMETER_SIZE  ];
		FLOAT m_fProgramFragmentParameter[MAXINUM_FRAGMENT_PARAMETER_COUNT * FRAGMENT_PARAMETER_SIZE];

		UINT16 m_uProgramTexture[MAXINUM_TEXTURE_COUNT];

		UINT8 m_uTextureFlag;

		CGraphicsResource::HANDLE m_VertexBuffer[MAXINUM_VERTEX_INDEX_COUNT];
		UINT8 m_uVertexBufferIndexFlag;

		CRenderSystem::RENDERSTATE m_RenderStateStack[CRenderSystem::TOTAL_RENDER_STATE];
		UINT m_RenderStateValueStack[CRenderSystem::TOTAL_RENDER_STATE];
		UINT m_uRenderStateStackCount;
		UINT m_uRenderStateFlag;

		CVertexDeclaration* m_pVertexDeclaration;

		UINT m_uViewportWidth;
		UINT m_uViewportHeight;
		INT m_nAntiAlias;

		ICompiler* m_pCompiler;

		CViewportResetHandler m_ViewportResetHandler;
	};

	ALCHEMY_INLINE UINT CFlasccContext3D::GetRenderSystemIndex()const
	{
		return m_uRenderSystemIndex;
	}
}