#pragma once

namespace Utilities
{
	template<typename IntegerType>
	IntegerType abs (IntegerType victim)
	{
		return victim >= 0 ? victim : -victim;
	}

	template<typename NumberType>
	NumberType sgn (NumberType num)
	{
		if (num == 0)
		{
			return (NumberType) 0;
		}
		else if (num > 0)
		{
			return (NumberType) 1;
		}
		else
		{
			return (NumberType) -1;
		}
	}
}