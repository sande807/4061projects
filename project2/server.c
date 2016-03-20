#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include "util.h"

/*
 * Identify the command used at the shell 
 */
int parse_command(char *buf)
{
	int cmd;

	if (starts_with(buf, CMD_CHILD_PID))
		cmd = CHILD_PID;
	else if (starts_with(buf, CMD_P2P))
		cmd = P2P;
	else if (starts_with(buf, CMD_LIST_USERS))
		cmd = LIST_USERS;
	else if (starts_with(buf, CMD_ADD_USER))
		cmd = ADD_USER;
	else if (starts_with(buf, CMD_EXIT))
		cmd = EXIT;
	else if (starts_with(buf, CMD_KICK))
		cmd = KICK;
	else
		cmd = BROADCAST;

	return cmd;
}

/*
 * List the existing users on the server shell
 */
int list_users(user_chat_box_t *users, int fd)
{
	/* 
	 * Construct a list of user names
	 * Don't forget to send the list to the requester!
	 */
	 
	/***** Insert YOUR code *******/
	int i ;
	int x=0;
	for (i = 0; i < MAX_USERS; i++) {
		//go through all the users
		
		//if a slot is empty ignore it
		if (users[i].status == SLOT_EMPTY){
			continue;
		}else{
			x++;
			if (write(fd, users[i].name, strlen(users[i].name) + 1) < 0) {
			//if it's -1 write some error
			fprintf(stderr, "Unable to list users.\n");
			}
		}
	}
	if(x==0){
		write(fd, "no users\n", strlen("no users\n") +1);
	}
}

/*
 * Add a new user
 */
int add_user(user_chat_box_t *users, char *buf, int server_fd)
{
	printf("in add user function\n") ;
	/***** Insert YOUR code *******/
	
	/* 
	 * Check if user limit reached.
	 *
	 * If limit is okay, add user, set up non-blocking pipes and
	 * notify on server shell
	 *
	 * NOTE: You may want to remove any newline characters from the name string 
	 * before adding it. This will help in future name-based search.
	 */
	 
	int i = 0 ;
	int flag_t;
	int flag_f;
	
	char * msg = "Adding user " ;
	char * s = buf ;
	
	printf("name = %s\n", s) ;
	
	while (i < MAX_USERS) {
		if (users[i].status == SLOT_FULL) {
			printf("cannot add user: slot full\n") ;
			i++;
		}
		else {
			printf("empty slot! time to add!\n") ;
			
			users[i].status = SLOT_FULL ;
			strcpy(users[i].name, s) ;
			
			if (pipe(users[i].ptoc) == -1 || pipe(users[i].ctop) == -1) {
				//pipe fails
				perror("Failed to create the pipe.");
				return 1 ;
			}

			//set up non-blocking pipes
			//flag_t = fcntl (* users[i].ptoc , F_GETFL, 0) ; //not sure what fd should be
			fcntl (users[i].ptoc[0], F_SETFL, O_NONBLOCK); 
			//flag_f = fcntl(* users[i].ctop, F_GETFL, 0);
			fcntl(users[i].ctop[0], F_SETFL, O_NONBLOCK);
			
			printf("user %s added!\n", users[i].name) ;
			
			if (write(users[i].ptoc[1], s, strlen(s) + 1) < 0) {
				perror("writing to server shell failed") ;
			}
			return i;
		}
	}
	return -1;
	
}

/*
 * Broadcast message to all users. Completed for you as a guide to help with other commands :-).
 */
int broadcast_msg(user_chat_box_t *users, char *buf, int fd, char *sender)
{
	int i;
	const char *msg = "Broadcasting...", *s;
	char text[MSG_SIZE];

	/* Notify on server shell */
	if (write(fd, msg, strlen(msg) + 1) < 0){
		perror("writing to server shell");
	}else{
		//printf("within broadcast, write works\n");
	}

	
	/* Send the message to all user shells */
	s = strtok(buf, "\n");
	sprintf(text, "%s : %s", sender, s);
	usleep(100);
	write(fd, text, strlen(text) + 1);
	for (i = 0; i < MAX_USERS; i++) {
		if (users[i].status == SLOT_EMPTY){
			continue;
		}
		if (write(users[i].ptoc[1], text, strlen(text) + 1) < 0)
			perror("write to child shell failed");
	}
}

/*
 * Close all pipes for this user
 */
void close_pipes(int idx, user_chat_box_t *users)
{
	/***** Insert YOUR code *******/
	close(users[idx].ptoc[0]);
	close(users[idx].ptoc[1]);
	close(users[idx].ctop[0]);
	close(users[idx].ctop[1]);
	
}

/*
 * Cleanup single user: close all pipes, kill user's child process, kill user 
 * xterm process, free-up slot.
 * Remember to wait for the appropriate processes here!
 */
void cleanup_user(int idx, user_chat_box_t *users)
{
	/***** Insert YOUR code *******/
	close_pipes(idx,users);
	kill(users[idx].child_pid,SIGKILL);
	kill(users[idx].pid,SIGKILL);
	users[idx].status = SLOT_EMPTY;
	//kill is probably incorrect, but this should be most of what we need to do
	
	//has the waiting been done here?

}

/*
 * Cleanup all users: given to you
 */
void cleanup_users(user_chat_box_t *users)
{
	int i;

	for (i = 0; i < MAX_USERS; i++) {
		if (users[i].status == SLOT_EMPTY)
			continue;
		cleanup_user(i, users);
	}
}

/*
 * Cleanup server process: close all pipes, kill the parent process and its 
 * children.
 * Remember to wait for the appropriate processes here!
 */
void cleanup_server(server_ctrl_t server_ctrl)
{
	/***** Insert YOUR code *******/
	//cleanup_users(users);
	//NEED TO CALL THIS BUT WE DONT GET THE USER STRUCTURE?
	close(server_ctrl.ptoc[0]);//close one pipe end 0
	close(server_ctrl.ptoc[1]);//close one pipe end 1
	close(server_ctrl.ctop[0]);//close the other pipe end 0
	close(server_ctrl.ctop[1]);//close the other pipe end 1
	kill(server_ctrl.child_pid,SIGKILL);//kill the child
	kill(server_ctrl.pid,SIGKILL);//kill yourself
	//kill command probably wrong but this is what we need to do
}

/*
 * Utility function.
 * Find user index for given user name.
 */
int find_user_index(user_chat_box_t *users, char *name)
{
	int i, user_idx = -1;

	if (name == NULL) {
		fprintf(stderr, "NULL name passed.\n");
		return user_idx;
	}
	for (i = 0; i < MAX_USERS; i++) {
		if (users[i].status == SLOT_EMPTY)
			continue;
		if (strncmp(users[i].name, name, strlen(name)) == 0) {
			user_idx = i;
			break;
		}
	}
	return user_idx;
}

/*
 * Utility function.
 * Given a command's input buffer, extract name.
 */
char *extract_name(int cmd, char *buf)
{
	char *s = NULL;

	s = strtok(buf, " ");
	s = strtok(NULL, " ");
	if (cmd == P2P)
		return s;	/* s points to the name as no newline after name in P2P */
	s = strtok(s, "\n");	/* other commands have newline after name, so remove it*/
	return s;
}

/*
 * Send personal message. Print error on the user shell if user not found.
 */
void send_p2p_msg(int idx, user_chat_box_t *users, char *buf)
{
	/* get the target user by name (hint: call (extract_name() and send message */
	
	/***** Insert YOUR code *******/
	
	int i ;
	char *p ;
	
	//get name of desired recipient
	p = extract_name(idx, buf) ;
	
	//if recipient doesn't exist, print error to user shell
	for (i = 0; i < MAX_USERS; i++) {
		
		if (users[i].status == SLOT_EMPTY)
			continue ;
			
		if (strcmp(users[i].name, p) == 0) {
			//send message
			//if (write(users[i].ptoc[1], msg, strlen(msg) + 1) < 0) {
			//	perror("writing to server shell");
			//}
			break ;
		}
	}
	
	//if user doesn't exists, print error
	
			
	
}

int main(int argc, char **argv)
{
	printf("starting\n");
	/***** Insert YOUR code *******/
	char command[MSG_SIZE];//a place to put the incoming command
	char *user1;//a place to put a users name
	char *user2;//a place for another user (p2p)
	char readint[MSG_SIZE];//create read character for converting ints for argv
	char writeint[MSG_SIZE];//create write character for converting ints for argv
	int cmd, i, newuserindex;//int version of command for parsing. also i.

	user_chat_box_t users[MAX_USERS];//an array of users
	//make all users empty
	for (i = 0; i < MAX_USERS; i++) {
		users[i].status = SLOT_EMPTY;
	}
	//create server structure
	server_ctrl_t server;

	int flag_t ;
	int flag_f ;
	
	//child pid for fork
	pid_t childpid ;

	//open pipes

	if (pipe(server.ptoc) == -1 || pipe(server.ctop) == -1) {
		//pipe fails
		perror("Failed to create the pipe.");
		return 1 ;
	}

	//make pipes non blocking
	//flag_t = fcntl(* server.ptoc, F_GETFL, 0) ;
	//fcntl(server.ptoc[0], F_SETFL, flag_t || O_NONBLOCK) ;
	//flag_f = fcntl(* server.ctop, F_GETFL, 0) ;
	//fcntl(server.ctop[0], F_SETFL, flag_f || O_NONBLOCK) ;

	/* Fork the server shell */
	childpid = fork() ;

	//fork fails
	if (childpid == -1) {
		perror("Failed to fork.") ;
		return 1 ;
	}

	if (childpid == 0) {
		//start server shell
		//printf("i am child, attempting to start shell\n");
		//printf("server.ptoc[0]:%d\n",server.ptoc[0]);
		//printf("server.ptoc[1]:%d\n",server.ptoc[1]);
		//printf("server.ctop[0]:%d\n",server.ctop[0]);
		//printf("server.ctop[1]:%d\n",server.ctop[1]);

		sprintf(readint,"%d", server.ptoc[0]);//turn server.ptoc[0] to the read character for argv
		sprintf(writeint, "%d" , server.ctop[1]);//turn server.ctop[1] to the write chracter for argv

		execl("./shell", "empty","Server", readint, writeint, NULL);//execute shell program, pass in servername, read, and write fds
	}else if (childpid > 0) {
		//begin server program
		//execl(XTERM_PATH, XTERM, "+hold","-e","./shell","user1", NULL);

		/* Inside the parent. This will be the most important part of this program. */
		/* Start a loop which runs every 1000 usecs.
	 	 * The loop should read messages from the server shell, parse them using the 
	 	 * parse_command() function and take the appropriate actions. */

		printf("starting server.c loop\n");
		while (1) {
			/* Let the CPU breathe */
			usleep(1000);
			//read the server shell, put result in command
			read(server.ctop[0], command, MSG_SIZE);

			printf("in server.c main loop command is: %s\n",command); 
			//parse command
			cmd = parse_command(command);
			
			//switch statement
			if(cmd == CHILD_PID){
				
				//this is for when a new shell is created, we need to send the childpid back to
				//the server and the server should store it in the new users childpid category so
				//the server can later clean everything up.
				
			}else if(cmd == LIST_USERS){
				
				printf("list users command found\n");
				if(list_users(users,server.ptoc[1]) < 0){
					perror("failed to list users");
				}
				printf("list users command done\n");

			}else if (cmd == ADD_USER){

				printf("add user command found\n");

				user1 = extract_name(cmd, command);

				newuserindex = add_user(users, user1, server.ctop[0]);//PROBABLY NOT RIGHT FILE DESCRIPTOR, FIX THIS
				printf("newuserindex:%d\n",newuserindex);
				
			}else if(cmd == KICK){
				printf("kick user\n");
				
				user1 = extract_name(cmd, command) ;
				
				i=find_user_index(users, user1);
				
				if(i<0){
					perror("user not found");
				}
				
				cleanup_user(i, users);
				
			}else if(cmd == EXIT){

				printf("exit server\n");
				cleanup_server(server);
				
			}else{			
				broadcast_msg(users,command,server.ptoc[1],"Server");
			}

			/* Fork a process if a user was added (ADD_USER) */
				/* Inside the child */
				/*
			 	 * Start an xterm with shell program running inside it.
			 	 * execl(XTERM_PATH, XTERM, "+hold", "-e", <path for the SHELL program>, ..<rest of the arguments for the shell program>..);
			 	 */
			if(cmd == ADD_USER){
				printf("next is to fork using execl xterm\n") ;
				childpid = fork();
				if (childpid == -1) {
					//fork fails
					perror("Failed to fork.") ;
					return 1 ;
				}
				else if (childpid == 0) {//if it's the child process open an xterm window and run ./shell and pass in some arguments
					printf("child program\n");
					sprintf(readint,"%d", users[newuserindex].ptoc[0]);//turn server.ptoc[0] to the read character for argv
					printf("readint:%s\n",readint);
					printf("users.ptoc[0]=%d\n",users[newuserindex].ptoc[0]);
					sprintf(writeint, "%d" , users[newuserindex].ctop[1]);//turn server.ctop[1] to the write chracter for argv
					printf("writeint:%s\n",writeint);
					printf("users.ctop[1]=%d\n",users[newuserindex].ctop[1]);
					execl(XTERM_PATH, XTERM, "+hold","-e","./shell",users[newuserindex].name,readint,writeint, NULL);
				}
				else if (childpid > 0) {
					continue;
				}
			}
			/* Back to our main while loop for the "parent" */
			/* 
		 	 * Now read messages from the user shells (ie. LOOP) if any, then:
		 	 * 		1. Parse the command
		 	 * 		2. Begin switch statement to identify command and take appropriate action
		 	 *
		 	 * 		List of commands to handle here:
		 	 * 			CHILD_PID
		 	 * 			LIST_USERS
		 	 * 			P2P
		 	 * 			EXIT
		 	 * 			BROADCAST
		 	 *
		 	 * 		3. You may use the failure of pipe read command to check if the 
		 	 * 		user chat windows has been closed. (Remember waitpid with WNOHANG 
		 	 * 		from recitations?)
		 	 * 		Cleanup user if the window is indeed closed.
		 	 */
		 	 
			for (i = 0; i < MAX_USERS; i++) {
				
				if (users[i].status == SLOT_EMPTY) {
					//if there is no user at this spot, skip it
					continue;
				}		
				
				//otherwise, see if they have something to read
				
				//read file descriptor for a command
				read(users[i].ctop[0], command, MSG_SIZE);//maybe check if there's nothing there?
				//NOT SURE IF FILE DESCRIPTOR IS CORRECT
				
				//parse command
				cmd = parse_command(command);
				
				//switch statement
				if(cmd == CHILD_PID){
					
					//no idea what this one does
					//
					
				}else if(cmd == LIST_USERS){
					
					if(list_users(users, users[i].ctop[1])<0)//send list of users to user shell
						perror("failed to list users");
						
				}else if (cmd == P2P){
					printf("p2p\n") ;
					
					send_p2p_msg(i,users,command);//pass the command, then use command to find which user its supposed to go to
					
				}else if(cmd == EXIT){
					
					cleanup_user(i, users);
					
				}else{
					
					//broadcast
					broadcast_msg(users,command,server.ptoc[1],"Server");//PROBABLY NOT RIGHT FILE DESCRIPTOR, FIX THIS
					
				}
			}
		}/* while loop ends when server shell sees the \exit command */
	}
	return 0;
}
