#Project 2
##Eric Sande, sande807 - Nick Orf, orfxx012

- The program starts by making a read pipe and write pipe, and then makes them non-blocking. After that, it forks to create a child, and starts the server shell.

- Once the server shell is running, it reads the given input. It parses the input, and goes through a switch statement for each command.

- If the command is CHILD_PID, it gives the server the correct pid for use in the \list command and in cleaning.

- If the command is LIST_USERS, the listusers function runs, which checks each slot in the user struct and lists the user when/if it was found.

- If the command is ADD_USER, it runs the adduser function, which appropriately creates a new user in the user struct and sets everything accordingly.

- If the command is KICK, it gets the name of the user we want to kick, then calls cleanup_user, which appropriately removes the user struct and everything in it.

- If the command is EXIT, it cleans up everything, the server, and everything in it.

- Otherwise, the command is broadcast_msg, which sends the message to each server along with whomever send it.

- Inside the child process, is follows a similar approach for all of the above commands.

- Our code is commented as well, which has a deeper explanation of each section of code, and was written at the time of development.
