#pragma once

#include "../Math/AlchemyFloat3.h"

namespace alchemy
{
	typedef struct  
	{
		union
		{
			struct
			{
				FLOAT fRight;
				FLOAT fTop;
				FLOAT fBack;

				FLOAT fLeft;
				FLOAT fBottom;
				FLOAT fFront;
			};

			struct
			{
				FLOAT3 Max;
				FLOAT3 Min;
			};
		};

	}BOUNDINGBOX;

	BOUNDINGBOX& Zero(BOUNDINGBOX& Destination);
	BOUNDINGBOX& Copy(BOUNDINGBOX& Destination, const BOUNDINGBOX& Source);
	FLOAT3& Size(FLOAT3& Output, const BOUNDINGBOX& Source);
	bool IsValuable(const BOUNDINGBOX& Destination);


	ALCHEMY_INLINE BOUNDINGBOX& Zero(BOUNDINGBOX& Destination)
	{
		Destination.fRight = Destination.fTop = Destination.fBack = Destination.fLeft = Destination.fBottom = Destination.fFront = 0.0f;

		return Destination;
	}

	ALCHEMY_INLINE BOUNDINGBOX& Copy(BOUNDINGBOX& Destination, const BOUNDINGBOX& Source)
	{
		Copy(Destination.Max, Source.Max);
		Copy(Destination.Min, Source.Min);

		return Destination;
	}

	ALCHEMY_INLINE FLOAT3& Size(FLOAT3& Output, const BOUNDINGBOX& Source)
	{
		Output.x = Source.fRight - Source.fLeft;
		Output.y = Source.fTop   - Source.fBottom;
		Output.z = Source.fBack  - Source.fFront;

		return Output;
	}

	ALCHEMY_INLINE bool IsValuable(const BOUNDINGBOX& Destination)
	{
		return Destination.fRight >= Destination.fLeft && Destination.fTop >= Destination.fBottom && Destination.fBack >= Destination.fFront;
	}

	class CBoundingBox :
		public BOUNDINGBOX
	{
	public:
		CBoundingBox(void) {}
		CBoundingBox(const BOUNDINGBOX& BoundingBox);
		~CBoundingBox(void);

		CBoundingBox& operator*=(const FLOAT3 Value);
		CBoundingBox& operator+=(const FLOAT3 Value);
	};

	ALCHEMY_INLINE CBoundingBox& CBoundingBox::operator*=(const FLOAT3 Value)
	{
		Multiply(Max, Max, Value);
		Multiply(Min, Min, Value);

		return *this;
	}

	ALCHEMY_INLINE CBoundingBox& CBoundingBox::operator+=(const FLOAT3 Value)
	{
		Add(Max, Max, Value);
		Add(Min, Min, Value);

		return *this;
	}
}