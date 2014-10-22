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

	template<typename ResultType>
	Stream<ResultType>& map (typename Identity<std::function<ResultType(ElementType)> >::type mapFunc);

	Stream<ElementType>& filter (typename Identity<std::function<bool(ElementType)> >::type filterFunc);

	Stream<ElementType>& limit (unsigned long numberOfElements);

	virtual Optional<ElementType> getNext () = 0;
};

template<typename SourceType, typename ResultType = SourceType>
class IntermediateStream : public Stream<ResultType>
{
public:
	IntermediateStream (Stream<SourceType>* parentStream) : _parent(parentStream)
  {
  }
	virtual ~IntermediateStream () = default;

	void reset () override
	{
		_parent->reset ();
	}

	Optional<ResultType> getNext () override = 0;

protected:
	Stream<SourceType>* _parent = nullptr;
};

template<typename SourceType>
class FilterStream : public IntermediateStream<SourceType, SourceType>
{
public:
	using FuncType = std::function<bool(const SourceType&)>;
	FilterStream (Stream<SourceType>* parentStream, typename Identity<FuncType>::type filterFunc)
		: IntermediateStream<SourceType, SourceType>(parentStream), _filterFunc(filterFunc)
	{}
	virtual ~FilterStream() {};

	Optional<SourceType> getNext () override
	{
		Optional<SourceType> current;
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

template<typename SourceType>
class LimitingStream : public IntermediateStream<SourceType, SourceType>
{
public:
	LimitingStream (Stream<SourceType>* parentStream, unsigned long numberOfElements)
		: IntermediateStream<SourceType, SourceType>(parentStream), _numberOfElements(numberOfElements)
	{}
	virtual ~LimitingStream() {};

	void reset () override
	{
		_currentElement = 0;
		this->_parent->reset ();
	}

	Optional<SourceType> getNext () override
	{
		Optional<SourceType> result;
		if (_currentElement < _numberOfElements)
		{
			result = this->_parent->getNext();
			++_currentElement;
		}
		return result;
	}

private:
	unsigned long _numberOfElements;
	unsigned long _currentElement = 0;
};

template<typename SourceType, typename ResultType>
class MappingStream : public IntermediateStream<SourceType, ResultType>
{
public:
	using FuncType = std::function<ResultType(const SourceType&)>;
	MappingStream (Stream<SourceType>* parentStream, typename Identity<FuncType>::type mappingFunc)
		: IntermediateStream<SourceType, ResultType>(parentStream), _mappingFunc(mappingFunc)
	{}


	Optional<ResultType> getNext () override
	{
		Optional<SourceType> current = this->_parent->getNext();
		Optional<ResultType> result;
		if (current.hasValue())
		{
			result.setValue (_mappingFunc(current.getValue()));
		}
		return result;
	}

private:
	FuncType _mappingFunc;
};



template<typename ElementType>
template<typename ResultType>
Stream<ResultType>& Stream<ElementType>::map (typename Identity<std::function<ResultType(ElementType)> >::type mapFunc)
{
	MappingStream<ElementType, ResultType>* mapStream = new MappingStream<ElementType, ResultType>(this, mapFunc);
	return *mapStream;
}

template<typename ElementType>
Stream<ElementType>& Stream<ElementType>::filter (typename Identity<std::function<bool(ElementType)> >::type filterFunc)
{
	FilterStream<ElementType>* filterStream = new FilterStream<ElementType>(this, filterFunc);
	return *filterStream;
}

template<typename ElementType>
Stream<ElementType>& Stream<ElementType>::limit (unsigned long numberOfElements)
{
	LimitingStream<ElementType>* limitStream = new LimitingStream<ElementType>(this, numberOfElements);
	return *limitStream;
}
//
//Stream<ElementType>& skip (unsigned int numberOfElements);
}

#endif /* __STREAM_H__ */
