#include <stdio.h>
#include <string.h>

int main(){
	char str[] = "make4061: util.a main.o";
	char *butt;
	char *target;
	char *dependencies[10];
	int num=0;
	int i=0;
	num = strlen(str);
	butt = str;
	printf("butt = %s\n", butt);
	for(i=0; i<10; i+=1){
		printf("butt character %d = %s\n",i, &butt[i]);
	}
	num = strcmp(butt, "make4061: util.a main.o");
	printf("num = %d\n", num);
	target = strstr(butt, ":");
	printf("target = %s\n", target);
	return 0;
}
