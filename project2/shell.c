#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "util.h"

/*
 * Read a line from stdin.
 */
char *sh_read_line(void)
{
	char *line = NULL;
	ssize_t bufsize = 0;
	
	if(getline(&line, &bufsize, stdin)<0)
		return "input failed";
	return line;
}

/*
 * Do stuff with the shell input here.
 */
int sh_handle_input(char *line, int fd_toserver)
{
	
	/***** Insert YOUR code *******/
	printf("handling input\n");
	//look for \seg command
	
	if(starts_with(line, CMD_SEG)){
		printf("seg fault\n");
		char *line = NULL;
		*line = '0';
	}
	printf("sending to server\n") ;
	printf("line equals: %s\n", line) ;
	//otherwise send input to server.c for processing
	write(fd_toserver, line, (strlen(line) + 1));
	printf("sent to server\n") ;
	/* Write message to server for processing */
	return 1;
}

/*
 * Check if the line is empty (no data; just spaces or return)
 */
int is_empty(char *line)
{
	while (*line != '\0') {
		if (!isspace(*line))
			return 0;
		line++;
	}
	return 1;
}
/*
 * Start the main shell loop:
 * Print prompt, read user input, handle it.
 */
void sh_start(char *name, int fd_toserver)
{
	/***** Insert YOUR code *******/
	print_prompt(name);
	char *input = NULL;
	input = (char *)malloc(MSG_SIZE);
	while(1){
		input = sh_read_line();
		if(is_empty(input)){//if empty reprint the prompt
			print_prompt(name);
		}else{//otherwise send line to be handled, then reprint prompt line
			printf("about to handle input\n") ;
			sh_handle_input(input, fd_toserver);
			printf("input handled, printing shell name\n") ;
			print_prompt(name);
		}
	}
}

int main(int argc, char **argv)
{
	
	/***** Insert YOUR code *******/
	printf("started shell\n");
	/* Extract pipe descriptors and name from argv */
	int inpipe;
	
	int fd_ts;
	int fd_fs;
	
	//child pid for fork
	pid_t childpid;
	
	char * name ;
	
	//extract name from argv
	name = argv[0] ;
	
	//extract pipe descriptors from argv
	fd_fs = atoi(argv[1]);
	fd_ts = atoi(argv[2]);
	printf("began shell with name: %s\n", name);
	printf("fd_ts:%d\n",fd_ts);
	printf("fd_fs:%d\n",fd_fs);
	fd_ts = 6;
	fd_fs = 3;
	printf("fd_ts:%d\n",fd_ts);
	printf("fd_fs:%d\n",fd_fs);

	/* Fork a child to read from the pipe continuously */
	childpid = fork() ;
	if (childpid == -1) {
		//fork fails
		perror("Failed to fork.") ;
		return 1 ;
	}
	/*
	 * Once inside the child
	 * look for new data from server every 1000 usecs and print it
	 */ 
	if (childpid == 0) {
		printf("child of shell to check for server.c input\n");
		char *buffer;
		while(1){
			usleep(1000);
			//inpipe = read(fd1[0], buffer, sizeof(buffer));//read pipe
			
			//if read < 0, reading failed, else it should work
			//sizeof(buffer) + 1?
			if (read(fd_fs, buffer, sizeof(buffer)) > 0) {
				printf("read successful\n") ;
			}
			
			if(buffer != NULL)
				printf("string from pipe: %s\n", buffer);//print buffer
		}
	}
	/* Inside the parent
	 * Send the child's pid to the server for later cleanup
	 * Start the main shell loop
	 */
	else if (childpid > 0) {
		printf("parent process of shell, next is sh_start\n");
		//send child's pid to the server for cleanup
		//write(fd1[1], childpid, sizeof(childpid));//write to pipe
		sh_start(name,fd_ts);
	}
	
}
