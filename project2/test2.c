#include <stdio.h>
int main(){
	printf("SERVER >> ");
	char *line = NULL;
	ssize_t bufsize = 0;
	getline(&line, &bufsize, stdin);

	printf("line: %s \n", line);

	return 0;
}
