/*
 * Optional.h
 *
 *  Created on: 21.10.2014
 *      Author: domenicjenz
 */


#ifndef __OPTIONAL_H__
#define __OPTIONAL_H__

namespace Utilities
{

template <typename T>
class Optional
{
public:
	Optional () = default;
	Optional (const T& value) : _value(value), _hasValue(true)
	{}
	Optional (const Optional<T>& optVal) = default;
	Optional (Optional<T>&& optVal) = default;

	Optional<T>& operator= (const Optional<T>& optVal) = default;
	Optional<T>& operator= (Optional<T>&& optVal) = default;

	void setValue (const T& value)
	{
		_value = value;
		_hasValue = true;
	}

	bool hasValue () const
	{
		return _hasValue;
	}

	const T& getValue ()
	{
		return _value;
	}

	const T& getValue (const T& defaultValue)
	{
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
Optional<ParamType> optionalNothing ()
{
	return Optional<ParamType>{};
}

}
#endif /* __OPTIONAL_H__ */
