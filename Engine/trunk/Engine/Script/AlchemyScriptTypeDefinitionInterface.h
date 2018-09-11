#pragma once

#include "AlchemyScriptDataTypeInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IScriptTypeDefinition)
	{
		virtual UINT GetDataTypeCount()const = 0;
		virtual IScriptDataType* GetDataType(UINT uIndex)const = 0;
		virtual const UINT* GetCastOpcode(UINT uIndex)const = 0;
	};
}