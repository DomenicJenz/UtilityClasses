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
#include <utility>
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
		_bigNumber.push_back (0);
	}
	BigIntegerBase (long number)
	{
		setFromNumber (number);
	}
	BigIntegerBase (const BigIntegerBase<BaseType>& copy)
	{
		copyFrom (copy);
	}
	BigIntegerBase (BigIntegerBase<BaseType>&& source) : BigIntegerBase<BaseType>{}
	{
		swap(*this, source);
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

  BigIntegerBase<BaseType>& operator= (BigIntegerBase<BaseType>&& other)
  {
  	swap(*this, other);
  	return *this;
  }

  BigIntegerBase& operator+= (const BigIntegerBase& rhs);
  BigIntegerBase operator+ (const BigIntegerBase& rhs) const;

  BigIntegerBase& operator-= (const BigIntegerBase& rhs);
  BigIntegerBase operator- (const BigIntegerBase& rhs) const;

  BigIntegerBase& operator*= (const BigIntegerBase& rhs);
  BigIntegerBase operator* (const BigIntegerBase& rhs) const;

  BigIntegerBase& operator/= (const BigIntegerBase& rhs);
  BigIntegerBase operator/ (const BigIntegerBase& rhs) const;

  virtual void insertIntoStream (std::ostream& os) const;

  std::string asString () const;

  void shiftLeft (unsigned int howMuch);

  void shiftRight (unsigned int howMuch);

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

  bool isBitSet (unsigned int index) const;

  void setBit (unsigned int index, bool setToOne = true);

  void resetBit (unsigned int index)
  {
  	setBit(index, false);
  }

  bool isPositive () const
  {
  	return _isPositive;
  }

  void printInternal () const
  {
  	for (int i = _bigNumber.size() - 1; i >= 0 ; --i)
  	{
  		std::cout << std::hex << (int) _bigNumber[i] << " ";
  	}
  	std::cout << std::dec << std::endl;
  }

  friend void swap(BigIntegerBase<BaseType>& a, BigIntegerBase<BaseType>& b)
  {
  	swap(a._bigNumber, b._bigNumber);
  	std::swap(a._isPositive, b._isPositive);
  }

private:
	std::vector<BaseType> _bigNumber;
	bool _isPositive = true;

	static const unsigned int _baseTypeSize = sizeof(BaseType) << 3;
	static const unsigned int _baseTypeHalfSize = sizeof(BaseType) << 2;
	static const BaseType _lowMask = ((BaseType) 1 << (sizeof(BaseType) << 2)) - 1;
	static const BaseType _highMask = (((BaseType) 1 << (sizeof(BaseType) << 2)) - 1) << (sizeof(BaseType) << 2);

	void copyFrom (const BigIntegerBase<BaseType>& rhs)
	{
		_isPositive = rhs._isPositive;
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
	_isPositive = (Utilities::sgn (number) >= 0);
}

template <typename BaseType>
bool BigIntegerBase<BaseType>::isBitSet (unsigned int index) const
{
	unsigned int chunkNr = index / _baseTypeSize;
	if (chunkNr >= _bigNumber.size())
	{
		return false;
	}
	else
	{
		unsigned int chunkIndex = index % _baseTypeSize;
		return (_bigNumber[chunkNr] & (1 << chunkIndex)) > 0;
	}
}

template <typename BaseType>
void BigIntegerBase<BaseType>::setBit (unsigned int index, bool setToOne)
{
	unsigned int chunkNr = index / _baseTypeSize;
	if (chunkNr >= _bigNumber.size())
	{
		unsigned int fillChunks = chunkNr - _bigNumber.size() + 1;
		for (unsigned int i = 0; i < fillChunks; ++i)
		{
			_bigNumber.push_back((BaseType) 0);
		}
	}
	unsigned int chunkIndex = index % _baseTypeSize;
	if (setToOne)
	{
		_bigNumber[chunkNr] |= (1 << chunkIndex);
	}
	else // set to Zero
	{
		_bigNumber[chunkNr] &= ~(1 << chunkIndex);
	}
}

template <typename BaseType>
void BigIntegerBase<BaseType>::shiftLeft (unsigned int howMuch)
{
	unsigned int blocks = howMuch / _baseTypeSize;
	unsigned int shiftPart1 = howMuch % _baseTypeSize;
	unsigned int shiftPart2 = _baseTypeSize - shiftPart1;
	size_t chunks = _bigNumber.size();
	if ((chunks == 0) || (howMuch == 0))
	{
		return;
	}
	for (unsigned int i = 0; i < blocks; ++i)
	{
		_bigNumber.push_back(0);
	}
	BaseType entry = _bigNumber[chunks-1] >> shiftPart2;
	if (entry > 0)
	{
		_bigNumber.push_back(entry);
	}
	for (unsigned int i = chunks + blocks - 1; i > blocks; --i)
	{
		entry = _bigNumber[i - blocks] << shiftPart1;
		entry += _bigNumber[i - blocks - 1] >> shiftPart2;
		_bigNumber[i] = entry;
	}
	_bigNumber[blocks] = _bigNumber[0] << shiftPart1;
	for (unsigned int i = 0; i < blocks; ++i)
	{
		_bigNumber[i] = 0;
	}
}

template <typename BaseType>
void BigIntegerBase<BaseType>::shiftRight (unsigned int howMuch)
{
	unsigned int blocks = howMuch / _baseTypeSize;
	unsigned int shiftPart1 = howMuch % _baseTypeSize;
	unsigned int shiftPart2 = _baseTypeSize - shiftPart1;
	size_t chunks = _bigNumber.size();
	if ((chunks == 0) || (howMuch == 0))
	{
		return;
	}
	for (unsigned int i = 0; i < chunks - blocks - 1; ++i)
	{
		_bigNumber[i] = _bigNumber[i + blocks] >> shiftPart1;
		_bigNumber[i] += _bigNumber[i + blocks+1] << shiftPart2;
	}
	_bigNumber[chunks - 1 - blocks] = _bigNumber[chunks - 1] >> shiftPart1;
	for (unsigned int i = 0; i < blocks; ++i)
	{
		_bigNumber[chunks - 1 - i] = 0;
	}
	cleanLeadingZeroes();
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
		tempRhs._isPositive = true;
		*this -= tempRhs;
		return *this;
	}
	else if (!this->isPositive () && rhs.isPositive ())
	{
		this->_isPositive = true;
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
BigIntegerBase<BaseType> BigIntegerBase<BaseType>::operator+ (const BigIntegerBase<BaseType>& rhs) const
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
		tempRhs._isPositive = true;
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
	_isPositive = resultIsPositive;

	return *this;
}

template <typename BaseType>
BigIntegerBase<BaseType> BigIntegerBase<BaseType>::operator- (const BigIntegerBase<BaseType>& rhs) const
{
	BigIntegerBase<BaseType> tempInt (*this);
	tempInt -= rhs;
	return tempInt;
}

template <typename BaseType>
BigIntegerBase<BaseType>& BigIntegerBase<BaseType>::operator*= (const BigIntegerBase<BaseType>& rhs)
{
	BigIntegerBase<BaseType> temp = (*this) * rhs;
	//_bigNumber = temp._bigNumber;
	//_isPositive = temp._isPositive;
	swap(*this, temp);
	return *this;
}

template <typename BaseType>
BigIntegerBase<BaseType> BigIntegerBase<BaseType>::operator* (const BigIntegerBase<BaseType>& rhs) const
{
	BigIntegerBase<BaseType> result;
	result._isPositive = (_isPositive && rhs._isPositive) || (!_isPositive && !rhs._isPositive);
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
BigIntegerBase<BaseType>& BigIntegerBase<BaseType>::operator/= (const BigIntegerBase& rhs)
{
	BigIntegerBase<BaseType> temp = (*this) / rhs;
	swap(*this, temp);
	return *this;
}

template <typename BaseType>
BigIntegerBase<BaseType> BigIntegerBase<BaseType>::operator/ (const BigIntegerBase& rhs) const
{
	BigIntegerBase<BaseType> result;
	BigIntegerBase<BaseType> divi;
	for (int i = (_bigNumber.size() * _baseTypeSize) - 1; i >= 0; --i)
	{
		divi.shiftLeft(1);
		if (this->isBitSet(i))
		{
			divi.setBit(0);
		}
		if (divi >= rhs)
		{
			result.setBit(i);
			divi -= rhs;
		}
	}
	result.cleanLeadingZeroes();

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
		for (int i = mySize - 1; i >= 0; --i)
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
		else
		{
			// if an entry != 0 is encountered, the erasing has to stop
			return;
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
	if (!_isPositive)
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
	if (!this->_isPositive)
	{
		result.insert(result.begin(), '-');
	}

	return result;
}


}


