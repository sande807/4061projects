#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>

#include "util.h"

//This function will parse makefile input from user or default makeFile. 
int parse(char * lpszFileName)
{
	int nLine=0;
	char szLine[1024];
	char * lpszLine;
	FILE * fp = file_open(lpszFileName);
	
	int i = 0 ;
	//struct target stuff[] = new target ;

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
		
		printf("\nline = %s", lpszLine) ;
		
		if(lpszLine == NULL || lpszLine[0] == '#'){//if the line is null or the first character is a pound then it is to be ignored
			printf("\ncomment or null line, do nothing"); 		
		}else if(lpszLine[0] == '\t'){//if the first line starts with a tab it is a comment
			printf("\nthis line is a command");
		}else{//otherwise it must be a target
			printf("\nthis line is not a command, null, or comment, must be a target/dependencies");
			printf("\nwill attempt to get next line");
			while(file_getline(szLine, fp)!=NULL){//get the next line
				printf("\nnext line is: %s", szLine);//print out that line, which should be a command
				lpszLine = szLine;
				if(lpszLine[0] == '\t'){//check to see if this command begins with a tab
					printf("this command starts with a tab so its good");
					break;
				}else if(lpszLine[0] == '\n' || lpszLine[0] == '#'){
					printf("tried to get command and found newline or comment, going to next line");
				}else{
					printf("this command does not begin with a tab, throw an error");
					break;
				}
			}
		}	
		/*
		 * SKIP
		 * 
		 * IF	lpszLine = \t
		 * COMMAND
		 * 
		 * ELSE
		 * 		Remove white-space
		 * TARGET
		 */
	}
	printf("\n") ;
	
		//You need to check below for parsing. 
		//Skip if blank or comment.
		//Remove leading whitespace.
		//Skip if whitespace-only.
		//Only single command is allowed.
		//If you found any syntax error, stop parsing. 
		//If lpszLine starts with '\t' it will be command else it will be target.
		//It is possbile that target may not have a command as you can see from the example on project write-up. (target:all)
		//You can use any data structure (array, linked list ...) as you want to build a graph

	//Close the makefile. 
	fclose(fp);

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
	if((parse(szMakefile)) == -1) 
	{
		return EXIT_FAILURE;
	}

	//after parsing the file, you'll want to check all dependencies (whether they are available targets or files)
	//then execute all of the targets that were specified on the command line, along with their dependencies, etc.
	return EXIT_SUCCESS;
}
