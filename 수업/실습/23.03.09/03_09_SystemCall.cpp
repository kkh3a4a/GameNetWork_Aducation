#include <iostream>
#include <chrono>
#include <thread>
using namespace std;
using namespace chrono;

constexpr int CACHE_LINE_SIZE = 20;


struct A {
	int value = 0;
};


int main()
{
	A a;
	A* b = new A;
	a.value = 2;
	b->value = 3;
	memmove(b, &a, sizeof(A));
	b->value = 5;

	cout << a.value << b->value << endl;
	for (int i = 0; i < 20; ++i) 
	{
		const int size = 1024 << i;
		long long* a = (long long*)malloc(size);
		unsigned int index = 0;
		volatile long long tmp = 0;
		const int num_data = size / 8;
		auto start = high_resolution_clock::now();
		for (int j = 0; j < 100000000; ++j) {
			tmp += a[index % num_data];
			index += CACHE_LINE_SIZE * 11;
		}
		auto duration = high_resolution_clock::now() - start;
		cout << "ARRAY SIZE " << size /1024 <<"KB, TIME : " << duration_cast<milliseconds>(duration).count() << " msec // " << tmp << endl;
		free(a);
	}

	
	









	//volatile long long tmp = 0;
	//auto start = high_resolution_clock::now();
	//for (int j = 0; j < 10000000; ++j) {
	//	tmp += j;
	//	//this_thread::yield();
	//}
	//auto duration = high_resolution_clock::now() - start;
	//cout << "Time " << duration_cast<milliseconds>(duration).count();
	//cout << " msec\n";
	//cout << "RESULT " << tmp << endl;
}