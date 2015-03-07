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
	int i;
	
	msgbuf *message = allocate_message(DEFAULT, TEST_MSG_1);
	msgbuf *message2 = allocate_message(DEFAULT, TEST_MSG_2);
	
		/** TEST1 code */
	a_count ++;
	release_processor();
	
	/*TEST5 code*/		
	//delayed_send(6, message2, 3);
	send_message(6, message);
		
	release_processor();

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
	int *sender = k_request_memory_block();
	msgbuf *message;
	
	
	/* TEST3 code */
	b_count = release_memory_block(test3_mem);
	release_processor();
	
	/* TEST5 stress test code */
	/*
	set_process_priority(7, HIGH);
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
	/*
	//fills up the memory block array. Also requests ALL memory.
	int number_mem_blocks = get_total_num_blocks(); //101
	void * mem_blocks[500];
	void * requested;
	int i;
	
	for (i = 0; i < number_mem_blocks - 3; i ++){
		requested = request_memory_block();
		mem_blocks[i] = requested;
	}
	c_count ++;
	
	//release all memory
	for (i = 0; i < (int)number_mem_blocks - 3; i ++){
		requested = mem_blocks[i];
		release_memory_block(requested);
	}
	
	release_processor();
	*/
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


	set_process_priority(1, LOWEST);
	receive_message(sender);
	
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
	
	set_process_priority(2, MEDIUM);


	//Check that process A is put on the correct ready queue
	initial = (int)get_process_priority(7);
	iterator = getReadyQ(initial)->first;

	while (iterator != NULL && ((PCB*)(iterator->data))->m_pid != 7) {
		iterator = iterator->next;
	}
	if (iterator == NULL) {
		failed = failed + 1;
	}
	
	
	//Check that setting priority to high preempts and updates queue correctly
	set_process_priority(7,HIGH);
	final = get_process_priority(7);
	
	//A popped from low ready queue
	iterator = getReadyQ(initial)->first;
	while (iterator != NULL && ((PCB*)(iterator->data))->m_pid != 7) {
		iterator = iterator->next;
	}
	if (iterator != NULL) {
		failed = failed + 1;
	}
	
	//A premepted
	if(a_count != 1) {
		failed = failed + 1;
	}
	
	//Get and set priorities correctly changed priorities
	if(initial != LOWEST || final != HIGH) {
		failed = failed + 1;
	}
	
	//Set priority to invalid value
	final = set_process_priority(7,10);
	if (final == RTX_OK) {
		failed = failed + 1;
	}
	

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

	set_process_priority(3, MEDIUM);
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

	endTest(failed + test2_count, 2);


	set_process_priority(3, LOW);

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
	requested = request_memory_block();
	test3_mem = requested;
	set_process_priority(4, MEDIUM);

	//Test that memory was allocated
	if (requested == NULL) {
		failed = failed + 1;
	}

	//shove B to high queue, B will attempt to free test3_mem
	set_process_priority(8,HIGH);

	if(b_count != -1) {
		failed ++;
	}
	
	release_memory_block(requested);

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

	/*
	void *test4_mem;
	
	set_process_priority(5,MEDIUM);
	
	//Take one memory block
	test4_mem = request_memory_block();
	
	//Have process C try consume all memory
	set_process_priority(9,HIGH);

	// check that C was blocked
	if(getBlockedResourceQ(HIGH) == NULL &&
		getBlockedResourceQ(HIGH)->first == NULL ){
		failed ++;
	}
	
	//Check that releasing memory unblocks C
	release_memory_block(test4_mem);
	if(c_count == 0) {
			failed ++;
	}
	if(getBlockedResourceQ(HIGH) != NULL && getBlockedResourceQ(HIGH)->first != NULL ){	
		failed ++;
	}
	
	 
	endTest(failed + test4_count, 4);
	set_process_priority(5,LOW);
*/
endTest(failed + test4_count, 4);
set_process_priority(5,LOWEST);
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests message passing
 */
void test5(void){
	PCB* next;
	msgbuf *message = request_memory_block();

	int failed = 0;

	int *sender = k_request_memory_block();
	msgbuf *message2;
	
	set_process_priority(6,MEDIUM);
	
	//Preempt and switch to A
	set_process_priority(7,HIGH);
	
	//Receive message sent from A
	message = receive_message(sender);
	
	//Check that sender was set correctly
	if (*sender != 7) {
		failed = failed + 1;
	}
	
	//Check that mtext was set correctly
	if (checkMessageText(message, TEST_MSG_1) == 0) {
		failed = failed + 1;
	}
	/*
	release_processor();
	set_process_priority(7,LOWEST);

	//check that not delayed message arives first
	message = receive_message(sender);
	if (checkMessageText(message, TEST_MSG_1) == 0) {
		failed = failed + 1;
	}
	
	//Check that delayed message arrives
	message = receive_message(sender);
	if (checkMessageText(message, TEST_MSG_2) == 0) {
		failed = failed + 1;
	}	
	
	release_memory_block(sender);
	
	
	set_process_priority(7, HIGH);
	
	if (messages_sent != messages_received ) {
		failed = failed + 1;
	}
	
	if (messages_received != NUM_TEST_MESSAGES) {
		failed = failed + 1;
	}
	*/
	release_memory_block(sender);


	endTest(failed + test5_count, 6);
	set_process_priority(6,LOWEST);
	
	//Signal to test handler that tests are finished running
	message2->mtype = DEFAULT;
	setMessageText(message2, TEST_MSG_1, sizeof(TEST_MSG_1));	
	send_message(1, message2);
	
	while(1) {
		release_processor();
	}
	
}
