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



void send_wall_clock_message(msgbuf *msg);
void CRT_print(void);

//null process
void null_process(void) {
	while (1) {
		release_processor();
	}
}

//checks if string is in WS hh:mm:ss format. 
bool check_format(char *str) {
	int i;
	for (i = 3; i < 10; i = i + 3) {
		if (str[i] == NULL || (str[i] < '0' || str[i] > '9' )||( str[i+1] < '0' || str[i+1] > '9') || (str[i+2] != ':' && i != 9))
			return false;
	}
	return true;
} 

//sends message to CRT_PID with string in hh:mm:ss format
void print_wall_clock(int hour, int minute, int second){
	char str[9];
	int i;
	msgbuf* msg;


	msg = k_allocate_message(DEFAULT, "", 1);

	
	str[0] = hour /10 + '0';
	str[1] = hour %10 + '0';
	str[2] = ':';
	str[3] = minute /10 + '0';
	str[4] = minute %10 + '0';
	str[5] = ':';
	str[6] = second /10 + '0';
	str[7] = second %10 + '0';
	
	for (i = 0; i < 8; i ++){
		msg->mtext[i] = str[i];
	}
	
	send_message(CRT_PID, msg);


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
		//registering to KCD
		msg = allocate_message(DEFAULT, " ", 1);
		msg->mtext[0] = 'W';
		msg->mtype = KCD_REG;
		send_message(KCD_PID, msg);
	
		msg = allocate_message(DEFAULT, " ", 1);
		msg->mtext[0] = 'W';
		msg->mtext[1] = 'R';
		msg->mtype = KCD_REG;
		send_message(KCD_PID, msg);
		
		msg = allocate_message(DEFAULT, " ", 1);
		msg->mtext[0] = 'W';
		msg->mtext[1] = 'T';
		msg->mtype = KCD_REG;
		send_message(KCD_PID, msg);
		
		msg = allocate_message(DEFAULT, " ", 1);
		msg->mtext[0] = 'W';
		msg->mtext[1] = 'S';
		msg->mtype = KCD_REG;
		send_message(KCD_PID, msg);

	//blocked on send!
	while(1){
		 msg = receive_message(output);
		
		//start the clock
		if (msg->mtext[0] == 'W' && msg->mtext[1] == NULL) {
			msg->mtext[0] == ' ';
			clock_on = true;
		}

		 if (msg != NULL && clock_on) { //checks if msg has text and clock is on
			 if (msg->mtext[0] == ' ' || msg->mtext[1] == NULL ) {
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
					print_wall_clock(hour,minute,second);
				 		__enable_irq();
					k_deallocate_message(msg);
					send_wall_clock_message(msg); //sends delayed message
				} else if (msg->mtext[1] == 'R') { //resets clock
						hour = 0;
						minute = 0;
						second = 0;
							__disable_irq();
						print_wall_clock(hour,minute,second);
							__enable_irq();

						//deallocate then create a new one.
						k_deallocate_message(msg);
				} else if (msg->mtext[1] == 'T') {
						hour = 0;
						minute = 0;
						second = 0;
						clock_on = false;
						k_deallocate_message(msg);
				} else if (msg->mtext[1] == 'S' && check_format(msg->mtext)) {
					for(i = 3; i < 10; i = i + 3) { 
						temp = (msg->mtext[i] - '0') * 10 + msg->mtext[i + 1] - '0';
						switch(i) {
							case 3:
								hour = temp % 24;
								break;
							case 6:
								minute = temp;
								break;
							case 9:
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
					print_wall_clock(hour,minute,second);
							__enable_irq();
					k_deallocate_message(msg);
			}else{ //else prints out the message
				k_send_message(CRT_PID, msg);
			}
		} else { //if message is null or clock is off, deallocates the message
			k_deallocate_message(msg);
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
	
		k_deallocate_message(msg);
		__enable_irq();
	}
}

void send_wall_clock_message(msgbuf *msg){ //sends a delayed message to wall_clock
		msg = allocate_message(DEFAULT, " ", 1);
		msg->mtext[0] = ' ';
		delayed_send(CLOCK_PID, msg,1); 

}

void KCD(void) {
	msgbuf * msg = NULL;
	msgbuf * msg_send = NULL;
	
	//house keeping.
	char g_buffer[128];
	char enter = '\x0D';
	char backspace = '\x08';
	char command = '%';
	
	char g_char_in;
	int * output = (int *)request_memory_block();
	int char_count = 0;
	bool waiting_for_command = false;
	int i;
	bool caught=false;
	
	//command registration 
	bool isCommand = true;
	char commands[NUM_PROCS][25][17]; //process id, number of commands registered to a single id, max length of a command
	int num_commands[NUM_PROCS];
	int count;
	int j;
	int k;

	for (i = 0; i < 128; i++)
		g_buffer[i] = NULL;

	
	while(1){
   	msg = receive_message(output);
		g_char_in = msg->mtext[0];
			//releasing the message here!!
		//k_deallocate_message(msg);
		
		if (msg->mtype == KCD_REG) {
			//need to register commands to particular process id
			//info includes command (only caring about input)
			//assuming that command is entire string
			
			//check how many commands have been registered to the process
			count = num_commands[*output];
			
			for (i = 0; i < 16; i++) {
				if (msg->mtext[i] != NULL) {
					commands[*output][count][i] = msg->mtext[i];
				}
				else {
					commands[*output][count][i] = '\n';
					break;
				}
				
			}
			if (i == 16) {
				commands[*output][count][i] = '\n';
			}
			num_commands[*output] = count + 1;
			deallocate_message(msg);
			
		} else {
			g_char_in = msg->mtext[0];
			deallocate_message(msg);
			//releasing the message here!!
			printf("%c", g_char_in);
			if (!waiting_for_command) {
				if (g_char_in == command) {
					waiting_for_command = true;
					char_count = char_count + 1;
					char_count = char_count + 0;
				}
			} else {
				if (g_char_in == backspace){ //backspace
					if(char_count > 0){
						char_count--;
					}
				} else if (char_count < 16 && g_char_in != enter) { //not enter
					g_buffer[char_count] = g_char_in;
					char_count++;
				}
				else if (waiting_for_command) {
					//iterate through ALL the commands, and check if a command was entered
				for (i = 0; i < NUM_PROCS; i++) { //all process ids
						isCommand = true;
						for (j = 0; j < num_commands[i]; j++) { //all commands registered
							isCommand = true;
							count = 1;
							while (count < char_count &&
										g_buffer[count] != NULL && //not out of bounds of message text
										g_buffer[count] != ' ' &&
										 commands[i][j][count-1] != '\n')  //not out of bounds of the command
							{
												if (g_buffer[count] != commands[i][j][count-1]) {
													isCommand = false;
													break;
												}
												count++;
							}
							if (isCommand && (g_buffer[count] == NULL || g_buffer[count] == ' ') && commands[i][j][count-1] == '\n'){ //reached end of blah
								msg_send = allocate_message(DEFAULT, "", 0);
								for (k = 1; k < char_count; k++) {
									msg_send->mtext[k-1] = g_buffer[k];
								}
								char_count = 0;
								isCommand = true;
								caught=true;
								send_message(i, msg_send);
								break;
							}
						}
						if (isCommand && num_commands[i] > 0) {
							break;
						} else {
							isCommand = true;
						}
					}
					if (!caught) {
						msg_send = allocate_message(DEFAULT, "", 0);
								for (k = 1; k < char_count; k++) {
									msg_send->mtext[k-1] = g_buffer[k];
								}
								send_message(CRT_PID, msg_send);
					}
					char_count = 0;
					caught=false;
					for (i = 0; i < 128; i++)
						g_buffer[i] = NULL;
				}
				else {
					char_count = 0;
					for (i = 0; i < 128; i++)
						g_buffer[i] = NULL;
				}
			}

		}
		
		/*if (!waiting_for_command) {
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
							k_send_message(pid_clock, msg_send);
						}
					}if(!caught){ //all KCD processes.
						msg_send = k_allocate_message(DEFAULT, "", 0);
						for (i = 0; i < char_count; i++) {
							msg_send->mtext[i] = g_buffer[i];
						}
						k_send_message(pid_crt, msg_send);
					}
				}
				waiting_for_command = false;
				char_count = 0;
				caught = false;
			}
		}*/
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
			deallocate_message(_msg);
			_msg_send = allocate_message(DEFAULT, "", 0);
			_msg_send->mtext[0] = _g_char_in;
			send_message(KCD_PID , _msg_send);

	}
	
	
}

void set_priority_process(void) {
	int * output = (int *)request_memory_block(); // the output parameter
	int i=2;
	int process_id;
	int priority;
	int status = RTX_OK;
	bool isError = false;
	msgbuf* msg;
	
	msg = allocate_message(DEFAULT, " ", 1);
	msg->mtext[0] = 'C';
	msg->mtype = KCD_REG;
	send_message(KCD_PID , msg);

	
	while(1){
		msg = receive_message(output);
		printReadyQ("");
		
		if (msg->mtext[i] >= '0' && msg->mtext[i] <= '9') { //look for first param
			if (msg->mtext[i+1] >= '0' && msg->mtext[i+1] <= '9') {
				process_id = (msg->mtext[i] - '0') * 10 + msg->mtext[i+1] - '0'; //2 digit id
				i += 2;
			} else {
				process_id = (msg->mtext[i] - '0'); //1 digit id
				i += 1;
			}
			if(process_id >=10){
				isError = true;
			}
		} else {
			isError = true;
		}
		
		if (!isError && msg->mtext[i] == ' ' && msg->mtext[i+1] >= '0' && msg->mtext[i+1] < (NUM_PRIORITIES + '0')&& msg->mtext[i+2] == NULL) {
			priority = (msg->mtext[i+1] - '0');
		} else {
			isError = true;
		}
		
		if (!isError) {
			status = set_process_priority(process_id, priority);
			if (status == RTX_ERR) {
				isError = true;
			}
		}
		
		if (isError) {
			printf("\n\rInvalid parameters for setting priority.\n\r");
		}
		
		//resetting vars
		isError = false;
		i = 2;
		status = RTX_OK;
		deallocate_message(msg);
		printReadyQ("");
	}

}
	
