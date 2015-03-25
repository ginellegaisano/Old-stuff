/**
 * @file:   usr_proc.c
 * @brief:  6 user tests and 2 random procs
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */
#include "k_rtx.h"
#include "rtx.h"
#include "k_memory.h"
#include "usr_proc.h"
#include "msg.h"
#include  "uart_polling.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */
extern int FAILED;

int COUNT_REPORT  = 2;
int wakeup10 = 3;


void A(void) //pid = 7
{
	msgbuf* msg;
	int num = 0;
	int* sender = request_memory_block();

	msg = allocate_message(DEFAULT, " ", 1);
	msg->mtext[0] = 'Z';
	msg->mtype = KCD_REG;
	send_message(KCD_PID, msg);

	while(1){
		msg = receive_message(sender);
		if(msg->mtext[0] == 'Z'){
			break;
		}
		release_memory_block(msg);
	}
	
	release_memory_block(msg);
	release_memory_block(sender);
	
	while(1) {
		msg = request_memory_block();
		msg->mtype = COUNT_REPORT;
		msg->mtext[0] = (char)num;
		send_message(8, msg);
		num = num + 1;
		release_processor();
	}
	
	while (1) {
			release_processor();
	}
	
}

void B(void) //pid = 8
{
	msgbuf *msg;
	int* sender;

	while(1){
		sender = request_memory_block();
		msg = receive_message(sender);
		release_memory_block(sender);
		send_message(9, msg);
	}
	
	
	while(1) {
		release_processor();
	}
}


void C(void) //pid == 9
{
	Queue q;
	int *sender = request_memory_block();
	msgbuf *msg;
	msgbuf *delay;
	msgbuf *receive;
	char print_msg[10] = {'P', 'r', 'o', 'c', 'e', 's', 's', ' ', 'C'};
	Element *element;
	int i = 0;

	q.first = NULL;
	q.last = NULL;
	
	while(1) {
		if(q.first == NULL){
			msg = receive_message(sender);
		} else {
			element = pop(&q);
			msg = (msgbuf *)(element->data);
			element->data = NULL;
			release_element_block(element);
		}
		if(msg->mtype == COUNT_REPORT && (int)(msg->mtext[0]) % 20 == 0){
			print_msg[9] = i++ + '0';
			setMessageText(msg, print_msg,10);
			msg->mtype = DEFAULT;
			send_message(CRT_PID, msg);
			
			delay = request_memory_block();
			delay->mtype = wakeup10;
			delay->mtext[0] = NULL;
			delayed_send(9, delay, 2);
			while(1) {
				//sender = request_memory_block();
				receive = receive_message(sender);
				//release_memory_block(sender);
				if(receive->mtype == wakeup10) {
					release_memory_block(receive);
					break;
				} else {
						element = request_element();
						element->data = receive;
						push(&q, element);
				}
			}
			
			
		} else {
			release_memory_block(msg);
		}
	}
	
	while(1) {
		release_processor();
	}
}


/**
 * @brief: 
 */
void test1(void){
	while(1) {
		release_processor();
	}
	
}

/**
 * @brief: 
 */
void test2(void){
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: 
 */
void test3(void){
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief:
 */
void test4(void){
	while(1) {
		release_processor();
	}
	
}

/**
 * @brief:
 */
void test5(void){
	while(1) {
		release_processor();
	}
	
}
