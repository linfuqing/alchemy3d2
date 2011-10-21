#pragma once

#include "../Core/AlchemyDataType.h"
#include "../Core/AlchemyReferenceCounter.h"

namespace alchemy
{
	class CSceneObject;
	class CEffectInfoInstance : public CReferenceCounter
	{
		friend class CProgramInstance;
	public:
		CEffectInfoInstance();
		virtual ~CEffectInfoInstance();

		virtual void OnPassLogic(CSceneObject* pSceneObject);

		virtual void OnAddToProgram();
	};
}