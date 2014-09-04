#include <iostream>
#include "BigInteger.h"

using namespace Utilities;

int main (int argc, char** argv)
{
	BigInteger big1(111);
	BigInteger big2(222);
	BigInteger big3(333);
	std::cout << "Hello WOrlds" << std::endl;
	big1.insertIntoStream(std::cout);
	big2.insertIntoStream(std::cout);
	big3.insertIntoStream(std::cout);
	std::cout << std::endl;
}