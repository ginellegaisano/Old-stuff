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
	char msg[1] = {' '};
	msgbuf *message = allocate_message(DEFAULT, msg, 1);


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
	send_message(1, message);
}	

/**
 * @brief: a process that tests the allocation and deallocation of a memory block
 */
void testHandler(void){
	
	int *sender = (int *)request_memory_block();
	msgbuf* msg;
	printTest();
	printf("START\n\r");
	printTest();
	printf("total %d tests\n\r", NUM_TESTS );

	release_processor();
	msg = receive_message(sender);
	deallocate_message(msg);
	msg = receive_message(sender);
	deallocate_message(msg);
	msg = receive_message(sender);
	deallocate_message(msg);
	msg = receive_message(sender);
	deallocate_message(msg);
	msg = receive_message(sender);
	deallocate_message(msg);
	
	printTest();
	printf("%d/%d tests OK\n\r", NUM_TESTS - FAILED, NUM_TESTS);
	printTest();
	printf("%d/%d tests FAIL\n\r", FAILED, NUM_TESTS);
	printTest();
	printf("END\n\r");

	release_memory_block(sender);
	
	while(1) {
		release_processor();
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
