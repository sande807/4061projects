#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int signo)
{
	if (signo == SIGINT)//if signal is an interrupt attempt
		printf("recieved SIGINT\n");//print this
}

void alarm_handler(int signo){
	
	if(signo == SIGALRM){//if signal is sig alarm
		printf("alarm\n");//print out alarm
		alarm(2);//reset alarm
	}

}

int main(void){
	if(signal(SIGINT, sig_handler) == SIG_ERR)//signal function sets up signal with handler
		printf("can't catch SIGINT\n");//if error
		
	if(signal(SIGALRM, alarm_handler) == SIG_ERR)//signal function sets up signal with handler
		printf("can't catch SIGALRM\n");//if error
		
	alarm(2);//begin alarm
		
	while(1)//infinite loop
		sleep(1);
		
	return 0;
}
