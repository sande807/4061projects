/* CSci4061 S2016 Assignment 2
section: 007 (Eric)
section: 002 (Nick)
date: mm/dd/yy
name: Eric Sande, Nick Orf
id: sande807, orfxx012 */

The purpose of this program is to simulate a chat system with multiple users and a server administrator, which users pipes and IPC. 

Compile this program by typing 'make' in the terminal.

To use this program, type './server' in the terminal. 

The program starts by making a read pipe and write pipe, and then makes them non-blocking. After that, it forks to create a child, and starts the server shell. Once the server shell is running, it reads the given input. It parses the input, and goes through a switch statement for each command. If the command is CHILDPID, it gives the server the correct pid for use in the \list command and in cleaning. If the command is LISTUSERS, the listusers function runs, which checks each slot in the user struct and lists the user when/if it was found. If the command is ADDUSER, it runs the adduser function, which appropriately creates a new user in the user struct and sets everything accordingly. If the command is KICK, it gets the name of the user we want to kick, then calls cleanupuser, which appropriately removes the user struct and everything in it. If the command is EXIT, it cleans up everything, the server, and everything in it. Otherwise, the command is broadcastmsg, which sends the message to each server along with whomever send it. Inside the child process, is follows a similar approach for all of the above commands. Our code is commented as well, which has a deeper explanation of each section of code, and was written at the time of development.

We did not make any explicit assumptions.

Our strategies for error handling were quite simple, if any of the functions returned invalid numbers, such as -1 or 0, and then using a 'perror', we write to the server shell and notify of the encountered error.
