#include <stdio.h>
#include <string.h>

int main(){
	char str[] = "make4061: util.a main.o";
	char *butt;
	char *target;
	char comment[]= "#this is a comment";
	char destination[64];
	int num = 0;
	int length;
	int i = 0;
	butt = str;
	printf("butt = %s\n", butt);
	//for(i=0; i<10; i+=1){
	//	printf("butt character %d = %s\n",i, &butt[i]);
	//}
	
	if (comment[0] =  '#'){
	printf("this is a comment: %s\n", comment);
	}else{
	printf("not a comment\n");
	}
	target = strstr(butt, ":");
	printf("target = %s\n", target);
	length = strlen(butt)-strlen(target);
	printf("target length = %d\n", length);
	printf("%*.*s\n",length,length,butt);
	//strncpy(destination,butt,length);
	//destination[length] = '\0';
	//printf("result of strncpy = %s\n", destination);
	return 0;
}

