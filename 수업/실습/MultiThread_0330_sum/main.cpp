#include<thread>
#include<iostream>
#include<vector>
#include<chrono>
#include<Windows.h>

#define MAXTHREAD 16

using namespace std;
using namespace std::chrono;

volatile int sum = 0;
volatile int sums[MAXTHREAD]{ 0 };

void thread_func(int thread_num, int thread_id)
{
	volatile int* volatile sum_temp = reinterpret_cast<volatile int*>(&sums[thread_id]);
	//printf("%d\n", thread_id);
	volatile int local_sum = 0;
	for (int i = 0; i < 500000000 / thread_num; ++i)
	{
		//local_sum += 2;
		*sum_temp += 2;
		//sum += 2;
		//cout << *sum_temp << endl;
	}
	//sum += local_sum;
}


int main() 
{

	vector<thread> threads;
	for (int i = 1; i <= MAXTHREAD; i *= 2)
	{
		threads.clear();
		sum = 0;
		//threads.resize(i);
		for (int sum_cnt = 0; sum_cnt < MAXTHREAD; ++sum_cnt)
		{
			sums[sum_cnt] = 0;
		}
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
		int Allsum = 0;
		for (int j = 0; j < i; ++j)
		{
			Allsum += sums[j];
		}
		
		
		cout << "thread [ " <<i<<" ] \t" << duration_cast<milliseconds>(d).count() << "msecs : sum = " << Allsum << endl;
	
		cout << endl;
	}

	return 0;
}