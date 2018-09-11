#include "AlchemyFlasccStdafx.h"
#include "AlchemyFlasccProgram3D.h"
#include "AlchemyFlasccContext3D.h"

using namespace alchemy;

CFlasccProgram3D::CFlasccProgram3D(void)
{
}

CFlasccProgram3D::~CFlasccProgram3D(void)
{
}

bool CFlasccProgram3D::_Create()
{
#ifdef AS3_H
	if(m_pCode && m_pAdditionalCode)
	{
		inline_as3(
			"import alchemy.core.AlchemyGraphicsManager;\n"
			"import alchemy.core.AlchemyRenderSystem;\n"
			"var graphicsManager = AlchemyGraphicsManager.instance;\n"
			"var renderSystem = graphicsManager.getRenderSystem(%0);\n"
			"var vertexCode = graphicsManager.getShaderCode(%1);\n"
			"var fragmentCode = graphicsManager.getShaderCode(%2);\n"
			"if(renderSystem && vertexCode && fragmentCode) renderSystem.createProgram3D(vertexCode, fragmentCode);\n"
			: : "r"( static_cast<CFlasccContext3D&>( GetGraphicsResourcePool() ).GetRenderSystemIndex() ), 
			"r"( *reinterpret_cast<PUINT>( m_pCode->GetBufferPointer() ) ), 
			"r"( *reinterpret_cast<PUINT>( m_pAdditionalCode->GetBufferPointer() ) )
			);

		return true;
	}

	return false;
#endif

	return true;
}