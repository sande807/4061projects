#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>

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
			//printf("command line:%s\n", targets[i].szCommand) ;
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
			//printf("\ntarget:%s\n", targets[i].szTarget) ;
			
			//get dependencies
			while ((lpszLine = strtok(NULL, " ")) != NULL){
				strcpy(targets[i].szDependencies[dCount], lpszLine) ;
				//printf("dependency:%s\n", targets[i].szDependencies[dCount]);
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
	char * format = "f:hnBm:";
	
	// Default makefile name will be Makefile
	char szMakefile[64] = "tc4";
	char szTarget[64];
	char szLog[64];	

	while((ch = getopt(argc, argv, format)) != -1) {
		switch(ch) {
			case 'f':
				strcpy(szMakefile, strdup(optarg));
				break;
			case 'n':
				break;
			case 'B':
				break;
			case 'm':
				strcpy(szLog, strdup(optarg));
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
		 * 
		 * ONLY ONE TARGET ALLOWED
		 * 
		 * this will be what is left in *argv
		 *
		 * WE NEED TO FORK AND EXEC THE SPECIFIC TARGET IN THIS IF STATEMENT
		 * I THINK
		 */
		 printf("---the specific target we are supposed to build is %s---\n\n",	 *argv);
	}
	else {
		//set target to be first target from makefile
		//no target specified in terminal, work line-by-line in makefile
		printf("----no target specified, build makefile step-by-step----\n\n") ;
		x = 0;
		//WE NEED TO FORK AND EXEC THE MAKEFILE IN THIS ELSE STATEMENT
		//I THINK
		/*int numTargets ;
		numTargets = parse(szMakefile, targets) ;
		printf("number of targets = %d\n", numTargets) ;
		
		int i, x ;		
		for(i = 0; i < numTargets; i++) {
			printf("target = %s\n", targets[i].szTarget) ;
			printf("command = %s\n", targets[i].szCommand) ;
			printf("dependency count = %d\n", targets[i].nDependencyCount) ;
			for(x = 0; x < targets[i].nDependencyCount; x++) {
				printf("dependency = %s\n", targets[i].szDependencies[x]) ;
			}
		}
		
		pid_t childPid ;
		char * dependency ;
		i = 0 ;
		x = 0 ;
		
		while (i <= (targets[x].nDependencyCount)) {
			printf("creating new process for dependecy #%d\n", i + 1) ;
			dependency = targets[x].szDependencies[i] ;
			i++ ;
		}*/
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
		for(i=0; i<numTargets; i+=1){
			printf("target = %s\n", targets[i].szTarget);
			printf("command = %s\n", targets[i].szCommand);
			printf("dependency count = %d\n", targets[i].nDependencyCount);
			for(y=0; y<targets[i].nDependencyCount; y++){
				printf("dependency = %s\n", targets[i].szDependencies[y]);
			}
		}
		printf("\n\n") ;

 		pid_t childpid;
 		pid_t waitreturn;
		int status;
		int numtokens;
		char *dependency;
		char *delimiter = " ";
		i = 0 ;
		char **myargv ;
		while (i <= (targets[x].nDependencyCount)) {
			printf("target %s, i=%d\n", targets[x].szTarget, i);
			if(i==targets[x].nDependencyCount){
				printf("all dependencies done. command \"%s\" execute\n", targets[x].szCommand);
				numtokens = makeargv(targets[x].szCommand, delimiter, &myargv);
				//printf("execvp = %d\n", execvp(myargv[x], &myargv[x])) ;
				//printf("numtokens = %d\n", numtokens);
				
				//numtokens = makeargv(argv[x+1], delimiter, &myargv) ;
				execvp(myargv[x], &myargv[x]) ;
				printf("after execution\n");
				myargv = NULL;
				perror("child failed to execute") ;
				exit(1);
			}
			printf("creating new process for #%d dependency\n", i+1);
			dependency = targets[x].szDependencies[i];
			printf("Dependency = %s\n", dependency);
			childpid = fork();
			if  (childpid == -1){
				perror("failed to fork");
				exit(0);
			}else if (childpid == 0) {
				//printf("I am a child with id %ld\n", (long)getpid());
				printf("Dependency is %s search for matching target\n", dependency);
				for(x=0; x<numTargets; x++){
					if(strcmp(targets[x].szTarget,dependency)==0){
						printf("found matching target:%s\n",targets[x].szTarget);
						i=0;
						break;
					}else{
						printf("not a matching target\n");
					}
				}
				if(x==numTargets){
					printf("no matching targets found\n");
					exit(3);
				}
			}else{
				waitreturn = wait(&status);
				i++;
				if(WIFEXITED(status)){
					printf("child exits with status %d\n",WEXITSTATUS(status));
				}
			}
		}
/*		if (nDependencyCount == 0) {
			execvp(szCommand[0], ...) ;
			exit(nStatus) ;
		}
		else {
			//wait pid = status
			//execvp(command, ...)
		}
		* 
		* if(targets[x].nDependencyCount == 0){
					printf("execute command: %s\n", targets[x].szCommand);
				}
*/
	}

	//after parsing the file, you'll want to check all dependencies (whether they are available targets or files)
	//then execute all of the targets that were specified on the command line, along with their dependencies, etc.
	return EXIT_SUCCESS;
}
