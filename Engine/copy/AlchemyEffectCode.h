#pragma once

#include "AlchemyCompilerInterface.h"
#include "AlchemyRenderMethod.h"

#include "../Core/AlchemyList.h"
#include "../Core/AlchemyMap.h"
#include "../Core/AlchemySmartClass.h"
#include "../Core/AlchemySmartPointer.h"
//#include "AlchemyRenderMethodInterface.h"

namespace alchemy
{
	struct IEffectInfo;
	struct IEffectInfoBlendMethod;
	class CEffectCode
	{
	public:
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
			NORMAL_VARIABLE = 0,
			ARRAY_VARIABLE,
			//MATRIX_VARIABLE,
			CAMERA_VARIABLE,
			MATERIAL_VARIABLE,
			LIGHT_VARIABLE,
			CONSTANT_VARIABLE,
			TEXTURE_VARIABLE,
			INPUT_VARIABLE, 
			BUFFER_VARIABLE
		}GLOBALVARIABLETYPE;
		
		typedef enum
		{
			GLOBAL_CODE_SHIFT = 12, 
			GLOBAL_CODE_MASK  = (1 << GLOBAL_CODE_SHIFT) - 1,
			
			VECTOR1_SIZE = 1 | (1 << GLOBAL_CODE_SHIFT),
			VECTOR2_SIZE = 1 | (2 << GLOBAL_CODE_SHIFT),
			VECTOR3_SIZE = 1 | (3 << GLOBAL_CODE_SHIFT),
			VECTOR4_SIZE = 1 | (4 << GLOBAL_CODE_SHIFT),

			MATRIX2X2_SIZE = 1 | (5 << GLOBAL_CODE_SHIFT),
			MATRIX3X3_SIZE = 1 | (6 << GLOBAL_CODE_SHIFT),
			MATRIX4X3_SIZE = 1 | (7 << GLOBAL_CODE_SHIFT),
			MATRIX4X4_SIZE = 1 | (8 << GLOBAL_CODE_SHIFT),
			
			BUFFER_STRIDE_SIZE = 1 | (1 << 1)
		}GLOBALCODECONSTANTS;

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

		/*typedef struct
		{
			IEffect::EFFECTTYPE ShaderType;
			GLOBALVARIABLETYPE           GlobalType;
			UINT                         uDetail;
			UINT                         uCount;
			const void*                  pValue;
		}GLOBALVARIABLEKEY;*/

		typedef struct
		{
			IEffect::EFFECTTYPE ShaderType;
			GLOBALVARIABLETYPE  GlobalType;
			UINT                uDetail;
			UINT                uCount;
			UINT                uIndex;
			const void*         pValue;
		}GLOBALVARIABLE, * LPGLOBALVARIABLE;
		
		typedef struct
		{
			IEffect::EFFECTTYPE Type;
			UINT uDeclaration;
			UINT uRegisterCount;
		}CONSTANTBUFFERDECLARATION, * LPCONSTANTBUFFERDECLARATION;

		typedef struct  
		{
			VARIABLETYPE            VariableType;
			ICompiler::REGISTERTYPE RegisterType;
			UINT                    uRegister;
			UINT                    uDeclaration;
			UINT                    uIndex;
			UINT                    uCount;
			UINT                    uNext;
		}USERVARIABLE, * LPUSERVARIABLE;

		typedef struct  
		{
			ICompiler::INSTRUCTION Opcode;

			UINT uCodeBegin;
			UINT uCodeLength;
			UINT uDestination;
			UINT uDestinationAddress;
			UINT uDestinationAddressInfo;
			UINT uDestinationIndex;
			UINT uDestinationIndexAddress;
			UINT uSource1;
			UINT uSourceAddress1;
			UINT uSourceAddressInfo1;
			UINT uSourceIndex1;
			UINT uSourceIndexAddress1;
			UINT uSource2;
			UINT uSourceAddress2;
			UINT uSourceAddressInfo2;
			UINT uSourceIndex2;
			UINT uSourceIndexAddress2;
			UINT uPrevious;
		}INSTRUCTION, * LPINSTRUCTION;

		typedef struct  
		{
			UINT uVertexCode;
			UINT uFragmentCode;
			UINT uVertexUserVariables;
			UINT uFragmentUserVariables;
			UINT uVertexConstantInstructionSet;
			UINT uFragmentConstantInstructionSet;
			UINT uIndex;
			UINT uNext;
		}RENDERTARGETCODE, * LPRENDERTARGETCODE;

		typedef struct
		{
			UINT uCode;
			//UINT uUserVariables;
			UINT uParametersVariable;
			UINT uResultVariable;
			UINT uShader;
			UINT uInstructionSet;
		}FUNCTION, * LPFUNCTION;

		typedef struct  
		{
			CVertexDeclaration::ELEMENTTYPE  Type;
			CVertexDeclaration::ELEMENTUSAGE Usage;
			UINT         uIndex;
		}ELEMENTDECLARATION, * LPELEMENTDECLARATION;

		/*typedef struct
		{
			CVertexDeclaration::ELEMENTTYPE  Type;
			CVertexDeclaration::ELEMENTUSAGE Usage;
		}SHADERPARAMETERKEY, * LPSHADERPARAMETERKEY;*/

		typedef struct
		{
			CVertexDeclaration::ELEMENTTYPE  Type;
			CVertexDeclaration::ELEMENTUSAGE Usage;
			UINT                             uIndex;
			UINT                             uNext;
		}SHADERPARAMETER, * LPSHADERPARAMETER;

		typedef struct
		{
			IEffect::EFFECTTYPE Type;
			UINT                         uParameters;
			//UINT                   uFunction;
		}SHADERFUNCTION, * LPSHADERFUNCTION;

		typedef struct  
		{
			UINT uVertexShader;
			UINT uFragmentShader;
			//UINT uVertexShaderCode;
			UINT uRenderTargetCodes;
			//UINT uVertexUserVariables;
			UINT uConstantInstructionSet;
		}PASS, * LPPASS;

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
			IEffect::EFFECTTYPE ShaderType;
			VARIABLETYPE        VariableType;
			UINT                uCount;
		}TEMPORARYDECLARATION, * LPTEMPORARYDECLARATION;

		typedef struct  
		{
			IEffect::EFFECTTYPE              ShaderType;
			CVertexDeclaration::ELEMENTTYPE  ElementType;
			CVertexDeclaration::ELEMENTUSAGE ElementUsage;
			UINT                             uIndex;
			bool                             bIsReadOnly;
			bool                             bIsForce;
		}ATTRIBUTEVARIABLEDECLARATION, * LPATTRIBUTEVARIABLEDECLARATION;

		typedef struct
		{
			IEffect::EFFECTTYPE ShaderType;
			GLOBALVARIABLETYPE           GlobalType;
			UINT                         uDetail;
			UINT                         uCount;
			UINT                         uIndex;
			const void*                  pValue;
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

		/*typedef struct GLOBALVARIABLEKEYLESS
		{
			bool operator()(const GLOBALVARIABLEKEY& Left, const GLOBALVARIABLEKEY& Right)const;
		}GLOBALVARIABLEKEYLESS;

		typedef struct GLOBALVARIABLEKEYEQUAL
		{
			bool operator()(const GLOBALVARIABLEKEY& Left, const GLOBALVARIABLEKEY& Right)const;
		}GLOBALVARIABLEKEYEQUAL;*/

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

		/*ALCHEMY_INTERFACE(IShaderParametersHandler)
		{
			virtual bool SetParameters(IEffect& Effect, UINT uPass) = 0;
		};*/
		
		class CRegisterApplicant
		{
			typedef struct  
			{
				UINT uBegin;
				UINT uCount;
			}REGISTERSEGMENT, * LPREGISTERSEGMENT;
		public:
			CRegisterApplicant(CEffectCode& Parent);
			~CRegisterApplicant();
			
			UINT Malloc(VARIABLETYPE Type, UINT uDeclaration, UINT uCount);
			void Free(UINT uRegister, VARIABLETYPE Type, UINT uDeclaration, UINT uCount);
			
			void Clear();
		private:
			UINT m_uRegisterCount;
			CPool<REGISTERSEGMENT> m_RegisterSegmentPool;
			CEffectCode& m_Parent;
		};

		class CGlobalInputHandler :
			public CSmartClass<CRenderMethod::IShaderParametersHandler>
		{
			friend class CEffectCode;

			//Double buffer cmp static handler.
			typedef struct  
			{
				IEffect::EFFECTTYPE          ShaderType;
				UINT                         uCode;
				UINT                         uIndex;
				UINT                         uCount;
				const void*                  pValue;
				bool                         bIsStatic;
			}GLOBALINPUT, * LPGLOBALINPUT;

			typedef struct  
			{
				const void* pValue;
				UINT uIndex;
				UINT uSize;
			}INPUTVALUE, * LPINPUTVALUE;

			typedef struct
			{
				ICompiler::INSTRUCTION Opcode;

				UINT uDestination;
				UINT uDestinationSize;
				UINT uDestinationAddress;
				UINT uDestinationIndex;
				UINT uDestinationIndexSize;
				UINT uDestinationIndexAddress;
				UINT uSource1;
				UINT uSourceSize1;
				UINT uSourceAddress1;
				UINT uSourceIndex1;
				UINT uSourceIndexSize1;
				UINT uSourceIndexAddress1;
				UINT uSource2;
				UINT uSourceSize2;
				UINT uSourceAddress2;
				UINT uSourceIndex2;
				UINT uSourceIndexSize2;
				UINT uSourceIndexAddress2;
			}GLOBALINSTRUCTION, * LPGLOBALINSTRUCTION;
		public:
			CGlobalInputHandler();
			virtual ~CGlobalInputHandler();

			const CRenderMethod::PARAMETERLIST* GetParameterlist(UINT uIndex)const;

			bool SetParameters(IEffect& Effect, UINT uIndex, bool bIsActivate);
			bool SetParameters(const void* pValue, UINT uBufferIndex, UINT uSize);
			bool SetParameters(const FLOAT* pfValues, UINT uBufferIndex, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount);
			bool SetParameters(const PFLOAT* ppfValues, UINT uBufferIndex, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount);

			virtual void Destroy();

		private:
			LPGLOBALINPUT* m_ppGlobalInput;
			PUINT m_puGlobalInputCount;
			CRenderMethod::LPPARAMETERLIST* m_ppParameterList;
			CList<GLOBALINSTRUCTION>* m_pInstructionSet;
			LPINPUTVALUE* m_ppInputValue;
			PUINT m_puInputValueCount;
			UINT m_uInputCount;
			PVOID m_pParameterBuffer;
			UINT m_uParameterBufferSize;
			CVector< CSmartPointer<CBuffer> > m_AddressBuffers;
		};

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
		
		class CConstantBufferDefiner :
			public CDefiner
		{
		public:
			CConstantBufferDefiner(CEffectCode& Parent);
			~CConstantBufferDefiner();
			
			bool BeginBuffer(IEffect::EFFECTTYPE Type);
			UINT AddConstant(GLOBALVARIABLETYPE GlobalType, UINT uDetail, UINT uCount = 1, UINT uIndex = 0);
			
			UINT EndDefine();
			
		private:
			LPCONSTANTBUFFERDECLARATION m_pConstantBufferDeclaration;
			CStructDefiner m_StructDefiner;
		};

		ALCHEMY_INTERFACE(IParameters)
		{
			virtual void AppendParameter(CVertexDeclaration::ELEMENTTYPE Type, CVertexDeclaration::ELEMENTUSAGE Usage, UINT uIndex) = 0;
		};

		class CShaderParametersDefiner :
			public CDefiner, public CSmartClass<IParameters>
		{
			/*typedef struct SHADERPARAMETERKEYLESS
			{
				bool operator()(const SHADERPARAMETERKEY& Left, const SHADERPARAMETERKEY& Right)const;
			}SHADERPARAMETERKEYLESS;

			typedef struct SHADERPARAMETERKEYEQUAL
			{
				bool operator()(const SHADERPARAMETERKEY& Left, const SHADERPARAMETERKEY& Right)const;
			}SHADERPARAMETERKEYEQUAL;*/
		public:
			CShaderParametersDefiner(CEffectCode& Parent);
			~CShaderParametersDefiner();

			//const UINT* GetShaderParameterKeyCount(const SHADERPARAMETERKEY& ShaderParameterKey);

			void AppendParameter(CVertexDeclaration::ELEMENTTYPE Type, CVertexDeclaration::ELEMENTUSAGE Usage, UINT uIndex);

			UINT EndDefine();

		private:
			LPSHADERPARAMETER m_pShaderParameter;
			//CMap<SHADERPARAMETERKEY, UINT, SHADERPARAMETERKEYLESS, SHADERPARAMETERKEYEQUAL> m_ShaderParameterCountMap;
		};

		ALCHEMY_INTERFACE(IInstructionSet)
		{
			virtual UINT ApplyVariable(VARIABLETYPE Type, UINT uCount) = 0;
			virtual bool FreeVariable(UINT uHandle) = 0;

			virtual bool AppendInstruction(
				ICompiler::INSTRUCTION Instruction,
				UINT uDestination,
				UINT uSource1,
				UINT uSource2) = 0;

			virtual bool AppendInstruction(
				ICompiler::INSTRUCTION Instruction,
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
				ICompiler::INSTRUCTION Instruction, 
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
			public CDefiner, public CSmartClass<IInstructionSet>
		{
			/*typedef struct  
			{
				UINT uBegin;
				UINT uCount;
			}REGISTERSEGMENT, * LPREGISTERSEGMENT;*/

			/*typedef struct  
			{
				ICompiler::REGISTERTYPE Type;
				UINT uRegister;
			}USERVARIABLEKEY, * LPUSERVARIABLEKEY;

			typedef struct USERVARIABLEKEYLESS
			{
				bool operator()(const USERVARIABLEKEY& Left, const USERVARIABLEKEY& Right)const;
			}USERVARIABLEKEYLESS;

			typedef struct USERVARIABLEKEYEQUAL
			{
				bool operator()(const USERVARIABLEKEY& Left, const USERVARIABLEKEY& Right)const;
			}USERVARIABLEKEYEQUAL;*/
		public:
			CBasicFunctionDefiner(CEffectCode& Parent);
			~CBasicFunctionDefiner();

			bool AppendInstruction(
				ICompiler::INSTRUCTION Instruction,
				UINT uDestination,
				UINT uSource1,
				UINT uSource2);

			bool AppendInstruction(
				ICompiler::INSTRUCTION Instruction,
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
				ICompiler::INSTRUCTION Instruction, 
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
			
			UINT ApplyConstant(UINT uVariable);

			bool FreeVariable(UINT uHandle);

			UINT AddSamplerVariable(const IEffect::SAMPLERDECLARATION& Sampler, UINT uIndex);

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
			IEffect::EFFECTTYPE m_Type;

		private:
			UINT __ApplyUserVariables();
			UINT __ApplyInstructionSet();

			STRING m_Code;
			LPFUNCTION m_pFunction;
			//UINT m_uTemporaryRegisterCount;

			//CMap<USERVARIABLEKEY, UINT, USERVARIABLEKEYLESS, USERVARIABLEKEYEQUAL> m_UserVariableDeclarationMap;
			//CMap<UINT           , UINT> m_VariableCountMap;
			//CPool<REGISTERSEGMENT> m_RegisterSegmentPool;
			CRegisterApplicant m_TemporaryRegisterApplicant;
			CVector<INSTRUCTION> m_InstructionSet;
			
			CMap<UINT, UINT> m_VariableConstantMap;
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
				IEffect::EFFECTTYPE     ShaderType;
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
				IEffect::EFFECTTYPE ShaderType;
				VARIABLETYPE                 VariableType;
				UINT                         uCount;
				UINT                         uHandle;
			}TEMPORARY;

			typedef struct ELEMENTLESS
			{
				bool operator()(const CEffectCode::ELEMENTDECLARATION& Left, const CEffectCode::ELEMENTDECLARATION& Right)const;
			}ELEMENTLESS;

			typedef struct ELEMENTEQUAL
			{
				bool operator()(const CEffectCode::ELEMENTDECLARATION& Left, const CEffectCode::ELEMENTDECLARATION& Right)const;
			}ELEMENTEQUAL;

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

			/*typedef enum
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
			}BLENDMETHOD;*/

			typedef struct
			{
				IEffectInfo           * pEffectInfo;
				IEffectInfoBlendMethod* pBlendMethod;
				UINT                             uInputColorKey;
				UINT                             uSourceColorKey;
				UINT                             uOutputColorKey;
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

			bool AddInfo(IEffectInfo& Info, IEffectInfoBlendMethod* pBlendMethod, UINT uInputColorKey = 0, UINT uOutputColorKey = 0);
			bool AddInfo(UINT uInputColorKey, IEffectInfoBlendMethod* pBlendMethod, UINT uSourceColorKey = 0, UINT uOutputColorKey = 0);
			//bool AddInfo(IEffectInfo& Info, BLENDMETHOD                       BlendMethod);

			UINT AddColorKey(CVertexDeclaration::ELEMENTTYPE Type = CVertexDeclaration::FLOAT4, bool bIsResultColor = false);

			UINT EndDefine();

		private:
			typedef struct
			{
				CVertexDeclaration::ELEMENTTYPE Type;
				UINT uVariable;
				UINT uRef;
				UINT uResultVariableAddress;
			}COLORKEY, * LPCOLORKEY;

			UINT __PopFreeTemporary(const TEMPORARYDECLARATION& TemporaryDeclaration);
			void __PushFreeTemporary(const TEMPORARYDECLARATION& TemporaryDeclaration, UINT uHandle, bool bIsCached);

			void __UpdateTemporary();
			void __FreeTemporary();

			bool __IsValuableColorKey(UINT uKey);
			UINT __GetColorKey(UINT uKey);
			UINT __GetColorValue(UINT uKey, UINT uResultVariable);
			bool __PushColorKey(UINT uKey);

			static const TEMPORARYDECLARATION sm_COLOR_KEY_TEMPORARYDECLARATION;

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

			UINT m_uVariableInfoCount;

			UINT m_uFinalOutputEffectInfoIndex;

			UINT m_uConstantVaryingCount;

			bool m_bIsBlend;

			CMap<ELEMENTDECLARATION, UINT                  , ELEMENTLESS             , ELEMENTEQUAL             > m_AttributeAddressMap;
			CMap<ELEMENTDECLARATION, UINT                  , ELEMENTLESS             , ELEMENTEQUAL             > m_VaryingAddressMap;

			CMap<ELEMENTDECLARATION, UINT                  , ELEMENTLESS             , ELEMENTEQUAL             > m_AttributeMap;
			CMap<ELEMENTDECLARATION, VARYING               , ELEMENTLESS             , ELEMENTEQUAL             > m_VaryingMap;

			//CMap<TEMPORARYDECLARATION, UINT                                                       , TEMPORARYDECLARATIONLESS  , TEMPORARYDECLARATIONEQUAL  > m_TemporaryCountMap;
			//CMap<TEMPORARY           , UINT                                                       , TEMPORARYLESS             , TEMPORARYEQUAL             > m_TemporaryHandleMap;

			CMap<ATTRIBUTEVARIABLEKEY, SHADERVARIABLE      , ATTRIBUTEVARIABLEKEYLESS, ATTRIBUTEVARIABLEKEYEQUAL> m_AttributeVariableMap;
			CMap<ATTRIBUTEVARIABLEKEY, UINT                , ATTRIBUTEVARIABLEKEYLESS, ATTRIBUTEVARIABLEKEYEQUAL> m_AttributeVariableMaxInfoMap;
			CMap<GLOBALVARIABLE      , SHADERVARIABLE      , GLOBALVARIABLELESS      , GLOBALVARIABLEEQUAL      > m_ConstantVariableMap;
			CMap<GLOBALVARIABLE      , UINT                , GLOBALVARIABLELESS      , GLOBALVARIABLEEQUAL      > m_ConstantVariableMaxInfoMap;

			CMap<ELEMENTDECLARATION  , SHADERVARIABLE      , ELEMENTLESS             , ELEMENTEQUAL             > m_AttributeShaderVariableMap;

			CMap<ELEMENTDECLARATION  , UINT                , ELEMENTLESS             , ELEMENTEQUAL             > m_AttributeVaryingMap;
			CMap<GLOBALVARIABLE      , ELEMENTDECLARATION  , GLOBALVARIABLELESS      , GLOBALVARIABLEEQUAL      > m_ConstantVaryingMap;

			CMap<TEMPORARYDECLARATION, CList<FREETEMPORARY>, TEMPORARYDECLARATIONLESS, TEMPORARYDECLARATIONEQUAL> m_FreeTemporaryMap;

			CVector<TEMPORARY>  m_FreeTemporaryCache;

			CVector<EFFECTINFO> m_EffectInfos;

			CPool<COLORKEY> m_ColorKeyPool;

			//CMap<GLOBALVARIABLE        , UINT   > m_ConstantMap;

			CShaderFunctionDefiner m_VertexShaderFunctionDefiner;
			CShaderFunctionDefiner m_FragmentShaderFunctionDefiner;

			CShaderParametersDefiner m_AttributesDefiner;
			CShaderParametersDefiner m_VaryingDefiner;

			CEffectCodeUtils m_EffectCodeUtils;
		};

		CEffectCode(ICompiler& Compiler);
		~CEffectCode(void);

		static bool IsElementVariable(VARIABLETYPE VariableType);

		static UINT BuildGlobalInputCode(VARIABLETYPE VariableType, UINT uRowOffset = 0, UINT uOffset = 0, bool bIsPointer = false);

		static void GetGlobalInputInfo(UINT uCode, VARIABLETYPE* pVariableType = ALCHEMY_NULL, PUINT puRowOffset = ALCHEMY_NULL, PUINT puOffset = ALCHEMY_NULL, bool* pbIsPointer = ALCHEMY_NULL);

		static VARIABLETYPE GetGlobalVariableType(GLOBALVARIABLETYPE GlobalVariableType, UINT uDetail);
		
		//UINT GetVertexShaderGlobalRegisterCount()const;
		//UINT GetFragmentShaderGlobalRegisterCount()const;

		UINT GetPassCount()const;
		ICompiler& GetCompiler();

		const VARIABLE                   * GetVariable          (UINT uHandle)const;
		const GLOBALVARIABLE             * GetGlobalVariable    (UINT uHandle)const;

		const USERVARIABLE               * GetUserVariable      (UINT uHandle)const;
		const UINT                       * GetUserVariables     (UINT uHandle)const;

		const IEffect::SAMPLERDECLARATION* GetSamplerDeclaration(UINT uHandle)const;

		const STRUCTDECLARATION          * GetStructDeclaration (UINT uHandle)const;
		const UINT                       * GetStruct            (UINT uHandle)const;

		const STRING                     * GetCode              (UINT uHandle)const;
		const RENDERTARGETCODE           * GetRenderTargetCode  (UINT uHandle)const;
		const UINT                       * GetRenderTargetCodes (UINT uHandle)const;

		const FUNCTION                   * GetFunction          (UINT uHandle)const;

		const SHADERPARAMETER            * GetShaderParameter   (UINT uHandle)const;
		const UINT                       * GetShaderParameters  (UINT uHandle)const;

		const SHADERFUNCTION             * GetShaderFunction    (UINT uHandle)const;

		const PASS                       * GetPass              (UINT uHandle)const;
		
		UINT GetTemporaryVariableCount(UINT uTemporary)const;

		ICompiler::REGISTERTYPE GetRegisterType(UINT uVariable)const;

		UINT GetRegisterCount(VARIABLETYPE Type, UINT uDeclaration = 0, bool bIsByteAlignment = true)const;

		UINT GetAddressVariable(UINT uSource, UINT uAddress, PUINT puRegisterCount = ALCHEMY_NULL);

		void Clear();

		UINT AddGlobalVariable(const GLOBALVARIABLE& GlobalVariable);
		//UINT AddSamplerVariable(const SAMPLERDECLARATION& Sampler, UINT uIndex);
		UINT AddPass(UINT uVertexShaderFunction, UINT uFragmentShaderFunction);

		void AppendCodeInstruction(STRING& Code, UINT uInstruction, const STRING& Source, PUINT puUserVaraible = ALCHEMY_NULL, UINT uDestination = 0, bool bIsUpdateMap = true, PUINT puConstantInstruction = ALCHEMY_NULL);
 
		bool BuildShaderFunctionCode(STRING& Desination, IEffect::EFFECTTYPE Type, UINT uUserVariables, UINT uCode);

		UINT SerializeAddress(const UINT* puAddress, UINT uAddressCount);
		void UnserializeAddress(UINT uAddress, PUINT puOutputAddress = ALCHEMY_NULL, PUINT puOutputAddressCount = ALCHEMY_NULL);

		void UnserializeInstructionAddress(
			UINT uSourceAddress, 
			UINT uDestinationAddress, 
			const UINT* puAddress,
			UINT uAddressCount, 
			PUINT puOutputAddress, 
			PUINT puOutputAddressInfo = ALCHEMY_NULL,
			PUINT puOutputAddressCount = ALCHEMY_NULL);

		CBuffer& CreatePassInfos(PUINT puRenderTargetIndexCount);
		CBuffer& CreateParameterList(CGlobalInputHandler** ppGlobalInputHandler, PUINT puRenderTargetIndexCount);
	private: 
		ICompiler&                                                                   m_Compiler;

		CPool<VARIABLE>                                                              m_VariablePool;
		CPool<GLOBALVARIABLE>                                                        m_GlobalVariablePool;

		CPool<USERVARIABLE>                                                          m_UserVariablePool;
		CPool<UINT>                                                                  m_UserVariablesPool;
		
		CPool<CONSTANTBUFFERDECLARATION>                                             m_ConstantBufferDeclarationPool;

		CPool<IEffect::SAMPLERDECLARATION>                                           m_SamplerDeclarationPool;

		CPool<STRUCTDECLARATION>                                                     m_StructDeclarationPool;
		CPool<UINT>                                                                  m_StructPool;
  
		CPool<STRING>                                                                m_CodePool;

		CPool<INSTRUCTION>                                                           m_InstructionPool;
		CPool<UINT>                                                                  m_InstructionSetPool;

		CPool<RENDERTARGETCODE>                                                      m_RenderTargetCodePool;
		CPool<UINT>                                                                  m_RenderTargetCodesPool;

		CPool<FUNCTION>                                                              m_FunctionPool;

		CPool<SHADERPARAMETER>                                                       m_ShaderParameterPool;
		CPool<UINT>                                                                  m_ShaderParametersPool;

		CPool<SHADERFUNCTION>                                                        m_ShaderFunctionPool;

		CPool< CSmartPointer<CBuffer> >                                              m_BufferPool;

		CVector<PASS>                                                                m_Passes;
		
		CVector< CVector<UINT> >                                                     m_CommonSamplers;
		
		CMap<UINT             , UINT                                               > m_BufferSamplerMap;

		//CMap<GLOBALVARIABLEKEY, UINT, GLOBALVARIABLEKEYLESS, GLOBALVARIABLEKEYEQUAL> m_GlobalVariableCountMap;

		CMap<GLOBALVARIABLE   , UINT, GLOBALVARIABLELESS   , GLOBALVARIABLEEQUAL   > m_GlobalVariableHandleMap;

		CMap<ADDRESSVARIABLE  , UINT, ADDRESSVARIABLELESS  , ADDRESSVARIABLEEQUAL  > m_AddressVariableHandleMap;
		
		CMap<UINT             , UINT                                               > m_TemporaryVariableCountMap;
		
		CMap<UINT             , UINT                                               > m_GlobalConstantBufferMap;

		//UINT                                                                         m_uVertexShaderGlobalRegisterCount;
		//UINT                                                                         m_uFragmentShaderGlobalRegisterCount;
		UINT                                                                         m_uBufferSamplerDeclaration;
		
		CRegisterApplicant                                                           m_VertexGlobalReigsterApplicant;
		CRegisterApplicant                                                           m_FragmentGlobalReigsterApplicant;

		CGlobalInputHandler*                                                         m_pCurrentGlobalInputHandler;
	};

	ALCHEMY_INLINE CEffectCode& CEffectCode::CDefiner::GetParent()
	{
		return m_Parent;
	}

	ALCHEMY_INLINE const CRenderMethod::PARAMETERLIST* CEffectCode::CGlobalInputHandler::GetParameterlist(UINT uIndex)const
	{
		if(uIndex < m_uInputCount && m_ppParameterList)
			return m_ppParameterList[uIndex];

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE UINT CEffectCode::CDefiner::_GetHandle(const CEffectCode::CDefiner& Definer)
	{
		return Definer.m_uHandle;
	}
	
	/*ALCHEMY_INLINE UINT CEffectCode::GetVertexShaderGlobalRegisterCount()const
	{
		return m_uVertexShaderGlobalRegisterCount;
	}
	
	ALCHEMY_INLINE UINT CEffectCode::GetFragmentShaderGlobalRegisterCount()const
	{
		return m_uFragmentShaderGlobalRegisterCount;
	}*/

	ALCHEMY_INLINE UINT CEffectCode::GetPassCount()const
	{
		return m_Passes.GetLength();
	}

	ALCHEMY_INLINE ICompiler& CEffectCode::GetCompiler()
	{
		return m_Compiler;
	}

	/*ALCHEMY_INLINE bool CEffectCode::CShaderParametersDefiner::SHADERPARAMETERKEYLESS::operator()(const SHADERPARAMETERKEY& Left, const SHADERPARAMETERKEY& Right)const
	{
		if(Left.Type < Right.Type)
			return true;
		else if(Left.Type == Right.Type && Left.Usage < Right.Usage)
			return true;

		return false;
	}*/

	/*ALCHEMY_INLINE bool CEffectCode::CBasicFunctionDefiner::USERVARIABLEKEYLESS::operator()(const USERVARIABLEKEY& Left, const USERVARIABLEKEY& Right)const
	{
		if(Left.Type < Right.Type)
			return true;
		else if(Left.Type == Right.Type && Left.uRegister < Right.uRegister)
			return true;

		return false;
	}*/

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::ELEMENTLESS::operator()(const CEffectCode::ELEMENTDECLARATION& Left, const CEffectCode::ELEMENTDECLARATION& Right)const
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

	/*ALCHEMY_INLINE bool CEffectCode::GLOBALVARIABLEKEYLESS::operator()(const GLOBALVARIABLEKEY& Left, const GLOBALVARIABLEKEY& Right)const
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
	}*/

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

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::ELEMENTEQUAL::operator()(const CEffectCode::ELEMENTDECLARATION& Left, const CEffectCode::ELEMENTDECLARATION& Right)const
	{
		return Left.Type == Right.Type && Left.Usage == Right.Usage && Left.uIndex == Right.uIndex;
	}

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::TEMPORARYDECLARATIONEQUAL::operator()(const TEMPORARYDECLARATION& Left, const TEMPORARYDECLARATION& Right)const
	{
		return Left.ShaderType == Right.ShaderType && Left.VariableType == Right.VariableType && Left.uCount == Right.uCount/* && Left.uDeclaration == Right.uDeclaration*/;
	}

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::ATTRIBUTEVARIABLEKEYEQUAL::operator()(const ATTRIBUTEVARIABLEKEY& Left, const ATTRIBUTEVARIABLEKEY& Right)const
	{
		return Left.ShaderType == Right.ShaderType && Left.ElementType == Right.ElementType && Left.ElementUsage == Right.ElementUsage && Left.uIndex == Right.uIndex;
	}

	ALCHEMY_INLINE bool CEffectCode::CPassDefiner::VARYINGEQUAL::operator()(const VARYING& Left, const VARYING& Right)const
	{
		return Left.uVertexShaderHandle == Right.uVertexShaderHandle && Left.uFragmentShaderHandle == Right.uFragmentShaderHandle && Left.IsUsed == Right.IsUsed;
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