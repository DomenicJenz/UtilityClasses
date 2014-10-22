/*
 * InfiniteStream.h
 *
 *  Created on: 23.10.2014
 *      Author: domenicjenz
 */

#pragma once

#include "Stream.h"

namespace Utilities
{

template<typename ElementType, typename SeedType = ElementType>
class InfiniteStream : public Stream<ElementType>
{
public:
	using GeneratorType = std::function<ElementType(SeedType&)>;
	InfiniteStream (typename Identity<GeneratorType>::type generator, SeedType initialSeed) : _generatorFunc (generator), _startSeed (initialSeed),
			_currentSeed (initialSeed)
	{}
	virtual ~InfiniteStream() {};

	void reset () override
	{
		_currentSeed = _startSeed;
	}

	Optional<ElementType> getNext () override
	{
		return Optional<ElementType>::value(_generatorFunc(_currentSeed));
	}

private:
	SeedType _startSeed;
	SeedType _currentSeed;
	GeneratorType _generatorFunc;
};

}
