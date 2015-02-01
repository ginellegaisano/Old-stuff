/**
 * @file:   usr_proc.c
 * @brief:  6 user tests and 2 random procs
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "k_rtx.h"
#include "rtx.h"
#include "uart_polling.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];

int FAILED = 0;
void * test5_mem = NULL;

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
	
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOW;
		g_test_procs[i].m_stack_size=USR_SZ_STACK;
		g_test_procs[i].m_is_i=false;
	}
	
	g_test_procs[0].mpf_start_pc = &null_process;
	g_test_procs[0].m_priority = 4;
	g_test_procs[1].mpf_start_pc = &testHandler;
	g_test_procs[2].mpf_start_pc = &test1;
	g_test_procs[3].mpf_start_pc = &test2;
	g_test_procs[4].mpf_start_pc = &test3;
	g_test_procs[5].mpf_start_pc = &test4;
	g_test_procs[6].mpf_start_pc = &test5;

}

void null_process(void) {
	while (1) {
		release_processor();
	}
}

/**
 * @brief: Empty Procedure
 */
void proc1(void)
{

	while (1) {
			release_processor();
	}
}

/**
 * @brief: Empty Procedure
 */
void proc2(void)
{
	
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

	
}

/**
 * @brief: a process that tests getting and setting priority as well as setting priority to an illegal value.
 */
void test1(void){
	int failed = 0;
	int initial = 0;
	int final = 0; 
	PCB* iterator;
	
	initial = (int)get_process_priority(1);
	iterator = getReadyQ(initial)->first;
	while (iterator != NULL && iterator->m_pid != 2) {
		iterator = iterator->next;
	}
	if (iterator == NULL) {
		failed = failed + 1;
	}
	
	set_process_priority(1,LOW);
	final = get_process_priority(1);
	
	iterator = getReadyQ(initial)->first;
	while (iterator != NULL && iterator->m_pid != 2) {
		iterator = iterator->next;
	}
	if (iterator != NULL) {
		failed = failed + 1;
	}
	iterator = getReadyQ(final)->first;
	while (iterator != NULL && iterator->m_pid != 2) {
		iterator = iterator->next;
	}
	if (iterator == NULL) {
		failed = failed + 1;
	}
	
	if(initial == final || final != 2) {
		failed = failed + 1;
	}
	initial = set_process_priority(1,10);
	if (initial == RTX_OK) {
		failed = failed + 1;
	}
	
	set_process_priority(1,LOWEST);
	
	iterator = getReadyQ(3)->first;
	while(iterator->next != NULL) {
		iterator = iterator->next;
	}
	
	if(failed == 0){
		printTest();
		printf("test 1 OK\n\r");
	} else {
		printTest();
		printf("test 1 FAIL\n\r");
		FAILED ++;
	}
	set_process_priority(1,LOWEST);
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
	PCB* iterator;
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
	
	iterator = getReadyQ(3)->first;
	while(iterator->next != NULL) {
		iterator = iterator->next;
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
	
	iterator = getReadyQ(3)->first;
	while(iterator->next != NULL) {
		iterator = iterator->next;
	}
	
	if(failed == 0){
		printTest();
		printf("test 2 OK\n\r");
	} else {
		printTest();
		printf("test 2 FAIL\n\r");
		FAILED ++;
	}
	
	release_processor();
	
	release_memory_block(test5_mem);
	set_process_priority(3, LOWEST);
	
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests memory ownership 
 */
void test3(void){
	int failed;
	void * requested;
	requested = request_memory_block();
	test5_mem = requested;

	//shove back on to ready queue
	set_process_priority(3,HIGH);
	set_process_priority(4,HIGH);

	release_processor();
	set_process_priority(4,LOWEST);
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
	release_processor();
	//set priority 0
	set_process_priority(4,MEDIUM);
	//call for memory -> will be blocked
	requested = request_memory_block();
	//release memory
	release_memory_block(requested);
	
		while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests the out of memory exception + tests the blocked queue size
 */
void test4(void){
	
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
	for (i = 0; i < number_mem_blocks - 2; i ++){
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
	for (i = 0; i < number_mem_blocks - 2; i ++){
		requested = mem_blocks[i];
		release_memory_block(requested);
	}
	//jump to proc 2;
	release_processor();
	//back from 2, check blocked q
	//fail if blocked q IS NOT empty and not mot moved to the ready queue
	if(getBlockedResourceQ(MEDIUM) != NULL && getBlockedResourceQ(MEDIUM)->first != NULL ){	
		failed ++;
	}
	
	if(failed == 0){
		printTest();
		printf("test 4 OK\n\r");
	} else {
		printTest();
		printf("test 4 FAIL\n\r");
		FAILED ++;
	}
		set_process_priority(5,LOWEST);
	set_process_priority(4,LOWEST);

	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests preemption
 */
void test5(void){
	PCB* next;
	int failed = 0;
	PCB* top;
	PCB* bottom;
	
	set_process_priority(1,MEDIUM);
	set_process_priority(2,HIGH);
	
	next = scheduler();
	
	if (next->m_pid != 3) {
		failed = failed + 1;
	}
	
	set_process_priority(1,LOWEST);
	pushToReadyQ(LOWEST, next);
	
	top = getReadyQ(LOWEST)->first;
	bottom = getReadyQ(LOWEST)->last;
	getReadyQ(LOWEST)->first = NULL;
	getReadyQ(LOWEST)->last = NULL;
	
	next = scheduler();
	
	if (next->m_pid != 1) {
		failed = failed + 1;
	}
	
	getReadyQ(LOWEST)->first = top;
	getReadyQ(LOWEST)->last = bottom;
	
	if(failed == 0){
		printf("G026_test: test 5 OK\n\r");
	} else {
		printf("G026_test: test 5 FAIL\n\r");
		FAILED ++;
	}
	
	while(1) {
		release_processor();
	}
	
}
