/*
 * Stream.h
 *
 *  Created on: 21.10.2014
 *      Author: domenicjenz
 */

#ifndef __STREAM_H__
#define __STREAM_H__

#include "Object.h"
#include "Optional.h"
#include <functional>
#include <list>

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
	virtual ~Stream ()
	{
	}

	virtual void reset () = 0;

	void forEach (std::function<void (const ElementType&)> eachFunc)
	{
		Optional<ElementType> currentVal;
		while ((currentVal = getNext()).hasValue ())
		{
			eachFunc (currentVal.getValue());
		}
	}

	/// Identity<std::function...> needed, so lambdas can be used directly ?! It tricks the type deduction.
	template<typename ResultType>
	ResultType foldRight (typename Identity<std::function<ResultType (ElementType, ResultType)> >::type foldFunc, const ResultType& initVal)
	{
		ResultType result = initVal;
		std::list<ElementType> allValues;
		Optional<ElementType> currentVal;
		while ((currentVal = getNext()).hasValue ())
		{
			allValues.push_front(currentVal.getValue());
		}
		for (const ElementType& elem : allValues)
		{
			result = foldFunc (elem, result);
		}
		return result;
	}

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

	//template<typename ResultType>
	//Stream<ResultType>& map (typename Identity<std::function<ResultType(ElementType)> >::type mapFunc);

	Stream<ElementType>& filter (typename Identity<std::function<bool(ElementType)> >::type filterFunc);

	//Stream<ElementType>& limit (unsigned int numberOfElements);

	//Stream<ElementType>& skip (unsigned int numberOfElements);

	virtual Optional<ElementType> getNext () = 0;
};

template<typename ElementType, typename ParentType = Stream<ElementType>>
class IntermediateStream : public Stream<ElementType>
{
public:
	IntermediateStream (ParentType* parentStream) : _parent(parentStream)
  {
  }
	virtual ~IntermediateStream () = default;

	void reset () override = 0;

	Optional<ElementType> getNext () override = 0;

protected:
	ParentType* _parent = nullptr;
};

template<typename ElementType, typename ParentType = Stream<ElementType>>
class FilterStream : public IntermediateStream<ElementType, ParentType>
{
public:
	using FuncType = std::function<bool(ElementType)>;
	FilterStream (ParentType* parentStream, typename Identity<FuncType>::type filterFunc)
		: IntermediateStream<ElementType, ParentType>(parentStream), _filterFunc(filterFunc)
	{}
	virtual ~FilterStream() {};

	void reset () override
	{
		this->_parent->reset();
	}

	Optional<ElementType> getNext () override
	{
		Optional<ElementType> current;
		bool found = false;
		while (!found && ((current = this->_parent->getNext()).hasValue()))
		{
			found = _filterFunc(current.getValue());
		}
		return current;
	}

private:
	FuncType _filterFunc;
};


//template<typename ResultType>
//Stream<ResultType>& map (typename Identity<std::function<ResultType(ElementType)> >::type mapFunc);
//

template<typename ElementType>
Stream<ElementType>& Stream<ElementType>::filter (typename Identity<std::function<bool(ElementType)> >::type filterFunc)
{
	FilterStream<ElementType, Stream<ElementType>>* filterStream = new FilterStream<ElementType, Stream<ElementType>>(this, filterFunc);
	return *filterStream;
}
//Stream<ElementType>& limit (unsigned int numberOfElements);
//
//Stream<ElementType>& skip (unsigned int numberOfElements);
}

#endif /* __STREAM_H__ */
