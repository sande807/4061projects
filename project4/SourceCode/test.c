#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.h"

int main (int argc, char **argv) {
	
	char * filename = "butthole" ;
	pthread_mutex_t lock ;
	
	if (argc != 6 && argc != 7) {
		printf("usage: %s port path num_dispatcher num_workers queue_length [cache_size]\n", argv[0]);
		return -1 ;
	}

	int p = (int) * argv[0] ;
	init(p) ;
	
	printf("server initialized!\n") ;
	
	pthread_mutex_lock (&lock) ;
	
	int i = accept_connection() ;
	
	pthread_mutex_unlock (&lock) ;
	
	printf("connection accepted!\n") ;
	
	return 0 ;
	
}
