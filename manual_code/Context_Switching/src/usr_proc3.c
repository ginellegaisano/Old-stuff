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

extern int a_count;
extern int b_count;
extern int c_count;

extern int test1_count;
extern int test2_count;
extern int test3_count;
extern int test4_count;
extern int test5_count;

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
	send_message(NUM_PROCS - 4, msg);

	while(1){
		msg = receive_message(sender);
		if(msg->mtext[0] == 'Z'){
			deallocate_message(msg);
			release_memory_block(sender);
			break;
		} else {
			release_memory_block(sender);
		}
	}
	
	while(1) {
		msg = allocate_message(COUNT_REPORT, " ", 1);
		msg->mtext[0] = num;
		send_message(8, msg);
		num = num + 1;
		release_processor();
	}
	/*
	while (1) {
			release_processor();
	}
	*/
}

void B(void) //pid = 8
{
	msgbuf *msg;
	int* sender = request_memory_block();
	
	while(1){
		msg = receive_message(sender);
		release_memory_block(sender);
		send_message(9, msg);
	}
	
	/*
	while(1) {
		release_processor();
	}
	*/
}


void C(void) //pid == 9
{
	Queue *q;
	int *sender = request_memory_block();
	Envelope *env;
	msgbuf *msg;
	msgbuf *delay;
	msgbuf *receive;
	char print_msg[9] = {'P', 'r', 'o', 'c', 'e', 's', 's', ' ', 'C'};
	Element *element = request_element();

	q->first = NULL;
	q->last = NULL;
	
	while(1) {
		if(q->first == NULL){
			msg = receive_message(sender);
		} else {
			env = (Envelope *)(pop(q)->data);
			msg = env->message;
		}
		if(msg->mtype == COUNT_REPORT && msg->mtext[0] % 20 == 0){
			setMessageText(msg, print_msg,9);
			send_message(NUM_PROCS-2, msg);
			
			delay = allocate_message(wakeup10, "", 0);
			delayed_send(9, delay, 10);
			while(1) {
				receive = receive_message(sender);
				if(receive->mtype == wakeup10) {
					deallocate_message(receive);
					break;
				} else {
						element->data = receive;
						push(q, element);
				}
			}
			
			
		} else {
			deallocate_message(msg);
		}
	}
	
	/*
	while(1) {
		release_processor();
	}
	*/
}


/**
 * @brief: 
 */
void test1(void){
	int failed = 0;

	
	
	
	endTest(failed + test1_count, 1);
	set_process_priority(2, LOWEST);
	while(1) {
		release_processor();
	}
	
}

/**
 * @brief: 
 */
void test2(void){
	int failed = 0;

	
	
	endTest(failed + test2_count, 2);
	set_process_priority(3, LOWEST);
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: 
 */
void test3(void){
	int failed = 0;
	
	endTest(failed + test3_count, 3);
	set_process_priority(4, LOWEST);
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief:
 */
void test4(void){
	int failed = 0;
	
	
	endTest(failed + test4_count, 4);
	set_process_priority(5, LOWEST);
	while(1) {
		release_processor();
	}
	
}

/**
 * @brief:
 */
void test5(void){
	int failed = 0;

	
	endTest(failed + test5_count, 5);
	set_process_priority(6, LOWEST);
	
	while(1) {
		release_processor();
	}
	
}
