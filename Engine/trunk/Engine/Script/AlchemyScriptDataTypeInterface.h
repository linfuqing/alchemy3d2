#pragma once

#include "AlchemyScriptInstructionSet.h"

#include "../Core/AlchemyInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IScriptDataType)
	{
		virtual UINT GetSize()const = 0;

		virtual void Compute(const CScriptInstructionSet::INSTRUCTION* pInstructionSet, CScriptMemery& Memery, const CScriptInstructionSet::ERRORINFO* pErrorInfos, UINT uErrorInfoCount)const = 0;
	};
}