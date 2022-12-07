/*

	That file is just for testing

*/

#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

#include "cardinal.h"

int main()
{
	// отрицательные числа и дабл врут
	cout << (++cardinal(5.5)).to_binary(true) << endl;
//	string s = "";
//	string s1 = "";
//	//cout << 0b1111111111111111111111111111111111111111111111111111111111111111ull << endl;
//	long long a1 = /*-9223372036854775807*/-11;
//	cardinal a(a1);
//	unsigned long long b1 = 0b1011111111111111111111111111111111111111111111111111111111111111ull;
//	cardinal b(b1);
//	cout << a.to_binary(true) << endl;
//	for (int i = 0; i < 64; i++)
//	{
//		s += (a1 & 1) + '0';
//		a1 >>= 1;
//	}
//	reverse(s.begin(), s.end());
//	cout << "                         " << s << endl;
//	cout << b.to_binary(true) << endl;
//	for (int i = 0; i < 64; i++)
//	{
//		s1 += (b1 & 1) + '0';
//		b1 >>= 1;
//	}
//	reverse(s1.begin(), s1.end());
//	cout << "                         " << s1 << endl;

	/*double a = 15, b = 25;
	uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	for (int i = 0; i < 15000000; i++)
	{
		a = a + b;
	}
	cout << duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - ms;*/
	cout << "\n\nreturn\n";
	return 0;
}
