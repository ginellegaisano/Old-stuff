/**
 * @file:   sys_proc.c
 * @brief:  6 user tests and 2 random procs
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "k_rtx.h"
#include "rtx.h"
#include "sys_proc.h"
#include "msg.h"
#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

void send_wall_clock_message(Message *msg, msgbuf *envelope);
void CRT_print(void);

void null_process(void) {
	while (1) {
		printf("NULL\n\r");
		release_processor();
	}
}


void wall_clock(void){
	int * output; // the output parameter
	int hour=0;
	int minute=0;
	int second=0;
	int temp = 0;
	int i=0;
	msgbuf* envelope;
	Message *msg;
	
	//blocked on send!
	while(1){
		 msg = (Message *)receive_message(output);
		printf("%d", sizeof(char));
		 if (msg != NULL) { //checks if msg got deallocated?
			 if (msg->message->mtext[0] == ' ') {
				 if (second > 60){
					 minute ++;
					 second = second % 60;
				 }
				 if (minute > 60){
					 hour = (hour + 1) % 24;
					 minute = minute % 60;
				 }
					printf("\n\r%02d:%02d:%02d\n\r", hour, minute, second);
					send_wall_clock_message(msg, envelope);
				} else if (msg->message->mtext[0] == 'R') {
						hour = 0;
						minute = 0;
						second = 0;
						printf("\n\r%02d:%02d:%02d\n\r", hour, minute, second);
						//deallocate then create a new one.
						send_wall_clock_message(msg, envelope);
				} else if (msg->message->mtext[0] == 'T') {
						hour = 0;
						minute = 0;
						second = 0;
						empty_mailbox();
				} else if (msg->message->mtext[0] == 'S') {
					for(i = 2; i < 10; i = i + 3) {
						temp = (msg->message->mtext[i] - '0') * 10 + msg->message->mtext[i + 1] - '0';
						switch(i) {
							case 2:
								hour = temp % 24;
								break;
							case 5:
								minute = temp;
								break;
							case 8:
								second = temp;
								break;
						}
					}
					if (second > 60) {
						 minute ++;
						 second = second % 60;
					}
					if (minute > 60){
								hour = (hour +1 ) % 24;
								minute = minute % 60;
					}
					printf("\n\r%02d:%02d:%02d\n\r", hour, minute, second);
					send_wall_clock_message(msg, envelope);
				
			}
		}
	}
}
void CRT_print(void){
	char * str;
	int * output; // the output parameter
	while(1){
		Message *msg = (Message *)receive_message(output);

		str = msg->message->mtext;
		printf("\n\r");
		printf("%s\n\r",str);
		
		//release the memory block . to be implemented. 
		k_release_memory_block(msg->message);
		k_release_memory_block(msg);
	}
}

void send_wall_clock_message(Message *msg, msgbuf *envelope){
		k_release_memory_block(msg->message);
		k_release_memory_block(msg);
		msg = (Message *) k_request_memory_block();
		envelope = (msgbuf *) k_request_memory_block();
		envelope->mtype = 0; 
		envelope->mtext[0] = ' ';
		msg->message = envelope;
		k_delayed_send(NUM_PROCS - 2, msg, 1); 
}

void UART_iprocess(void){
	//this is actually KCD 
	Message * msg = NULL;
	
	//house keeping.
	char g_buffer[128];
	int PID_Clock = NUM_PROCS - 3;
	int PID_CRT = NUM_PROCS -2;
	char enter = '\x0D';
	char backspace = '\x08';
	char command = '%';
	
	char g_char_in;
	int * output;
	int char_count =0;
	bool clock_on = false;
	bool waiting_for_command = false;
	msgbuf * envelope;
	int i;
	bool caught=false;
	
	while(1){
		msg = (Message *)receive_message(output);
		
		g_char_in = msg->message->mtext[0];
		//releasing the message here!!
		printf("%c", g_char_in);
		if (!waiting_for_command) {
				if(g_char_in == 'r') {
					printReadyQ(" ");
			}
			else if (g_char_in == 'b') {
					printBlockedQ(" ");
			}
			else if(g_char_in == 'm'){
				printBlockedReceiveQ(" ");
			}
			else if (g_char_in == command) {
				waiting_for_command = true;
			}
		} else {
			if (g_char_in == backspace){ //backspace
				if(char_count > 0){
					char_count--;
				}
			}else if (char_count < 16 && g_char_in != enter) { //not enter
				g_buffer[char_count] = g_char_in;
				char_count++;
			}
			else { //enter pressed or char_count exceeded.
				msg = (Message *) k_request_memory_block();
				envelope = (msgbuf *) k_request_memory_block();
				envelope->mtype = 0;
				if (char_count > 1){ //check if they pressed anything??
					if (g_buffer[0] == 'W'){ //all clock functions.
						if(char_count ==1){
							if (!clock_on) clock_on = true;
							envelope->mtext[0] = ' ';
							caught = true;
						}
						if(clock_on){ //starting clock_on 
							if(char_count==11 && g_buffer[1] == 'S' && check_format((char *)g_buffer)) {
								for (i = 1; i < char_count; i++) {
									envelope->mtext[i-1] = g_buffer[i];
								}
								caught = true;
							}else if(char_count == 2 && g_buffer[1] == 'R'){
								envelope->mtext[0] = g_buffer[1];
								caught = true;
							}else if(char_count ==2 && g_buffer[1] == 'T'){
								envelope->mtext[0] = g_buffer[1];
								clock_on = false;
								caught = true;
							}
						}
						if(caught){
							msg->message = envelope;
							k_send_message(PID_Clock, msg);
						}
					}if(!caught){ //all KCD processes.
						for (i = 0; i < char_count; i++) {
							envelope->mtext[i] = g_buffer[i];
						}
						msg->message = envelope;
						k_send_message(PID_CRT, msg);
					}
				}
				waiting_for_command = false;
				char_count = 0;
			}
		}
	}
}
