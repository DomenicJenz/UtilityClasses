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
class Stream
{
public:
	Stream () = default;
	virtual ~Stream () = default;

	virtual void reset () = 0;

	void forEach (std::function<void (const T&)> eachFunc)
	{
		Optional<T> currentVal;
		while ((currentVal = getNext()).hasValue ())
		{
			eachFunc (currentVal.getValue());
		}
	}

private:
	virtual Optional<T> getNext () = 0;

};


}

#endif /* __STREAM_H__ */
