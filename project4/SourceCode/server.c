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

//make these variables global so they can be used in the worker and dispatch threads
FILE *log_f;
int num_workers, num_dispatcher, queuesz;
struct request_queue *q;
int *slot;
int currentslot;
int total=0;
pthread_mutex_t lock;
pthread_cond_t cv;
char *path;

//Structure for queue.
typedef struct request_queue {
        int   	m_socket;
        char   	m_szRequest[MAX_REQUEST_LENGTH];
        char	m_filename[1024];
} request_queue_t;

void * dispatch(void * arg) {
	//the dispatcher thread will continuously run these operations
	//it will attempt to get a connnection using the accept_connection function which will return a file descriptor
	//if that is unsuccessful it will exit thread
	//then it will try to get request, if that fails it will go to the next iteration of the loop
	//if successful it will send the request to the worker thread
	while(1) {
		int i=0;
		//find an open space in the queue
		while(1) {
			//infinitely looks for an open slot
			if(slot[i] == 0){//if the slot is 0 it's open
				slot[i] = 1;//set this slot to 1 now cause we are going to use it
				break;//break, now i equals the slot
			}
			i++;
			//we did not find an open slot so update i
			if(i==queuesz){
				i=0;
			}
		}
		//get file descriptor from accept_connection()
		//if fd is negative then error, thread should exit
		if((q[i].m_socket = accept_connection()) < 0)
		{
			slot[i] = 0;//free up slot before exit;
			pthread_exit(NULL);
		}
		//use file descriptor to get request. if return value == zero then success
		//if success then it will continue to do work. if failure it will skip this and continue
		//file is undefined before this point. if successful the filename will be stored in the file

		if(get_request(q[i].m_socket, q[i].m_filename) == 0)
		{
			currentslot=i;
			pthread_cond_signal(&cv);//send a signal via the condition variable
		}
	}
	
    return NULL;
}

void * worker(void * arg){
	int threadid = pthread_self();
	int n;
	int i = 0;
	int numbytes = 0;
	int num_requests_handled = 0;
	char *tolog;
	char *filename;
	char *test;
	char *filepath = (char *) malloc(1024);
	char *type;
	char *gif = "/image/gif";
	char *jpg = "/image/jpg";
	char *plain = "/text/plain";
	char *html = "/text/html";
	char *buf;
	FILE *fp;
	
	while(1) {
		//get lock
		pthread_mutex_lock(&lock) ;
		
		//wait for condition variable from dispatch which says theres a request ready for work
		pthread_cond_wait(&cv, &lock);
		
		//figure out the current slot
		i=currentslot;
		
		//get the filename
		filename = q[i].m_filename;
		n = strlen(filename)-4;
		test = &filename[n];
		//figure out q[i] content type
		if(strcmp(test, ".gif")==0){
			type = gif;
		}else if(strcmp(test, ".jpg")==0){
			type = jpg;
		}else if(strcmp(test, ".htm")==0){
			type = html;
		}else{
			type = plain;
		}
		//open the file and put it in the buffer and then figure out the number of bytes and set numbytes to that
		//use path
		strcpy(filepath, "0");
		strcpy(filepath, path);
		strcat(filepath, filename);
		fp = fopen(filepath,"rb");
		
		//figure out number of bytes
		fseek(fp, 0L, SEEK_END);//find end of file
		numbytes=ftell(fp);//get number of bytes
		fseek(fp, 0L, SEEK_SET);//go back to beginning of file
		buf = (char *) malloc(numbytes);//allocate space in the buffer
		fread(buf,numbytes,1,fp);//read the file into the buffer
		
		//return the result to the user. if there is a failure return error
		if (return_result(q[i].m_socket, type, buf, numbytes) != 0) {
			//set buf to the error message apparently
			buf = "failure to return result";
			num_requests_handled++;//update the number of requests this thread has handled
			//write to the log
			//open log for writing to
			log_f = fopen("web_server_log","a");
			fprintf(log_f, "[%d][%d][%d][%s][%s]\n", threadid,num_requests_handled,q[i].m_socket,filename,buf);
			fclose(log_f);
			//return error
			return_error(q[i].m_socket, buf);
		}else{
			//success
			num_requests_handled++;//update the number of requests this thread has handled
		
			//write to the log
			//open log for writing to
			log_f = fopen("web_server_log","a");
			fprintf(log_f, "[%d][%d][%d][%s][%d]\n", threadid,num_requests_handled,q[i].m_socket,filename,numbytes);
			fclose(log_f);
		}
		
		//free up stuff and unlock
		free(buf);
		slot[i] = 0;//free slot
		pthread_mutex_unlock(&lock) ;//unlock	
	}	
		
	return NULL;
}

int main(int argc, char **argv) {
        //Error check first.
        if(argc != 6 && argc != 7) {
                printf("usage: %s port path num_dispatcher num_workers queue_length [cache_size]\n", argv[0]);
                return -1;
        }

        int i;
        
        //get port, then initialize
        int p = atoi(argv[1] );
        init(p) ;
        
        //get path
        path = argv[2];
        
        //need to create dispatcher threads and worker threads
        //get number of dispatchers and workers from argv
        num_dispatcher = atoi(argv[3]);
        num_workers = atoi(argv[4]);
        queuesz = atoi(argv[5]);
 
        //make sure number of each threads doesn't exceed 100
        if (num_dispatcher > MAX_THREADS) {
			num_dispatcher = MAX_THREADS ;
		}
		if (num_workers > MAX_THREADS) {
			num_workers = MAX_THREADS ;
		}
		
        //make an array of stuff
        struct request_queue queue[queuesz];
        q = queue;
        
        //make an array to decide if a slot is empty or not
        int array[queuesz];
        for(i=0;i<queuesz;i++){
			array[i]=0;
		}
		slot = array;
		
		//set up locks and condition variables
		pthread_mutex_init(&lock, NULL);
		pthread_cond_init (&cv, NULL);
		
		//erase anything in the file
		log_f = fopen("web_server_log","w");
		fprintf(log_f, "\n");
		fclose(log_f);
        
        //create an array of dispatcher and worker threads from those values
		pthread_t d_threads[num_dispatcher];
		pthread_t w_threads[num_workers];
	
		//create both worker and dispatcher threads
		//for dispatcher threads call the function dispatch
        for(i=0; i<num_dispatcher; i++){
			pthread_create(&d_threads[i], NULL, dispatch, NULL);
		}
		//for the worker threads call the function worker
        for(i=0; i<num_workers; i++){
			pthread_create(&w_threads[i], NULL, worker, NULL);
		}
		
		//join both threads
		for(i=0; i<num_dispatcher; i++){
			pthread_join(d_threads[i], NULL);
		}
		
		for(i=0; i<num_workers; i++){
			pthread_join(w_threads[i], NULL);
		}
		
		return 0;
}
