/* CSci4061 S2016 Assignment 4
* Name: Eric Sande, Nick Orf
* X500: sande807, ordxx012 */

To compile our program, simply run 'make' from the command line. To run our program, type './web_server', the port you want the server to run on, the path to the folder containing the files you want the server to get, the number of dispatcher threads, the number of worker threads, and the size of the queue. Ex: ./web_server 9000 <path-to-testing-folder> 5 5 100. After this is running, open a second terminal and type 'wget <file>' or 'wget -i <path-to-testing-folder/file>'. If you want to see the output in an external file, use the "-O" flag along with the name of the file you want to view it in at the end of the file name. You can also open up a web browser window, while the first terminal is still running, and type in the file name and it will download itself.

Our code first creates the desired amount of worker/dispatcher threads and joins them. It runs using two functions, 'dispatch' and 'worker'. In dispatch it first finds an open spot in the queue to get the request, then it establishes a connection and gets the request (the last two are functions given to us in 'util.h'). The worker function detects which filetype the requested file is, reads the file into the buffer, and then tries to return the result; if the result cannot be returned, it returns an error instead (these functions are also given to us in 'util.h'). 

Our code contains many comments which further elaborate on our design process.


