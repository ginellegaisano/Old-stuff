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
void * test5_mem = NULL;
char TEST_MSG_1[] = "aaaaaaaaaaaa";
char TEST_MSG_2[] = "babel";

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

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOW;
		g_test_procs[i].m_stack_size=USR_SZ_STACK;
	}
	
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
void A(void)
{
	msgbuf *message = request_memory_block();
	
	message->mtype = DEFAULT;
	setMessageText(message, TEST_MSG_1, sizeof(TEST_MSG_1));
	
	send_message(6, message);
	set_process_priority(7, LOWEST);
	
	
	setMessageText(message, TEST_MSG_2, sizeof(TEST_MSG_2));
	k_delayed_send(6, message, 3);
	
	setMessageText(message, TEST_MSG_1, sizeof(TEST_MSG_2));
	send_message(6, message);
	
	set_process_priority(7, LOWEST);
	
	while (1) {
			release_processor();
	}
}

/**
 * @brief: Empty Procedure
 */
void B(void)
{
	set_process_priority(8,LOWEST);
	//release process
	while(1) {
		release_processor();
	}
}

/**
 * @brief: Empty Procedure
 */
void C(void)
{
	set_process_priority(9,LOWEST);
	//release process
	while(1) {
		release_processor();
	}
}
/**
 * @brief: a process that tests the allocation and deallocation of a memory block
 */
void testHandler(void){
	printTest();
	printf("START\n\r");
	printTest();
	printf("total %d tests\n\r", NUM_TESTS );
	set_process_priority(1, LOWEST);
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
	/*
	int failed = 0;
	int initial = 0;
	int final = 0; 
	Element* iterator;

	initial = (int)get_process_priority(6);
	iterator = getReadyQ(initial)->first;

	while (iterator != NULL && ((PCB*)(iterator->data))->m_pid != 6) {
		iterator = iterator->next;
	}
	if (iterator == NULL) {
		failed = failed + 1;
	}
	
	set_process_priority(2,HIGH);
	set_process_priority(6,HIGH);
	final = get_process_priority(6);
	
	iterator = getReadyQ(initial)->first;
	while (iterator != NULL && ((PCB*)(iterator->data))->m_pid != 6) {
		iterator = iterator->next;
	}
	if (iterator != NULL) {
		failed = failed + 1;
	}
	iterator = getReadyQ(final)->first;
	while (iterator != NULL && ((PCB*)(iterator->data))->m_pid != 6) {
		iterator = iterator->next;
	}
	if (iterator == NULL) {
		failed = failed + 1;
	}
	
	if(initial == final || final != HIGH) {
		failed = failed + 1;
	}
	final = set_process_priority(6,10);
	if (final == RTX_OK) {
		failed = failed + 1;
	}
	
	set_process_priority(6,LOW);

	if(failed == 0){
		printTest();
		printf("test 1 OK\n\r");
	} else {
		printTest();
		printf("test 1 FAIL\n\r");
		FAILED ++;
	}
	set_process_priority(2,LOWEST);
	*/
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
/*
	int failed = 0;
	int ret_code;
	void * requested;
	int initial;
	int final;
	
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
	
	if(failed == 0){
		printTest();
		printf("test 2 OK\n\r");
	} else {
		printTest();
		printf("test 2 FAIL\n\r");
		FAILED ++;
	}
	set_process_priority(3,LOWEST);


	release_memory_block(test5_mem);
	set_process_priority(4,HIGH);
	set_process_priority(3, LOWEST);
*/

	set_process_priority(3, LOWEST);
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests memory ownership 
 */
void test3(void){
	/*
	int failed = 0; 
	void * requested;
	requested = request_memory_block();
	test5_mem = requested;

	//shove back on to ready queue
	set_process_priority(3,HIGH);

	//release_processor();
	if (test5_mem == NULL) {
		failed = failed + 1;
	}
	release_memory_block(requested);

	if(failed == 0){
		printTest();
		printf("test 3 OK\n\r");
	} else {
		printTest();
		printf("test 3 FAIL\n\r");
		FAILED ++;
	}
	set_process_priority(4,LOWEST);
	
	//call for memory -> will be blocked
	requested = request_memory_block();
	//release memory
	release_memory_block(requested);
	*/
	set_process_priority(4,LOWEST);
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests the out of memory exception + tests the blocked queue size
 */
void test4(void){
	/*
	int number_mem_blocks = get_total_num_blocks(); //101
	void * mem_blocks[500];
	void * requested;
	int i;
	int failed = 0;
	//set priority 0
	set_process_priority(5,MEDIUM);
	set_process_priority(4,MEDIUM);
	//release_processor();
	//fills up the memory block array. Also requests ALL memory.
	for (i = 0; i < number_mem_blocks - 3; i ++){
		requested = request_memory_block();
		mem_blocks[i] = requested;
	}
	//jump to proc2
	release_processor();
	//back from proc2, test blocked_resource_qs size (for priority 0)
	//fail if blocked q is empty
	if(getBlockedResourceQ(MEDIUM) == NULL &&
		getBlockedResourceQ(MEDIUM)->first == NULL ){
		failed ++;
	}
	
	//release all memory
	for (i = 0; i < (int)number_mem_blocks - 3; i ++){
		requested = mem_blocks[i];
		release_memory_block(requested);
	}
	//jump to proc 2;
	//back from 2, check blocked q
	//fail if blocked q IS NOT empty and not mot moved to the ready  
	if(getBlockedResourceQ(MEDIUM) != NULL && getBlockedResourceQ(MEDIUM)->first != NULL ){	
		failed ++;
	}
	set_process_priority(4,LOWEST);

	if(failed == 0){
		printTest();
		printf("test 4 OK\n\r");
	} else {
		printTest();
		printf("test 4 FAIL\n\r");
		FAILED ++;
	}
	*/
	set_process_priority(5,LOWEST);
	
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests message passing
 */
void test5(void){
	int failed = 0;

	int *sender = k_request_memory_block();
	msgbuf *message;
	
	set_process_priority(6,MEDIUM);
	set_process_priority(7,HIGH);
	
	message = receive_message(sender);
	
	if (*sender != 7) {
		failed = failed + 1;
	}
	
	if (checkMessageText(message, TEST_MSG_1) == 0) {
		failed = failed + 1;
	}
	
	set_process_priority(7,HIGH);
	
	message = receive_message(sender);
	if (checkMessageText(message, TEST_MSG_1) == 0) {
		failed = failed + 1;
	}
	
	/*message = receive_message(sender);
	if (checkMessageText(message, TEST_MSG_2) == 0) {
		failed = failed + 1;
	}*/
	
	
	if(failed == 0){
		printTest();
		printf("test 5 OK\n\r");
	} else {
		printTest();
		printf("test 5 FAIL\n\r");
		FAILED ++;
	}
	release_memory_block(sender);
	set_process_priority(6,LOWEST);
	
	while(1) {
		release_processor();
	}
	
}
