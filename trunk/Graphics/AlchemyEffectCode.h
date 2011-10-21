#pragma once

#include "AlchemyCompilerInterface.h"

#include "../Core/AlchemyList.h"
#include "../Core/AlchemyMap.h"
#include "../Core/AlchemySmartPointer.h"
#include "../Core/AlchemyReferenceCounterInterface.h"
//#include "AlchemyRenderMethodInterface.h"

namespace alchemy
{
	class CEffectInfoInterface;
	class CEffectInfoBlendMethodInterface;
	class CEffectCode
	{
	public:

		typedef enum
		{
			TEXTURE1D = 0,
			TEXTURE2D,
			TEXTURE3D,
			TEXTURE_CUBE,

			TOTAL_SAMPLER_TYPE
		}SAMPLERTYPE;

		typedef enum
		{
			NONE = 0,
			POINT,
			LINEAR,
			ANISOTROPIC,
			PYRAMIDALQUAD,
			GAUSSIANQUAD,
			CONVOLUTIONMONO,

			TOTAL_SAMPLER_FILTER
		}SAMPLERFILTER;

		typedef enum 
		{
			WRAP = 0,
			MIRROR,
			CLAMP,
			BORDER,
			MIRROR_ONCE,

			TOTAL_SAMPLER_ADDRESS_MODE
		}SAMPLERADDRESSMODE;

		typedef enum
		{
			UNKNOWN_VARIABLE = 0,

			VECTOR1_VARIABLE = CVertexDeclaration::FLOAT1,
			VECTOR2_VARIABLE = CVertexDeclaration::FLOAT2,
			VECTOR3_VARIABLE = CVertexDeclaration::FLOAT3,
			VECTOR4_VARIABLE = CVertexDeclaration::FLOAT4,

			MATRIX2X2_VARIABLE,
			MATRIX3X3_VARIABLE,
			MATRIX4X3_VARIABLE,
			MATRIX4X4_VARIABLE,

			SAMPLER_VARIABLE,
			STRUCT_VARIABLE
		}VARIABLETYPE;

		typedef enum
		{
			NORMAL_VARIABLE,
			ARRAY_VARIABLE,
			MATRIX_VARIABLE,
			CAMERA_VARIABLE,
			MATERIAL_VARIABLE,
			LIGHT_VARIABLE,
			CONSTANT_VARIABLE,
			TEXTURE_VARIABLE,
			INPUT_VARIABLE,
		}GLOBALVARIABLETYPE;

		typedef struct
		{
			SAMPLERTYPE        Type;
			SAMPLERFILTER      MinFilter;
			SAMPLERFILTER      MagFilter;
			SAMPLERFILTER      MipFilter;
			SAMPLERADDRESSMODE AddressU;
			SAMPLERADDRESSMODE AddressV;
			SAMPLERADDRESSMODE AddressW;
		}SAMPLERDECLARATION;

		typedef struct
		{
			VARIABLETYPE Type;
			UINT         uStruct;
			UINT         uNext;
		}STRUCTDECLARATION, * LPSTRUCTDECLARATION;

		typedef struct
		{
			VARIABLETYPE Type;
			UINT         uRegister;
			UINT         uDeclaration;
			UINT         uFunction;
			UINT         uGlobal;
			UINT         uParent;
		}VARIABLE, * LPVARIABLE;

		typedef struct
		{
			CEffectInterface::EFFECTTYPE ShaderType;
			GLOBALVARIABLETYPE           GlobalType;
			UINT                         uDetail;
			UINT                         uCount;
			const void*                  pValue;
		}GLOBALVARIABLEKEY;

		typedef struct
		{
			CEffectInterface::EFFECTTYPE ShaderType;
			GLOBALVARIABLETYPE           GlobalType;
			UINT                         uDetail;
			UINT                         uCount;
			UINT                         uIndex;
			const void*                  pValue;
		}GLOBALVARIABLE, * LPGLOBALVARIABLE;

		typedef struct  
		{
			CCompilerInterface::REGISTERTYPE Type;
			UINT                             uRegister;
			UINT                             uDeclaration;
			UINT                             uNext;
		}USERVARIABLE;

		typedef struct
		{
			UINT uCode;
			UINT uUserVariables;
			UINT uParametersVariable;
			UINT uResultVariable;
			UINT uShader;
		}FUNCTION, * LPFUNCTION;

		typedef struct
		{
			CVertexDeclaration::ELEMENTTYPE  Type;
			CVertexDeclaration::ELEMENTUSAGE Usage;
		}SHADERPARAMETERKEY, * LPSHADERPARAMETERKEY;

		typedef struct
		{
			CVertexDeclaration::ELEMENTTYPE  Type;
			CVertexDeclaration::ELEMENTUSAGE Usage;
			UINT                             uIndex;
			UINT                             uNext;
		}SHADERPARAMETER, * LPSHADERPARAMETER;

		typedef struct
		{
			CEffectInterface::EFFECTTYPE Type;
			UINT                         uParameters;
			//UINT                   uFunction;
		}SHADERFUNCTION, * LPSHADERFUNCTION;

		typedef struct  
		{
			UINT uVertexShader;
			UINT uFragmentShader;
		}PASS, * LPPASS;

		typedef struct
		{
			CVertexDeclaration::ELEMENTTYPE  Type;
			CVertexDeclaration::ELEMENTUSAGE Usage;
			UINT                             uIndex;
		}ELEMENTPARAMETER, * LPELEMENTPARAMETER;

		typedef struct  
		{
			CVertexDeclaration::ELEMENTTYPE  Type;
			CVertexDeclaration::ELEMENTUSAGE Usage;
			UINT                             uIndex;
			bool                             bIsForce;
		}ATTRIBUTE, * LPATTRIBUTE;

		typedef struct  
		{
			GLOBALVARIABLETYPE Type;
			UINT               uDetail;
			UINT               uCount;
			UINT               uIndex;
			const void*        pValue;
			bool               bIsForce;
		}CONSTANT, * LPCONSTANT;

		typedef struct  
		{
			UINT uVertexShaderHandle;
			UINT uFragmentShaderHandle;
			bool IsUsed;
		}VARYING, * LPVARYING;

		typedef struct  
		{
			CEffectInterface::EFFECTTYPE ShaderType;
			VARIABLETYPE                 VariableType;
			UINT                         uCount;
		}TEMPORARYDECLARATION, * LPTEMPORARYDECLARATION;

		typedef struct  
		{
			CEffectInterface::EFFECTTYPE     ShaderType;
			CVertexDeclaration::ELEMENTTYPE  ElementType;
			CVertexDeclaration::ELEMENTUSAGE ElementUsage;
			UINT                             uIndex;
			bool                             bIsReadOnly;
			bool                             bIsForce;
		}ATTRIBUTEVARIABLEDECLARATION, * LPATTRIBUTEVARIABLEDECLARATION;

		typedef struct
		{
			CEffectInterface::EFFECTTYPE ShaderType;
			GLOBALVARIABLETYPE           GlobalType;
			UINT                         uDetail;
			UINT                         uCount;
			UINT                         uIndex;
			const VOID*                  pValue;
			bool                         bIsReadOnly;
			bool                         bIsForce;
		}CONSTANTVARIABLEDECLARATION, * LPCONSTANTVARIABLEDECLARATION;

		typedef struct  
		{
			UINT uSource;
			UINT uHandle;
			bool bIsUsed;
		}SHADERVARIABLE, *LPSHADERVARIABLE;

		typedef struct  
		{
			UINT uParent;
			UINT uAddress;
		}ADDRESSVARIABLE;

		typedef struct GLOBALVARIABLEKEYLESS
		{
			bool operator()(const GLOBALVARIABLEKEY& Left, const GLOBALVARIABLEKEY& Right)const;
		}GLOBALVARIABLEKEYLESS;

		typedef struct GLOBALVARIABLEKEYEQUAL
		{
			bool operator()(const GLOBALVARIABLEKEY& Left, const GLOBALVARIABLEKEY& Right)const;
		}GLOBALVARIABLEKEYEQUAL;

		typedef struct GLOBALVARIABLELESS
		{
			bool operator()(const GLOBALVARIABLE& Left, const GLOBALVARIABLE& Right)const;
		}GLOBALVARIABLELESS;

		typedef struct GLOBALVARIABLEEQUAL
		{
			bool operator()(const GLOBALVARIABLE& Left, const GLOBALVARIABLE& Right)const;
		}GLOBALVARIABLEEQUAL;

		typedef struct ADDRESSVARIABLELESS
		{
			bool operator()(const ADDRESSVARIABLE& Left, const ADDRESSVARIABLE& Right)const;
		}ADDRESSVARIABLELESS;

		typedef struct ADDRESSVARIABLEEQUAL
		{
			bool operator()(const ADDRESSVARIABLE& Left, const ADDRESSVARIABLE& Right)const;
		}ADDRESSVARIABLEEQUAL;

		class CEffectParameterHandlerInterface : 
			public CReferenceCounterInterface

		{
		public:
			CEffectParameterHandlerInterface() {}
			virtual ~CEffectParameterHandlerInterface() {}

			virtual bool SetParameters(CEffectInterface& Effect, UINT uPass) = 0;
		};

		class CGlobalInputHandler :
			public CEffectParameterHandlerInterface
		{
			friend class CEffectCode;

			typedef struct  
			{
				CEffectInterface::EFFECTTYPE ShaderType;
				UINT                         uCode;
				UINT                         uIndex;
				UINT                         uCount;
				const void*                  pValue;
			}GLOBALINPUT, * LPGLOBALINPUT;
		public:
			CGlobalInputHandler();
			~CGlobalInputHandler();

			bool SetParameters(CEffectInterface& Effect, UINT uPass);

		private:
			LPGLOBALINPUT* m_ppGlobalInput;
			PUINT m_puGlobalInputCount;
			UINT m_uPassCount;
		};

		typedef CSmartPointer<CGlobalInputHandler> GLOBALINPUTHANDLER, * LPGLOBALINPUTHANDLER;

		class CDefiner
		{
			friend class CEffectCode;
		public:
			CDefiner(CEffectCode& Parent);
			virtual ~CDefiner();

			CEffectCode& GetParent();

			virtual UINT EndDefine() = 0;

		protected:
			static UINT _GetHandle(const CDefiner& Definer);

			CEffectCode& m_Parent;

			UINT m_uHandle;
		};

		class CStructDefiner :
			public CDefiner
		{
		public:
			CStructDefiner(CEffectCode& Parent);
			~CStructDefiner();

			void AddType(VARIABLETYPE Type, UINT uStruct = 0);

			UINT EndDefine();
		private:
			LPSTRUCTDECLARATION m_pCurrentDeclaration; 
		};

		class CShaderParametersDefiner :
			public CDefiner
		{
			typedef struct SHADERPARAMETERKEYLESS
			{
				bool operator()(const SHADERPARAMETERKEY& Left, const SHADERPARAMETERKEY& Right)const;
			}SHADERPARAMETERKEYLESS;

			typedef struct SHADERPARAMETERKEYEQUAL
			{
				bool operator()(const SHADERPARAMETERKEY& Left, const SHADERPARAMETERKEY& Right)const;
			}SHADERPARAMETERKEYEQUAL;
		public:
			CShaderParametersDefiner(CEffectCode& Parent);
			~CShaderParametersDefiner();

			const UINT* GetShaderParameterKeyCount(const SHADERPARAMETERKEY& ShaderParameterKey);

			void AppendParameter(CVertexDeclaration::ELEMENTTYPE Type, CVertexDeclaration::ELEMENTUSAGE Usage);

			UINT EndDefine();

		private:
			LPSHADERPARAMETER m_pShaderParameter;
			CMap<SHADERPARAMETERKEY, UINT, SHADERPARAMETERKEYLESS, SHADERPARAMETERKEYEQUAL> m_ShaderParameterCountMap;
		};

		class CInstructionSetInterface
		{
		public:
			CInstructionSetInterface() {}
			virtual ~CInstructionSetInterface() {}

			virtual UINT ApplyVariable(VARIABLETYPE Type, UINT uCount) = 0;

			virtual bool AppendInstruction(
				CCompilerInterface::INSTRUCTION Instruction,
				UINT uDestination,
				UINT uSource1,
				UINT uSource2) = 0;

			virtual bool AppendInstruction(
				CCompilerInterface::INSTRUCTION Instruction,
				UINT uDestination, 
				const UINT* puDestinationAddress, 
				UINT uDestinationAddressCount, 
				UINT uSource1, 
				const UINT* puAddress1, 
				UINT uAddressCount1, 
				UINT uSource2, 
				const UINT* puAddress2, 
				UINT uAddressCount2) = 0;

			virtual bool AppendInstruction(
				CCompilerInterface::INSTRUCTION Instruction, 
				UINT uDestination, 
				UINT uDestinationIndex,
				UINT uDestinationIndexAddress, 
				const UINT* puDestinationAddress, 
				UINT uDestinationAddressCount, 
				UINT uSource1, 
				UINT uSourceIndex1,
				UINT uSourceIndexAddress1, 
				const UINT* puAddress1, 
				UINT uAddressCount1, 
				UINT uSource2, 
				UINT uSourceIndex2,
				UINT uSourceIndexAddress2,
				const UINT* puAddress2, 
				UINT uAddressCount2) = 0;
		};

		class CBasicFunctionDefiner :
			public CDefiner, public CInstructionSetInterface
		{
			typedef struct  
			{
				UINT uBegin;
				UINT uCount;
			}REGISTERSEGMENT, * LPREGISTERSEGMENT;

			typedef struct  
			{
				CCompilerInterface::REGISTERTYPE Type;
				UINT uRegister;
			}USERVARIABLEKEY, * LPUSERVARIABLEKEY;

			typedef struct USERVARIABLEKEYLESS
			{
				bool operator()(const USERVARIABLEKEY& Left, const USERVARIABLEKEY& Right)const;
			}USERVARIABLEKEYLESS;

			typedef struct USERVARIABLEKEYEQUAL
			{
				bool operator()(const USERVARIABLEKEY& Left, const USERVARIABLEKEY& Right)const;
			}USERVARIABLEKEYEQUAL;
		public:
			CBasicFunctionDefiner(CEffectCode& Parent);
			~CBasicFunctionDefiner();

			bool AppendInstruction(
				CCompilerInterface::INSTRUCTION Instruction,
				UINT uDestination,
				UINT uSource1,
				UINT uSource2);

			bool AppendInstruction(
				CCompilerInterface::INSTRUCTION Instruction,
				UINT uDestination, 
				const UINT* puDestinationAddress, 
				UINT uDestinationAddressCount, 
				UINT uSource1, 
				const UINT* puAddress1, 
				UINT uAddressCount1, 
				UINT uSource2, 
				const UINT* puAddress2, 
				UINT uAddressCount2);

			bool AppendInstruction(
				CCompilerInterface::INSTRUCTION Instruction, 
				UINT uDestination, 
				UINT uDestinationIndex,
				UINT uDestinationIndexAddress, 
				const UINT* puDestinationAddress, 
				UINT uDestinationAddressCount, 
				UINT uSource1, 
				UINT uSourceIndex1,
				UINT uSourceIndexAddress1, 
				const UINT* puAddress1, 
				UINT uAddressCount1, 
				UINT uSource2, 
				UINT uSourceIndex2,
				UINT uSourceIndexAddress2,
				const UINT* puAddress2, 
				UINT uAddressCount2);

			UINT ApplyVariable(VARIABLETYPE Type, UINT uDeclaration, UINT uCount);
			UINT ApplyVariable(VARIABLETYPE Type, UINT uCount);
			UINT ApplyVariable(UINT uShaderParameters);

			bool FreeVariable(UINT uHandle);

			//bool EndFunction(UINT uResultVariable);

			UINT EndDefine();

		protected:
			UINT _ApplyVariable(VARIABLETYPE Type, UINT uDeclaration, UINT uCount, bool bIsTemporary);
			UINT _ApplyVariable(UINT uShaderParameters, bool bIsTemporary);

			//virtual bool _IsValuableResultVariable(UINT uResultVariable);

			virtual void _ApplyFunction();

			UINT m_uUserVariables;
			UINT m_uParametersVariable;
			UINT m_uResultVariable;
			CEffectInterface::EFFECTTYPE m_Type;

		private:
			UINT __ApplyUserVariables();

			STRING m_Code;
			LPFUNCTION m_pFunction;
			UINT m_uTemporaryRegisterCount;

			CMap<USERVARIABLEKEY, UINT, USERVARIABLEKEYLESS, USERVARIABLEKEYEQUAL> m_UserVariableDeclarationMap;
			CMap<UINT           , UINT> m_VariableCountMap;
			CPool<REGISTERSEGMENT> m_RegisterSegmentPool;
		};

		class CShaderFunctionDefiner :
			public CBasicFunctionDefiner
		{
		public:
			CShaderFunctionDefiner(CEffectCode& Parent, bool bIsVertexShader);
			~CShaderFunctionDefiner();

			/*bool AppendParameter(CVertexDeclaration::ELEMENTTYPE Type, CVertexDeclaration::ELEMENTUSAGE Usage);

			UINT EndAppendParameter();*/

			//bool EndFunction(UINT uResultVariable);
			UINT BeginFunction(UINT uParameters);
			UINT EndFunction  (UINT uParameters);

			UINT EndDefine();
		protected:
			void _ApplyFunction();

			//bool _IsValuableResultVariable(UINT uResultVariable);

			//CPool<SHADERPARAMETER> m_ShaderParameterPool;

			//LPSHADERPARAMETER m_pShaderParameter;
		};

		class CPassDefiner :
			public CDefiner
		{
			typedef struct  
			{
				CEffectInterface::EFFECTTYPE     ShaderType;
				CVertexDeclaration::ELEMENTTYPE  ElementType;
				CVertexDeclaration::ELEMENTUSAGE ElementUsage;
				UINT                             uIndex;
			}ATTRIBUTEVARIABLEKEY, * LPATTRIBUTEVARIABLEKEY;

			typedef struct  
			{
				UINT uSortValue;
				UINT uHandle;
			}FREETEMPORARY;

			typedef struct
			{
				CEffectInterface::EFFECTTYPE ShaderType;
				VARIABLETYPE                 VariableType;
				UINT                         uCount;
				UINT                         uHandle;
			}TEMPORARY;

			typedef struct VERTEXELEMENTPARAMETERLESS
			{
				bool operator()(const ELEMENTPARAMETER& Left, const ELEMENTPARAMETER& Right)const;
			}VERTEXELEMENTPARAMETERLESS;

			typedef struct VERTEXELEMENTPARAMETEREQUAL
			{
				bool operator()(const ELEMENTPARAMETER& Left, const ELEMENTPARAMETER& Right)const;
			}VERTEXELEMENTPARAMETEREQUAL;

			typedef struct TEMPORARYDECLARATIONESS
			{
				bool operator()(const TEMPORARYDECLARATION& Left, const TEMPORARYDECLARATION& Right)const;
			}TEMPORARYDECLARATIONLESS;

			typedef struct TEMPORARYDECLARATIONEQUAL
			{
				bool operator()(const TEMPORARYDECLARATION& Left, const TEMPORARYDECLARATION& Right)const;
			}TEMPORARYDECLARATIONEQUAL;

			/*typedef struct TEMPORARYLESS
			{
				bool operator()(const TEMPORARY& Left, const TEMPORARY& Right)const;
			}TEMPORARYLESS;

			typedef struct TEMPORARYEQUAL
			{
				bool operator()(const TEMPORARY& Left, const TEMPORARY& Right)const;
			}TEMPORARYEQUAL;*/

			typedef struct ATTRIBUTEVARIABLEKEYLESS
			{
				bool operator()(const ATTRIBUTEVARIABLEKEY& Left, const ATTRIBUTEVARIABLEKEY& Right)const;
			}ATTRIBUTEVARIABLEKEYLESS;

			typedef struct ATTRIBUTEVARIABLEKEYEQUAL
			{
				bool operator()(const ATTRIBUTEVARIABLEKEY& Left, const ATTRIBUTEVARIABLEKEY& Right)const;
			}ATTRIBUTEVARIABLEKEYEQUAL;

			typedef struct VARYINGLESS
			{
				bool operator()(const VARYING& Left, const VARYING& Right)const;
			}VARYINGLESS;

			typedef struct VARYINGEQUAL
			{
				bool operator()(const VARYING& Left, const VARYING& Right)const;
			}VARYINGEQUAL;

			typedef struct FREETEMPORARYLESS
			{
				bool operator()(const FREETEMPORARY& Left, const FREETEMPORARY& Right)const;
			}FREETEMPORARYLESS;

			typedef struct FREETEMPORARYEQUAL
			{
				bool operator()(const FREETEMPORARY& Left, const FREETEMPORARY& Right)const;
			}FREETEMPORARYEQUAL;
		public:
			//typedef void BLENDFUNCTION(UINT, UINT, UINT, CInstructionSetInterface&), (* LPBLENDFUNCTION)(UINT, UINT, UINT, CInstructionSetInterface&);

			typedef enum
			{
				SELECTARG1 = 0,
				SELECTARG2,
				MODULATE,
				MODULATE2X,
				MODULATE4X,
				ADD,
				ADDSIGNED,
				ADDSIGNED2X,
				SUBTRACT,
				ADDSMOOTH,
				BLENDDIFFUSEALPHA,
				BLENDTEXTUREALPHA,
				BLENDFACTORALPHA,
				BLENDTEXTUREALPHAPM,
				BLENDCURRENTALPHA,
				PREMODULATE,
				MODULATEALPHA_ADDCOLOR,
				MODULATECOLOR_ADDALPHA,
				MODULATEINVALPHA_ADDCOLOR,
				MODULATEINVCOLOR_ADDALPHA,
				BUMPENVMAP,
				BUMPENVMAPLUMINANCE,
				DOTPRODUCT3,
				MULTIPLYADD,
				LERP,

				TOTAL_BLEND_METHOD
			}BLENDMETHOD;

			typedef struct
			{
				CEffectInfoInterface           * pEffectInfo;
				CEffectInfoBlendMethodInterface* pBlendMethod;
				//LPBLENDFUNCTION pfnBlendMethod;

			}EFFECTINFO, * LPEFFECTINFO;

			class CEffectCodeUtils
			{
			public:
				CEffectCodeUtils(CEffectCode& Parent);
				~CEffectCodeUtils();

				UINT GetAddressVariable(UINT uSource, UINT uAddress);

			private:
				CEffectCode& m_Parent;
			};

			CPassDefiner(CEffectCode& Parent);
			~CPassDefiner();

			//static const BLENDFUNCTION* GetBlendFunction(BLENDMETHOD BlendMethod);

			bool AddInfo(CEffectInfoInterface& Info, CEffectInfoBlendMethodInterface* pBlendMethod);
			bool AddInfo(CEffectInfoInterface& Info, BLENDMETHOD                       BlendMethod);

			UINT EndDefine();

		private:
			UINT __PopFreeTemporary(const TEMPORARYDECLARATION& TemporaryDeclaration);
			void __PushFreeTemporary(const TEMPORARYDECLARATION& TemporaryDeclaration, UINT uHandle, bool bIsCached);

			void __UpdateTemporary();
			void __FreeTemporary();

			//LPEFFECTINFO m_pEffectInfos;
			//UINT m_uEffectInfoCount;

			PUINT m_puBuffer;
			UINT  m_uBufferLength;

			LPVARYING m_pVaryingBuffer;
			UINT m_uVaryingBufferLength;

			LPSHADERVARIABLE m_pVariableBuffer;
			UINT m_uVariableBufferLength;

			UINT m_uAttributeAddress;
			UINT m_uVaryingAddress;

			bool m_bIsBlend;

			CMap<ELEMENTPARAMETER    , UINT                                                       , VERTEXELEMENTPARAMETERLESS, VERTEXELEMENTPARAMETEREQUAL> m_AttributeAddressMap;
			CMap<ELEMENTPARAMETER    , UINT                                                       , VERTEXELEMENTPARAMETERLESS, VERTEXELEMENTPARAMETEREQUAL> m_VaryingAddressMap;

			CMap<ELEMENTPARAMETER    , UINT                                                       , VERTEXELEMENTPARAMETERLESS, VERTEXELEMENTPARAMETEREQUAL> m_AttributeMap;
			CMap<ELEMENTPARAMETER    , VARYING                                                    , VERTEXELEMENTPARAMETERLESS, VERTEXELEMENTPARAMETEREQUAL> m_VaryingMap;

			//CMap<TEMPORARYDECLARATION, UINT                                                       , TEMPORARYDECLARATIONLESS  , TEMPORARYDECLARATIONEQUAL  > m_TemporaryCountMap;
			//CMap<TEMPORARY           , UINT                                                       , TEMPORARYLESS             , TEMPORARYEQUAL             > m_TemporaryHandleMap;

			CMap<ATTRIBUTEVARIABLEKEY, SHADERVARIABLE                                             , ATTRIBUTEVARIABLEKEYLESS  , ATTRIBUTEVARIABLEKEYEQUAL  > m_AttributeVariableMap;
			CMap<ATTRIBUTEVARIABLEKEY, UINT                                                       , ATTRIBUTEVARIABLEKEYLESS  , ATTRIBUTEVARIABLEKEYEQUAL  > m_AttributeVariableMaxInfoMap;
			CMap<GLOBALVARIABLE      , SHADERVARIABLE                                             , GLOBALVARIABLELESS        , GLOBALVARIABLEEQUAL        > m_ConstantVariableMap;
			CMap<GLOBALVARIABLE      , UINT                                                       , GLOBALVARIABLELESS        , GLOBALVARIABLEEQUAL        > m_ConstantVariableMaxInfoMap;

			CMap<VARYING             , SHADERVARIABLE                                             , VARYINGLESS               , VARYINGEQUAL               > m_VaryingVariableMap;

			CMap<ELEMENTPARAMETER    , UINT                                                       , VERTEXELEMENTPARAMETERLESS, VERTEXELEMENTPARAMETEREQUAL> m_AttributeVaryingMap;
			CMap<GLOBALVARIABLE      , ELEMENTPARAMETER                                           , GLOBALVARIABLELESS        , GLOBALVARIABLEEQUAL        > m_ConstantVaryingMap;

			CMap<TEMPORARYDECLARATION, CList<FREETEMPORARY>                                       , TEMPORARYDECLARATIONLESS  , TEMPORARYDECLARATIONEQUAL  > m_FreeTemporaryMap;

			CVector<TEMPORARY>  m_FreeTemporaryCache;

			CVector<EFFECTINFO> m_EffectInfos;

			//CMap<GLOBALVARIABLE        , UINT   > m_ConstantMap;

			CShaderFunctionDefiner m_VertexShaderFunctionDefiner;
			CShaderFunctionDefiner m_FragmentShaderFunctionDefiner;

			CShaderParametersDefiner m_AttributesDefiner;
			CShaderParametersDefiner m_VaryingDefiner;

			CEffectCodeUtils m_EffectCodeUtils;
		};

		CEffectCode(CCompilerInterface& Compiler);
		~CEffectCode(void);

		static bool IsElementVariable(VARIABLETYPE VariableType);

		static UINT BuildGlobalInputCode(VARIABLETYPE VariableType, UINT uRowOffset, UINT uOffset, bool bIsPointer);

		static void GetGlobalInputInfo(UINT uCode, VARIABLETYPE* pVariableType = ALCHEMY_NULL, PUINT puRowOffset = ALCHEMY_NULL, PUINT puOffset = ALCHEMY_NULL, bool* pbIsPointer = ALCHEMY_NULL);

		static VARIABLETYPE GetGlobalVariableType(GLOBALVARIABLETYPE GlobalVariableType, UINT uDetail);

		UINT GetPassCount()const;
		CCompilerInterface& GetCompiler();

		const VARIABLE          * GetVariable          (UINT uHandle)const;
		const GLOBALVARIABLE    * GetGlobalVariable    (UINT uHandle)const;

		const USERVARIABLE      * GetUserVariable      (UINT uHandle)const;
		const UINT              * GetUserVariables     (UINT uHandle)const;

		const SAMPLERDECLARATION* GetSamplerDeclaration(UINT uHandle)const;

		const STRUCTDECLARATION * GetStructDeclaration (UINT uHandle)const;
		const UINT              * GetStruct            (UINT uHandle)const;

		const STRING            * GetCode              (UINT uHandle)const;
		const FUNCTION          * GetFunction          (UINT uHandle)const;

		const SHADERPARAMETER   * GetShaderParameter   (UINT uHandle)const;
		const UINT              * GetShaderParameters  (UINT uHandle)const;

		const SHADERFUNCTION    * GetShaderFunction    (UINT uHandle)const;

		const PASS              * GetPass              (UINT uHandle)const;

		CCompilerInterface::REGISTERTYPE GetRegisterType(UINT uVariable)const;

		UINT GetRegisterCount(VARIABLETYPE Type, UINT uDeclaration = 0, bool bIsByteAlignment = true)const;

		UINT GetAddressVariable(UINT uSource, UINT uAddress);

		void Clear();

		UINT AddGlobalVariable(const GLOBALVARIABLEKEY& GlobalVariableKey);
		UINT AddSamplerVariable(const SAMPLERDECLARATION& Sampler, UINT uIndex);
		UINT AddPass(UINT uVertexShaderFunction, UINT uFragmentShaderFunction);

		CEffectInterface::EFFECTTYPE BuildShaderFunctionCode(STRING& Desination, UINT uShaderFunction);

		CBuffer& CreateParameterList(LPGLOBALINPUTHANDLER* ppGlobalInputHandler);
	private:

		CCompilerInterface&                                                          m_Compiler;

		CPool<VARIABLE>                                                              m_VariablePool;
		CPool<GLOBALVARIABLE>                                                        m_GlobalVariablePool;

		CPool<USERVARIABLE>                                                          m_UserVariablePool;
		CPool<UINT>                                                                  m_UserVariablesPool;

		CPool<SAMPLERDECLARATION>                                                    m_SamplerDeclarationPool;

		CPool<STRUCTDECLARATION>                                                     m_StructDeclarationPool;
		CPool<UINT>                                                                  m_StructPool;
  
		CPool<STRING>                                                                m_CodePool;
		CPool<FUNCTION>                                                              m_FunctionPool;

		CPool<SHADERPARAMETER>                                                       m_ShaderParameterPool;
		CPool<UINT>                                                                  m_ShaderParametersPool;

		CPool<SHADERFUNCTION>                                                        m_ShaderFunctionPool;

		CVector<PASS>                                                                m_Passes;

		CMap<GLOBALVARIABLEKEY, UINT, GLOBALVARIABLEKEYLESS, GLOBALVARIABLEKEYEQUAL> m_GlobalVariableCountMap;

		CMap<GLOBALVARIABLE   , UINT, GLOBALVARIABLELESS   , GLOBALVARIABLEEQUAL   > m_GlobalVariableHandleMap;

		CMap<ADDRESSVARIABLE  , UINT, ADDRESSVARIABLELESS  , ADDRESSVARIABLEEQUAL  > m_AddressVariableHandleMap;

		UINT                                                                         m_uVertexShaderGlobalRegisterCount;
		UINT                                                                         m_uFragmentShaderGlobalRegisterCount;

		LPGLOBALINPUTHANDLER                                                         m_pCurrentGlobalInputHandler;
	};

	ALCHEMY_INLINE CEffectCode& CEffectCode::CDefiner::GetParent()
	{
		return m_Parent;
	}

	ALCHEMY_INLINE UINT CEffectCode::CDefiner::_GetHandle(const CEffectCode::CDefiner& Definer)
	{
		return Definer.m_uHandle;
	}

	ALCHEMY_INLINE UINT CEffectCode::GetPassCount()const
	{
		return m_Passes.GetLength();
	}

	ALCHEMY_INLINE CCompilerInterface& CEffectCode::GetCompiler()
	{
		return m_Compiler;
	}

	ALCHEMY_INLINE bool CEffectCode::CShaderParametersDefiner::SHADERPARAMETERKEYLESS::operator()(const SHADERPARAMETERKEY& Left, const SHADERPARAMETERKEY& Right)const
	{
		if(Left.Type < Right.Type)
			return true;
		else if(Left.Type == Right.Type && Left.Usage < Right.Usage)
			return true;

		return false;
	}

	ALCHEMY_INLINE bool CEffectCode::CBasicFunctionDefiner::USERVARIABLEKEYLESS::operator()(const USERVARIABLEKEY& Left, const USERVARIABLEKEY& Right)const
	{
		if(Left.Type < Right.Type)
			return true;
		else if(Left.Type == Right.Type && Left.uRegister < Right.uRegister)
			return true;

		return false;
	}

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::VERTEXELEMENTPARAMETERLESS::operator()(const ELEMENTPARAMETER& Left, const ELEMENTPARAMETER& Right)const
	{
		if(Left.Type < Right.Type)
			return true;
		else if(Left.Type == Right.Type && Left.Usage < Right.Usage)
			return true;
		else if(Left.Type == Right.Type && Left.Usage == Right.Usage && Left.uIndex < Right.uIndex)
			return true;

		return false;
	}

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::TEMPORARYDECLARATIONLESS::operator()(const TEMPORARYDECLARATION& Left, const TEMPORARYDECLARATION& Right)const
	{
		if(Left.ShaderType < Right.ShaderType)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.VariableType < Right.VariableType)
			return true;

		return false;
	}

	/*ALCHEMY_INLINE bool CEffectCode::CPassDefiner::TEMPORARYLESS::operator()(const TEMPORARY& Left, const TEMPORARY& Right)const
	{
		if(Left.ShaderType < Right.ShaderType)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.VariableType < Right.VariableType)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.VariableType == Right.VariableType && Left.uIndex < Right.uIndex)
			return true;

		return false;
	}*/

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::ATTRIBUTEVARIABLEKEYLESS::operator()(const ATTRIBUTEVARIABLEKEY& Left, const ATTRIBUTEVARIABLEKEY& Right)const
	{
		if(Left.ShaderType < Right.ShaderType)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.ElementType < Right.ElementType)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.ElementType == Right.ElementType && Left.ElementUsage < Right.ElementUsage)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.ElementType == Right.ElementType && Left.ElementUsage == Right.ElementUsage && Left.uIndex < Right.uIndex)
			return true;

		return false;
	}

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::VARYINGLESS::operator()(const VARYING& Left, const VARYING& Right)const
	{
		if(Left.uVertexShaderHandle < Right.uVertexShaderHandle)
			return true;
		else if(Left.uVertexShaderHandle == Right.uVertexShaderHandle && Left.uFragmentShaderHandle < Right.uFragmentShaderHandle)
			return true;
		else if(Left.uVertexShaderHandle == Right.uVertexShaderHandle && Left.uFragmentShaderHandle == Right.uFragmentShaderHandle && Left.IsUsed < Right.IsUsed)
			return true;

		return false;
	}

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::FREETEMPORARYLESS::operator()(const FREETEMPORARY& Left, const FREETEMPORARY& Right)const
	{
		if(Left.uSortValue < Right.uSortValue)
			return true;
		else if(Left.uSortValue == Right.uSortValue && Left.uHandle < Right.uHandle)
			return true;

		return false;
	}

	ALCHEMY_INLINE bool CEffectCode::GLOBALVARIABLEKEYLESS::operator()(const GLOBALVARIABLEKEY& Left, const GLOBALVARIABLEKEY& Right)const
	{
		if(Left.ShaderType < Right.ShaderType)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.GlobalType < Right.GlobalType)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.GlobalType == Right.GlobalType && Left.uDetail < Right.uDetail)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.GlobalType == Right.GlobalType && Left.uDetail == Right.uDetail && Left.uCount < Right.uCount)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.GlobalType == Right.GlobalType && Left.uDetail == Right.uDetail && Left.uCount == Right.uCount && Left.pValue < Right.pValue)
			return true;

		return false;
	}

	ALCHEMY_INLINE bool CEffectCode::GLOBALVARIABLELESS::operator()(const GLOBALVARIABLE& Left, const GLOBALVARIABLE& Right)const
	{
		if(Left.ShaderType < Right.ShaderType)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.GlobalType < Right.GlobalType)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.GlobalType == Right.GlobalType && Left.uDetail < Right.uDetail)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.GlobalType == Right.GlobalType && Left.uDetail == Right.uDetail && Left.uCount < Right.uCount)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.GlobalType == Right.GlobalType && Left.uDetail == Right.uDetail && Left.uCount == Right.uCount && Left.uIndex < Right.uIndex)
			return true;
		else if(Left.ShaderType == Right.ShaderType && Left.GlobalType == Right.GlobalType && Left.uDetail == Right.uDetail && Left.uCount == Right.uCount && Left.uIndex == Right.uIndex && Left.pValue < Right.pValue)
			return true;

		return false;
	}

	ALCHEMY_INLINE bool CEffectCode::ADDRESSVARIABLELESS::operator()(const ADDRESSVARIABLE& Left, const ADDRESSVARIABLE& Right)const
	{
		if(Left.uParent < Right.uParent)
			return true;
		else if(Left.uParent == Right.uParent && Left.uAddress < Right.uAddress)
			return true;

		return false;
	};

	ALCHEMY_INLINE bool CEffectCode::CShaderParametersDefiner::SHADERPARAMETERKEYEQUAL::operator()(const SHADERPARAMETERKEY& Left, const SHADERPARAMETERKEY& Right)const
	{
		return Left.Type == Right.Type && Left.Usage == Right.Usage;
	}

	ALCHEMY_INLINE bool CEffectCode::CBasicFunctionDefiner::USERVARIABLEKEYEQUAL::operator()(const USERVARIABLEKEY& Left, const USERVARIABLEKEY& Right)const
	{
		return Left.Type == Right.Type && Left.uRegister == Right.uRegister;
	}

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::VERTEXELEMENTPARAMETEREQUAL::operator()(const ELEMENTPARAMETER& Left, const ELEMENTPARAMETER& Right)const
	{
		return Left.Type == Right.Type && Left.Usage == Right.Usage && Left.uIndex == Right.uIndex;
	}

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::TEMPORARYDECLARATIONEQUAL::operator()(const TEMPORARYDECLARATION& Left, const TEMPORARYDECLARATION& Right)const
	{
		return Left.ShaderType == Right.ShaderType && Left.VariableType == Right.VariableType/* && Left.uDeclaration == Right.uDeclaration*/;
	}

	/*ALCHEMY_INLINE bool CEffectCode::CPassDefiner::TEMPORARYEQUAL::operator()(const TEMPORARY& Left, const TEMPORARY& Right)const
	{
		return Left.ShaderType == Right.ShaderType && Left.VariableType == Right.VariableType/* && Left.uDeclaration == Right.uDeclaration && Left.uIndex == Right.uIndex;
	}*/

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::ATTRIBUTEVARIABLEKEYEQUAL::operator()(const ATTRIBUTEVARIABLEKEY& Left, const ATTRIBUTEVARIABLEKEY& Right)const
	{
		return Left.ShaderType == Right.ShaderType && Left.ElementType == Right.ElementType && Left.ElementUsage == Right.ElementUsage && Left.uIndex == Right.uIndex;
	}

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::VARYINGEQUAL::operator()(const VARYING& Left, const VARYING& Right)const
	{
		return Left.uVertexShaderHandle == Right.uVertexShaderHandle && Left.uFragmentShaderHandle == Right.uFragmentShaderHandle && Left.IsUsed == Right.IsUsed;
	}

	ALCHEMY_INLINE bool CEffectCode::GLOBALVARIABLEKEYEQUAL::operator()(const GLOBALVARIABLEKEY& Left, const GLOBALVARIABLEKEY& Right)const
	{
		return Left.ShaderType == Right.ShaderType && Left.GlobalType == Right.GlobalType && Left.uCount == Right.uCount && Left.uDetail == Right.uDetail && Left.pValue == Right.pValue;
	}

	ALCHEMY_INLINE bool CEffectCode::GLOBALVARIABLEEQUAL::operator()(const GLOBALVARIABLE& Left, const GLOBALVARIABLE& Right)const
	{
		return Left.ShaderType == Right.ShaderType && Left.GlobalType == Right.GlobalType && Left.uDetail == Right.uDetail && Left.uCount == Right.uCount && Left.uIndex == Right.uIndex && Left.pValue == Right.pValue;
	}

	ALCHEMY_INLINE bool CEffectCode::ADDRESSVARIABLEEQUAL::operator()(const ADDRESSVARIABLE& Left, const ADDRESSVARIABLE& Right)const
	{
		return Left.uParent == Right.uParent && Left.uAddress == Right.uAddress;
	}

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::FREETEMPORARYEQUAL::operator()(const FREETEMPORARY& Left, const FREETEMPORARY& Right)const
	{
		return Left.uSortValue == Right.uSortValue && Left.uHandle == Right.uHandle;
	}
}