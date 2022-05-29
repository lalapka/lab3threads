#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "SafeQueue.h"


using namespace std;

class ThreadPool {
private:
  bool m_shutdown;
  SafeQueue<function<void()>> m_queue;
  vector<thread*> m_threads;
  mutex m_conditional_mutex;
  condition_variable m_conditional_lock; //�������� ����������
  int threadsCount;
  bool deleteFlag;
  int deleteThreadId;
public:
	ThreadPool(const int n_threads);
	~ThreadPool();
	// Inits thread pool
	void init();
	// Add thread
	void addThread();
	// Remove thread
	void removeThread();
	// Waits until threads finish their current task and shutdowns the pool
	void shutdown();
	// getters
	bool getShutdownFlag();
	mutex& getConditionalMutex();
	SafeQueue<function<void()>>& getQueue();
	condition_variable& getConditionalLock();
	int getThreadsCount();
	int getCurrentThreadCount();
	void deleteThreadById(int threadId);
	void setDeleteFlag(bool flag);
	void setDeleteThreadId(int id);

	//�������� �������, ������� ����� ����������� ����� ���������
    // Submit a function to be executed asynchronously by the pool
	template<typename F, typename...Args>
	auto submit(F&& f, Args&&... args) -> future<decltype(f(args...))> { //���������� �������
		//������� ������� � ������������� �����������, ������� � ����������
		// Create a function with bounded parameters ready to execute
		function<decltype(f(args...))()> task = bind(forward<F>(f), forward<Args>(args)...);
		//������������� ��, ����� ����� ����������� ����������, ���������, ���������
		// Encapsulate it into a shared ptr in order to be able to copy construct / assign 
		auto task_ptr = make_shared<packaged_task<decltype(f(args...))()>>(task);

		// Wrap packaged task into void function 
		function<void()> wrapper_func = [task_ptr]() {
			(*task_ptr)(); 
		};

		// Enqueue generic wrapper function - ������ � ������� �������
		m_queue.enqueue(wrapper_func);

		// Wake up one thread if its waiting
		m_conditional_lock.notify_one();

		// Return future from promise
		return task_ptr->get_future();
	}
};
