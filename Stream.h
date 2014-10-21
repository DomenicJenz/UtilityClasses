/*
 * Stream.h
 *
 *  Created on: 21.10.2014
 *      Author: domenicjenz
 */

#ifndef __STREAM_H__
#define __STREAM_H__

namespace Utilities
{

template<typename T>
class Stream
{
	Stream () = default;
	~Stream () = default;

	bool getNext () = 0;
};


}

#endif /* __STREAM_H__ */
