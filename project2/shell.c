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

	getline(&line, &bufsize, stdin);
	printf("line: %s \n", line);
	printf("exit sh read line\n");
	return line;
}

/*
 * Do stuff with the shell input here.
 */
int sh_handle_input(char *line, int fd_toserver)
{
	
	/***** Insert YOUR code *******/
	
 	/* Check for \seg command and create segfault */
	printf("handling input! jk i do nothing \n");
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
	char *input;
	ssize_t bufsize = 0;

	//read user input
	while(1){
		printf("start loop\n");
		usleep(1000);
		input = sh_read_line();
		if(is_empty(input)){
		
		}else{
			input = sh_read_line();
			printf("something has been written\n");
			printf("%s\n", input);
			sh_handle_input(input,fd_toserver);
			printf("\n");
			print_prompt(name);
		}
	}


}

int main(int argc, char **argv)
{
	
	/***** Insert YOUR code *******/
	printf("started shell yay\n");
	//child pid for fork
	pid_t childpid ;
	char *name = argv[0];
	
	/* Extract pipe descriptors and name from argv */
	int fd1[2];
	int fd2[2];
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
	}
	/* Inside the parent
	 * Send the child's pid to the server for later cleanup
	 * Start the main shell loop
	 */
	else if (childpid > 0) {
		printf("parent process of shell, next is sh_start\n");
		sh_start(name,fd1[0]);
	}
	
}
