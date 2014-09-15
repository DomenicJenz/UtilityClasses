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
	big3.shiftLeft(7);
	std::cout << "Shifted: " << big3 << std::endl;
	big3.shiftRight(8);
	std::cout << "Back: " << big3 << std::endl;
}
