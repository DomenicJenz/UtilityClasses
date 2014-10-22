/*
 * Stream.h
 *
 *  Created on: 21.10.2014
 *      Author: domenicjenz
 */

#ifndef __STREAM_H__
#define __STREAM_H__

#include "Optional.h"
#include <functional>

namespace Utilities
{
template<typename T>
struct Identity
{
	typedef T type;
};

template<typename ElementType>
class Stream
{
public:
	Stream () = default;
	virtual ~Stream () = default;

	virtual void reset () = 0;

	void forEach (std::function<void (const ElementType&)> eachFunc)
	{
		Optional<ElementType> currentVal;
		while ((currentVal = getNext()).hasValue ())
		{
			eachFunc (currentVal.getValue());
		}
	}

//	template<typename ResultType>
//	ResultType foldRight (std::function<ResultType (ElementType, ResultType)> foldFunc, const ResultType& initVal)
//	{
//		ResultType result = initVal;
//		Optional<ElementType> currentVal;
//		while ((currentVal = getNext()).hasValue ())
//		{
//			result = foldFunc (currentVal.getValue (), result);
//		}
//		return result;
//	}

	/// Identity<std::function...> needed, so lambdas can be used directly ?! It tricks the type deduction.
	template<typename ResultType>
	ResultType foldLeft (typename Identity<std::function<ResultType (ResultType, ElementType)> >::type foldFunc, const ResultType& initVal)
	{
		ResultType result = initVal;
		Optional<ElementType> currentVal;
		while ((currentVal = getNext()).hasValue ())
		{
			result = foldFunc (result, currentVal.getValue ());
		}
		return result;
	}


private:
	virtual Optional<ElementType> getNext () = 0;

};

}

#endif /* __STREAM_H__ */
