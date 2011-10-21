#pragma once

#include "AlchemyRGBColor.h"
#include "AlchemyARGBColor.h"
#include "AlchemyHighLevelGraphicsResource.h"
#include "AlchemyGraphicsBuffer.h"

#include "../Core/AlchemyBitFlag.h"
#include "../Core/AlchemyVector.h"

namespace alchemy
{
	class CSceneManager;
	class CMaterial :
		public CHighLevelGraphicsResource
	{
		friend class CSceneManager;

		typedef enum
		{
			TOTAL_TEXTURE_COUNT = ALCHEMY_TOTAL_BITS(UINT)
		}CONTAINS;
	public:
		CMaterial(void);
		~CMaterial(void);

		const CARGBColor& GetAmbient()const;

		const CRGBColor& GetDiffuse()const;
		const CRGBColor& GetSpecular()const;
		const CRGBColor& GetEmissive()const;

		FLOAT GetPower()const;

		const CVector<LPTEXTURE>& GetTextures()const;

		bool AddTexture(CGraphicsResource::HANDLE Texture);
		bool RemoveTexture(UINT uIndex);

		void Destroy();
	protected:
		CARGBColor m_Ambient;

		CRGBColor m_Diffuse;
		CRGBColor m_Specular;
		CRGBColor m_Emissive;
		FLOAT m_fPower;

	private:
		CVector<LPTEXTURE> m_Textures;
	};

	ALCHEMY_INLINE const CARGBColor& CMaterial::GetAmbient()const
	{
		return m_Ambient;
	}

	ALCHEMY_INLINE const CRGBColor& CMaterial::GetDiffuse()const
	{
		return m_Diffuse;
	}

	ALCHEMY_INLINE const CRGBColor& CMaterial::GetSpecular()const
	{
		return m_Specular;
	}

	ALCHEMY_INLINE const CRGBColor& CMaterial::GetEmissive()const
	{
		return m_Emissive;
	}

	ALCHEMY_INLINE FLOAT CMaterial::GetPower()const
	{
		return m_fPower;
	}

	ALCHEMY_INLINE const CVector<LPTEXTURE>& CMaterial::GetTextures()const
	{
		return m_Textures;
	}
}