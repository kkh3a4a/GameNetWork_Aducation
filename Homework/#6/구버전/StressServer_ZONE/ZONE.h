#pragma once
#include<mutex>

using namespace std;
class SESSION;


class NODE {
	mutex n_lock;
public:
	int id;
	NODE* volatile next;
	
	NODE() : id(-1), next(nullptr){}
	NODE(int x) : id(x), next(nullptr){}
	void lock()
	{
		n_lock.lock();
	}
	void unlock()
	{
		n_lock.unlock();
	}
};


class ZONE {
	NODE head, tail;
public:
	ZONE()
	{
		head.id = 0x80000000;
		tail.id = 0x7FFFFFFF;
		head.next = &tail;
		tail.next = nullptr;
	}
	bool ADD(int x)
	{
		while (1) {
			NODE* prev = &head;
			NODE* curr = prev->next;
			while (curr->id < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->lock(); curr->lock();
			if (validate(prev, curr))
			{
				if (curr->id != x) {
					NODE* node = new NODE{ x };
					node->next = curr;
					prev->next = node;
					curr->unlock();
					prev->unlock();
					return true;
				}
				else
				{
					curr->unlock();
					prev->unlock();
					return false;
				}
			}
			curr->unlock();
			prev->unlock();
		}
	}

	bool DEL(int x)
	{
		while (1) {
			NODE* prev = &head;
			NODE* curr = prev->next;
			while (curr->id < x) {
				prev = curr;
				curr = curr->next;
			}
			prev->lock(); curr->lock();
			if (validate(prev, curr))
			{
				if (curr->id != x) {
					curr->unlock();
					prev->unlock();
					return false;
				}
				else {
					prev->next = curr->next;
					curr->unlock();
					prev->unlock();
					//delete curr;
					return true;
				}
			}
			curr->unlock();
			prev->unlock();
		}
	}
	bool validate(NODE* prev, NODE* curr)
	{
		NODE* node = &head;
		while (node->id <= prev->id) {
			if (node == prev)
			{
				return prev->next == curr;
			}
			node = node->next;
		}

		return false;
	}
};