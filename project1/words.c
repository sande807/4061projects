#include <stdio.h>
#include <string.h>

int main() {
	char message[] = "all: make4061 butt-> stuff" ;
	char* word ;
	
	word = strtok(message, ":") ;
	printf("target:%s\n", word) ;
	
	word = strtok(NULL, " ") ;
	printf("target2:%s\n", word) ;	
}	
