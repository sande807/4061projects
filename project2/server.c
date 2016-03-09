#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
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
	 int i;
	 /***** Insert YOUR code *******/
	for (i = 0; i < MAX_USERS; i++) {
		//go through all the users
		
		//if a slot is empty ignore it
		if (users[i].status == SLOT_EMPTY)
			continue;
			
		//otherwise write the name of that user to the file descriptor given
		if (write(fd, users[i].name, strlen(users[i].name) + 1) < 0)
			//if it's -1 write some error
			perror("write to child shell failed");
	}
	//is this function complete?
}

/*
 * Add a new user
 */
int add_user(user_chat_box_t *users, char *buf, int server_fd)
{
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
	
	//user limit is 10, check limit
	//if 

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
	if (write(fd, msg, strlen(msg) + 1) < 0)
		perror("writing to server shell");
	
	/* Send the message to all user shells */
	s = strtok(buf, "\n");
	sprintf(text, "%s : %s", sender, s);
	for (i = 0; i < MAX_USERS; i++) {
		if (users[i].status == SLOT_EMPTY)
			continue;
		if (write(users[i].ptoc[1], text, strlen(text) + 1) < 0)
			perror("write to child shell failed");
	}
	
	//is this function complete?
}

/*
 * Close all pipes for this user
 */
void close_pipes(int idx, user_chat_box_t *users)
{
	/***** Insert YOUR code *******/
	int i ;
	
	//is this correct?
	//do we have to use pids here? i don't think so but maybe
	for (i = 0; i < MAX_USERS; i++) {
		
		if (users[i].status == SLOT_EMPTY)
			continue ;
			
		close(idx) ;
	}
	
}

/*
 * Cleanup single user: close all pipes, kill user's child process, kill user 
 * xterm process, free-up slot.
 * Remember to wait for the appropriate processes here!
 */
void cleanup_user(int idx, user_chat_box_t *users)
{
	/***** Insert YOUR code *******/
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
	
	char *msg ;
	int i ;
	
	msg = extract_name(idx, buf) ;
	
	for (i = 0; i < MAX_USERS; i++) {
		
		if (users[i].status == SLOT_EMPTY)
			continue ;
			
		//if (
		
	}
	
}

int main(int argc, char **argv)
{
	printf("wokring") ;
	/***** Insert YOUR code *******/
	char command[MSG_SIZE];//a place to put the incoming command
	char user1[MSG_SIZE];//a place to put a users name
	char user2[MSG_SIZE];//a place for another user (p2p)
	int cmd, i;//int version of command for parsing. also i.
	user_chat_box_t users[10];//an array of users
	server_ctrl_t server;

	//pipe arrays
	int fd1[2] ;//server.c to server shell
	int fd2[2] ;//server shell to server.c
	
	//child pid for fork
	pid_t childpid ;
	
	/* open non-blocking bi-directional pipes for communication with server shell */
	if (pipe(fd1) == -1 || pipe(fd2) == -1) {
		//pipe fails
		perror("Failed to create the pipe.") ;
		return 1 ;
	}

	/* Fork the server shell */
	childpid = fork() ;
	if (childpid == -1) {
		//fork fails
		perror("Failed to fork.") ;
		return 1 ;
	}
		/* 
	 	 * Inside the child.
		 * Start server's shell.
	 	 * exec the SHELL program with the required program arguments.
	 	 */
	
	/* pipes need to be passed to the server SHELL process as arguments
	 * this way the SHELL knows which pipes it needs to read and wrtie on
	 * for communicating with the server
	 * 
	 * the name of the server needs to be passed to the SHELL process so
	 * it can be displayed as part of the SHELL prompt
	 */
	 
	if (childpid == 0) {
		//start server shell?
	}
	else {
		//exec shell program?
	}

	/* Inside the parent. This will be the most important part of this program. */

		/* Start a loop which runs every 1000 usecs.
	 	 * The loop should read messages from the server shell, parse them using the 
	 	 * parse_command() function and take the appropriate actions. */
		while (1) {
			/* Let the CPU breathe */
			usleep(1000);
			/* 
		 	 * 1. Read the message from server's shell, if any
		 	 * 2. Parse the command
		 	 * 3. Begin switch statement to identify command and take appropriate action
		 	 * 		List of commands to handle here:
		 	 * 			CHILD_PID
		 	 * 			LIST_USERS
		 	 * 			ADD_USER
		 	 * 			KICK
		 	 * 			EXIT
		 	 * 			BROADCAST 
		 	 */
			
			//read fd2[0] (server shell to program), put the result in command
			read(fd2[0], command, MSG_SIZE);//maybe check if there's nothing there?
			//NOT SURE IF FILE DESCRIPTOR IS CORRECT
			
			//parse command
			cmd = parse_command(command);
			
			//switch statement
			if(cmd == CHILD_PID){
				
				//no idea what this one does
				//
				
			}else if(cmd == LIST_USERS){
				
				//PROBABLY NOT RIGHT FILE DESCRIPTOR
				if(list_users(users,fd1[1])<0)
					//send list of users to server shell(fd1[0])???
					perror("failed to list users");
					
			}else if (cmd == ADD_USER){
				
				//user is a char array, and therefore should be treated as such
				//user1[0] = extract_name(cmd, command); this brings warning but is allowed
				user1[0] = *extract_name(cmd, command) ;
				
				add_user(users, user1, fd1[0]);//PROBABLY NOT RIGHT FILE DESCRIPTOR, FIX THIS
				
			}else if(cmd == KICK){
				
				//user is a char array, and therefore should be treated as such
				//user1[0] = extract_name(cmd, command) ; this brings warning but is allowed
				user1[0] = *extract_name(cmd, command) ;
				
				i=find_user_index(users, user1);
				
				if(i<0){
					perror("user not found");
				}
				
				cleanup_user(i, users);
				
			}else if(cmd == EXIT){
				
				cleanup_server(server);
				
			}else{
				
				//broadcast
				broadcast_msg(users,command,fd1[0],"server");//PROBABLY NOT RIGHT FILE DESCRIPTOR, FIX THIS
				
			}
			/* Fork a process if a user was added (ADD_USER) */
				/* Inside the child */
				/*
			 	 * Start an xterm with shell program running inside it.
			 	 * execl(XTERM_PATH, XTERM, "+hold", "-e", <path for the SHELL program>, ..<rest of the arguments for the shell program>..);
			 	 */
			if (childpid == 0) {
				//start new user shell?
			}
			else {
				//exec user program?
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
			for (i = 0; i < MAX_USERS; i++) {//this will go through all the users
				
				if (users[i].status == SLOT_EMPTY)//if there is no user at this spot, skip it
					continue;
				
				//otherwise, see if they have something to read
				
				//read file descriptor for a command
				read(fd2[0], command, MSG_SIZE);//maybe check if there's nothing there?
				//NOT SURE IF FILE DESCRIPTOR IS CORRECT
				
				//parse command
				cmd = parse_command(command);
				
				//switch statement
				if(cmd == CHILD_PID){
					
					//no idea what this one does
					//
					
				}else if(cmd == LIST_USERS){
					
					//PROBABLY NOT RIGHT FILE DESCRIPTOR
					if(list_users(users,fd1[1])<0)//send list of users to server shell(fd1[0])???
						perror("failed to list users");
						
				}else if (cmd == P2P){
					
					send_p2p_msg(i,users,command);//pass the command, then use command to find which user its supposed to go to
					
				}else if(cmd == EXIT){
					
					cleanup_user(i, users);
					
				}else{
					
					//broadcast
					broadcast_msg(users,command,fd1[0],"server");//PROBABLY NOT RIGHT FILE DESCRIPTOR, FIX THIS
					
				}
			}
	}	/* while loop ends when server shell sees the \exit command */

	return 0;
}
