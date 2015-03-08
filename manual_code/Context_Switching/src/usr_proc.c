/**
 * @file:   usr_proc.c
 * @brief:  6 user tests and 2 random procs
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "k_rtx.h"
#include "msg.h"
#include "rtx.h"
#include "uart_polling.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* initialization table item */
PROC_INIT g_test_procs[NUM_PROCS];
 
int FAILED = 0;
void * test3_mem = NULL;
char TEST_MSG_1[] = "aaaaaaaaaaaa";
char TEST_MSG_2[] = "babel";
char TEST_MSG_3[] = "delay";

int messages_sent = 0;
int messages_received = 0;
int NUM_TEST_MESSAGES = 600;

int a_count = 0;
int b_count = 0;
int c_count = 0;

int test1_count = 0;
int test2_count = 0;
int test3_count = 0;
int test4_count = 0;
int test5_count = 0;



typedef struct LinkedElement LinkedElement;

struct LinkedElement {
		void* current;
		void* next;
};

void printTest() {
	#ifdef DEBUG_0
						printf("G026_test: ");
	#endif /* DEBUG_0 */
}

void endTest(int failed, int test) {
	//Reset dummy process priorities


	set_process_priority(7, LOWEST);
	set_process_priority(8, LOWEST);
	set_process_priority(9, LOWEST);
	set_process_priority(1, LOWEST);

	
	a_count = 0;
	b_count = 0;
	c_count = 0;

	test1_count = 0;
	test2_count = 0;
	test3_count = 0;
	test4_count = 0;
	test5_count = 0;

	if(failed == 0){
		printTest();
		printf("test %d OK\n\r", test);
	} else {
		printTest();
		printf("test %d FAIL\n\r", test);
		FAILED ++;
	}
}	

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOW;
		g_test_procs[i].m_stack_size=USR_SZ_STACK;
	}
	
	g_test_procs[6].m_priority=LOWEST;
	g_test_procs[7].m_priority=LOWEST;
	g_test_procs[8].m_priority=LOWEST;
	
	
	g_test_procs[0].mpf_start_pc = &testHandler;
	g_test_procs[1].mpf_start_pc = &test1;
	g_test_procs[2].mpf_start_pc = &test2;
	g_test_procs[3].mpf_start_pc = &test3;
	g_test_procs[4].mpf_start_pc = &test4;
	g_test_procs[5].mpf_start_pc = &test5;
	g_test_procs[6].mpf_start_pc = &A;
	g_test_procs[7].mpf_start_pc = &B;
	g_test_procs[8].mpf_start_pc = &C;
}

/**
 * @brief: Empty Procedure
 */
void A(void) //pid = 7
{
	int *sender;
	msgbuf *message;
	msgbuf *message2 = allocate_message(DEFAULT, TEST_MSG_2);
	msgbuf *message_delay = allocate_message(DEFAULT, TEST_MSG_3);

		/** TEST1 code */
	/*a_count ++;
	set_process_priority(7,MEDIUM);
	*/
	/*TEST5 code*/		
	//test blocking receive(receive before send in test
	/*message = receive_message(sender);
	deallocate_message(message);
	
	//test that delay send arrives last
	delayed_send(6, message_delay, 3);

	send_message(6, message2);
		
	release_processor();*/

	/*TEST5 stress test code	*/
		/*

	for (i = 0; i < NUM_TEST_MESSAGES; i++) {
		message = allocate_message(DEFAULT, TEST_MSG_1);
	
		send_message(8, message);
		messages_sent = messages_sent + 1;
		release_processor();
	}
	*/
	while (1) {
			release_processor();
	}
}

/**
 * @brief: Empty Procedure
 */
void B(void) //pid = 8
{
	int i;
	//int *sender = k_request_memory_block();
	msgbuf *message;
	
	
	/* TEST3 code */
	/*b_count = release_memory_block(test3_mem);
	release_processor();*/
	
	/* TEST5 stress test code */
	/*
	set_process_priority(7, MEDIUM);
	for (i = 0; i < NUM_TEST_MESSAGES; i++) {
		message = receive_message(sender);
		
		messages_received = messages_received + 1;
		release_processor();
	}
	*/
	while(1) {
		release_processor();
	}
}

/**
 * @brief: Empty Procedure
 */
void C(void) //pid == 9
{
	
	/*TEST4 code */
	
	//fills up the memory block array. Also requests ALL memory.
	int number_mem_blocks = getTotalFreeMemory(); //101
	void * mem_blocks[500];
	int i = 0;
	
	for (i = 0; i < number_mem_blocks; i ++){
		mem_blocks[i] = request_memory_block();
	}
	i = 0;
	//release all memory
	while(mem_blocks[i] != NULL) {
		release_memory_block(mem_blocks[i]);
		i++;
	}
		
	release_processor();
	
	//release process
	while(1) {
		release_processor();
	}
}
/**
 * @brief: a process that tests the allocation and deallocation of a memory block
 */
void testHandler(void){
	int *sender;
	printTest();
	printf("START\n\r");
	printTest();
	printf("total %d tests\n\r", NUM_TESTS );

	release_processor();
	
	printTest();
	printf("%d/%d tests OK\n\r", NUM_TESTS - FAILED, NUM_TESTS);
	printTest();
	printf("%d/%d tests FAIL\n\r", FAILED, NUM_TESTS);
	printTest();
	printf("END\n\r");
		
	while(1) {
		release_processor();
	}
}

/**
 * @brief: a process that tests getting and setting priority as well as setting priority to an illegal value.
 */
void test1(void){
	
	int failed = 0;
	int initial = 0;
	int final = 0; 
	Element* iterator;
	
	/*set_process_priority(2, MEDIUM);


	//Check that process A is put on the correct ready queue
	initial = (int)get_process_priority(7);
	iterator = getReadyQ(initial)->first;

	while (iterator != NULL && ((PCB*)(iterator->data))->m_pid != 7) {
		iterator = iterator->next;
	}
	if (iterator == NULL) {
		failed = failed + 1;
	}
	
	
	//Check that setting priority to medium preempts and updates queue correctly
	set_process_priority(7,HIGH);
	final = get_process_priority(7);
	
	//check that A popped from lowest ready queue
	iterator = getReadyQ(initial)->first;
	while (iterator != NULL && ((PCB*)(iterator->data))->m_pid != 7) {
		iterator = iterator->next;
	}
	if (iterator != NULL) {
		failed = failed + 1;
	}
	
	//check that A premepted
	if(a_count != 1) {
		failed = failed + 1;
	}
	
	//Get and set priorities correctly changed priorities
	if(initial != LOWEST || final != MEDIUM) {
		failed = failed + 1;
	}
	
	//Set priority to invalid value
	final = set_process_priority(7,10);
	if (final == RTX_OK) {
		failed = failed + 1;
	}
	
*/
	endTest(failed + test1_count, 1);
	set_process_priority(2, LOWEST);
	
	while(1) {
		release_processor();
	}
	
}

/**
 * @brief: a process that tests the allocation and deallocation of a memory block
	*  			 and when trying to free a memory block twice, returns an error
 */
void test2(void){
	int failed = 0;
	int ret_code;
	void * requested;
	int initial;
	int final;

	/*set_process_priority(3, MEDIUM);
	initial = getMSP();

	requested = request_memory_block();

	final = getMSP();
	
	if(initial - final != 128) {
		failed = failed + 1;
	}
	release_memory_block(requested);
	if(initial != getMSP()) {
		failed = failed + 1;
	}

	requested = request_memory_block();
	ret_code = release_memory_block(requested);
	if (ret_code != RTX_OK) {
		failed++;
	}

	ret_code = release_memory_block(requested);

	if (ret_code != RTX_ERR) {
		failed++;
	}
*/
	endTest(failed + test2_count, 2);
	set_process_priority(3, LOWEST);
	
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests memory ownership 
 */
void test3(void){
	int failed = 0; 
	void * requested;
	/*requested = request_memory_block();
	test3_mem = requested;
	
	set_process_priority(4, MEDIUM);

	//Test that memory was allocated
	if (requested == NULL) {
		failed = failed + 1;
	}

	//shove B to medium queue, B will attempt to free test3_mem
	set_process_priority(8,MEDIUM);
	release_processor();

	if(b_count != -1) {
		failed ++;
	}
	
	release_memory_block(requested);*/

	endTest(failed + test3_count, 3);
	set_process_priority(4, LOWEST);
	
		while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests the out of memory exception + tests the blocked queue size
 */
void test4(void){
	int failed = 0;
	int initialFree = getTotalFreeMemory();
	int finalFree = getTotalFreeMemory();

	void *test4_mem;
	
	set_process_priority(5,MEDIUM);
	
	//Take one memory block
	test4_mem = request_memory_block();
	finalFree = getTotalFreeMemory();

	//Have process C try consume all memory
	set_process_priority(9,MEDIUM);
	release_processor();

	// check that C was blocked
	if(getBlockedResourceQ(MEDIUM) == NULL &&
		getBlockedResourceQ(MEDIUM)->first == NULL ){
		failed ++;
	}
	
	//Check that releasing memory unblocks C
	release_memory_block(test4_mem);
	release_processor();
	finalFree = getTotalFreeMemory();

	if(getBlockedResourceQ(MEDIUM) != NULL && getBlockedResourceQ(MEDIUM)->first != NULL ){	
		failed ++;
	}

	finalFree = getTotalFreeMemory();
	if(finalFree != initialFree ){
		failed ++;
	}
	
	 
	endTest(failed + test4_count, 4);
	set_process_priority(5,LOW);

	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests message passing
 */
void test5(void){
	int failed = 0;

	/*msgbuf *message_send = allocate_message(DEFAULT, TEST_MSG_1);
	
	msgbuf *message_receive;
	msgbuf *message_receive_delay;
	int *sender = (int *)request_memory_block();
	int *sender2 = (int *)request_memory_block();

	
	set_process_priority(6,MEDIUM);
	
	//Preempt and switch to A
	set_process_priority(7,MEDIUM);
	release_processor();
	send_message(7, message_send);

	
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
	
	if (checkMessageText(message_receive_delay, TEST_MSG_3) == 0) {
		failed = failed + 1;
	}
	deallocate_message(message_receive_delay);
	deallocate_message(message_receive);

	release_memory_block(sender);
	release_memory_block(sender2);

	if(failed != 0) {
		printf("a");
	}*/
	endTest(failed + test5_count, 5);
	set_process_priority(1, HIGH);	
	
	while(1) {
		release_processor();
	}
	
}
