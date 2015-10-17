/*
 * Optional.cpp
 *
 *  Created on: 14.08.2015
 *      Author: domenicjenz
 */

#include "Optional.h"

namespace Utilities {

std::istream& operator>>(std::istream& is, Optional<std::string>& value)
{
	std::string temp(std::istreambuf_iterator<char>(is), {});
	value.setValue(temp);
	return is;
}

}
