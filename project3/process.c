#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "process.h"

// messaging config
int WINDOW_SIZE;
int MAX_DELAY;
int TIMEOUT;
int DROP_RATE;

// process information
process_t myinfo;
int mailbox_id;
// a message id is used by the receiver to distinguish a message from other messages
// you can simply increment the message id once the message is completed
int message_id = 0;
// the message status is used by the sender to monitor the status of a message
message_status_t message_stats;
// the message is used by the receiver to store the actual content of a message
message_t message;

int num_available_packets; // number of packets that can be sent (0 <= n <= WINDOW_SIZE)
int is_receiving = 0; // a helper varibale may be used to handle multiple senders

int sig_wait = 1;

/**
 * TODO complete the definition of the function
 * 1. Save the process information to a file and a process structure for future use.
 * 2. Setup a message queue with a given key.
 * 3. Setup the signal handlers (SIGIO for handling packet, SIGALRM for timeout).
 * Return 0 if success, -1 otherwise.
 */
//complete
int init(char *process_name, key_t key, int wsize, int delay, int to, int drop) {
	printf("init\n");
    myinfo.pid = getpid();
    strcpy(myinfo.process_name, process_name);
    myinfo.key = key;

    // open the file
    FILE* fp = fopen(myinfo.process_name, "wb");
    if (fp == NULL) {
        printf("Failed opening file: %s\n", myinfo.process_name);
        return -1;
    }
    // write the process_name and its message keys to the file
    if (fprintf(fp, "pid:%d\nprocess_name:%s\nkey:%d\n", myinfo.pid, myinfo.process_name, myinfo.key) < 0) {
        printf("Failed writing to the file\n");
        return -1;
    }
    fclose(fp);

    WINDOW_SIZE = wsize;
    MAX_DELAY = delay;
    TIMEOUT = to;
    DROP_RATE = drop;

    printf("[%s] pid: %d, key: %d\n", myinfo.process_name, myinfo.pid, myinfo.key);
    printf("window_size: %d, max delay: %d, timeout: %d, drop rate: %d%%\n", WINDOW_SIZE, MAX_DELAY, TIMEOUT, DROP_RATE);

    // TODO setup a message queue and save the id to the mailbox_id
    //setting up message queue    
	if ((mailbox_id = msgget(key, IPC_CREAT)) == -1)
		perror("failed to create message queue") ;    

    // TODO set the signal handler for receiving packets
	if(signal(SIGIO, receive_packet) == SIG_ERR)//signal function sets up signal with handler
		perror("failed to set up SIGIO handler\n");//if error
		
	if(signal(SIGALRM, timeout_handler) == SIG_ERR)//signal function sets up signal with handler
		perror("failed to set up SIGALRM handler\n");//if error
	
    return 0;
}

/**
 * Get a process' information and save it to the process_t struct.
 * Return 0 if success, -1 otherwise.
 */
int get_process_info(char *process_name, process_t *info) {
    char buffer[MAX_SIZE];
    char *token;
    
    // open the file for reading
    FILE* fp = fopen(process_name, "r");
    if (fp == NULL) {
        return -1;
    }
    // parse the information and save it to a process_info struct
    while (fgets(buffer, MAX_SIZE, fp) != NULL) {
        token = strtok(buffer, ":");
        if (strcmp(token, "pid") == 0) {
            token = strtok(NULL, ":");
            info->pid = atoi(token);
        } else if (strcmp(token, "process_name") == 0) {
            token = strtok(NULL, ":");
            strcpy(info->process_name, token);
        } else if (strcmp(token, "key") == 0) {
            token = strtok(NULL, ":");
            info->key = atoi(token);
        }
    }
    fclose(fp);
    return 0;
}

/**
 * TODO Send a packet to a mailbox identified by the mailbox_id, and send a SIGIO to the pid.
 * Return 0 if success, -1 otherwise.
 */
//completed
int send_packet(packet_t *packet, int mailbox_id, int pid) {
	printf("sending packet %d to pid: %d\n",packet->packet_num,pid);//notify shell sending packet
	
	//printf("sending packet type %d\n", (int)packet->mtype);
	//printf("sending packet num %d\n", packet->packet_num);
	//printf("sending packet message id %d\n", packet->message_id);
	//printf("sending packet data %s\n", packet->data);
	//if(msgsnd(mailbox_id,packet,sizeof(packet->data),0)== -1){
	
	if (msgsnd(mailbox_id, (void *) &packet, (size_t) sizeof(packet->data), 0) == -1) {
		return -1;//send the packet to the mailbox id, if -1 return -1
	}
	if(kill(pid,SIGIO)==-1){
		return -1;//send sigio to pid so that the receiver knows there is a packet
	}

	return 0;
}

/**
 * Get the number of packets needed to send a data, given a packet size.
 * Return the number of packets if success, -1 otherwise.
 */
int get_num_packets(char *data, int packet_size) {
    if (data == NULL) {
        return -1;
    }
    if (strlen(data) % packet_size == 0) {
        return strlen(data) / packet_size;
    } else {
        return (strlen(data) / packet_size) + 1;
    }
}

/**
 * Create packets for the corresponding data and save it to the message_stats.
 * Return 0 if success, -1 otherwise.
 */
int create_packets(char *data, message_status_t *message_stats) {
    if (data == NULL || message_stats == NULL) {
        return -1;
    }
    int i, len;
    for (i = 0; i < message_stats->num_packets; i++) {
        if (i == message_stats->num_packets - 1) {
            len = strlen(data)-(i*PACKET_SIZE);
        } else {
            len = PACKET_SIZE;
        }
        message_stats->packet_status[i].is_sent = 0;
        message_stats->packet_status[i].ACK_received = 0;
        message_stats->packet_status[i].packet.message_id = -1;
        message_stats->packet_status[i].packet.mtype = DATA;
        message_stats->packet_status[i].packet.pid = myinfo.pid;
        strcpy(message_stats->packet_status[i].packet.process_name, myinfo.process_name);
        message_stats->packet_status[i].packet.num_packets = message_stats->num_packets;
        message_stats->packet_status[i].packet.packet_num = i;
        message_stats->packet_status[i].packet.total_size = strlen(data);
        memcpy(message_stats->packet_status[i].packet.data, data+(i*PACKET_SIZE), len);
    }
    return 0;
}

/**
 * Get the index of the next packet to be sent.
 * Return the index of the packet if success, -1 otherwise.
 */
int get_next_packet(int num_packets) {
    int packet_idx = rand() % num_packets;
    int i = 0;

    i = 0;
    while (i < num_packets) {
        if (message_stats.packet_status[packet_idx].is_sent == 0) {
            // found a packet that has not been sent
            return packet_idx;
        } else if (packet_idx == num_packets-1) {
            packet_idx = 0;
        } else {
            packet_idx++;
        }
        i++;
    }
    // all packets have been sent
    return -1;
}

/**
 * Use probability to simulate packet loss.
 * Return 1 if the packet should be dropped, 0 otherwise.
 */
int drop_packet() {
    if (rand() % 100 < DROP_RATE) {
        return 0;
    }
    return 1;
}

/**
 * TODO Send a message (broken down into multiple packets) to another process.
 * We first need to get the receiver's information and construct the status of
 * each of the packet.
 * Return 0 if success, -1 otherwise.
 */
int send_message(char *receiver, char* content) {
    if (receiver == NULL || content == NULL) {
        printf("Receiver or content is NULL\n");
        return -1;
    }
    // get the receiver's information
    if (get_process_info(receiver, &message_stats.receiver_info) < 0) {
        printf("Failed getting %s's information.\n", receiver);
        return -1;
    }
    // get the receiver's mailbox id
    message_stats.mailbox_id = msgget(message_stats.receiver_info.key, 0666);
    if (message_stats.mailbox_id == -1) {
        printf("Failed getting the receiver's mailbox. errno:%d\n",errno);
        return -1;
    }
    // get the number of packets
    int num_packets = get_num_packets(content, PACKET_SIZE);
    if (num_packets < 0) {
        printf("Failed getting the number of packets.\n");
        return -1;
    }
    // set the number of available packets
    if (num_packets > WINDOW_SIZE) {
        num_available_packets = WINDOW_SIZE;
    } else {
        num_available_packets = num_packets;
    }
    // setup the information of the message
    message_stats.is_sending = 1;
    message_stats.num_packets_received = 0;
    message_stats.num_packets = num_packets;
    message_stats.packet_status = malloc(num_packets * sizeof(packet_status_t));
    if (message_stats.packet_status == NULL) {
        return -1;
    }
    // parition the message into packets
    if (create_packets(content, &message_stats) < 0) {
        printf("Failed paritioning data into packets.\n");
        message_stats.is_sending = 0;
        free(message_stats.packet_status);
        return -1;
    }

    // TODO send packets to the receiver
    // the number of packets sent at a time depends on the WINDOW_SIZE.
    // you need to change the message_id of each packet (initialized to -1)
    // with the message_id included in the ACK packet sent by the receiver
    //printf("num packets %d\n", message_stats.num_packets);
    //printf("sending packets to process: %d\n", message_stats.receiver_info.pid);
    //printf("sending packets to mailbox: %d\n",message_stats.mailbox_id);
    
    int index,sent_packets,out_packets;
    sent_packets = 0;
    index =	get_next_packet(message_stats.num_packets);
    send_packet(&message_stats.packet_status[index].packet,message_stats.mailbox_id, message_stats.receiver_info.pid);//send the first packet
    
    sig_wait =1;
    while(sig_wait);//this will wait for the first ACK pack from the first message.
    //when the ack is sent back it will contain the message id
    //now we can use the message id
    
    while(sent_packets<message_stats.num_packets){
		
		out_packets = (sent_packets - message_stats.num_packets_received);//determine how many packets are out
		
		if(out_packets< WINDOW_SIZE){//if there are less packets out than the window size, send out a packet
			
			index =	get_next_packet(message_stats.num_packets);//get the index of the next packet
			
			message_stats.packet_status[index].packet.message_id = message_id;
			
			send_packet(&message_stats.packet_status[index].packet,message_stats.mailbox_id, message_stats.receiver_info.pid);//send the packet
			
			sent_packets++;//update the number of sent packets
			
		}
	}
    return 0;
}

/**
 * TODO Handle TIMEOUT. Resend previously sent packets whose ACKs have not been
 * received yet. Reset the TIMEOUT.
 */
//complete
void timeout_handler(int sig) {
	if(sig == SIGALRM){
		//resend previous packets with no ACKS recieved
		printf("TIMEOUT: resend packets\n");
		int pid;
		int mailbox;
		int i;
		packet_t *pack;
		for(i= 0; i < message_stats.num_packets; i++) {
			if (message_stats.packet_status[i].is_sent == 1 && message_stats.packet_status[i].ACK_received == 0) {// found a packet that has been sent but not recieved
					pid = message_stats.packet_status[i].packet.pid;
					mailbox = message_stats.mailbox_id;
					pack = &message_stats.packet_status[i].packet;
					send_packet(pack,mailbox,pid);
			}
		}
		//reset the TIMEOUT
		alarm(TIMEOUT);
	}
}

/**
 * TODO Send an ACK to the sender's mailbox.
 * The message id is determined by the receiver and has to be included in the ACK packet.
 * Return 0 if success, -1 otherwise.
 */
//almost complete
//need to get data and total_size
int send_ACK(int mailbox_id, pid_t pid, int packet_num) {
    // TODO construct an ACK packet\
    //construct packet from stuff
    printf("sending ackpack\n");
    packet_t ackPack ;
    ackPack.mtype = ACK ;
    ackPack.message_id = -1 ;
    ackPack.pid = pid ;
    strcpy(ackPack.process_name, myinfo.process_name) ;
    ackPack.num_packets = packet_num ;
    //ackPack.total_size

    strcpy(ackPack.data, "hello") ;

    int delay = rand() % MAX_DELAY;
    sleep(delay);

    // TODO send an ACK for the packet it received
	//send message to message queue, return -1 if fails
	if (msgsnd(mailbox_id, (void *) &ackPack, sizeof(ackPack.data), 0) == -1){
		return -1 ;
	}	
		
	if(kill(pid,SIGIO)==-1){
		return -1;
	}
	
    return 0;
}

/**
 * TODO Handle DATA packet. Save the packet's data and send an ACK to the sender.
 * You should handle unexpected cases such as duplicate packet, packet for a different message,
 * packet from a different sender, etc.
 */
//complete except for handling unexpected cases
void handle_data(packet_t *packet, process_t *sender, int sender_mailbox_id) {
		//save packets data
		int i = packet->packet_num;
		strcpy(message_stats.packet_status[i].packet.data, packet->data);
		message_stats.num_packets_received++;
		if(message_stats.num_packets_received==message_stats.num_packets){
			message.is_complete = 1;
		}
		
		//send an ACK to sender
		int pid = sender->pid;//use this to derefernce pointers to structures
		int num = packet->packet_num;//same as above
		printf("Send an ACK for packet %d to pid: %d\n", num, pid);//notify shell
		send_ACK(sender_mailbox_id, pid, num);
}

/**
 * TODO Handle ACK packet. Update the status of the packet to indicate that the packet
 * has been successfully received and reset the TIMEOUT.
 * You should handle unexpected cases such as duplicate ACKs, ACK for completed message, etc.
 */
 //mostly complete. special cases to do?
void handle_ACK(packet_t *packet) {
		message_id = packet->message_id;
		printf("Received an ACK for packet %d\n", packet->packet_num);
		//update status of packet to indicate recieved
		int i = packet->packet_num;//get the packet number from the packet information
		//use message status global variable to find the packet status for the specific status and set the ack recieved value to 1
        if(message_stats.packet_status[i].ACK_received == 1){
			//duplicate ack. do something?
		}else{
			message_stats.packet_status[i].ACK_received = 1;
		}
		//reset TIMEOUT
		alarm(TIMEOUT);
}

/**
 * Get the next packet (if any) from a mailbox.
 * Return 0 (false) if there is no packet in the mailbox
 */
int get_packet_from_mailbox(int mailbox_id) {
    struct msqid_ds buf;

    return (msgctl(mailbox_id, IPC_STAT, &buf) == 0) && (buf.msg_qnum > 0);
}

/**
 * TODO Receive a packet.
 * If the packet is DATA, send an ACK packet and SIGIO to the sender.
 * If the packet is ACK, update the status of the packet.
 */
//NOT DONE, STILL NEED TO GET THE PACKET FROM MSGRCV
void receive_packet(int sig) {
	printf("received packet\n");
	sig_wait=1;
	packet_t pack;
	
	if(msgrcv(mailbox_id,(void *)&pack,(size_t)sizeof(pack.data),0,0)==-1){
		printf("msgrcv failed %d\n", errno);
	}
	
	//printf("packet type: %d\n", (int)pack.mtype);
	//printf("packet data: %s\n", pack.data);
	//printf("packet num %d\n", pack.packet_num);
	//printf("packet message id %d\n", pack.message_id);
    // TODO you have to call drop_packet function to drop a packet with some probability
    if (drop_packet()) {//if drop packet returns 1 the packet was not dropped
		if(pack.mtype == DATA){
			//call handle data to handle the data, this also sends the ACK
			handle_data(&pack,&myinfo,mailbox_id);
			
		}else if(pack.mtype == ACK){
			//if it is an ACK then just hand that off to handle ACK
			handle_ACK(&pack);
		}
    }
}

/**
 * TODO Initialize the message structure and wait for a message from another process.
 * Save the message content to the data and return 0 if success, -1 otherwise
 * 
 */
int receive_message(char *data) {
	//save message content to message data structure
	//message_stats is the status structure that contains number of packets and other info
	//message contains the actual message data
	
	//initializing message data
	message.num_packets_received = 0;
	message.is_complete=0;
	message.is_received=0;	
	message.data = data;
	
	while(!message.is_complete){}//wait for the message to be completed
	
	int i;
	for(i=0; i<message_stats.num_packets; i++){
		strcat(message.data, message_stats.packet_status[i].packet.data);//build the message using all the packets data
	}
    return 0;
}
