#pragma once

#include "AlchemyProgramInstance.h"

namespace alchemy
{
	class CSceneObject;

	class CEnvironmentProgram : 
		public CProgramInstance
	{
	public:
		CEnvironmentProgram();
		virtual ~CEnvironmentProgram(void);

		LPTEXTURE GetEnvironmentTexture();

		bool Create(UINT uEnvironmentSize);

		virtual bool OnPreRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag);

	protected:
		LPTEXTURE			m_pEnvironmentTexture;
	};

	ALCHEMY_INLINE LPTEXTURE CEnvironmentProgram::GetEnvironmentTexture()
	{
		return m_pEnvironmentTexture;
	}
}