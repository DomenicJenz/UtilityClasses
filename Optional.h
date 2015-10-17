/*
 * Optional.h
 *
 *  Created on: 21.10.2014
 *      Author: domenicjenz
 */


#ifndef __OPTIONAL_H__
#define __OPTIONAL_H__

#include <istream>
#include <ostream>

namespace Utilities
{

struct OptionalNothingType
{
	OptionalNothingType() {}
};

const OptionalNothingType OptionalNothing;

template <typename T>
class Optional
{
public:
	Optional () = default;
	Optional (const T& value) : _value(value), _hasValue(true) {}
	Optional (const Optional<T>& optVal) = default;
	Optional (Optional<T>&& optVal) = default;
	Optional (const OptionalNothingType&) {}
	Optional<T>& operator= (const Optional<T>& optVal) = default;
	Optional<T>& operator= (Optional<T>&& optVal) = default;
	Optional<T>& operator= (const OptionalNothingType&) {
		_hasValue = false;
		return *this;
	}
	Optional<T>& operator= (const T& value) {
		setValue(value);
		return *this;
	}

	void reset () {
		_hasValue = false;
	}

	void setValue (const T& value) {
		_value = value;
		_hasValue = true;
	}

	bool hasValue () const {
		return _hasValue;
	}

	const T& getValue () {
		return _value;
	}

	const T& getValue (const T& defaultValue) {
		return _hasValue ? _value : defaultValue;
	}

	static Optional<T> nothing ()
	{
		return Optional<T>();
	}

	static Optional<T> value (const T& val)
	{
		return Optional<T>(val);
	}

	friend std::ostream& operator<<(std::ostream& os, const Optional<T>& value);
	friend std::istream& operator>>(std::istream& is, Optional<T>& value);

private:
	T _value;
	bool _hasValue = false;
};

template<typename ParamType>
Optional<ParamType> optionalValue (const ParamType& parVal)
{
	return Optional<ParamType>{parVal};
}

template<typename ParamType>
std::ostream& operator<<(std::ostream& os, const Optional<ParamType>& value)
{
	if (value.hasValue()) {
		os << value._value;
	}
	return os;
}

template<typename ParamType>
std::istream& operator>>(std::istream& is, Optional<ParamType>& value)
{
	int c = is.peek();  // is something in the stream ?
	if (c == EOF) {
		value.reset();
	} else {
		is >> value._value;
	  value._hasValue = true;
	}
}

std::istream& operator>>(std::istream& is, Optional<std::string>& value);



}
#endif /* __OPTIONAL_H__ */
