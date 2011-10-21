#pragma once

#include "AlchemyEffectInfoInterface.h"
#include "AlchemyLight.h"

//////////////////////////////////////////////////////////////////////////
//CDiffuseDirectionLightEffect
//CDiffuseSpotLightEffect
//CPhongPointLightEffect
//CPhongDirectionLightEffect
//CPhongSpotLightEffect
//CBlinnPhongDirectionLightEffect
//CBlinnPhongSpotLightEffect
//////////////////////////////////////////////////////////////////////////
namespace alchemy
{
	class COldLightEffect
	{
	public:
		typedef enum
		{
			AMBIENT_LIGHT = 0,
			DIFFUSE_LIGHT,
			SPECULAR_LIGHT
		}EFFECT;

		typedef enum
		{
			PHONG,
			BLINN_PHONG
		}SHADEMODE;

		class CAbstractLightEffect :
			public CEffectInfoInterface
		{
			friend class COldLightEffect;
		public:
			CAbstractLightEffect(COldLightEffect& Parent);
			~CAbstractLightEffect();

		protected:
			COldLightEffect& m_Parent;
		};

		class CAmbientLightEffect :
			public CAbstractLightEffect
		{
			typedef enum
			{
				TOTAL_SAMPLERS = 0
			}SAMPLERINDEX;

			typedef enum
			{
				ATTRIBUTE_POSITION = 0,

				TOTAL_ATTRIBUTES
			}ATTRIBUTEINDEX;

			typedef enum
			{
				VARYING_POSITIONT = 0,

				TOTAL_VARYING
			}VARYINGINDEX;

			typedef enum
			{
				CONSTANT_WORLD_VIEW_PROJECTION_MATRIX = 0,
				CONSTANT_MATERIAL_AMBIENT,

				TOTAL_CONSTANTS
			}CONSTANSINDEX;

			typedef enum
			{
				TOTAL_TEMPORARY = 0
			}TEMPORARYINDEX;

			typedef enum
			{
				TOTAL_ATTRIBUTE_VARIABLES = 0
			}ATTRIBUTEVARIABLEINDEX;

			typedef enum
			{
				TOTAL_CONSTANT_VARIABLES = 0
			}CONSTANTVARIABLEINDEX;
		public:
			CAmbientLightEffect(COldLightEffect& Parent);
			~CAmbientLightEffect(void);

			UINT                                             GetEffectInfoFlag    (                             );

			const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          );

			const CEffectCode::ELEMENTPARAMETER            * GetAttributes        (UINT& uAttributeCount        );
			const CEffectCode::ELEMENTPARAMETER            * GetVarying           (UINT& uVaryingCount          );

			const CEffectCode::GLOBALVARIABLE              * GetConstants         (UINT& uConstantCount         );

			const CEffectCode::TEMPORARYDECLARATION        * GetTemporary         (UINT& uTemporaryCount        );

			const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* GetAttributeVariables(UINT& uAttributeVariableCount);

			const CEffectCode::CONSTANTVARIABLEDECLARATION * GetConstantVariables (UINT& uConstantVariableCount );

			bool ShaderDefineHandler(
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
				CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

		private:
			static const CEffectCode::ELEMENTPARAMETER sm_ATTRIBUTES[TOTAL_ATTRIBUTES];
			static const CEffectCode::ELEMENTPARAMETER sm_VARYING   [TOTAL_VARYING   ];

			static const CEffectCode::GLOBALVARIABLE   sm_CONSTANS  [TOTAL_CONSTANTS ];
		};

		class CDiffusePointLightEffect :
			public CAbstractLightEffect
		{
			typedef enum
			{
				TOTAL_SAMPLERS = 0
			}SAMPLERINDEX;

			typedef enum
			{
				ATTRIBUTE_POSITION = 0,
				ATTRIBUTE_NORMAL,

				TOTAL_ATTRIBUTES
			}ATTRIBUTEINDEX;

			typedef enum
			{
				VARYING_POSITIONT = 0,
				VARYING_POSITION,
				VARYING_NORAML,
				//VARYING_COLOR,

				TOTAL_VARYING
			}VARYINGINDEX;

			typedef enum
			{
				CONSTANT_WORLD_VIEW_PROJECTION_MATRIX = 0,

				CONSTANT_LIGHT_LOCAL_POSITION,

				TOTAL_CONSTANTS
			}CONSTANSINDEX;

			typedef enum
			{
				TEMPORARY_LIGHT_DIRECTION = 0,

				TEMPORARY_DIFFUSE_COLOR,

				TEMPORARY_INTENSITY,

				TOTAL_TEMPORARY
			}TEMPORARYINDEX;

			typedef enum
			{
				TOTAL_ATTRIBUTE_VARIABLES = 0
			}ATTRIBUTEVARIABLEINDEX;

			typedef enum
			{
				CONSTANT_VARIABLE_LIGHT_DIFFUSE = 0,

				TOTAL_CONSTANT_VARIABLES
			}CONSTANTVARIABLEINDEX;
		public:
			CDiffusePointLightEffect(COldLightEffect& Parent, UINT uLightIndex);
			~CDiffusePointLightEffect(void);

			UINT                                             GetEffectInfoFlag    (                             );

			const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          );

			const CEffectCode::ELEMENTPARAMETER            * GetAttributes        (UINT& uAttributeCount        );
			const CEffectCode::ELEMENTPARAMETER            * GetVarying           (UINT& uVaryingCount          );

			const CEffectCode::GLOBALVARIABLE              * GetConstants         (UINT& uConstantCount         );

			const CEffectCode::TEMPORARYDECLARATION        * GetTemporary         (UINT& uTemporaryCount        );

			const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* GetAttributeVariables(UINT& uAttributeVariableCount);

			const CEffectCode::CONSTANTVARIABLEDECLARATION * GetConstantVariables (UINT& uConstantVariableCount );

			bool ShaderDefineHandler(
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
				CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

		private:
			static const CEffectCode::ELEMENTPARAMETER           sm_ATTRIBUTES       [TOTAL_ATTRIBUTES];
			static const CEffectCode::ELEMENTPARAMETER           sm_VARYING          [TOTAL_VARYING   ];
			static const CEffectCode::TEMPORARYDECLARATION       sm_TEMPORARY        [TOTAL_TEMPORARY ];

			             CEffectCode::GLOBALVARIABLE              m_Constants        [TOTAL_CONSTANTS ];
						 CEffectCode::CONSTANTVARIABLEDECLARATION m_ConstantVariables[TOTAL_CONSTANT_VARIABLES];


		    UINT m_uLightIndex;
		};

		class CBlinnPhongPointLightEffect :
			public CAbstractLightEffect
		{
			typedef enum
			{
				TOTAL_SAMPLERS = 0
			}SAMPLERINDEX;

			typedef enum
			{
				ATTRIBUTE_POSITION = 0,
				ATTRIBUTE_NORMAL,

				TOTAL_ATTRIBUTES
			}ATTRIBUTEINDEX;

			typedef enum
			{
				VARYING_POSITIONT = 0,
				VARYING_POSITION,
				VARYING_NORAML,

				TOTAL_VARYING
			}VARYINGINDEX;

			typedef enum
			{
				CONSTANT_WORLD_VIEW_PROJECTION_MATRIX = 0,

				CONSTANT_LIGHT_SPECULAR,
				CONSTANT_LIGHT_LOCAL_POSITION,

				CONSTANT_MATERIAL_POWER,

				CONSTANT_CAMERA_LOCAL_DIRECTION_NEGATE,

				TOTAL_CONSTANTS
			}CONSTANSINDEX;

			typedef enum
			{
				TEMPORARY_LIGHT_DIRECTION = 0,

				TEMPORARY_HALF_ANGLE_VECTOR,

				TEMPORARY_SPECULAR_COLOR,

				TEMPORARY_INTENSITY,

				TOTAL_TEMPORARY
			}TEMPORARYINDEX;

			typedef enum
			{
				TOTAL_ATTRIBUTE_VARIABLES = 0
			}ATTRIBUTEVARIABLEINDEX;

			typedef enum
			{
				TOTAL_CONSTANT_VARIABLES = 0
			}CONSTANTVARIABLEINDEX;
		public:
			CBlinnPhongPointLightEffect(COldLightEffect& Parent, UINT uLightIndex);
			~CBlinnPhongPointLightEffect(void);

			UINT                                             GetEffectInfoFlag    (                             );

			const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          );

			const CEffectCode::ELEMENTPARAMETER            * GetAttributes        (UINT& uAttributeCount        );
			const CEffectCode::ELEMENTPARAMETER            * GetVarying           (UINT& uVaryingCount          );

			const CEffectCode::GLOBALVARIABLE              * GetConstants         (UINT& uConstantCount         );

			const CEffectCode::TEMPORARYDECLARATION        * GetTemporary         (UINT& uTemporaryCount        );

			const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* GetAttributeVariables(UINT& uAttributeVariableCount);

			const CEffectCode::CONSTANTVARIABLEDECLARATION * GetConstantVariables (UINT& uConstantVariableCount );

			bool ShaderDefineHandler(
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
				CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

		private:
			static const CEffectCode::ELEMENTPARAMETER     sm_ATTRIBUTES[TOTAL_ATTRIBUTES];
			static const CEffectCode::ELEMENTPARAMETER     sm_VARYING   [TOTAL_VARYING   ];
			static const CEffectCode::TEMPORARYDECLARATION sm_TEMPORARY [TOTAL_TEMPORARY ];

			CEffectCode::GLOBALVARIABLE                    m_Constants  [TOTAL_CONSTANTS ];

			UINT                                           m_uLightIndex;
		};

		class CMaterialDiffuseEffect :
			public CAbstractLightEffect
		{
			typedef enum
			{
				TOTAL_SAMPLERS = 0
			}SAMPLERINDEX;

			typedef enum
			{
				ATTRIBUTE_POSITION = 0,

				TOTAL_ATTRIBUTES
			}ATTRIBUTEINDEX;

			typedef enum
			{
				VARYING_POSITIONT = 0,

				TOTAL_VARYING
			}VARYINGINDEX;

			typedef enum
			{
				CONSTANT_WORLD_VIEW_PROJECTION_MATRIX = 0,
				CONSTANT_MATERIAL_DIFFUSE,

				TOTAL_CONSTANTS
			}CONSTANSINDEX;

			typedef enum
			{
				TOTAL_TEMPORARY = 0
			}TEMPORARYINDEX;

			typedef enum
			{
				TOTAL_ATTRIBUTE_VARIABLES = 0
			}ATTRIBUTEVARIABLEINDEX;

			typedef enum
			{
				TOTAL_CONSTANT_VARIABLES = 0
			}CONSTANTVARIABLEINDEX;
		public:
			CMaterialDiffuseEffect(COldLightEffect& Parent);
			~CMaterialDiffuseEffect(void);

			UINT                                             GetEffectInfoFlag    (                             );

			const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          );

			const CEffectCode::ELEMENTPARAMETER            * GetAttributes        (UINT& uAttributeCount        );
			const CEffectCode::ELEMENTPARAMETER            * GetVarying           (UINT& uVaryingCount          );

			const CEffectCode::GLOBALVARIABLE              * GetConstants         (UINT& uConstantCount         );

			const CEffectCode::TEMPORARYDECLARATION        * GetTemporary         (UINT& uTemporaryCount        );

			const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* GetAttributeVariables(UINT& uAttributeVariableCount);

			const CEffectCode::CONSTANTVARIABLEDECLARATION * GetConstantVariables (UINT& uConstantVariableCount );

			bool ShaderDefineHandler(
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
				CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

		private:
			static const CEffectCode::ELEMENTPARAMETER sm_ATTRIBUTES[TOTAL_ATTRIBUTES];
			static const CEffectCode::ELEMENTPARAMETER sm_VARYING   [TOTAL_VARYING   ];

			static const CEffectCode::GLOBALVARIABLE   sm_CONSTANS  [TOTAL_CONSTANTS ];
		};

		class CMaterialSpecularEffect :
			public CAbstractLightEffect
		{
			typedef enum
			{
				TOTAL_SAMPLERS = 0
			}SAMPLERINDEX;

			typedef enum
			{
				ATTRIBUTE_POSITION = 0,

				TOTAL_ATTRIBUTES
			}ATTRIBUTEINDEX;

			typedef enum
			{
				VARYING_POSITIONT = 0,

				TOTAL_VARYING
			}VARYINGINDEX;

			typedef enum
			{
				CONSTANT_WORLD_VIEW_PROJECTION_MATRIX = 0,
				CONSTANT_MATERIAL_SPECULAR,

				TOTAL_CONSTANTS
			}CONSTANSINDEX;

			typedef enum
			{
				TOTAL_TEMPORARY = 0
			}TEMPORARYINDEX;

			typedef enum
			{
				TOTAL_ATTRIBUTE_VARIABLES = 0
			}ATTRIBUTEVARIABLEINDEX;

			typedef enum
			{
				TOTAL_CONSTANT_VARIABLES = 0
			}CONSTANTVARIABLEINDEX;
		public:
			CMaterialSpecularEffect(COldLightEffect& Parent);
			~CMaterialSpecularEffect(void);

			UINT                                             GetEffectInfoFlag    (                             );

			const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          );

			const CEffectCode::ELEMENTPARAMETER            * GetAttributes        (UINT& uAttributeCount        );
			const CEffectCode::ELEMENTPARAMETER            * GetVarying           (UINT& uVaryingCount          );

			const CEffectCode::GLOBALVARIABLE              * GetConstants         (UINT& uConstantCount         );

			const CEffectCode::TEMPORARYDECLARATION        * GetTemporary         (UINT& uTemporaryCount        );

			const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* GetAttributeVariables(UINT& uAttributeVariableCount);

			const CEffectCode::CONSTANTVARIABLEDECLARATION * GetConstantVariables (UINT& uConstantVariableCount );

			bool ShaderDefineHandler(
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
				CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

		private:
			static const CEffectCode::ELEMENTPARAMETER sm_ATTRIBUTES[TOTAL_ATTRIBUTES];
			static const CEffectCode::ELEMENTPARAMETER sm_VARYING   [TOTAL_VARYING   ];

			static const CEffectCode::GLOBALVARIABLE   sm_CONSTANS  [TOTAL_CONSTANTS ];
		};

		COldLightEffect(void);
		~COldLightEffect(void);

		void EnableEffect(UINT uEffectFlag);
		UINT GetEffectFlag()const;

		void AddLight(CLight::TYPE LightType, SHADEMODE LightShadeMode);
		bool SetLight(CLight::TYPE LightType, SHADEMODE LightShadeMode, UINT uIndex);

		void BuildLightEffectPass(CEffectCode::CPassDefiner& PassDefiner);
	private:
		CAmbientLightEffect m_AmbientLight;

		//CMaterialAmbientEffect m_MaterialAmbient;
		CMaterialDiffuseEffect m_MaterialDiffuse;
		CMaterialSpecularEffect m_MaterialSpecular;

		CVector<CAbstractLightEffect*> m_DiffuseLights;
		CVector<CAbstractLightEffect*> m_SpecularLights;

		UINT m_uEffectFlag;

		UINT m_uLightCount;

		UINT m_uTemporaryDiffuseColor;
		UINT m_uTemporarySpecularColor;
		UINT m_uTemporaryLightDirection;
		UINT m_uTemporaryColor;
	};

	//////////////////////////////////////////////////////////////////////////
	//Ambient Light
	//////////////////////////////////////////////////////////////////////////
	ALCHEMY_INLINE UINT                                       COldLightEffect::CAmbientLightEffect::GetEffectInfoFlag     (                             )
	{
		return ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT);
	}

	ALCHEMY_INLINE const CEffectInfoInterface::SAMPLER      * COldLightEffect::CAmbientLightEffect::GetSamplers  (UINT& uSamplerCount  )
	{
		uSamplerCount = TOTAL_SAMPLERS;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* COldLightEffect::CAmbientLightEffect::GetAttributes(UINT& uAttributeCount)
	{
		uAttributeCount = TOTAL_ATTRIBUTES;

		return sm_ATTRIBUTES;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* COldLightEffect::CAmbientLightEffect::GetVarying   (UINT& uVaryingCount  )
	{
		uVaryingCount = TOTAL_VARYING;

		return sm_VARYING;
	}

	ALCHEMY_INLINE const CEffectCode::GLOBALVARIABLE        * COldLightEffect::CAmbientLightEffect::GetConstants (UINT& uConstantCount )
	{
		uConstantCount = TOTAL_CONSTANTS;

		return sm_CONSTANS;
	}

	ALCHEMY_INLINE const CEffectCode::TEMPORARYDECLARATION          * COldLightEffect::CAmbientLightEffect::GetTemporary (UINT& uTemporaryCount)
	{
		uTemporaryCount = TOTAL_TEMPORARY;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* COldLightEffect::CAmbientLightEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
	{
		uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::CONSTANTVARIABLEDECLARATION * COldLightEffect::CAmbientLightEffect::GetConstantVariables  (UINT& uConstantVariableCount)
	{
		uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

		return ALCHEMY_NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	//Diffuse Point Light
	//////////////////////////////////////////////////////////////////////////
	ALCHEMY_INLINE UINT                                       COldLightEffect::CDiffusePointLightEffect::GetEffectInfoFlag     (                             )
	{
		return ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT);
	}

	ALCHEMY_INLINE const CEffectInfoInterface::SAMPLER      * COldLightEffect::CDiffusePointLightEffect::GetSamplers  (UINT& uSamplerCount  )
	{
		uSamplerCount = TOTAL_SAMPLERS;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* COldLightEffect::CDiffusePointLightEffect::GetAttributes(UINT& uAttributeCount)
	{
		uAttributeCount = TOTAL_ATTRIBUTES;

		return sm_ATTRIBUTES;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* COldLightEffect::CDiffusePointLightEffect::GetVarying   (UINT& uVaryingCount  )
	{
		uVaryingCount = TOTAL_VARYING;

		return sm_VARYING;
	}

	ALCHEMY_INLINE const CEffectCode::GLOBALVARIABLE        * COldLightEffect::CDiffusePointLightEffect::GetConstants (UINT& uConstantCount )
	{
		uConstantCount = TOTAL_CONSTANTS;

		return m_Constants;
	}

	ALCHEMY_INLINE const CEffectCode::TEMPORARYDECLARATION          * COldLightEffect::CDiffusePointLightEffect::GetTemporary (UINT& uTemporaryCount)
	{
		uTemporaryCount = TOTAL_TEMPORARY;

		return sm_TEMPORARY;
	}

	ALCHEMY_INLINE const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* COldLightEffect::CDiffusePointLightEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
	{
		uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::CONSTANTVARIABLEDECLARATION * COldLightEffect::CDiffusePointLightEffect::GetConstantVariables  (UINT& uConstantVariableCount)
	{
		uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

		return m_ConstantVariables;
	}

	//////////////////////////////////////////////////////////////////////////
	//Blinn-Phong Point Light Effect
	//////////////////////////////////////////////////////////////////////////
	ALCHEMY_INLINE UINT                                       COldLightEffect::CBlinnPhongPointLightEffect::GetEffectInfoFlag     (                             )
	{
		return ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT);
	}

	ALCHEMY_INLINE const CEffectInfoInterface::SAMPLER      * COldLightEffect::CBlinnPhongPointLightEffect::GetSamplers  (UINT& uSamplerCount  )
	{
		uSamplerCount = TOTAL_SAMPLERS;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* COldLightEffect::CBlinnPhongPointLightEffect::GetAttributes(UINT& uAttributeCount)
	{
		uAttributeCount = TOTAL_ATTRIBUTES;

		return sm_ATTRIBUTES;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* COldLightEffect::CBlinnPhongPointLightEffect::GetVarying   (UINT& uVaryingCount  )
	{
		uVaryingCount = TOTAL_VARYING;

		return sm_VARYING;
	}

	ALCHEMY_INLINE const CEffectCode::GLOBALVARIABLE        * COldLightEffect::CBlinnPhongPointLightEffect::GetConstants (UINT& uConstantCount )
	{
		uConstantCount = TOTAL_CONSTANTS;

		return m_Constants;
	}

	ALCHEMY_INLINE const CEffectCode::TEMPORARYDECLARATION          * COldLightEffect::CBlinnPhongPointLightEffect::GetTemporary (UINT& uTemporaryCount)
	{
		uTemporaryCount = TOTAL_TEMPORARY;

		return sm_TEMPORARY;
	}

	ALCHEMY_INLINE const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* COldLightEffect::CBlinnPhongPointLightEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
	{
		uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::CONSTANTVARIABLEDECLARATION * COldLightEffect::CBlinnPhongPointLightEffect::GetConstantVariables  (UINT& uConstantVariableCount)
	{
		uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

		return ALCHEMY_NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	//Diffuse Material
	//////////////////////////////////////////////////////////////////////////
	ALCHEMY_INLINE UINT                                       COldLightEffect::CMaterialDiffuseEffect::GetEffectInfoFlag     (                             )
	{
		return ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT);
	}

	ALCHEMY_INLINE const CEffectInfoInterface::SAMPLER      * COldLightEffect::CMaterialDiffuseEffect::GetSamplers  (UINT& uSamplerCount  )
	{
		uSamplerCount = TOTAL_SAMPLERS;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* COldLightEffect::CMaterialDiffuseEffect::GetAttributes(UINT& uAttributeCount)
	{
		uAttributeCount = TOTAL_ATTRIBUTES;

		return sm_ATTRIBUTES;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* COldLightEffect::CMaterialDiffuseEffect::GetVarying   (UINT& uVaryingCount  )
	{
		uVaryingCount = TOTAL_VARYING;

		return sm_VARYING;
	}

	ALCHEMY_INLINE const CEffectCode::GLOBALVARIABLE        * COldLightEffect::CMaterialDiffuseEffect::GetConstants (UINT& uConstantCount )
	{
		uConstantCount = TOTAL_CONSTANTS;

		return sm_CONSTANS;
	}

	ALCHEMY_INLINE const CEffectCode::TEMPORARYDECLARATION          * COldLightEffect::CMaterialDiffuseEffect::GetTemporary (UINT& uTemporaryCount)
	{
		uTemporaryCount = TOTAL_TEMPORARY;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* COldLightEffect::CMaterialDiffuseEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
	{
		uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::CONSTANTVARIABLEDECLARATION * COldLightEffect::CMaterialDiffuseEffect::GetConstantVariables  (UINT& uConstantVariableCount)
	{
		uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

		return ALCHEMY_NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	//Specular Material
	//////////////////////////////////////////////////////////////////////////
	ALCHEMY_INLINE UINT                                       COldLightEffect::CMaterialSpecularEffect::GetEffectInfoFlag     (                             )
	{
		return ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT);
	}

	ALCHEMY_INLINE const CEffectInfoInterface::SAMPLER      * COldLightEffect::CMaterialSpecularEffect::GetSamplers  (UINT& uSamplerCount  )
	{
		uSamplerCount = TOTAL_SAMPLERS;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* COldLightEffect::CMaterialSpecularEffect::GetAttributes(UINT& uAttributeCount)
	{
		uAttributeCount = TOTAL_ATTRIBUTES;

		return sm_ATTRIBUTES;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* COldLightEffect::CMaterialSpecularEffect::GetVarying   (UINT& uVaryingCount  )
	{
		uVaryingCount = TOTAL_VARYING;

		return sm_VARYING;
	}

	ALCHEMY_INLINE const CEffectCode::GLOBALVARIABLE        * COldLightEffect::CMaterialSpecularEffect::GetConstants (UINT& uConstantCount )
	{
		uConstantCount = TOTAL_CONSTANTS;

		return sm_CONSTANS;
	}

	ALCHEMY_INLINE const CEffectCode::TEMPORARYDECLARATION          * COldLightEffect::CMaterialSpecularEffect::GetTemporary (UINT& uTemporaryCount)
	{
		uTemporaryCount = TOTAL_TEMPORARY;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* COldLightEffect::CMaterialSpecularEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
	{
		uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::CONSTANTVARIABLEDECLARATION * COldLightEffect::CMaterialSpecularEffect::GetConstantVariables  (UINT& uConstantVariableCount)
	{
		uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

		return ALCHEMY_NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	//Light Effect
	//////////////////////////////////////////////////////////////////////////
	ALCHEMY_INLINE void COldLightEffect::EnableEffect(UINT uEffectFlag)
	{
		m_uEffectFlag = uEffectFlag;
	}

	ALCHEMY_INLINE UINT COldLightEffect::GetEffectFlag()const
	{
		return m_uEffectFlag;
	}
}