#ifndef ALCHEMY_FUNCTION
#define ALCHEMY_FUNCTION

namespace alchemy
{
	template<typename T>
	struct BASICLESS
	{
		bool operator()(const T& Left, const T& Right)const
		{
			return Left < Right;
		}
	};

	template<typename T>
	struct BASICEQUAL
	{
		bool operator()(const T& Left, const T& Right)const
		{
			return Left == Right;
		}
	};
}

#endif