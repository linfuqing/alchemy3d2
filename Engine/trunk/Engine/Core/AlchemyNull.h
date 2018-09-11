#pragma once

namespace alchemy
{
#define ALCHEMY_NULL g_NULL

	class CNull
	{
	public:
		CNull() {}
		~CNull() {}

		template<class TYPE>
		operator TYPE*() const
		{ 
			return 0; 
		} 

		template<class CLASS, class TYPE>
		operator TYPE CLASS::*() const
		{ 
			return 0; 
		}

	private:
		void operator&() const;

	};

	const CNull g_NULL;
}
