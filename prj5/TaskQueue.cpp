/**
* Task Queue implementation
*/
#include "TaskQueue.h"
#include <pthread.h>

using namespace std;

void Task::join(){
	pthread_mutex_lock(&lock);
		
		while (!started || !finished){
			pthread_cond_wait(&wait, &lock);
		}
	pthread_mutex_unlock(&lock); 
}

void *Task::workerLoop(void *aux){
	ThreadData *data = (ThreadData *)aux;
	while (1){
		pthread_mutex_lock(&(data->mutex)); 
			while (data->t == NULL){
				pthread_cond_wait(&(data->todo), &(data->mutex)); 
			}
		Task* task = data->t;
		pthread_mutex_unlock(&(data->mutex)); 
		pthread_mutex_lock(&(task->lock));
		task->started = true; 
		pthread_mutex_unlock(&(task->lock));
		if (task->getType() == TASK_EXITER){
			data->status = THREAD_KILLED;
		}
		task->run();
		pthread_mutex_lock(&(task->lock));
		task->finished = true;
		pthread_cond_broadcast(&(task->wait)); 
		pthread_mutex_unlock(&(task->lock));
		data->status = THREAD_WAITING;
		data->parent->notifyDone(data);
	} 
	return NULL;  
}

void *TaskQueue::supervisorLoop(void *data){
	TaskQueue *tq = (TaskQueue *)data; 
	while(1){
		pthread_mutex_lock(&(tq->mutex));
		while(tq->waitingThreads.empty() || tq->taskQueue.empty()){
			if (tq->numWorkers == 0){
				int tmp; pthread_exit(&tmp); 
			}
			pthread_cond_wait(&tq->wait, &tq->mutex);
		}
		ThreadData *thread = tq->waitingThreads.front(); tq->waitingThreads.pop();
		Task *task = tq->taskQueue.front(); tq->taskQueue.pop();
		pthread_mutex_unlock(&(tq->mutex)); 

		pthread_mutex_lock(&(thread->mutex));
		thread->tid = task->getTid();
		thread->t = task; 
		thread->status = THREAD_RUNNING;
		pthread_cond_signal(&(thread->todo)); 
		pthread_mutex_unlock(&(thread->mutex));  
	}
	return NULL;
}

unsigned int Task::getTid(){
	return tid;
}

void TaskQueue::initThread(ThreadData *w){
	pthread_mutex_init(&w->mutex, NULL);
	pthread_cond_init(&w->todo, NULL);
	w->parent = this;
	w->t=NULL; 
	w->tid=0;
	pthread_create(&w->thread, NULL, Task::workerLoop, w);  
}

void TaskQueue::notifyDone(ThreadData *td){
	td->t = NULL;
	pthread_mutex_lock(&mutex); 
	switch(td->status){
		case THREAD_RUNNING:td->status = THREAD_WAITING;
		case THREAD_WAITING:
			waitingThreads.push(td);  
			break;
		case THREAD_KILLED: 
			numWorkers --;
			break;    
	}
	pthread_cond_signal(&wait);
	pthread_mutex_unlock(&mutex);  
}

TaskQueue::TaskQueue(unsigned int numWorkers1): numWorkers(numWorkers1){
	workers = new ThreadData[numWorkers];
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&wait, NULL);

	pthread_mutex_lock(&mutex);
	for (int i = 0; i < numWorkers; i++){
		initThread(workers + i);
		waitingThreads.push(workers+i);
	}
	task_no = 1;
	pthread_create(&controller, NULL, TaskQueue::supervisorLoop, this);
	pthread_mutex_unlock(&mutex); 
}

int TaskQueue::enqueue(Task *t){ 
	int id = t->tid = task_no; 
	task_no ++; 
	t->started = false;
	t->finished = false; 
	pthread_mutex_init(&(t->lock), NULL);
	pthread_cond_init(&(t->wait), NULL);

	pthread_mutex_lock(&mutex);
	taskQueue.push(t);
	pthread_cond_signal(&wait); 
	pthread_mutex_unlock(&mutex);
	return id;
}