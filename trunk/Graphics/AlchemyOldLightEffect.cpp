#include "AlchemyOldLightEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTPARAMETER COldLightEffect::CAmbientLightEffect::sm_ATTRIBUTES[COldLightEffect::CAmbientLightEffect::TOTAL_ATTRIBUTES] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0} };
const CEffectCode::ELEMENTPARAMETER COldLightEffect::CAmbientLightEffect::sm_VARYING   [COldLightEffect::CAmbientLightEffect::TOTAL_VARYING   ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} };
const CEffectCode::GLOBALVARIABLE   COldLightEffect::CAmbientLightEffect::sm_CONSTANS  [COldLightEffect::CAmbientLightEffect::TOTAL_CONSTANTS ] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::MATRIX_VARIABLE, CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX, 1, 0, ALCHEMY_NULL}, {CEffectInterface::FRAGMENT_SHADER, CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_AMBIENT, 1, 0, ALCHEMY_NULL} };

const CEffectCode::ELEMENTPARAMETER     COldLightEffect::CDiffusePointLightEffect::sm_ATTRIBUTES[COldLightEffect::CDiffusePointLightEffect::TOTAL_ATTRIBUTES] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0}, {CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL, 0} };
const CEffectCode::ELEMENTPARAMETER     COldLightEffect::CDiffusePointLightEffect::sm_VARYING   [COldLightEffect::CDiffusePointLightEffect::TOTAL_VARYING   ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0}, {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0}, {CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL, 0} };
const CEffectCode::TEMPORARYDECLARATION COldLightEffect::CDiffusePointLightEffect::sm_TEMPORARY [COldLightEffect::CDiffusePointLightEffect::TOTAL_TEMPORARY ] = { {CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, {CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, {CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1} };

const CEffectCode::ELEMENTPARAMETER     COldLightEffect::CBlinnPhongPointLightEffect::sm_ATTRIBUTES[COldLightEffect::CBlinnPhongPointLightEffect::TOTAL_ATTRIBUTES] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0}, {CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL, 0} };
const CEffectCode::ELEMENTPARAMETER     COldLightEffect::CBlinnPhongPointLightEffect::sm_VARYING   [COldLightEffect::CBlinnPhongPointLightEffect::TOTAL_VARYING   ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0}, {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0}, {CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL, 0} };
const CEffectCode::TEMPORARYDECLARATION COldLightEffect::CBlinnPhongPointLightEffect::sm_TEMPORARY [COldLightEffect::CBlinnPhongPointLightEffect::TOTAL_TEMPORARY ] = { {CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, {CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, {CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, {CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1} };

//const CEffectCode::ELEMENTPARAMETER CLightEffect::CMaterialAmbientEffect::sm_ATTRIBUTES[CLightEffect::CMaterialAmbientEffect::TOTAL_ATTRIBUTES] = { {CVertexDeclaration::FLOAT3, CVertexDeclaration::POSITION , 0} };
//const CEffectCode::ELEMENTPARAMETER CLightEffect::CMaterialAmbientEffect::sm_VARYING   [CLightEffect::CMaterialAmbientEffect::TOTAL_VARYING   ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0}, {CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR, 0} };
//const CEffectCode::GLOBALVARIABLE         CLightEffect::CMaterialAmbientEffect::sm_CONSTANS  [CLightEffect::CMaterialAmbientEffect::TOTAL_CONSTANTS ] = { {CEffectCode::MATRIX, CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX, 0}, {CEffectCode::MATERIAL, CRenderMethod::MATERIAL_AMBIENT, 0} };

const CEffectCode::ELEMENTPARAMETER COldLightEffect::CMaterialDiffuseEffect::sm_ATTRIBUTES[COldLightEffect::CMaterialDiffuseEffect::TOTAL_ATTRIBUTES] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0} };
const CEffectCode::ELEMENTPARAMETER COldLightEffect::CMaterialDiffuseEffect::sm_VARYING   [COldLightEffect::CMaterialDiffuseEffect::TOTAL_VARYING   ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} };
const CEffectCode::GLOBALVARIABLE   COldLightEffect::CMaterialDiffuseEffect::sm_CONSTANS  [COldLightEffect::CMaterialDiffuseEffect::TOTAL_CONSTANTS ] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::MATRIX_VARIABLE, CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX, 1, 0, ALCHEMY_NULL}, {CEffectInterface::FRAGMENT_SHADER, CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_DIFFUSE, 1, 0, ALCHEMY_NULL} };

const CEffectCode::ELEMENTPARAMETER COldLightEffect::CMaterialSpecularEffect::sm_ATTRIBUTES[COldLightEffect::CMaterialSpecularEffect::TOTAL_ATTRIBUTES] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0} };
const CEffectCode::ELEMENTPARAMETER COldLightEffect::CMaterialSpecularEffect::sm_VARYING   [COldLightEffect::CMaterialSpecularEffect::TOTAL_VARYING   ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} };
const CEffectCode::GLOBALVARIABLE   COldLightEffect::CMaterialSpecularEffect::sm_CONSTANS  [COldLightEffect::CMaterialSpecularEffect::TOTAL_CONSTANTS ] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::MATRIX_VARIABLE, CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX, 1, 0, ALCHEMY_NULL}, {CEffectInterface::FRAGMENT_SHADER, CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_SPECULAR, 1, 0, ALCHEMY_NULL} };

COldLightEffect::CAbstractLightEffect::CAbstractLightEffect(COldLightEffect& Parent) :
m_Parent(Parent)
{

}

COldLightEffect::CAbstractLightEffect::~CAbstractLightEffect()
{

}

COldLightEffect::CAmbientLightEffect::CAmbientLightEffect(COldLightEffect& Parent) :
CAbstractLightEffect(Parent)
{
}

COldLightEffect::CAmbientLightEffect::~CAmbientLightEffect(void)
{
}

bool COldLightEffect::CAmbientLightEffect::ShaderDefineHandler(
	UINT uOutputColor, 
	const UINT* puSamplers, 
	const UINT* puAttributes, 
	const UINT* puConstants, 
	const CEffectCode::VARYING* pVarying, 
	const UINT* puTemporary,
	const CEffectCode::SHADERVARIABLE* pAttributeVariable,
	const CEffectCode::SHADERVARIABLE* pConstantVariable,
	CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
	CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
	if(!pPositiont->IsUsed)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pPositiont->uVertexShaderHandle, puAttributes[ATTRIBUTE_POSITION], puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);

	if(m_Parent.m_uTemporaryColor)
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uOutputColor, puConstants[CONSTANT_MATERIAL_AMBIENT], m_Parent.m_uTemporaryColor);
	else
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MOV, uOutputColor, puConstants[CONSTANT_MATERIAL_AMBIENT], 0                         );

	return true;
}


COldLightEffect::CDiffusePointLightEffect::CDiffusePointLightEffect(COldLightEffect& Parent, UINT uLightIndex) :
CAbstractLightEffect(Parent),
m_uLightIndex(uLightIndex)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;

	//pConstant = &m_Constants[CONSTANT_WORLD_MATRIX];
	//pConstant->Type    = CEffectCode::MATRIX;
	//pConstant->uDetail = CRenderMethod::WORLD_MATRIX;
	//pConstant->uIndex  = 0;

	pConstant = &m_Constants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX];
	pConstant->ShaderType = CEffectInterface::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::MATRIX_VARIABLE;
	pConstant->uDetail    = CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant = &m_Constants[CONSTANT_LIGHT_LOCAL_POSITION];
	pConstant->ShaderType = CEffectInterface::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::LIGHT_LOCAL_POSITION;
	pConstant->uCount     = 1;
	pConstant->uIndex     = uLightIndex;
	pConstant->pValue     = ALCHEMY_NULL;

	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariable;
	pConstantVariable = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_DIFFUSE];
	pConstantVariable->ShaderType  = CEffectInterface::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_DIFFUSE;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;
}

COldLightEffect::CDiffusePointLightEffect::~CDiffusePointLightEffect(void)
{
}

bool COldLightEffect::CDiffusePointLightEffect::ShaderDefineHandler(
	UINT uOutputColor, 
	const UINT* puSamplers, 
	const UINT* puAttributes, 
	const UINT* puConstants, 
	const CEffectCode::VARYING* pVarying, 
	const UINT* puTemporary,
	const CEffectCode::SHADERVARIABLE* pAttributeVariable,
	const CEffectCode::SHADERVARIABLE* pConstantVariable,
	CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
	CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
	if(!pPositiont->IsUsed)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pPositiont->uVertexShaderHandle, puAttributes[ATTRIBUTE_POSITION], puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);

	const CEffectCode::VARYING* pPosition = &pVarying[VARYING_POSITION];
	if(!pPosition->IsUsed)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pPosition->uVertexShaderHandle, puAttributes[ATTRIBUTE_POSITION], 0);

	UINT uAddress[] = {0, 1, 2};
	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::SUB, puTemporary[TEMPORARY_LIGHT_DIRECTION], uAddress, 3, puConstants[CONSTANT_LIGHT_LOCAL_POSITION], uAddress, 3, pPosition->uFragmentShaderHandle, uAddress, 3);

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::NRM, puTemporary[TEMPORARY_LIGHT_DIRECTION], puTemporary[TEMPORARY_LIGHT_DIRECTION], 0);

	const CEffectCode::VARYING* pNormal = &pVarying[VARYING_NORAML];
	if(!pNormal->IsUsed)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pNormal->uVertexShaderHandle, ALCHEMY_NULL, 0, puAttributes[ATTRIBUTE_NORMAL], ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::DP3, puTemporary[TEMPORARY_INTENSITY], pNormal->uFragmentShaderHandle  , puTemporary[TEMPORARY_LIGHT_DIRECTION]);

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::SAT, puTemporary[TEMPORARY_INTENSITY], puTemporary[TEMPORARY_INTENSITY], 0                                     );

	if( ALCHEMY_TEST_BIT(m_Parent.m_uEffectFlag, SPECULAR_LIGHT) )
		m_Parent.m_uTemporaryLightDirection = puTemporary[TEMPORARY_LIGHT_DIRECTION];

	if(!m_Parent.m_uTemporaryDiffuseColor)
		m_Parent.m_uTemporaryDiffuseColor = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);

	if(m_uLightIndex)
	{
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, puTemporary[TEMPORARY_DIFFUSE_COLOR], pConstantVariable[CONSTANT_VARIABLE_LIGHT_DIFFUSE].uHandle, puTemporary[TEMPORARY_INTENSITY] );

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, m_Parent.m_uTemporaryDiffuseColor   , puTemporary[TEMPORARY_DIFFUSE_COLOR]                       , m_Parent.m_uTemporaryDiffuseColor);
	}
	else
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, m_Parent.m_uTemporaryDiffuseColor   , pConstantVariable[CONSTANT_VARIABLE_LIGHT_DIFFUSE].uHandle, puTemporary[TEMPORARY_INTENSITY] );
	
	//const CEffectCode::VARYING* pColor = &pVarying[VARYING_COLOR];
	//if(!pColor->IsUsed)
	//	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MOV, uOutputColor, ALCHEMY_NULL, 0, pColor->uFragmentShaderHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	return true;
}

COldLightEffect::CBlinnPhongPointLightEffect::CBlinnPhongPointLightEffect(COldLightEffect& Parent, UINT uLightIndex) :
CAbstractLightEffect(Parent),
m_uLightIndex(uLightIndex)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;

	pConstant = &m_Constants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX];
	pConstant->ShaderType = CEffectInterface::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::MATRIX_VARIABLE;
	pConstant->uDetail    = CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant = &m_Constants[CONSTANT_LIGHT_SPECULAR];
	pConstant->ShaderType = CEffectInterface::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::LIGHT_SPECULAR;
	pConstant->uCount     = 1;
	pConstant->uIndex     = uLightIndex;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant = &m_Constants[CONSTANT_LIGHT_LOCAL_POSITION];
	pConstant->ShaderType = CEffectInterface::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::LIGHT_LOCAL_POSITION;
	pConstant->uCount     = 1;
	pConstant->uIndex     = uLightIndex;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant = &m_Constants[CONSTANT_MATERIAL_POWER];
	pConstant->ShaderType = CEffectInterface::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::MATERIAL_VARIABLE;
	pConstant->uDetail    = CRenderMethod::MATERIAL_POWER;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant = &m_Constants[CONSTANT_CAMERA_LOCAL_DIRECTION_NEGATE];
	pConstant->ShaderType = CEffectInterface::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;
}

COldLightEffect::CBlinnPhongPointLightEffect::~CBlinnPhongPointLightEffect(void)
{
}

bool COldLightEffect::CBlinnPhongPointLightEffect::ShaderDefineHandler(
	UINT uOutputColor, 
	const UINT* puSamplers, 
	const UINT* puAttributes, 
	const UINT* puConstants, 
	const CEffectCode::VARYING* pVarying, 
	const UINT* puTemporary,
	const CEffectCode::SHADERVARIABLE* pAttributeVariable,
	const CEffectCode::SHADERVARIABLE* pConstantVariable,
	CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
	CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
	if(!pPositiont->IsUsed)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pPositiont->uVertexShaderHandle, ALCHEMY_NULL, 0, puAttributes[ATTRIBUTE_POSITION], ALCHEMY_NULL, 0, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX], ALCHEMY_NULL, 0);

	UINT uLightDirection;
	if(!m_Parent.m_uTemporaryLightDirection)
	{
		const CEffectCode::VARYING* pPosition = &pVarying[VARYING_POSITION];
		if(!pPosition->IsUsed)
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pPosition->uVertexShaderHandle, puAttributes[ATTRIBUTE_POSITION], 0);

		UINT uAddress[] = {0, 1, 2};
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::SUB, puTemporary[TEMPORARY_LIGHT_DIRECTION], uAddress, 3, puConstants[CONSTANT_LIGHT_LOCAL_POSITION], uAddress, 3, pPosition->uFragmentShaderHandle, uAddress, 3);

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::NRM, puTemporary[TEMPORARY_LIGHT_DIRECTION], puTemporary[TEMPORARY_LIGHT_DIRECTION], 0);

		uLightDirection = puTemporary[TEMPORARY_LIGHT_DIRECTION];
	}
	else
		uLightDirection = m_Parent.m_uTemporaryLightDirection;

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, puTemporary[TEMPORARY_HALF_ANGLE_VECTOR], uLightDirection, puConstants[CONSTANT_CAMERA_LOCAL_DIRECTION_NEGATE]);

	const CEffectCode::VARYING* pNormal = &pVarying[VARYING_NORAML];
	if(!pNormal->IsUsed)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pNormal->uVertexShaderHandle, puAttributes[ATTRIBUTE_NORMAL], 0);

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::DP3, puTemporary[TEMPORARY_INTENSITY], pNormal->uFragmentShaderHandle  , puTemporary[TEMPORARY_HALF_ANGLE_VECTOR]);

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::SAT, puTemporary[TEMPORARY_INTENSITY], puTemporary[TEMPORARY_INTENSITY], 0                                       );

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::POW, puTemporary[TEMPORARY_INTENSITY], puTemporary[TEMPORARY_INTENSITY], puConstants[CONSTANT_MATERIAL_POWER    ]);

	if(!m_Parent.m_uTemporarySpecularColor)
		m_Parent.m_uTemporarySpecularColor = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);

	if(m_uLightIndex)
	{
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, puTemporary[TEMPORARY_SPECULAR_COLOR], puConstants[CONSTANT_LIGHT_SPECULAR] , puTemporary[TEMPORARY_INTENSITY]  );

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, m_Parent.m_uTemporarySpecularColor   , puTemporary[TEMPORARY_SPECULAR_COLOR], m_Parent.m_uTemporarySpecularColor);
	}
	else
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, m_Parent.m_uTemporarySpecularColor   , puConstants[CONSTANT_LIGHT_SPECULAR] , puTemporary[TEMPORARY_INTENSITY]  );

	return true;
}

//CLightEffect::CMaterialAmbientEffect::CMaterialAmbientEffect(CLightEffect& Parent) :
//CAbstractLightEffect(Parent)
//{
//}
//
//CLightEffect::CMaterialAmbientEffect::~CMaterialAmbientEffect(void)
//{
//}
//
//bool CLightEffect::CMaterialAmbientEffect::ShaderDefineHandler(
//	UINT uOutputColor, 
//	const UINT* puSamplers, 
//	const UINT* puAttributes, 
//	const UINT* puConstants, 
//	const CEffectCode::VARYING* pVarying, 
//	const UINT* puTemporary,
//	CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
//	CEffectCode::CInstructionSetInterface& FragmentShaderDefiner)
//{
//	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
//	if(!pPositiont->IsUsed)
//	{
//		UINT uAddress[4] = {0, 1, 2, 3};
//		VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pPositiont->uVertexShaderHandle, ALCHEMY_NULL, 0, puAttributes[ATTRIBUTE_POSITION], uAddress, 4, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX], ALCHEMY_NULL, 0);
//	}
//
//	const CEffectCode::VARYING* pColor = &pVarying[VARYING_COLOR];
//
//	if( m_Parent.m_uLightCount && ( ALCHEMY_TEST_BIT(m_Parent.m_uEffectFlag, DIFFUSE_LIGHT) || ALCHEMY_TEST_BIT(m_Parent.m_uEffectFlag, SPECULAR_LIGHT) ) )
//	{
//		if(!m_Parent.m_uTemporaryColor)
//			m_Parent.m_uTemporaryColor = VertexShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE);
//
//		if(pColor->IsUsed)
//			VertexShaderDefiner.AppendInstruction(CCompilerInterface::MUL, m_Parent.m_uTemporaryColor , ALCHEMY_NULL, 0, puConstants[CONSTANT_MATERIAL_AMBIENT], ALCHEMY_NULL, 0, pColor->uVertexShaderHandle, ALCHEMY_NULL, 0);
//	}
//	else if(pColor->IsUsed)
//		VertexShaderDefiner.AppendInstruction(CCompilerInterface::MUL, pColor->uVertexShaderHandle, ALCHEMY_NULL, 0, puConstants[CONSTANT_MATERIAL_AMBIENT], ALCHEMY_NULL, 0, pColor->uVertexShaderHandle, ALCHEMY_NULL, 0);
//
//	if(!pColor->IsUsed)
//		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MOV, uOutputColor, ALCHEMY_NULL, 0, pColor->uFragmentShaderHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);
//
//	return true;
//}
//

COldLightEffect::CMaterialDiffuseEffect::CMaterialDiffuseEffect(COldLightEffect& Parent) :
CAbstractLightEffect(Parent)
{
}

COldLightEffect::CMaterialDiffuseEffect::~CMaterialDiffuseEffect(void)
{
}

bool COldLightEffect::CMaterialDiffuseEffect::ShaderDefineHandler(
	UINT uOutputColor, 
	const UINT* puSamplers, 
	const UINT* puAttributes, 
	const UINT* puConstants, 
	const CEffectCode::VARYING* pVarying, 
	const UINT* puTemporary,
	const CEffectCode::SHADERVARIABLE* pAttributeVariable,
	const CEffectCode::SHADERVARIABLE* pConstantVariable,
	CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
	CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
	if(!pPositiont->IsUsed)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pPositiont->uVertexShaderHandle, puAttributes[ATTRIBUTE_POSITION], puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);

	if(m_Parent.m_uTemporaryDiffuseColor)
	{
		if( ALCHEMY_TEST_BIT(m_Parent.m_uEffectFlag, SPECULAR_LIGHT) || ALCHEMY_TEST_BIT(m_Parent.m_uEffectFlag, AMBIENT_LIGHT) )
		{
			if( !m_Parent.m_uTemporaryColor)
				m_Parent.m_uTemporaryColor = m_Parent.m_uTemporaryDiffuseColor;

			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, m_Parent.m_uTemporaryColor, puConstants[CONSTANT_MATERIAL_DIFFUSE], m_Parent.m_uTemporaryDiffuseColor);
		}
		else
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uOutputColor              , puConstants[CONSTANT_MATERIAL_DIFFUSE], m_Parent.m_uTemporaryDiffuseColor);
	}

	return true;
}

COldLightEffect::CMaterialSpecularEffect::CMaterialSpecularEffect(COldLightEffect& Parent) :
CAbstractLightEffect(Parent)
{
}

COldLightEffect::CMaterialSpecularEffect::~CMaterialSpecularEffect(void)
{
}

bool COldLightEffect::CMaterialSpecularEffect::ShaderDefineHandler(
	UINT uOutputColor, 
	const UINT* puSamplers, 
	const UINT* puAttributes, 
	const UINT* puConstants, 
	const CEffectCode::VARYING* pVarying, 
	const UINT* puTemporary,
	const CEffectCode::SHADERVARIABLE* pAttributeVariable,
	const CEffectCode::SHADERVARIABLE* pConstantVariable,
	CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
	CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
	if(!pPositiont->IsUsed)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pPositiont->uVertexShaderHandle, puAttributes[ATTRIBUTE_POSITION], puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);

	if(m_Parent.m_uTemporarySpecularColor)
	{
		UINT uDestinationColor = uOutputColor;
		if( ALCHEMY_TEST_BIT(m_Parent.m_uEffectFlag, AMBIENT_LIGHT) )
			uDestinationColor = m_Parent.m_uTemporaryColor ? m_Parent.m_uTemporaryColor : m_Parent.m_uTemporarySpecularColor;

		if(m_Parent.m_uTemporaryColor)
		{
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, m_Parent.m_uTemporarySpecularColor, puConstants[CONSTANT_MATERIAL_SPECULAR], m_Parent.m_uTemporarySpecularColor);

			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uDestinationColor, m_Parent.m_uTemporaryColor             , m_Parent.m_uTemporarySpecularColor);
		}
		else
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uDestinationColor, puConstants[CONSTANT_MATERIAL_SPECULAR], m_Parent.m_uTemporarySpecularColor);

		m_Parent.m_uTemporaryColor = uDestinationColor;
	}

	return true;
}

COldLightEffect::COldLightEffect(void) :
m_AmbientLight(*this),
//m_MaterialAmbient(*this),
m_MaterialDiffuse(*this),
m_MaterialSpecular(*this),
m_uEffectFlag(0xffffffff),
m_uLightCount(0)
{
}

COldLightEffect::~COldLightEffect(void)
{
	CVector<CAbstractLightEffect*>::CIterator Iterator;
	Iterator = m_DiffuseLights.GetBeginIterator();
	while( CVector<CAbstractLightEffect*>::IsValuable(Iterator) )
	{
		ALCHEMY_DEBUG_DELETE(*Iterator);

		++ Iterator;
	}

	Iterator = m_SpecularLights.GetBeginIterator();
	while( CVector<CAbstractLightEffect*>::IsValuable(Iterator) )
	{
		ALCHEMY_DEBUG_DELETE(*Iterator);

		++ Iterator;
	}
}

void COldLightEffect::AddLight(CLight::TYPE LightType, SHADEMODE LightShadeMode)
{
	CAbstractLightEffect* pLight;
	switch(LightType)
	{
	case CLight::POINT:
		ALCHEMY_DEBUG_NEW( pLight, CDiffusePointLightEffect(*this, m_uLightCount) );
		m_DiffuseLights.Add(pLight);

		switch(LightShadeMode)
		{
		case BLINN_PHONG:
			ALCHEMY_DEBUG_NEW( pLight, CBlinnPhongPointLightEffect(*this, m_uLightCount) );
			m_SpecularLights.Add(pLight);
			break;
		}

		break;
	}

	++ m_uLightCount;
}

bool COldLightEffect::SetLight(CLight::TYPE LightType, SHADEMODE LightShadeMode, UINT uIndex)
{
	CAbstractLightEffect* pLight;
	if(uIndex < m_uLightCount)
	{
		++ m_uLightCount;
		switch(LightType)
		{
		case CLight::POINT:
			ALCHEMY_DEBUG_NEW( pLight, CDiffusePointLightEffect(*this, m_uLightCount ++) );
			if( m_DiffuseLights.Set(pLight, uIndex) )
			{
				switch(LightShadeMode)
				{
				case BLINN_PHONG:
					ALCHEMY_DEBUG_NEW( pLight, CBlinnPhongPointLightEffect(*this, m_uLightCount) );
					return m_SpecularLights.Set(pLight, uIndex);
					break;
				}

			}
			break;
		}

		return false;
	}

	ALCHEMY_DEBUG_WARNING("CLightEffect::SetLight: Index out of range.");

	return false;
}

void COldLightEffect::BuildLightEffectPass(CEffectCode::CPassDefiner& PassDefiner)
{
	m_uTemporaryColor = 0;

	if( ALCHEMY_TEST_BIT(m_uEffectFlag, DIFFUSE_LIGHT) || ALCHEMY_TEST_BIT(m_uEffectFlag, SPECULAR_LIGHT) )
	{
		m_uTemporaryDiffuseColor  = 0;
		m_uTemporarySpecularColor = 0;

		CAbstractLightEffect** ppLightEffect = ALCHEMY_NULL;
		for(UINT i = 0; i < m_uLightCount; ++ i)
		{
			m_uTemporaryLightDirection = 0;

			if( ALCHEMY_TEST_BIT(m_uEffectFlag, DIFFUSE_LIGHT) )
			{
				ppLightEffect = m_DiffuseLights.Get(i);
				if(ppLightEffect && *ppLightEffect)
					PassDefiner.AddInfo(**ppLightEffect, ALCHEMY_NULL);
			}

			if( ALCHEMY_TEST_BIT(m_uEffectFlag, SPECULAR_LIGHT) )
			{
				ppLightEffect = m_SpecularLights.Get(i);
				if(ppLightEffect && *ppLightEffect)
					PassDefiner.AddInfo(**ppLightEffect, ALCHEMY_NULL);
			}
		}

		if( ALCHEMY_TEST_BIT(m_uEffectFlag, DIFFUSE_LIGHT) )
			PassDefiner.AddInfo(m_MaterialDiffuse, ALCHEMY_NULL);

		if( ALCHEMY_TEST_BIT(m_uEffectFlag, SPECULAR_LIGHT) )
			PassDefiner.AddInfo(m_MaterialSpecular, ALCHEMY_NULL);
	}

	if( ALCHEMY_TEST_BIT(m_uEffectFlag, AMBIENT_LIGHT) )
	{
		PassDefiner.AddInfo(m_AmbientLight, ALCHEMY_NULL);

		//PassDefiner.AddInfo(m_MaterialAmbient, ALCHEMY_NULL);
	}
}