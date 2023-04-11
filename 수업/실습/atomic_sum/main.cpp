#include<thread>
#include<iostream>
#include<vector>
#include<chrono>
#include<Windows.h>
#include<atomic>
#define MAXTHREAD 16

using namespace std;
using namespace std::chrono;

atomic<int> sum = 0;
volatile int sums[MAXTHREAD]{ 0 };

void thread_func(int thread_num, int thread_id)
{
	for (int i = 0; i < 100000000 / thread_num; ++i)
	{
		sum += 1;
	}
}


int main() 
{

	vector<thread> threads;
	for (int i = 1; i <= MAXTHREAD; i *= 2)
	{
		threads.clear();
		sum = 0; 
		auto t = high_resolution_clock::now();
		for (int j = 0; j < i; ++j)
		{
			threads.push_back((thread{ thread_func, i ,j}));
		}
		
		for (auto& a : threads)
		{
			a.join();
		}
		auto d = high_resolution_clock::now() - t;
		
		
		
		cout << "thread [ " <<i<<" ] \t" << duration_cast<milliseconds>(d).count() << "msecs : sum = " << sum << endl;
	
		cout << endl;
	}

	return 0;
}