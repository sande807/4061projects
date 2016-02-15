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
int parse(char * lpszFileName, struct target targets[20])
{
	int nLine=0;
	char szLine[1024];
	char * lpszLine;
	FILE * fp = file_open(lpszFileName);
	
	//counters for dependencies/commands 
	int dCount = 0 ;
	int cCount = 0 ;
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
			i+=1;
			//printf("\ncommand line :%s\n", (*stuff).szCommand) ;
		}
		else{
			//target line
			
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
			//printf("\ntarget :%s\n", (*stuff).szTarget) ;
			
			//get dependencies
			while ((lpszLine = strtok(NULL, " ")) != NULL){
				strcpy(targets[i].szDependencies[dCount], lpszLine) ;
				//printf("dependency:%s\n", targets[i].szDependencies[dCount]) ;
				dCount++ ;
				targets[i].nDependencyCount++ ;
			}			
		}
	}
	printf("\n") ;

	//Close the makefile. 
	fclose(fp);
	
	//free(stuff) ;

	return 0;
}

void show_error_message(char * lpszFileName)
{
	fprintf(stderr, "Usage: %s [options] [target] : only single target is allowed.\n", lpszFileName);
	fprintf(stderr, "-f FILE\t\tRead FILE as a maumfile.\n");
	fprintf(stderr, "-h\t\tPrint this message and exit.\n");
	fprintf(stderr, "-n\t\tDon't actually execute commands, just print them.\n");
	fprintf(stderr, "-B\t\tDon't check files timestamps.\n");
	fprintf(stderr, "-m FILE\t\tRedirect the output to the file specified .\n");
	exit(0);
}

int main(int argc, char **argv) 
{	
	//declare stuff of type target_t and allocate memory for it
	//target_t *stuff ;
	//stuff = (target_t *) malloc(sizeof(target_t)) ;
	//(*stuff).nDependencyCount = 0 ;
	struct target targets[20];
	
	// Declarations for getopt
	extern int optind;
	extern char * optarg;
	int ch;
	char * format = "f:hnBm:";
	
	// Default makefile name will be Makefile
	char szMakefile[64] = "Makefile";
	char szTarget[64];
	char szLog[64];

	while((ch = getopt(argc, argv, format)) != -1) 
	{
		switch(ch) 
		{
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

	if(argc > 1)
	{
		show_error_message(argv[0]);
		return EXIT_FAILURE;
	}

	//You may start your program by setting the target that make4061 should build.
	//if target is not set, set it to default (first target from makefile)
	if(argc == 1)
	{
		//set target makefile should build
	}
	else
	{
		//set target to be first target from makefile
	}


	/* Parse graph file or die */
	if((parse(szMakefile, targets)) == -1) 
	{
		return EXIT_FAILURE;
	}
	else {
		int i;
		int x;
		for(i=0; i<2; i+=1){
			printf("target = %s\n", targets[i].szTarget);
			printf("command = %s\n", targets[i].szCommand);
			printf("dependency count = %d\n", targets[i].nDependencyCount);
			for(x=0; x<targets[i].nDependencyCount; x++){
				printf("dependency = %s\n", targets[i].szDependencies[x]);
			}
		}

/*
		int i = 0 ;
		while (i < (nDependencyCount)) {
			pid = fork() ;
			if (pid = 0) {
				i = 0 ;
				//targets[dependencies]
			}
			i++ ;
		}
		if (nDependencyCount == 0) {
			execvp(szCommand[0], ...) ;
			exit(nStatus) ;
		}
		else {
			//wait pid = status
			//execvp(command, ...)
		}*/
	}

	//after parsing the file, you'll want to check all dependencies (whether they are available targets or files)
	//then execute all of the targets that were specified on the command line, along with their dependencies, etc.
	return EXIT_SUCCESS;
}
