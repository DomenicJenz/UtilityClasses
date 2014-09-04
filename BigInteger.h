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
	BaseType _higherMask, _lowerMask;

	void copyFrom (const BigIntegerBase<BaseType>& rhs)
	{
		_positive = rhs._positive;
		_bigNumber = rhs._bigNumber;
	}
	
	void setMasks ();


	int compare (const BigIntegerBase& rhs) const;

	void cleanLeadingZeroes ();

	/**
	 * exclude eventual leading zero entries in the vector
	 */
	unsigned int getRealSize () const;

	/**
	 * helper function for the addition of BigIntegers, which handles the addition of value, which are half the size
	 * of BIGINT_BASETYPE
	 */
	BaseType addHalf (BaseType half1, BaseType half2, BaseType* result);

	BaseType getHigherPart (BaseType victim)
	{
		return ((victim & _higherMask) >> _baseTypeHalfSize);
	}

	BaseType getLowerPart (BaseType victim)
	{
		return (victim & _lowerMask);
	}
};

typedef BigIntegerBase<unsigned char> BigInteger;

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
		std::cout << (int) tempPackage << std::endl;
		_bigNumber.push_back (tempPackage);
		absVal = absVal >> _baseTypeSize;
	}
	_positive = (Utilities::sgn (number) >= 0);
}

// TODO: Check for overflow in result
template<typename BaseType>
BaseType BigIntegerBase<BaseType>::addHalf (BaseType half1, BaseType half2, BaseType* carry)
{
	BaseType result = half1 + half2 + *carry;
	*carry = getHigherPart (result);
	result = getLowerPart (result);
	return result;
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
	unsigned int mySize = _bigNumber.size ();
	unsigned int otherSize = rhs._bigNumber.size ();
	//int min = DJBasicFunctions::min (mySize, otherSize);
	int maxSize = std::max (mySize, otherSize);

	if (maxSize > mySize)
	{
		// the result will be larger than me, so fill it up with zeroes before the addition process begins
		for (int i = mySize; i < maxSize; i++)
		{
			_bigNumber.push_back (0);
		}
	}

	BaseType carry = 0;
	for (int i = 0; i < maxSize; i++)
	{
		BaseType currentPartThis = _bigNumber[i];
		BaseType currentPartRhs = rhs._bigNumber[i];
		//BIGINT_BASETYPE lowerRes, higherRes;
		_bigNumber[i] = currentPartThis + currentPartRhs + carry;
		// overflow occured
		if ((_bigNumber[i] < currentPartThis) || ((_bigNumber[i] == currentPartThis) && (currentPartRhs > 0)))
		{
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		//lowerRes = addHalf (getLowerPart (currentPartThis), getLowerPart (currentPartRhs), carry);
		//higherRes = addHalf (getHigherPart (currentPartThis), getHigherPart (currentPartRhs), carry);
		//_bigNumber[i] = (higherRes << _baseTypeHalfSize) + lowerRes;
	}

	if (carry > 0)
	{
		_bigNumber.push_back (carry);
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
	const std::vector<BaseType>* biggerSide;
	const std::vector<BaseType>* smallerSide;
	bool resultIsPositive;

	// we want to subtract the smaller number from the bigger one
	if (comparison >= 0)
	{
		resultIsPositive = true;
		biggerSide = &this->_bigNumber;
		smallerSide = &rhs._bigNumber;
	}
	else
	{
		resultIsPositive = false;
		biggerSide = &rhs._bigNumber;
		smallerSide = &this->_bigNumber;
	}

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
	// (-a) - (-b) = -a + b = b - a
	else if (!this->isPositive () && !rhs.isPositive ())
	{
		resultIsPositive = !resultIsPositive; // if a-b would have been positive, so b-a is negative
	}

	int carry = 0;
	int smallerSize = smallerSide->size ();
	int biggerSize = biggerSide->size ();
	BaseType partResult;
	BaseType currentBigger, currentSmaller;
	for (int i = 0; i < smallerSize; i++)
	{
		currentBigger = biggerSide->at (i);
		currentSmaller = smallerSide->at (i);
		partResult = currentBigger - currentSmaller - carry;
		// utilizing the effect of an underflow, where -1 = 255, a negative number will be "bigger" than the one from which was
		// subtracted
		if ((partResult > currentBigger) || ((partResult == currentBigger) && (currentSmaller > 0)))
		{
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		_bigNumber[i] = partResult;
	}
	for (int i = smallerSize; i < biggerSize; i++)
	{
		partResult = biggerSide->at (i) - carry;
		if (partResult > biggerSide->at (i))
		{
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		_bigNumber[i] = partResult;
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
	return *this;
}

template <typename BaseType>
const BigIntegerBase<BaseType> BigIntegerBase<BaseType>::operator* (const BigIntegerBase<BaseType>& rhs) const
{
	BigIntegerBase<BaseType> tempInt (*this);
	tempInt *= rhs;
	return tempInt;
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
	_lowerMask = ((BaseType) 1 << _baseTypeHalfSize) - 1;
	_higherMask = _lowerMask << _baseTypeHalfSize;
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
	std::cout << _bigNumber.size() << " digits: ";
	for (const auto& digit : _bigNumber)
	{
		std::cout << (int) digit << " ";
	}
	std::cout << std::endl;
	while (temp.size() > 0)
	{
		rest = divideBy10 (temp);
		result.insert(result.begin(), rest + '0');
	}

	return result;
}


}


