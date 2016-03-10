#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include "util.h"

int main(int argc, char **argv)
{
	int status, nbytes, flags;
	int fd1[2];
	int fd2[2];
	char *string = "you suck\n";
	char buffer[80];
	ssize_t bufsize = 0;

	if (pipe(fd1) == -1 || pipe(fd2) == -1) {
		//pipe fails
		perror("Failed to create the pipe.") ;
		return 1 ;
	}
	printf("created pipe, fork next\n");
	
	pid_t childpid;
	childpid = fork();
	
	if (childpid == -1) {
		//fork fails
		perror("Failed to fork.") ;
		return 1 ;
	}

	if (childpid == 0) {
		printf("child program\n");

		//write to pipe what the user inputs
		printf("user input within child process: ");
		getline(&string, &bufsize, stdin);//get user input
		printf("string: %s\n", string);

		write(fd1[1], string, (strlen(string) + 1));//write to pipe
		
		printf("child is done\n");
		exit(0);//child is done now
	}
	else if (childpid > 0) {
		//exec shell program?
		wait(&status);

		printf("parent program\n");

		//read and print whats in the pipe
		nbytes = read(fd1[0], buffer, sizeof(buffer));//read and put in buffer
		printf("string from pipe: %s\n", buffer);//print buffer

		printf("done\n");
	}
}
