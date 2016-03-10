#include <stdio.h>

int is_empty(char *line)
{
	while (*line != '\0') {
		if (!isspace(*line))
			return 0;
		line++;
	}
	return 1;
}

int main(){
	printf("SERVER >> ");
	char *line = NULL;
	ssize_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	if(is_empty(line)){
		printf("is empty\n");
	}
	printf("line: %s \n", line);
	return 0;
}
