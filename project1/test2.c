#include <stdio.h>
#include <string.h>

int main(){
	char str[] = "make4061: util.a main.o";
	char *pch;
	char *target;
	char *dependencies[10];
	int num;
	int i=0;
	num = strlen(str);
	printf("str length = %d\n", num);
	printf ("splitting string \"%s\" into tokens:\n", str);
	pch = strtok (str,": ");
	//printf("i is %d\n", i);
	while (pch != NULL){
		//printf("i is %d", i);
		if(i==0){
			//printf("the target is:%s\n", pch );
			target = pch;
		}else{
			//printf("dependency number %d is:%s\n", i-1,pch);
			dependencies[i-1] = pch;
		}
		i= i+1;
		pch = strtok (NULL, ": ");
	}
	printf("target = %s\n", target);
	printf("dependency = %s\n", dependencies[0]);
	printf("dependency = %s\n", dependencies[1]);
	return 0;
}
