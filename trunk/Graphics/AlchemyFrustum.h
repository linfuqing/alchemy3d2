#pragma once

#include "../SceneManagement/AlchemyClipperInterface.h"

namespace alchemy
{
	class CFrustum :
		public CClipperInterface
	{
	public:
		typedef enum
		{
			TOP_SIDE,
			BOTTOM_SIDE,
			LEFT_SIDE,
			RIGHT_SIDE,
			FRONT_SIDE,
			BACK_SIDE,

			TOTAL_SIDE
		}SIDE;

	public:
		CFrustum(void);
		~CFrustum(void);

		bool Test(const CBoundingVolumeInterface& BoundingVolume, UINT uTestFlag)const;

		bool Test(const CBoundingVolumeInterface& BoundingVolume)const;

		void Transform(const MATRIX4X4& Matrix);

		void ExtractFromMatrix(const MATRIX4X4& Matrix);

		void Normalize();

	private:
		CPlane	m_Planes[TOTAL_SIDE];
	};
}