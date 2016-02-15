#include <stdio.h>
#include <string.h>

int main(){
	char str[] = "make4061: util.a main.o";
	char *butt;
	char *target;
	char *target2;
	char comment[]= "#this is a comment";
	char destination[8];
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
	for(i=0;i<length;i+=1){
		destination[i] = butt[i];	
	}
	printf("destination = %s\n",destination);
	printf("target =%s\n",target);
	for(i=0;i<20;i+=1){
		target = strstr(&target[1], " ");
		target2 = strstr(&target[1], " ");
		if(target2 == NULL){
			num = i+1;
			i = 2000;
			length = strlen(target);
		}else{
			length = strlen(target) - strlen(target2);
		}
		printf("target =%s\n", target);
		printf("target2 =%s\n", target2);
		for(i=1; i<length; i+=1){
			destination[i-1] = target[i];
		}
		printf("dependency length = %d\n",length);
		printf("destination = %s\n",destination);
	}
	printf("number of dependencies = %d\n", num);
	return 0;
}

