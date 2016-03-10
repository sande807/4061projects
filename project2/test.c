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
*/
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
		if (write(fd, users[i].name, strlen(users[i].name) + 1) < 0)
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
	int status, nbytes, flags;
	int fd1[2];
	int fd2[2];
	printf("hello world\n");
	fcntl (fd1[0], F_SETFL, flags | O_NONBLOCK);
	fcntl (fd2[0], F_SETFL, flags | O_NONBLOCK);
	char *string = "pipes suck\n";
	char buffer[80];
	ssize_t bufsize = 0;
	
	if (pipe(fd1) == -1 || pipe(fd2) == -1) {
		//pipe fails
		perror("Failed to create the pipe.") ;
		return 1 ;
	}
	printf("created pipe, fork next\n");

	pid_t childpid;
	childpid = fork();
	
	if (childpid == -1) {
		//fork fails
		perror("Failed to fork.") ;
		return 1 ;
	}

	if (childpid == 0) {
		printf("child program\n");
		//write to pipe and then immeadiately read from pipe
		write(fd1[1], string, (strlen(string) + 1));//write to pipe
		nbytes = read(fd1[0], buffer, sizeof(buffer));//read pipe
		printf("string from pipe within child process: %s\n", buffer);//print buffer
		//add string to pipe again
		write(fd1[1], string, (strlen(string) + 1));//write to pipe
		//wait for user input string, write to pipe
		printf("user >> ");//print prompt
		getline(&string, &bufsize, stdin);//get user input
		printf("got user input\n");//let em know we got it
		write(fd1[1], string, (strlen(string) + 1));//write to pipe again
		exit(0);//child is done now
	}
	else if (childpid > 0) {
		//exec shell program?
		wait(&status);
		printf("parent program\n");
		//read and print whats in the pipe
		nbytes = read(fd1[0], buffer, sizeof(buffer));//read and put in buffer
		printf("string from pipe: %s\n", buffer);//print buffer
		//this should be the second string in the pipe, user input
		nbytes = read(fd1[0], buffer, sizeof(buffer));//read and put in buffer
		printf("second string from pipe: %s\n", buffer);//print buffer
		printf("done\n");

	}
	
}
