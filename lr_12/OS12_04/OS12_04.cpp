#include <iostream>
#pragma comment(lib, "..\\Debug\\OS12_LIB.lib")
#include "OS12_04.h"

int main()
{
	try
	{
		void* h1 = OS12_LIB::Init();
		void* h2 = OS12_LIB::Init();

		std::cout << "OS12::Adder::Add(h1, 2, 3) = " << OS12_LIB::ADDER::Add(h1, 2, 3) << "\n";
		std::cout << "OS12::Adder::Add(h2, 2, 3) = " << OS12_LIB::ADDER::Add(h2, 2, 3) << "\n";

		std::cout << "OS12::Adder::Sub(h1, 2, 3) = " << OS12_LIB::ADDER::Sub(h1, 2, 3) << "\n";
		std::cout << "OS12::Adder::Sub(h2, 2, 3) = " << OS12_LIB::ADDER::Sub(h2, 2, 3) << "\n";

		std::cout << "OS12::Multiplier::Mul(h1, 2, 3) = " << OS12_LIB::MULTIPLIER::Mul(h1, 2, 3) << "\n";
		std::cout << "OS12::Multiplier::Mul(h2, 2, 3) = " << OS12_LIB::MULTIPLIER::Mul(h2, 2, 3) << "\n";

		std::cout << "OS12::Multiplier::Div(h1, 2, 3) = " << OS12_LIB::MULTIPLIER::Div(h1, 2, 3) << "\n";
		std::cout << "OS12::Multiplier::Div(h2, 2, 3) = " << OS12_LIB::MULTIPLIER::Div(h2, 2, 3) << "\n";

		OS12_LIB::Dispose(h1);
		OS12_LIB::Dispose(h2);

	}
	catch (int e) { 
		std::cout << "OS12: error = " << e << "\n"; 
	}
}