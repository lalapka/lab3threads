#pragma once
#include <mutex> //взаимоисключение
#include <queue> //очередь

using namespace std;

// Thread safe implementation of a Queue using an queue - реализация очеред
template <typename T>
class SafeQueue {
private:
	queue<T> m_queue; //очередь
	mutex m_mutex; //взаимоисключение
public:
	SafeQueue() { }
	SafeQueue(SafeQueue& other) { }
	~SafeQueue() { }

	bool empty() {
		unique_lock<mutex> lock(m_mutex);
		return m_queue.empty();
	}

	int size() {
		unique_lock<mutex> lock(m_mutex);
		return m_queue.size();
	}

	void enqueue(T& t) { //ставит в очередь
		unique_lock<mutex> lock(m_mutex); //блокировка мьютекс
		m_queue.push(t); //помещает элемент в очередь
	}

	bool dequeue(T& t) { //удаляет из очереди
		unique_lock<mutex> lock(m_mutex);

		if (m_queue.empty()) {
			return false;
		}
		t = move(m_queue.front());
		m_queue.pop();
		return true;
	}
};