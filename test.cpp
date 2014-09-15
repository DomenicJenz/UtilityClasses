#include <iostream>
#include "BigInteger.h"

using namespace Utilities;

int main (int argc, char** argv)
{
	BigInteger big1(1234);
	BigInteger big2(4321);
	BigInteger big3;
	std::cout << big1.asString() << " * " << big2<< std::endl;
	big3 = big1 + big2;
	std::cout << big3 << std::endl;
	BigInteger big4;
	big4.setBit(20);
	big4.setBit(4);
	std::cout << big4 << std::endl;
	big4.shiftRight(1);
	std::cout << big4.isBitSet(4) << " " << big4.isBitSet(3) << std::endl;
}
