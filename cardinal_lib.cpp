#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

#include "cardinal.h"

int main()
{
	double a = 15, b = 25;
	uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	for (int i = 0; i < 15000000; i++)
	{
		a = a + b;
	}
	cout << duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - ms;
	cout << "\n\nreturn\n";
	return 0;
}
