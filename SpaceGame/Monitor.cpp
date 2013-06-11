
#include <pthread.h>
#include "Monitor.h"

Monitor::Monitor(){
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&read_wait, NULL); 
	pthread_cond_init(&write_wait, NULL);
	num_readers = 0;
	num_writers = 0;
	writing = false; 
}

void Monitor::Enter(char mode){
	if (mode == 'w'){
		pthread_mutex_lock(&lock);
		num_writers++; 
		while (num_readers > 0 || writing){
			pthread_cond_wait(&write_wait, &lock); 
		}
		num_writers--; 
		writing = true; 
		pthread_mutex_unlock(&lock);
		
	}else //(mode == 'r')
	{
		pthread_mutex_lock(&lock);
		while (writing || num_writers > 0){
			pthread_cond_wait(&read_wait, &lock);
		}
		num_readers ++; 
		pthread_mutex_unlock(&lock); 
	}
		
}

void Monitor::Exit(char mode){
	if (mode == 'w'){
		pthread_mutex_lock(&lock);
		writing = false; 
		if (num_writers == 0)
			pthread_cond_broadcast(&read_wait); 
		else
			pthread_cond_signal(&write_wait); 
		pthread_mutex_unlock(&lock);
	} else {
		pthread_mutex_lock(&lock);
		num_readers--;
		if (num_readers == 0){
			pthread_cond_signal(&write_wait); 
		}
		pthread_mutex_unlock(&lock);
	}

}	

