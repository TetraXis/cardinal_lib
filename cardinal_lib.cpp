/*

	This file is just for testing

*/

#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

#include "cardinal.h"

int main()
{
	cardinal a = 15.0, b = 25.0;
	uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	for (int i = 0; i < 1000; i++)
	{
		a = a + b;
	}
	cout << duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - ms;
	cout << "\n\nreturn\n";
	return 0;
}
