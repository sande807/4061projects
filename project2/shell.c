#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
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
	
	//look for \seg command
	if(starts_with(line, CMD_SEG)){
		char *line = NULL;
		*line = '0';
	}
	
	//otherwise send input to server.c for processing
	write(fd_toserver, line, (strlen(line) + 1));
	
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
		
			usleep(1000);
		
			print_prompt(name);

		}else{//otherwise send line to be handled, then reprint prompt line
			
			sh_handle_input(input, fd_toserver);

			usleep(1000);

			print_prompt(name);
		}
	}
}

int main(int argc, char **argv)
{
	
	/***** Insert YOUR code *******/

	pid_t childpid;//child pid for fork
	char * name ;//name holder for name of shell
	int fd_ts, fd_fs;//fd to the pipes that go to the server and come from the server
	char pidmessage[MSG_SIZE];
	//extract name from argv
	name = argv[1] ;
	
	//extract pipe descriptors from argv
	fd_fs = atoi(argv[2]);
	fd_ts = atoi(argv[3]);

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
		char buffer[MSG_SIZE];
		while(1){
			usleep(1000);
			
			//if read < 0, reading failed, else it should work
			//sizeof(buffer) + 1?
			if (read(fd_fs, buffer, MSG_SIZE) < 0) {
				continue ;
			}
			if(buffer == NULL){
				continue;
			}else{
				printf("%s\n", buffer);//print buffer
			}
		}
	}
	/* Inside the parent
	 * Send the child's pid to the for later cleanup
	 * Start the main shell loop
	 */
	else if (childpid > 0) {
		//send child's pid to the server for cleanup
		//SEND AS CHILD PID COMMAND
		sprintf(pidmessage, "\\child_pid %d", childpid);
		
		write(fd_ts, pidmessage, sizeof(pidmessage)+1);//write to pipe
		sh_start(name,fd_ts);
	}
	
}
