/* csci4061 S2016 Assignment 4 
* section: 007, 002
* date: 05/02/16
* names: Eric Sande, Nick Orf
* UMN Internet ID, Student ID: sande807 4828951, orfxx012 4669898
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.h"

#define MAX_THREADS 100
#define MAX_QUEUE_SIZE 100
#define MAX_REQUEST_LENGTH 64

//Structure for queue.
typedef struct request_queue
{
        int   	m_socket;
        char   	m_szRequest[MAX_REQUEST_LENGTH];
} request_queue_t;

void * dispatch(void * arg)
{
        return NULL;
}

void * worker(void * arg)
{
        return NULL;
}

//make these variables global so they can be used in the worker and dispatch threads
int num_workers, num_dispatcher;

int main(int argc, char **argv)
{
        //Error check first.
        if(argc != 6 && argc != 7)
        {
                printf("usage: %s port path num_dispatcher num_workers queue_length [cache_size]\n", argv[0]);
                return -1;
        }

        printf("Call init() first and make a dispather and worker threads\n");
        
        //get port, then initialize
        int p = (int)* argv[0] ;
        init(p) ;
        
        //need to create dispatcher threads and worker threads
        //get number of dispatchers and workers from argv
        num_dispatcher = (int)* argv[2];
        num_workers = (int)* argv[3];
        
        //create an array of dispatcher and worker threads from those values
		pthread_t d_threads[num_dispatcher];
		pthread_t w_threads[num_workers];
		
		//for each dispatcher and worker thread create the thread and join it
		//for dispatcher threads call the function dispatch
		int i;
        for(i=0; i<num_dispatcher; i++){
			pthread_create(&d_threads[i], NULL, dispatch, NULL);
			pthread_join(d_threads[i], NULL);
		}
		//for the worker threads call the function worker
        for(i=0; i<num_workers; i++){
			pthread_create(&w_threads[i], NULL, worker, NULL);
			pthread_join(w_threads[i], NULL);
		}
		return 0;
}
