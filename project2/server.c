#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include "util.h"
/* CSci4061 S2016 Assignment 2
section: 007 (Eric)
section: 002 (Nick)
date: mm/dd/yy
name: Eric Sande, Nick Orf
id: sande807, orfxx012 */

/*
 * Identify the command used at the shell 
 */
int parse_command(char *buf) {
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
int list_users(user_chat_box_t *users, int fd) {
	/* 
	 * Construct a list of user names
	 * Don't forget to send the list to the requester!
	 */
	 
	/***** Insert YOUR code *******/
	char * noUsers = "<no users>" ;
	int i ;
	int x=0;
	for (i = 0; i < MAX_USERS; i++) {
		//go through all the users
		//if a slot is empty ignore it
		if (users[i].status == SLOT_EMPTY){
			continue ;
		}else{
			x++;
			usleep(2000);
			if (write(fd, users[i].name, strlen(users[i].name) + 1) < 0) {
				//if it's -1 write some error
				fprintf(stderr, "Unable to list users.");
			}
		}
	}
	if(x==0){
		write(fd, noUsers, strlen(noUsers) +1);
	}
}

/*
 * Add a new user
 */
int add_user(user_chat_box_t *users, char *buf, int server_fd) {
	
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
	
	char * msg = "\nAdding user " ;
	char * s = buf ;
	char * dots = "..." ;
	
	while (i < MAX_USERS) {
		if (users[i].status == SLOT_FULL) {
			i++;
		}
		else {
			
			users[i].status = SLOT_FULL ;//set users status to filled
			strcpy(users[i].name, s);//set users name
			
			if (pipe(users[i].ptoc) == -1 || pipe(users[i].ctop) == -1) {//create pipes
				//pipe fails
				perror("Failed to create the pipe.");
				return 1 ;
			}

			//set up non-blocking pipes			
			fcntl(users[i].ptoc[0], F_SETFL, O_NONBLOCK) ;
			fcntl(users[i].ctop[0], F_SETFL, O_NONBLOCK) ;
			
			sprintf(s, "%s%s%s", msg, users[i].name, dots);
			if (write(server_fd, s, strlen(s) + 1) < 0) {
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
int broadcast_msg(user_chat_box_t *users, char *buf, int fd, char *sender) {
	int i;
	const char *msg = "Broadcasting...", *s;
	char text[MSG_SIZE];

	/* Notify on server shell */
	if (write(fd, msg, strlen(msg) + 1) < 0){
		perror("writing to server shell");
	}
	
	/* Send the message to all user shells */
	s = strtok(buf, "\n");
	sprintf(text, "%s : %s", sender, s);
	usleep(100);
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
void close_pipes(int idx, user_chat_box_t *users) {
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
void cleanup_user(int idx, user_chat_box_t *users) {
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
void cleanup_users(user_chat_box_t *users) {
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
void cleanup_server(server_ctrl_t server_ctrl) {
	/***** Insert YOUR code *******/
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
int find_user_index(user_chat_box_t *users, char *name) {
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
char *extract_name(int cmd, char *buf) {
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
void send_p2p_msg(int idx, user_chat_box_t *users, char *buf) {
	/* get the target user by name (hint: call (extract_name() and send message */
	
	/***** Insert YOUR code *******/
	
	int i ;
	int count ;
	char * p = NULL;
	char * message = NULL;
	char msg[MSG_SIZE];
	char command[MSG_SIZE];
	char * error = "User not found." ;
	
	//save buf in command
	sprintf(command, "%s", buf);
	
	//get name of desired recipient
	p = extract_name(P2P, buf) ;

	//find the message by looking for the name, then looking for the next space using strstr
	message = strstr(command, p);
	message = strstr(message, " ");

	//create message
	sprintf(msg, "%s:%s", users[idx].name, message);
	
	//if recipient doesn't exist, print error to user shell
	for (i = 0; i < MAX_USERS; i++) {
		
		if (users[i].status == SLOT_EMPTY)
			continue ;
			
		if (strcmp(users[i].name, p) == 0) {//found user
			//send message
			if (write(users[i].ptoc[1], msg, strlen(msg) + 1) < 0) {
				perror("writing to server shell");
			}
			//count equals 1 if the user has been found and the message has been sent
			count = 1 ;
			break ;
		}
	}
	
	//if count doesn't equal 1, the user doesn't exist
	//if user doesn't exists, print error on user screen
	if (count != 1) {
		if (write(users[idx].ptoc[1], error, strlen(error) + 1) < 0) {
			perror("no user") ;
		}
	}
	
	
			
	
}

int main(int argc, char **argv) {
	/***** Insert YOUR code *******/
	char command[MSG_SIZE];//a place to put the incoming command

	sprintf(command,"%s","\0");

	char *user1;//a place to put a users name
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
	
	//child pid for fork
	pid_t childpid ;

	//open pipes

	if (pipe(server.ptoc) == -1 || pipe(server.ctop) == -1) {
		//pipe fails
		perror("Failed to create the pipe.");
		return 1 ;
	}

	//make pipes non blocking
	fcntl(server.ptoc[0], F_SETFL, O_NONBLOCK) ;
	fcntl(server.ctop[0], F_SETFL, O_NONBLOCK) ;

	/* Fork the server shell */
	childpid = fork() ;

	//fork fails
	if (childpid == -1) {
		perror("Failed to fork.") ;
		return 1 ;
	}

	if (childpid == 0) {
		//start server shell

        //turn server.ptoc[0] to the read character for argv
		sprintf(readint,"%d", server.ptoc[0]);
        
        //turn server.ctop[1] to the write chracter for argv
		sprintf(writeint, "%d" , server.ctop[1]);
        
        //execute shell program, pass in servername, read, and write fds
		execl("./shell", "empty","Server", readint, writeint, NULL);
        
	}else if (childpid > 0) {
		server.pid = childpid;
		//begin server program

		/* Inside the parent. This will be the most important part of this program. */
		/* Start a loop which runs every 1000 usecs.
	 	 * The loop should read messages from the server shell, parse them using the 
	 	 * parse_command() function and take the appropriate actions. */

		while (1) {
			/* Let the CPU breathe */
			usleep(1000);
			//read the server shell, put result in command
			if(read(server.ctop[0], command, MSG_SIZE)<0){
				if(waitpid((server.pid),0,WNOHANG)<0){
						printf("\n");
						return 0;
				}
			}else{
				//parse command
				cmd = parse_command(command);
				
				//switch statement
				if(cmd == CHILD_PID){
					
					//this is for when a new shell is created, we need to send the childpid back to
					//the server and the server should store it in the new users childpid category so
					//the server can later clean everything up.
					user1 = extract_name(cmd, command);
					server.child_pid = atoi(user1);
					read(server.ctop[0], command, MSG_SIZE);
					
				}else if(cmd == LIST_USERS){
					
					if(list_users(users,server.ptoc[1]) < 0){
						perror("failed to list users");
					}
	
				}else if (cmd == ADD_USER){
		
					user1 = extract_name(cmd, command);
	
					newuserindex = add_user(users, user1, server.ptoc[1]) ;
										
				}else if(cmd == KICK){
					
					user1 = extract_name(cmd, command) ;
					
					i=find_user_index(users, user1);
					
					if(i<0){
						perror("user not found");
					}
					
					cleanup_user(i, users);
					
				}else if(cmd == EXIT){
	
					cleanup_users(users);
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
					childpid = fork();
					users[newuserindex].pid = childpid;
					if (childpid == -1) {
						//fork fails
						perror("Failed to fork.") ;
						return 1 ;
					}
	
					else if (childpid == 0) {
						//if it's the child process open an xterm window and run ./shell and pass in some arguments
						//turn ptoc[0] to the read character for argv
						sprintf(readint,"%d", users[newuserindex].ptoc[0]);
						
						//turn server.ctop[1] to the write chracter for argv
						sprintf(writeint, "%d" , users[newuserindex].ctop[1]);
						
						//begin xterm window with command ./shell and give the argv username, and two file descriptors
						execl(XTERM_PATH, XTERM, "+hold","-e","./shell",users[newuserindex].name,readint,writeint, NULL);
					}
					else if (childpid > 0) {
						continue;
					}
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
				usleep(1000);
				if (users[i].status == SLOT_EMPTY) {
					//if there is no user at this spot, skip it
					continue;
				}		
				
				//otherwise, see if they have something to read
				//read file descriptor for a command
				if(read(users[i].ctop[0], command, MSG_SIZE)<0){
						if(waitpid((users[i].pid),0,WNOHANG)<0){//if this statement == 0 that means a seg fault or the window closed
							cleanup_user(i,users);//clean up the user
						}
						continue;//otherwise there is nothing in the pipe, so just move on
				}

				//parse command
				cmd = parse_command(command);
				
				//switch statement
				if(cmd == CHILD_PID){
				
					user1 = extract_name(cmd, command);
					users[i].child_pid = atoi(user1);
					read(users[i].ctop[0], command, MSG_SIZE);
					
				}else if(cmd == LIST_USERS){
					
					if(list_users(users, users[i].ptoc[1])<0)//send list of users to user shell
						perror("failed to list users");
						
				}else if (cmd == P2P){
					
					send_p2p_msg(i,users,command);//pass the command, then use command to find which user its supposed to go to
					
				}else if(cmd == EXIT){

					cleanup_user(i, users);
					
				}else{
					//broadcast
					broadcast_msg(users,command,server.ptoc[1],users[i].name);//PROBABLY NOT RIGHT FILE DESCRIPTOR, FIX THIS
					
				}
			}
		}/* while loop ends when server shell sees the \exit command */
	}
	return 0;
}
