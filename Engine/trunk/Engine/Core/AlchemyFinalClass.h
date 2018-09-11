#pragma once

namespace alchemy
{
#define ALCHEMY_FINAL_CLASS(ClassName) class ClassName : virtual public CFinalClass<ClassName>
#define ALCHEMY_FINAL_CLASS_EXTENDS(ClassName) ALCHEMY_FINAL_CLASS(ClassName), 

	template<class T>
	class CFinalClass
	{
		friend T;

	private:
		CFinalClass(void)
		{
		}

		~CFinalClass(void)
		{
		}
	};
}