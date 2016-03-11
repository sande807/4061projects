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
	for (i = 0; i < MAX_USERS; i++) {
		//go through all the users
		
		//if a slot is empty ignore it
		if (users[i].status == SLOT_EMPTY)
			continue;
			
		//otherwise write the name of that user to the file descriptor given
		if (write(fd, users[i].name, strlen(users[i].name) + 1) < 0) {
			//if it's -1 write some error
			/* i think we are supposed to write an error message to screen
			 * instead of sending an error that would kill the entire 
			 * process.
			 *
			perror("write to child shell failed");
			*/
			fprintf(stderr, "Unable to list users.\n");
			
			/* or do we simply say there are no useres to list and do
			 * the following
			frpintf(stderr, "<no users>\n") ;
			*/	
		}
	}
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

	int i ;
	int flags ;
	
	char *msg = "Adding user " ;
	char *s = strtok(buf, "\n") ;
	
	for (i = 0; i < MAX_USERS; i++) {
		
		if (users[i].status == SLOT_FULL)
			continue ;
			
		printf("adding user") ;
		
		//set user name	
		strcpy(users[i].name, s) ;
		
		//set up non-blocking pipes
		flags = fcntl (server_fd, F_GETFL, 0) ; //not sure what fd should be
		fcntl (users[i].ptoc[0], F_SETFL, flags | O_NONBLOCK); 
		
		//notify on server shell
		if (write(server_fd, strcat(msg, s), strlen(msg) + 1) < 0)
			perror("writing to server shell") ;
		
		break;		
	}
	
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
	char user1[MSG_SIZE];//a place to put a users name
	char user2[MSG_SIZE];//a place for another user (p2p)
	int cmd, i;//int version of command for parsing. also i.
	int usercount = 0;//user count cause this is easier
	user_chat_box_t users[MAX_USERS];//an array of users
	server_ctrl_t server;

	//pipe arrays
	int fd_tshell[2] ;//server.c to server shell
	int fd_fshell[2] ;//server shell to server.c
	
	int flag_t ;
	int flag_f ;
	
	//child pid for fork
	pid_t childpid ;
	
	/* open non-blocking bi-directional pipes for communication with server shell */
	
	//code for making pipes non-blocking
	//int flags ;
	//flags = fcntl (fd, F_GETFL, 0) ; not sure what fd should be
	//fcntl (fd1[0], F_SETFL, flags | O_NONBLOCK);
	//fcntl (fd2[0], F_SETFL, flags | O_NONBLOCK);

	
	if (pipe(fd_tshell) == -1 || pipe(fd_fshell) == -1) {
		//pipe fails
		perror("Failed to create the pipe.") ;
		return 1 ;
	}
	
	flag_t = fcntl(* fd_tshell, F_GETFL, 0) ;
	fcntl(fd_tshell[0], F_SETFL, flag_t | O_NONBLOCK) ;
	
	flag_f = fcntl(* fd_fshell, F_GETFL, 0) ;
	fcntl(fd_fshell[1], F_SETFL, flag_f | O_NONBLOCK) ;
	
	
	
	
	printf("create pipe done\n");
	/* Fork the server shell */
	printf("fork now\n");
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
		//start server shell
		//int write = fd_fshell[1];
		//int read = fd_tshell[0];
		printf("i am child, attempting to start shell\n");
		execl("./shell", "Server", fd_fshell, fd_tshell, NULL);
	}
	else if (childpid > 0) {
		//exec shell program
		printf("parent process, continuing to server program\n");
		//execl(XTERM_PATH, XTERM, "+hold","-e","./shell","user1", NULL);
	//}
	/* Inside the parent. This will be the most important part of this program. */

		/* Start a loop which runs every 1000 usecs.
	 	 * The loop should read messages from the server shell, parse them using the 
	 	 * parse_command() function and take the appropriate actions. */
		printf("starting server.c loop\n");
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
			printf("before read\n");
			read(fd_fshell[0], command, MSG_SIZE);//maybe check if there's nothing there?
			printf("after read\n") ;
			//NOT SURE IF FILE DESCRIPTOR IS CORRECT
			printf("in server.c main loop command is: %s\n",command); 
			//parse command
			cmd = parse_command(command);
			
			//switch statement
			if(cmd == CHILD_PID){
				
				//no idea what this one does
				
			}else if(cmd == LIST_USERS){
				
				//PROBABLY NOT RIGHT FILE DESCRIPTOR
				if(list_users(users,fd_tshell[1]) < 0)
					//send list of users to server shell(fd1[0])???
					perror("failed to list users");
					
			}else if (cmd == ADD_USER){
				
				//user is a char array, and therefore should be treated as such
				user1[0] = *extract_name(cmd, command) ;
				printf("adding user, not in func") ;
				add_user(users, user1, fd_fshell[0]);//PROBABLY NOT RIGHT FILE DESCRIPTOR, FIX THIS
				
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
				broadcast_msg(users,command,fd_tshell[0],"server");//PROBABLY NOT RIGHT FILE DESCRIPTOR, FIX THIS
				
			}

			/* Fork a process if a user was added (ADD_USER) */
				/* Inside the child */
				/*
			 	 * Start an xterm with shell program running inside it.
			 	 * execl(XTERM_PATH, XTERM, "+hold", "-e", <path for the SHELL program>, ..<rest of the arguments for the shell program>..);
			 	 */
			if(cmd == ADD_USER){
				childpid = fork();
				if (childpid == -1) {
					//fork fails
					perror("Failed to fork.") ;
					return 1 ;
				}

				if (childpid == 0) {//if it's the child process open an xterm window and run ./shell and pass in some arguments
					printf("child program\n");
					execl(XTERM_PATH, XTERM, "+hold","-e","./shell","username", NULL);
				}
				else if (childpid > 0) {
					//exec shell program?
					wait();
					printf("parent program\n");
					printf("done\n");	
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
					
					//PROBABLY NOT RIGHT FILE DESCRIPTOR
					if(list_users(users,fd_tshell[1])<0)//send list of users to server shell(fd1[0])???
						perror("failed to list users");
						
				}else if (cmd == P2P){
					
					send_p2p_msg(i,users,command);//pass the command, then use command to find which user its supposed to go to
					
				}else if(cmd == EXIT){
					
					cleanup_user(i, users);
					
				}else{
					
					//broadcast
					broadcast_msg(users,command,fd_tshell[1],"server");//PROBABLY NOT RIGHT FILE DESCRIPTOR, FIX THIS
					
				}
			}
		}/* while loop ends when server shell sees the \exit command */
	}
	return 0;
}
