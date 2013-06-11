/**
Monitor.h
simple implementation of read/write locks
*/ 

#include <pthread.h>
#include <queue> 

class Monitor{
	private:	 
	pthread_mutex_t lock; 
	pthread_cond_t read_wait;
	pthread_cond_t write_wait;
	unsigned int num_readers;
	unsigned int num_writers;
	bool writing; 
	void readerEnter();
	void writerEnter(); 
	public: 
	Monitor();
	void Enter(char mode);
	void Exit(char mode);     
};

