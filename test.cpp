#include <iostream>
#include "BigInteger.h"

using namespace Utilities;

int main (int argc, char** argv)
{
	BigInteger big1(65536);
	std::cout << big1.asString() << std::endl;
	std::cout << "Hello World" << std::endl;
	std::cout << std::endl;
}
