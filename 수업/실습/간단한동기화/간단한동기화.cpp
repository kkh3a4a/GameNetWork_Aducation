
#include<iostream>
#include<thread>
#include<vector>
#include<mutex>
using namespace std;

int sum = 0;


volatile bool done = false;
volatile int* bound;
int error = 0;

void worker()
{
	for (int j = 0; j <= 100000000; ++j) *bound = -(1 + *bound);
	done = true;
}
void watcher()
{
	while (!done) {
		int v = *bound;
		if ((v != 0x00000000) && (v != 0xFFFFFFFF)) {
			cout << v << endl;
			error++;
		}
	}
}

int main() {

	bound = new int{0};
	int a[32];
	long long addr = reinterpret_cast<long long>(&a[31]);
	addr = (addr / 64) * 64;
	addr = addr - 1;
	bound = reinterpret_cast<int*>(addr);
	*bound = 0;
	thread t1{ worker };
	thread t2{ watcher };
	t1.join();
	t2.join();
	cout << "error : " << error << endl;
}