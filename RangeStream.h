/*
 * RangeStream.h
 *
 *  Created on: 21.10.2014
 *      Author: domenicjenz
 */


#ifndef __RANGESTREAM_H__
#define __RANGESTREAM_H__

#include "Stream.h"

namespace Utilities
{

template <typename T>
class RangeStream : public Stream<T>
{
public:
	RangeStream (T start, T end, T step = 1) : _start(start), _end(end), _step(step), _current(start)
  {}

	void reset () override
	{
		_current = _start;
	}

	Optional<T> getNext () override
	{
		Optional<T> result;
		if (_current <= _end)
		{
			result = Optional<T> (_current);
			_current += _step;
		}
		else
		{
			result = Optional<T> ();
		}
		return result;
	}

private:
	T _start;
	T _end;
	T _step;
	T _current;


};



}

#endif /* __RANGESTREAM_H__ */
