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
        
        return 0;
}
