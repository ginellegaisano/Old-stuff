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



void * test3_mem = NULL;
char TEST_MSG_1[7] = {'a','a','a','a','a','a','a'};
char TEST_MSG_2[5] = {'b','a','b','e','l'};
char TEST_MSG_3[5] = {'d','e','l','a','y'};

int messages_sent = 0;
int messages_received = 0;
int NUM_TEST_MESSAGES = 500;
int NUM_TEST_MESSAGES_MAILBOX = 15; //Note this number should not be larger than 40

/**
 * @brief: Empty Procedure
 */
void A(void) //pid = 7
{
	int *sender = request_memory_block();
	msgbuf *message;
	msgbuf *message_delay;

	/* Test4 code */
	receive_message(sender); //should block and fall back to test4
	a_count ++;
	release_processor();

	
	/*Test 5 code */	
	message = allocate_message(DEFAULT, TEST_MSG_2, 5);
	message_delay = allocate_message(DEFAULT, TEST_MSG_3, 5);
	
	delayed_send(6, message_delay, 3);
	send_message(6, message);
		
	release_processor();
	
	while (1) {
			release_processor();
	}
}

/**
 * @brief: Empty Procedure
 */
void B(void) //pid = 8
{
	int *sender = request_memory_block();
	int i;
	msgbuf *message;
	
	/* Test1 code */
	for (i = 0; i < NUM_TEST_MESSAGES_MAILBOX; i++) {
		message = receive_message(sender);
		deallocate_message(message);
	}
	release_memory_block(sender);
	release_processor();
	
	/** TEST2 code */
	for (i = 0; i < NUM_TEST_MESSAGES; i++) {
		message = allocate_message(DEFAULT, TEST_MSG_1, 7);
		send_message(3, message);
		messages_sent = messages_sent + 1;
		release_processor();
	}
	
	
	while(1) {
		release_processor();
	}
}

/**
 * @brief: Empty Procedure
 */
void C(void) //pid == 9
{
	int *sender = request_memory_block();
	receive_message(sender);//should get blocked and switch back to test3
	c_count ++;
	release_processor();
	
	
	//release process
	while(1) {
		release_processor();
	}
}


/**
 * @brief: a process that tests mailboxes, uses B
 */
void test1(void){
	int failed = 0;
	int i;
	int message_count = 0;
	msgbuf *message;
	Queue *mailbox = gp_pcbs[8]->mailbox;
	Element *iterator;

	set_process_priority(2, MEDIUM);
	set_process_priority(8, MEDIUM);

	//Send multiple messages to B
	for (i = 0; i < NUM_TEST_MESSAGES_MAILBOX; i++) {
		message = allocate_message(DEFAULT, TEST_MSG_1, 7);
		send_message(8, message);
	}
	
	iterator = mailbox->first;
	while(iterator != NULL){
		iterator = iterator->next;
		message_count++;
	}
	
	if(message_count != NUM_TEST_MESSAGES_MAILBOX){
		failed ++;
	}
	
	release_processor();
	
	//Here B will receive multiple messages
	
	iterator = mailbox->first;
	if(iterator != NULL){
		failed ++;
	}
	
	endTest(failed + test1_count, 1);
	set_process_priority(2, LOWEST);
	while(1) {
		release_processor();
	}
	
}

/**
 * @brief: a process that stress tests sending and receiving many messages, uses B
 */
void test2(void){
	int failed = 0;
	int i = 0;
	int initial;
	int final;
	msgbuf *message;
	int *sender = request_memory_block();
	
	set_process_priority(3,MEDIUM);
	set_process_priority(8,MEDIUM);

			//Stress test
	initial = getTotalFreeMemory();
	release_processor();
	
	for (i = 0; i < NUM_TEST_MESSAGES; i++) {
		message = receive_message(sender);
		deallocate_message(message);
		messages_received ++;
		release_processor();
	}
	
	final = getTotalFreeMemory();

	
	if(messages_received != messages_sent){
		printf("messages : %d != %d\n\r", messages_received, messages_sent);
		failed ++;
	}
	
	if(initial > final) {
		printf("memeory : %d != %d\n\r", initial, final);
		failed ++;	
	}
	
	initial = getTotalFreeMemory();
	for (i = 0; i < NUM_TEST_MESSAGES; i++) {
		message = allocate_message(DEFAULT, TEST_MSG_1, 7);
		send_message(3, message);
		message = receive_message(sender);
		deallocate_message(message);
	}
	final = getTotalFreeMemory();

	if(initial != final) {
			failed ++;
	}
	
	endTest(failed + test2_count, 2);
	set_process_priority(3, LOWEST);
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests blocking in message passing, uses C
 */
void test3(void){
	int failed = 0;
	Queue *blocked;
	msgbuf *message = allocate_message(DEFAULT, TEST_MSG_1, 7);

	set_process_priority(4, MEDIUM);
	set_process_priority(9, MEDIUM);
	release_processor();
	
	//Here C attempts to receive a message and is blocked
	
	//Test that C was placed on the blocked on receive queue
	blocked = getBlockedReceiveQ(MEDIUM);
	if(((PCB *)blocked->first->data)->m_pid != 9 ){
		failed ++;
	}
	
	send_message(9, message);
	release_processor();
	
	//Here C should get unblocked and execute
	
	if(c_count == 0){
		failed ++;
	}
	
	//Test that C was removed from the blocked on receive queue
	blocked = getBlockedReceiveQ(MEDIUM);
	if(((PCB *)blocked->first->data)->m_pid == 9 ){
		failed ++;
	}
	
	endTest(failed + test3_count, 3);
	set_process_priority(4, LOWEST);
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests preemption in message passing, uses A
 */
void test4(void){
	int failed = 0;
	msgbuf *message = allocate_message(DEFAULT, TEST_MSG_1, 7);

	set_process_priority(5, MEDIUM);
	set_process_priority(7, HIGH); //should preempt to A
	
	//Here A should attempt to receive a message and be blocked
	
	send_message(7, message); //should preempt to A
	
	//Check that A finished executing properly
	if(a_count == 0) {
		failed ++;
	}	
	
	endTest(failed + test4_count, 4);
	set_process_priority(5, LOWEST);
	while(1) {
		release_processor();
	}
	
}

/**
 * @brief: a process that tests getting messages and delayed messages, uses A
 */
void test5(void){
	int failed = 0;

	msgbuf *message_send = allocate_message(DEFAULT, TEST_MSG_1, 7);
	msgbuf *message_receive;
	msgbuf *message_receive_delay;
	
	int *sender = (int *)request_memory_block();

	set_process_priority(6,MEDIUM);
	set_process_priority(7,MEDIUM);
	release_processor();
	
	//Here A sends two messages with and without delay
	
	//Receive message sent from A
	message_receive = receive_message(sender);
	message_receive_delay = receive_message(sender);

	//Check that sender was set correctly
	if (*sender != 7) {
		failed = failed + 1;
	}
	
	//Check that mtext was set correctly
	if (checkMessageText(message_receive, TEST_MSG_2) == 0) {
		failed = failed + 1;
	}
	
	//Check that delayed message arived last
	if (checkMessageText(message_receive_delay, TEST_MSG_3) == 0) {
		failed = failed + 1;
	}
	
	//Clean up memory used in test
	deallocate_message(message_receive_delay);
	deallocate_message(message_receive);
	release_memory_block(sender);
	

	endTest(failed + test5_count, 5);
	set_process_priority(6, LOWEST);
	
	while(1) {
		release_processor();
	}
	
}
