#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

#include "process.h"

int mailbox_id;
packet_t packet;
sigset_t set;
pid_t childpid;
int wait = 1;

void sig_handler(int signo)
{
	if (signo == SIGIO)//if signal is an interrupt attempt
		printf("recieved SIGIO\n");//print this
		wait = 0;
		//packet_t newpack;
		//msgrcv(mailbox_id,&newpack,sizeof(newpack),1,0);
		//printf(newpack->data);
}

void alarm_handler(int signo){
	
	if(signo == SIGALRM){//if signal is sig alarm
		printf("alarm\n");//print out alarm
		alarm(10);//reset alarm
	}

}

int main(void){
	if(signal(SIGIO, sig_handler) == SIG_ERR)//signal function sets up signal with handler
		printf("can't catch SIGINT\n");//if error
		
	if(signal(SIGALRM, alarm_handler) == SIG_ERR)//signal function sets up signal with handler
		printf("can't catch SIGALRM\n");//if error
	
	if (mailbox_id = msgget(1, IPC_CREAT) == -1)
		perror("failed to create message queue") ;    

	alarm(10);//begin alarm
	
	sigemptyset(&set);
	sigaddset(&set, SIGIO);
	
	childpid = fork();
	if(childpid == -1){
		return -1;
	}else if(childpid == 0){
		printf("child %d, setting variables\n", (int)getpid());
		packet.mtype = 1;
		strcpy(packet.data,"hello world\n");
		packet.pid = (int)getpid();
		printf(packet.data);
		printf("child, send message\n");
		if(msgsnd(mailbox_id,&packet,sizeof(packet),0)== -1){
			printf("message send failed %d\n", errno);
		}
		printf("child, sending sig to %d\n", (int)getppid());
		kill(getppid(),SIGIO);
	}else{
		printf("parent process %d, waiting now\n", (int)getpid());
		while(wait){}
		printf("after signal\n");
	}
		

	return 0;
}
