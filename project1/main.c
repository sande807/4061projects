#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <time.h>

#include "util.h"
//This function will parse makefile input from user or default makeFile 
int parse(char * lpszFileName, struct target targets[20]) {
	int nLine=0;
	char szLine[1024];
	char * lpszLine;
	FILE * fp = file_open(lpszFileName);
	
	//counters for dependencies/targets
	int dCount = 0 ;
	int tCount = 0 ;
	int i = 0;

	if(fp == NULL)
	{
		return -1;
	}

	while(file_getline(szLine, fp) != NULL) 
	{
		nLine++;
		// this loop will go through the given file, one line at a time
		// this is where you need to do the work of interpreting
		// each line of the file to be able to deal with it later

		//Remove newline character at end if there is one
		lpszLine = strtok(szLine, "\n"); 
			
		//You need to check below for parsing. 
		//Skip if blank or comment.
		//Remove leading whitespace.
		//Skip if whitespace-only.
		//Only single command is allowed.
		//If you found any syntax error, stop parsing. 
		//If lpszLine starts with '\t' it will be command else it will be target.
		//It is possbile that target may not have a command as you can see from the example on project write-up. (target:all)
		//You can use any data structure (array, linked list ...) as you want to build a graph
		
		if(lpszLine == NULL || lpszLine[0] == '#'){
			//either no line or comment
			//do nothing
		}
		else if(lpszLine[0] == '\t'){
			//command line
			lpszLine = strtok(szLine, "\t") ;
			strcpy(targets[i].szCommand,lpszLine) ;
			i+=1 ;
		}
		else{
			//target line
			tCount++ ;
			//get target
			lpszLine = strtok(szLine, ":") ;
			if (lpszLine == NULL) {
				/* this line is either not a valid target line, command
				 * line, or violates syntax in some form...
				 * either way, terminate with an error
				 */
				perror ("invalid line...") ;
			}
			strcpy(targets[i].szTarget,lpszLine) ;
			
			//get dependencies
			while ((lpszLine = strtok(NULL, " ")) != NULL){
				strcpy(targets[i].szDependencies[dCount], lpszLine) ;
				dCount++;
				targets[i].nDependencyCount++ ;
			}
			dCount = 0;			
		}
	}

	//Close the makefile. 
	fclose(fp);

	return tCount;
}

void show_error_message(char * lpszFileName) {
	fprintf(stderr, "Usage: %s [options] [target] : only single target is allowed.\n", lpszFileName);
	fprintf(stderr, "-f FILE\t\tRead FILE as a maumfile.\n");
	fprintf(stderr, "-h\t\tPrint this message and exit.\n");
	fprintf(stderr, "-n\t\tDon't actually execute commands, just print them.\n");
	fprintf(stderr, "-B\t\tDon't check files timestamps.\n");
	fprintf(stderr, "-m FILE\t\tRedirect the output to the file specified .\n");
	exit(0);
}

int main(int argc, char **argv) {	
	struct target targets[20];
	
	// Declarations for getopt
	extern int optind;
	extern char * optarg;
	int ch;
	int x;
	int nflag = 0;
	int mflag = 0;
	int bflag = 0;
	int targetflag = 0;
	char * format = "f:hnBm:";
	int fd;
	char * specifictarget;
	
	
	// Default makefile name will be Makefile
	char szMakefile[64] = "Makefile";
	char szTarget[64];
	char szLog[64];
	
	char * specTarget ;

	while((ch = getopt(argc, argv, format)) != -1) {
		switch(ch) {
			case 'f':
				strcpy(szMakefile, strdup(optarg));
				break;
			case 'n':
				nflag = 1;
				break;
			case 'B':
				bflag = 1;
				break;
			case 'm':
				strcpy(szLog, strdup(optarg));
				fopen(szLog,"ab+");
				mflag = 1;
				break;
			case 'h':
			default:
				show_error_message(argv[0]);
				exit(1);
		}
	}
	
	argc -= optind;
	argv += optind;	
	// at this point, what is left in argv is the targets that were 
	// specified on the command line. argc has the number of them.
	// If getopt is still really confusing,
	// try printing out what's in argv right here, then just running 
	// with various command-line arguments.

	if(argc > 1) {
		show_error_message(argv[0]);
		return EXIT_FAILURE;
	}

	//You may start your program by setting the target that make4061 should build.
	//if target is not set, set it to default (first target from makefile)
	if(argc == 1) {
		//set target makefile should build
		/* makefile should only build the specific target specified
		 * and then stop. we need to search our target object to find 
		 * where the specific target is located, handle dependencies
		 * appropriately, and then build the target.
		 */
		 specifictarget = *argv;
		 targetflag = 1;
	}
	else {
		//set target to be first target from makefile
		x = 0;
	}


	/* Parse graph file or die */
	
	int numTargets ;//this is for the number of targets
	numTargets = parse(szMakefile, targets);
	
	//printf("number of targets = %d\n", numTargets); 
	if(numTargets == -1) 
	{
		return EXIT_FAILURE;
	}
	else {
		int i;
		int y;
		if (targetflag) {
			//if there is a specific target to be built
			for (x = 0; x < numTargets; x++) {
				//search through all the targets for the right one
				if(strcmp(targets[x].szTarget, specifictarget) == 0) {
					//if the target matches the right target break, so x
					//matches the specific target
					break;
				}
			}
			if (x == numTargets) {
				perror("failed to find target");
				exit(3);
			}
		}
		if (mflag) {
			//if the m flag is set, then it should have already opened the file or created it
			
			//get the file descriptor
			fd = open(szLog, O_WRONLY);
			if (fd == -1) {//if it failed to open quit
				perror("Failed to open");
				exit(0);
			}
			
			if (dup2(fd,STDOUT_FILENO) == -1) {
				//then change the standard out to our file using dup2
				perror("Failed to redirect stdout.");
				exit(0);
			}
		}
 		pid_t childpid;
 		pid_t waitreturn;
		int status;
		int numtokens;
		char *dependency;
		char *delimiter = " ";
		i = 0 ;
		char **myargv ;
		while(i <= (targets[x].nDependencyCount)) {
			if(i == targets[x].nDependencyCount) {
				if(nflag || mflag) {
					//this flag means they just want it printed, not executed
					printf("command: %s\n", targets[x].szCommand);
					if(x == 0) {
						for(x = 0; x<numTargets; x++) {
							if(strcmp(targets[x].szTarget, "clean") == 0) {
								//exit if target is clean
								break;
							}
						}
						if(x!=numTargets){
							printf("command: %s\n", targets[x].szCommand);
						}
					}
					exit(1);
				}
				else{
					if(x == 0) {
						//first target, all others should be done, check for clean
						childpid = fork();
						if(childpid == -1){
							perror("failed to fork");
							exit(0);
						}
						else if(childpid == 0){
							for(x=0; x<numTargets; x+=1){
								if(strcmp(targets[x].szTarget, "clean")==0){
									break;
								}
							}
							if(x == numTargets){
								//exit if we cannot find clean
								exit(3);
							}
							numtokens = makeargv(targets[x].szCommand, delimiter, &myargv);
							if (execvp(myargv[0], &myargv[0]) == -1) {
								perror("child failed to execute");
								exit(2);
							}
						}
						else {
							waitreturn = wait(&status);
						}
						if(bflag != 0){
							//if the b flag is not set we want to check timestamp
							time_t fileTime = time(NULL) ;
							
							if(fileTime == get_file_modification_time(targets[x].szTarget)){
								//if the time is the same it's been compiled recently
								//therefore do not compile
								exit(0);
							}
						}
					}
					else{
						//not first target, just compile regularly
						if(bflag != 0){
							//if the b flag is not set we want to check timestamp
							time_t fileTime = time(NULL) ;
							
							if(fileTime == get_file_modification_time(targets[x].szTarget)){
								//if the time is the same it's been compiled recently
								//therefore do not compile
								exit(0);
							}
						}
						numtokens = makeargv(targets[x].szCommand, delimiter, &myargv);
						if (execvp(myargv[0], &myargv[0]) == -1) {
							perror("child failed to execute");
							exit(2);
						}
					}
				}
			}
			dependency = targets[x].szDependencies[i];
			childpid = fork();
			if(childpid == -1) {
				perror("failed to fork");
				exit(0);
			}
			else if (childpid == 0) {
				for(x=0; x<numTargets; x++){
					if(strcmp(targets[x].szTarget,dependency)==0){
						//matching target has been found, break
						i = 0 ;
						break;
					}
				}
				if(x == numTargets) {
					//no matching targets found
					exit(3);
				}
			}
			else {
				waitreturn = wait(&status);
				i++;
			}
		}

	}
	return EXIT_SUCCESS;
}
