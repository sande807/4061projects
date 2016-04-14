/* CSci4061 S2016 Assignment 3
* Name: Eric Sande, Nick Orf
* X500: sande807, orfxx012 */

- To compile our program, type make in the command line. To run the program, you need two windows. In the first window, type ./application, along with 6 parameters; the first being the name, i.e A, the second is the process key, the third is the window size, the fourth is the max delay, the fifth is the timeout, and the final is the drop rate. Once that has been entered, type 'receiver' or 'r' when prompted and switch to the second window. In the second window, type ./application, along with 6 different values for the parameters used above. When prompted, type 'sender' or 's'. Type the name you wrote in the command line in the first window as the 'Receiver name', and then type any sort of message in the 'Data' line. Wait for the window to stop printing information, then switch to the second window, and your message will be printed at the bottom.

- 2 processes are created; one for sending a message and one for receiving a message. The send process breaks the message into several packets and, using a sliding window, sends each packet to the receiver. Along with that goes a SIGIO signal. It also sets an alarm which will resend all packets who have not received an ACK if the alarm times-out. The receive process gets the packets in it's mailbox and sends back an ACK for each packet. It then constructs the message from the packets, which might have come in unordered, and prints the message to the terminal.

- A short TIMEOUT period could result in ACKs not returning, because the program did not wait long enough. If the period was very long, it could result in the program waiting for an ACK that never comes.

- A small window size isn't effective. Less packets can be sent/handled with a small window size and therefore the message takes longer to be created. A large window size seems arbitrary, because you could just send the entire message at once and avoid packets altogether. Additionally, if there is a TIMEOUT in the large window size, a large amount of packets would have to be resent.
