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
#include "timer.h"
#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */


bool check_format(char *str) {
	int i;
	for (i = 3; i < 10; i = i + 3) {
		if (str[i] < '0' && str[i] > '9' && str[i+1] < '0' && str[i+1] > '9')
			return false;
	}
	return true;
} 

void send_wall_clock_message(msgbuf *msg);
void CRT_print(void);

void null_process(void) {
	while (1) {
		release_processor();
	}
}


void wall_clock(void){
	int * output = (int *)request_memory_block(); // the output parameter
	int hour=0;
	int minute=0;
	int second=0;

	int temp = 0;
	int i=0;
	bool clock_on = false;
	msgbuf* msg;
	
	//blocked on send!
	while(1){
		 msg = receive_message(output);
		//printf("%d", sizeof(char));
		if (msg->mtext[0] == 'W') {
			clock_on = true;
			msg->mtext[0] = ' ';
		}

		 if (msg != NULL && clock_on) { //checks if msg got deallocated?
			 if (msg->mtext[0] == ' ') {
				 second++;
				 if (second >= 60){
					 minute ++;
					 second = second % 60;
				 }
				 if (minute >= 60){
					 hour = (hour + 1) % 24;
					 minute = minute % 60;
				 }
				 		__disable_irq();

					printf("\n\r%02d:%02d:%02d\n\r", hour, minute, second);
				 		__enable_irq();

					send_wall_clock_message(msg);
				} else if (msg->mtext[0] == 'R') {
						hour = 0;
						minute = 0;
						second = 0;
							__disable_irq();

						printf("\n\r%02d:%02d:%02d\n\r", hour, minute, second);
							__enable_irq();

						//deallocate then create a new one.
						send_wall_clock_message(msg);
				} else if (msg->mtext[0] == 'T') {
						hour = 0;
						minute = 0;
						second = 0;
						clock_on = false;
				} else if (msg->mtext[0] == 'S') {
					for(i = 2; i < 10; i = i + 3) {
						temp = (msg->mtext[i] - '0') * 10 + msg->mtext[i + 1] - '0';
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
					if (second >= 60) {
						 minute ++;
						 second = second % 60;
					}
					if (minute >= 60){
								hour = (hour +1 ) % 24;
								minute = minute % 60;
					}
							__disable_irq();

					printf("\n\r%02d:%02d:%02d\n\r", hour, minute, second);
							__enable_irq();

					send_wall_clock_message(msg);
			}
		}
	}
}
void CRT_print(void){
	char * str;
	int * output = (int *)request_memory_block(); // the output parameter
	while(1){
		msgbuf *msg = receive_message(output);
		__disable_irq();
		str = msg->mtext;
		printf("\n\r");
		printf("%s\n\r",str);
		str = NULL;
		//release the memory block . to be implemented. 
		k_deallocate_message(msg);
		__enable_irq();
	}
}

void send_wall_clock_message(msgbuf *msg){
		k_deallocate_message(msg);
		msg = k_allocate_message(DEFAULT, " ", 1);
		msg->mtext[0] = ' ';
		k_delayed_send(NUM_PROCS - 3, msg,1); 
}

void KCD(void) {
	msgbuf * msg = NULL;
	msgbuf * msg_send = NULL;
	//house keeping.
	char g_buffer[128];
	int PID_Clock = NUM_PROCS - 3;
	int PID_CRT = NUM_PROCS - 2;
	char enter = '\x0D';
	char backspace = '\x08';
	char command = '%';
	
	char g_char_in;
	int * output = (int *)request_memory_block();
	int char_count =0;
	bool clock_on = false;
	bool waiting_for_command = false;
	int i;
	bool caught=false;

	while(1){
   	msg = receive_message(output);
		
		g_char_in = msg->mtext[0];
		//releasing the message here!!
		printf("%c", g_char_in);
		k_deallocate_message(msg);
		if (!waiting_for_command) {
				if(g_char_in == 'r') {
					__disable_irq();
					printReadyQ(" ");
					__enable_irq();
			}
			else if (g_char_in == 'b') {
				__disable_irq();
					printBlockedQ(" ");
				__enable_irq();
			}
			else if(g_char_in == 'm'){
				__disable_irq(); 
				printBlockedReceiveQ(" ");
				__enable_irq();
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
				if (char_count > 0){ //check if they pressed anything
					if (g_buffer[0] == 'W'){ //all clock functions start
						if(char_count ==1){
							if (!clock_on) clock_on = true;
							msg_send = k_allocate_message(DEFAULT, "", 0);
							msg_send->mtext[0] = 'W';
							caught = true;
						}
						if(clock_on){ //starting clock_on 
							if(char_count==11 && g_buffer[1] == 'S' && check_format((char *)g_buffer)) { //setting clock
								msg_send = k_allocate_message(DEFAULT, "", 0);
								for (i = 1; i < char_count; i++) {
									msg_send->mtext[i-1] = g_buffer[i];
								}
								caught = true;
							}else if(char_count == 2 && g_buffer[1] == 'R'){ //reset clock
								msg_send = k_allocate_message(DEFAULT, "", 0);
								msg_send->mtext[0] = g_buffer[1];
								caught = true;
							}else if(char_count ==2 && g_buffer[1] == 'T'){ //terminate clock
								msg_send = k_allocate_message(DEFAULT, "", 0);
								msg_send->mtext[0] = g_buffer[1];
								clock_on = false;
								caught = true;
							}
						}
						if(caught){ //if clock function was called.
							k_send_message(PID_Clock, msg_send);
						}
					}if(!caught){ //all KCD processes.
						msg_send = k_allocate_message(DEFAULT, "", 0);
						for (i = 0; i < char_count; i++) {
							msg_send->mtext[i] = g_buffer[i];
						}
						k_send_message(PID_CRT, msg_send);
					}
				}
				waiting_for_command = false;
				char_count = 0;
				caught = false;
			}
		}
	}
}

void UART_iprocess(void){
	//this is actually KCD 
	msgbuf * _msg = NULL;
	msgbuf * _msg_send = NULL;
	char _g_char_in;

	int * output = (int *)request_memory_block();
	while(1){
   		_msg = receive_message(output);

			_g_char_in = _msg->mtext[0];
			k_deallocate_message(_msg);
			_msg_send = k_allocate_message(DEFAULT, "", 0);
			_msg_send->mtext[0] = _g_char_in;
			k_send_message(NUM_PROCS - 4, _msg_send);
	}
	
	
}
