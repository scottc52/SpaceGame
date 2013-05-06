/**
* Thread Queue
*/
#ifndef _TASK_QUEUE_H
#define _TASK_QUEUE_H
#include <pthread.h>
#include <queue>

#define DEF_NUM_THREADS 3 

#define THREAD_WAITING 1
#define THREAD_KILLED 2
#define THREAD_RUNNING 3

using namespace::std;

class TaskQueue; //forward Declare
class Task;

typedef struct threadData{
	TaskQueue *parent;
	pthread_t thread;  
	pthread_mutex_t mutex;
	pthread_cond_t todo; 
	Task *t;
	unsigned int tid;
	unsigned int status;   
} ThreadData;

class TaskQueue{
private:  
	ThreadData *workers;
	pthread_t controller; 
	pthread_mutex_t mutex;
	pthread_cond_t  wait; 
	queue<Task *> taskQueue;
	queue<ThreadData *> waitingThreads;
	void initThread(ThreadData *w);
	int numWorkers;     
public:	
	unsigned int task_no; 
	TaskQueue(unsigned int numThreads = DEF_NUM_THREADS);
	int enqueue(Task *t);
	static TaskQueue *defQueue;
	void notifyDone(ThreadData *td);
	static void *supervisorLoop(void *data);
};

#define TASK_GENERAL 1
#define TASK_EXITER 2

class Task{
private: 
	unsigned int tid;
	pthread_mutex_t lock;
	pthread_cond_t wait; 
protected:
 	bool started;
 	bool finished;  
public:
	void join(); 
	unsigned int getTid();
	static void *workerLoop(void *aux);
	friend int TaskQueue::enqueue(Task *t);
	virtual void run(){};
	virtual int getType(){
		return TASK_GENERAL;
	}
	virtual ~Task(){}
};

class Exit_Task: public Task{
private:
	static const int type = TASK_EXITER;
public:
	
	void run(){
		int retval;
		pthread_exit(&retval);  
	}
	void join(){
		return;
	} 
	int getType(){
		return TASK_EXITER;
	} 
};


#endif

