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
}	

/**
 * @brief: a process that tests the allocation and deallocation of a memory block
 */
void testHandler(void){
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

	g_test_procs[6].m_priority=MEDIUM;
	g_test_procs[7].m_priority=MEDIUM;
	g_test_procs[8].m_priority=MEDIUM;


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
