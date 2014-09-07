/*
 * BigIntegerBase.h
 *
 *  Created on: Apr 2, 2012
 *      Author: nucsoft
 */

#pragma once

#include <vector>
#include <cstdlib>
#include <string>
#include <iostream>
#include <ostream>
#include <limits>
#include "HelperFunctions.h"



namespace Utilities
{

template<typename BaseType = unsigned char>
class BigIntegerBase
{
public:
	BigIntegerBase ()
	{
		setMasks ();
		_bigNumber.push_back (0);
		_positive = true;
	}
	BigIntegerBase (long number)
	{
		setMasks ();
		setFromNumber (number);
	}
	BigIntegerBase (const BigIntegerBase<BaseType>& copy)
	{
		setMasks ();
		copyFrom (copy);
	}
	virtual ~BigIntegerBase () = default;


  template <typename T>
  void setFromNumber (T number);

  BigIntegerBase<BaseType>& operator= (const BigIntegerBase<BaseType>& rhs)
  {
  	if (this != &rhs)
  	{
  		copyFrom (rhs);
  	}
  	return *this;
  }

  BigIntegerBase& operator+= (const BigIntegerBase& rhs);
  const BigIntegerBase operator+ (const BigIntegerBase& rhs) const;

  BigIntegerBase& operator-= (const BigIntegerBase& rhs);
  const BigIntegerBase operator- (const BigIntegerBase& rhs) const;

  BigIntegerBase& operator*= (const BigIntegerBase& rhs);
  const BigIntegerBase operator* (const BigIntegerBase& rhs) const;

  virtual void insertIntoStream (std::ostream& os) const;

  std::string asString () const;

  bool operator<= (const BigIntegerBase& rhs) const
	{
  	return (this->compare (rhs) <= 0);
	}

  bool operator< (const BigIntegerBase& rhs) const
  {
  	return (this->compare (rhs) < 0);
  }

  bool operator>= (const BigIntegerBase& rhs) const
	{
  	return (this->compare (rhs) >= 0);
	}

  bool operator> (const BigIntegerBase& rhs) const
  {
  	return (this->compare (rhs) > 0);
  }

  bool operator== (const BigIntegerBase& rhs) const
	{
  	return (this->compare (rhs) == 0);
	}

  bool isPositive () const
  {
  	return _positive;
  }


private:
	std::vector<BaseType> _bigNumber;
	bool _positive;

	unsigned int _baseTypeSize, _baseTypeHalfSize;
	BaseType _highMask, _lowMask;

	void copyFrom (const BigIntegerBase<BaseType>& rhs)
	{
		_positive = rhs._positive;
		_bigNumber = rhs._bigNumber;
	}
	
	void setMasks ();

	void addEntry (std::vector<BaseType>* victim, BaseType entry, unsigned int index) const;

	void multiplyAddEntry (std::vector<BaseType>* victim, BaseType entry1, BaseType entry2, unsigned int index) const;

	void subtractEntry (std::vector<BaseType>* victim, BaseType entry, unsigned int index) const;

	int compare (const BigIntegerBase& rhs) const;

	void cleanLeadingZeroes ();

	/**
	 * exclude eventual leading zero entries in the vector
	 */
	unsigned int getRealSize () const;

	BaseType getHighPart (BaseType victim) const
	{
		return ((victim & _highMask) >> _baseTypeHalfSize);
	}

	BaseType getLowPart (BaseType victim) const
	{
		return (victim & _lowMask);
	}
};

typedef BigIntegerBase<unsigned char> BigInteger;

template <typename BaseType>
std::ostream& operator<< (std::ostream& os, const BigIntegerBase<BaseType>& num)
{
	os << num.asString();
	return os;
}

template <typename BaseType>
template <typename T>
void BigIntegerBase<BaseType>::setFromNumber (T number)
{
	_bigNumber.clear ();
	T absVal = std::abs (number);
	if (absVal == 0)
	{
		_bigNumber.push_back(0);
	}

	int typeSize = sizeof (T) << 3;
	BaseType tempPackage;
	T packageMask = ((BaseType) -1);
	while (absVal > 0)
	{
		tempPackage = absVal & packageMask;
		//std::cout << (int) tempPackage << std::endl;
		_bigNumber.push_back (tempPackage);
		absVal = absVal >> _baseTypeSize;
	}
	_positive = (Utilities::sgn (number) >= 0);
}

template<typename BaseType>
void BigIntegerBase<BaseType>::addEntry (std::vector<BaseType>* victim, BaseType entry, unsigned int index) const
{
	size_t victimSize = victim->size();
	while (entry > 0)
	{
		if (index >= victimSize)
		{
			victim->push_back(entry);
			entry = 0;
		}
		else
		{
			BaseType maxAllowed = std::numeric_limits<BaseType>::max() - (*victim)[index];
			BaseType result = (*victim)[index] + entry;
			(*victim)[index] = result;
			entry = (entry > maxAllowed) ? 1 :0;
			++index;
		}
	}
}

template<typename BaseType>
void BigIntegerBase<BaseType>::multiplyAddEntry (std::vector<BaseType>* victim, BaseType entry1, BaseType entry2, unsigned int index) const
{
	BaseType entry1Low = getLowPart(entry1);
	BaseType entry2Low = getLowPart(entry2);
	BaseType entry1High = getHighPart(entry1);
	BaseType entry2High = getHighPart(entry2);
	BaseType low[2];
	low[0] = entry1Low * entry2Low;
	BaseType high[2];
	high[1] = entry1High * entry2High;
	BaseType middle1 = entry1Low * entry2High;
	BaseType middle2 = entry1High * entry2Low;
	low[1] = getLowPart(middle1);
	low[1] += getLowPart(middle2);
	high[0] = getHighPart(middle1);
	high[0] += getHighPart(middle2);
	high[0] += getHighPart(low[1]);
	low[1] = getLowPart(low[1]) << _baseTypeHalfSize;
	addEntry(victim, low[0], index);
	addEntry(victim, low[1], index);
	addEntry(victim, high[0], index+1);
	addEntry(victim, high[1], index+1);
}

template<typename BaseType>
void BigIntegerBase<BaseType>::subtractEntry (std::vector<BaseType>* victim, BaseType entry, unsigned int index) const
{
	size_t victimSize = victim->size();
	while ((entry > 0) && (index < victimSize))
	{
		BaseType result = (*victim)[index] - entry;
		entry = (entry > (*victim)[index]) ? 1 : 0;
		(*victim)[index] = result;
		++index;
	}
}

template<typename BaseType>
BigIntegerBase<BaseType>& BigIntegerBase<BaseType>::operator+= (const BigIntegerBase<BaseType>& rhs)
{
	if (this->isPositive () && !rhs.isPositive ())
	{
		BigIntegerBase tempRhs (rhs);
		tempRhs._positive = true;
		*this -= tempRhs;
		return *this;
	}
	else if (!this->isPositive () && rhs.isPositive ())
	{
		this->_positive = true;
		*this = rhs - (*this);
		return *this;
	}
	unsigned int rhsSize = rhs._bigNumber.size ();
	for (unsigned int i = 0; i < rhsSize; ++i)
	{
		addEntry (&_bigNumber, rhs._bigNumber[i], i);
	}

	return *this;
}

template<typename BaseType>
const BigIntegerBase<BaseType> BigIntegerBase<BaseType>::operator+ (const BigIntegerBase<BaseType>& rhs) const
{
	BigIntegerBase<BaseType> tempInt (*this);
	tempInt += rhs;
	return tempInt;
}

template<typename BaseType>
BigIntegerBase<BaseType>& BigIntegerBase<BaseType>::operator-= (const BigIntegerBase<BaseType>& rhs)
{
	int comparison = this->compare (rhs);
	std::vector<BaseType> smallerSide;
	bool resultIsPositive;

	// a - (-b) = a + b
	if (this->isPositive () && !rhs.isPositive ())
	{
		BigIntegerBase tempRhs (rhs);
		tempRhs._positive = true;
		*this += tempRhs;
		return *this;
	}
	// (-a) - b = -(a+b), this is already negative, as well as the result
	else if (!this->isPositive () && rhs.isPositive ())
	{
		*this += rhs;
		return *this;
	}

	// we want to subtract the smaller number from the bigger one
	if (comparison >= 0)
	{
		resultIsPositive = true;
		smallerSide = rhs._bigNumber;
	}
	else
	{
		resultIsPositive = false;
		smallerSide = this->_bigNumber;
		_bigNumber = rhs._bigNumber;
	}

	// (-a) - (-b) = -a + b = b - a
	if (!this->isPositive () && !rhs.isPositive ())
	{
		resultIsPositive = !resultIsPositive; // if a-b would have been positive, then b-a is negative
	}

	unsigned int smallerSize = smallerSide.size();
	for (unsigned int i = 0; i < smallerSize; ++i)
	{
		subtractEntry(&_bigNumber, smallerSide[i], i);
	}

	cleanLeadingZeroes ();
	_positive = resultIsPositive;

	return *this;
}

template <typename BaseType>
const BigIntegerBase<BaseType> BigIntegerBase<BaseType>::operator- (const BigIntegerBase<BaseType>& rhs) const
{
	BigIntegerBase<BaseType> tempInt (*this);
	tempInt -= rhs;
	return tempInt;
}

template <typename BaseType>
BigIntegerBase<BaseType>& BigIntegerBase<BaseType>::operator*= (const BigIntegerBase<BaseType>& rhs)
{
	BigIntegerBase<BaseType> temp = (*this) * rhs;
	_bigNumber = temp._bigNumber;
	_positive = temp._positive;
	return *this;
}

template <typename BaseType>
const BigIntegerBase<BaseType> BigIntegerBase<BaseType>::operator* (const BigIntegerBase<BaseType>& rhs) const
{
	BigIntegerBase<BaseType> result;
	result._positive = (_positive && rhs._positive) || (!_positive && !rhs._positive);
	size_t mySize = _bigNumber.size();
	size_t rhsSize = rhs._bigNumber.size();
	for (size_t l = 0; l < mySize; ++l)
	{
		for (size_t r = 0; r < rhsSize; ++r)
		{
			multiplyAddEntry(&(result._bigNumber), _bigNumber[l], rhs._bigNumber[r], l+r);
		}
	}
	return result;
}

template <typename BaseType>
int BigIntegerBase<BaseType>::compare (const BigIntegerBase<BaseType>& rhs) const
{

	int mySize = getRealSize ();
	int otherSize = rhs.getRealSize ();
	// that handles the case 0 == 0. Both should be positive, but you never know ...
	if ((mySize == 0) && (otherSize == 0))
	{
		return 0;
	}
	if (this->isPositive () && !rhs.isPositive ())
	{
		return 1;
	}
	else if (!this->isPositive () && rhs.isPositive ())
	{
		return -1;
	}
	// now we can be sure that both numbers have the same sign, the modificator now handles the cases that both
	// are positive (1) or both are negative (-1)
	int modificator = this->isPositive () ? 1 : -1;

	if (mySize < otherSize)
	{
		return -1 * modificator;
	}
	else if (mySize > otherSize)
	{
		return 1 * modificator;
	}
	else
	{
		for (int i = mySize - 1; i >= 0; i++)
		{
			if (_bigNumber[i] < rhs._bigNumber[i])
			{
				return -1 * modificator;
			}
			else if (_bigNumber[i] > rhs._bigNumber[i])
			{
				return 1 * modificator;
			}
		}
		return 0;
	}
}

template <typename BaseType>
void BigIntegerBase<BaseType>::cleanLeadingZeroes ()
{
	for (int i = _bigNumber.size () - 1; i >= 0; i--)
	{
		if (_bigNumber[i] == 0)
		{
			_bigNumber.erase (_bigNumber.begin () + i);
		}
	}
}

template <typename BaseType>
unsigned int BigIntegerBase<BaseType>::getRealSize () const
{
	int vectorSize = _bigNumber.size ();
	for (int i = vectorSize - 1; i >= 0; i--)
	{
		if (_bigNumber[i] != 0)
		{
			return i + 1;
		}
	}
	return 0;
}

template <typename BaseType>
void BigIntegerBase<BaseType>::insertIntoStream (std::ostream& os) const
{
	os << "BigInteger ( ";
	if (!_positive)
	{
		os << "-";
	}
	for (int i = _bigNumber.size() - 1; i >= 0; i--)
	{
		os << std::hex << (unsigned int) _bigNumber[i] << " ";
	}
	os << std::dec << ")";
}

template <typename BaseType>
void BigIntegerBase<BaseType>::setMasks ()
{
	_baseTypeSize = sizeof (BaseType) << 3;
	_baseTypeHalfSize = _baseTypeSize >> 1;
	_lowMask = ((BaseType) 1 << _baseTypeHalfSize) - 1;
	_highMask = _lowMask << _baseTypeHalfSize;
}

template <typename BaseType>
BaseType divideBy10 (std::vector<BaseType>& num)
{
	std::size_t vecSize = num.size();
	if (vecSize == 0)
	{
		return 0;
	}
	BaseType currentVal = 0;
	BaseType currentRest = 0;
	BaseType carryRest = 0;
	BaseType temp = 0;
	bool killLast = num[vecSize - 1] < 10;
	for (int i = vecSize - 1; i >= 0; --i)
	{
		temp = carryRest * 6;
		currentVal = (num[i] / 10) + (carryRest * 25) + (temp / 10);
		currentRest = (num[i] % 10) + (temp % 10);
		num[i] = currentVal + currentRest / 10;
		carryRest = currentRest % 10;
	}
	if (killLast)
	{
		num.pop_back();
	}
	return carryRest;
}

template <typename BaseType>
std::string BigIntegerBase<BaseType>::asString () const
{
	std::string result;
	std::vector<BaseType> temp = _bigNumber;
	BaseType rest;

	while (temp.size() > 0)
	{
		rest = divideBy10 (temp);
		result.insert(result.begin(), rest + '0');
	}
	if (!this->_positive)
	{
		result.insert(result.begin(), '-');
	}

	return result;
}


}


