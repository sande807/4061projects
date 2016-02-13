#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"

struct target target1[100];
int numtargets = 0;
int i = 0;
int x = 0;
char *str;
char make[] = "make4061";

int main(){
	//creating an example target from project description
	str = make;
	strcpy(target1[numtargets].szTarget,str);
	strcpy(target1[numtargets].szDependencies[0],"util.a");
	target1[numtargets].nDependencyCount=1;
	strcpy(target1[numtargets].szDependencies[1],"main.o");
	target1[numtargets].nDependencyCount+=1;
	strcpy(target1[numtargets].szCommand, "gcc -0 make4061 main.o util.a");
	numtargets += 1;
	//creating a second target
	strcpy(target1[numtargets].szTarget,"main.o");
	strcpy(target1[numtargets].szDependencies[0],"main.c");
	target1[numtargets].nDependencyCount=1;
	strcpy(target1[numtargets].szCommand, "gcc -c main.c");
	numtargets += 1;
	for(i = 0; i<numtargets; i+=1){//print the targets, the number of dependencies and their command
		printf("Target = %s\n", target1[i].szTarget);
		printf("Dependency Count = %d\n",target1[i].nDependencyCount);
		while(x< target1[i].nDependencyCount){
			printf("Dependency = %s\n", target1[i].szDependencies[x]);
			x+=1;
		}
		x = 0;
		printf("Command = %s\n", target1[i].szCommand);
	}
}
