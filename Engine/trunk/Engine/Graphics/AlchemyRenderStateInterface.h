#pragma once
/*
 *  AlchemyRenderStateHandlerInterface.h
 *  IOS
 *
 *  Created by lin fuqing on 12-7-20.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "../Core/AlchemyInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IRenderState)
	{
		typedef enum  
		{
			FILL_MODE = 0,
			
			SHADE_MODE,
			
			DEPTH_WRITEE_ENABLE,
			DEPTH_FUNC,
			
			SRC_BLEND,
			DEST_BLEND,
			ALPHA_REF,
			ALPHA_FUNC,
			
			CULL_MODE,
			
			STENCIL_FAIL,
			STENCIL_Z_FAIL,
			STENCIL_PASS,
			STENCIL_FUNC,
			STENCIL_REF,
			STENCIL_READ_MASK,
			STENCIL_WRITE_MASK,
			CCW_STENCIL_FAIL,
			CCW_STENCIL_Z_FAIL,
			CCW_STENCIL_PASS,
			CCW_STENCIL_FUNC,
			
			COLOR_WRITE_ENABLE,
			
			TOTAL_RENDER_STATE
		}RENDERSTATE;
		
		typedef enum
		{
			POINT = 0,
			WIREFRAME,
			SOLID,
			
			TOTAL_FILL_MODE
		}FILLMODE;
		
		typedef enum
		{
			FLAT = 0,
			GOURAUD,
			PHONG,
			
			TOTAL_SHADE_MODE
		}SHADEMODE;
		
		typedef enum
		{
			CULL_NONE = 0,
			CCW,
			CW,
			
			TOTAL_CULL_MODE
		}CULLMODE;
		
		typedef enum
		{
			NEVER = 0,
			LESS,
			EQUAL,
			LESS_EQUAL,
			GREATER,
			NOT_EQUAL,
			GREATER_EQUAL,
			ALWAYS,
			
			TOTAL_CMP_FUNC
		}CMPFUNC;
		
		typedef enum 
		{
			ZERO = 0,
			ONE,
			SRC_COLOR,
			INV_SRC_COLOR,
			SRC_ALPHA,
			INV_SRC_ALPHA,
			DEST_ALPHA,
			INV_DEST_ALPHA,
			DEST_COLOR,
			INV_DEST_COLOR,
			SRC_ALPHA_SAT,
			
			TOTAL_BLEND_MODE
		}BLENDMODE;
		
		typedef enum
		{
			KEEP = 0,
			THROW,
			REPLACE,
			INCR_SAT,
			DECR_SAT,
			INV_ERT,
			INCR,
			DECR,
			
			TOTAL_STENCIL_OP
		}STENCILOP;
		
		typedef enum
		{
			ALPHA = 0, 
			BLUE, 
			GREEN, 
			RED,
			
			TOTAL_COLOR_WRITE_ENABLE
		}COLORWRITEENABLE;
		
		typedef struct
		{
			RENDERSTATE State;
			UINT uValue;
		}RENDERSTATEINFO, * LPRENDERSTATEINFO;
		
		virtual bool RegisterRenderState() = 0;
		virtual bool RestoreRenderState() = 0;
		
		virtual bool SetRenderState(const RENDERSTATEINFO* pRenderStateInfos, UINT uRenderStateInfoCount, bool bIsRegister) = 0;
	};
	
	ALCHEMY_INLINE bool SetRenderState(IRenderState& RenderState, IRenderState::RENDERSTATE State, UINT uValue, bool bIsRegister = false)
	{
		static IRenderState::RENDERSTATEINFO s_RenderState;
		s_RenderState.State = State;
		s_RenderState.uValue = uValue;
		
		return RenderState.SetRenderState(&s_RenderState, 1, bIsRegister);
	}
}