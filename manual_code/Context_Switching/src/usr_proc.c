/**
 * @file:   usr_proc.c
 * @brief:  Two user processes: proc1 and proc2
 * @author: Yiqing Huang
 * @date:   2014/01/17
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];
int FAILED = 0;

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=USR_SZ_STACK;
		g_test_procs[i].m_is_i=false;
	}
	
	g_test_procs[0].mpf_start_pc = &null_process;
	g_test_procs[0].m_priority = 4;
	g_test_procs[1].mpf_start_pc = &test1;
	g_test_procs[2].mpf_start_pc = &test2;
	g_test_procs[3].mpf_start_pc = &test3;
	g_test_procs[4].mpf_start_pc = &test4;
	g_test_procs[5].mpf_start_pc = &test5;
	g_test_procs[6].mpf_start_pc = &test6;
	g_test_procs[7].mpf_start_pc = &proc1;
	g_test_procs[8].mpf_start_pc = &proc2;
}

void null_process(void) {
	while (1) {
		#	ifdef DEBUG_0
						printf("null!!\n\r");			
			#endif 
		release_processor();
	}
}

/**
 * @brief: a process that prints five uppercase letters
 *         and then yields the cpu.
 */
void proc1(void)
{
	int i = 0;
	int ret_val = 10;
				#ifdef DEBUG_0
						printf("proc1!!\n\r");			
			#endif 

	while (1) {
		if ( i != 0 && i%5 == 0 ) {
			uart0_put_string("\n\r");

			ret_val = release_processor();
			
			#ifdef DEBUG_0
						printf("proc1: ret_val=%d\n\r", ret_val);
			#endif /* DEBUG_0 */
		}
		uart0_put_char('A' + i%26);
		i++;
	}
}

/**
 * @brief: a process that prints five numbers
 *         and then yields the cpu.
 */
void proc2(void)
{
	int i = 0;
	int ret_val = 20;
	#ifdef DEBUG_0
				printf("proc2 1: ret_val=%d\n\r", ret_val);
			#endif /* DEBUG_0 */
	ret_val = release_processor();
	#ifdef DEBUG_0
				printf("proc2 2: ret_val=%d\n\r", ret_val);
			#endif /* DEBUG_0 */
	while (0) {
		if ( i != 0 && i%5 == 0 ) {
			uart0_put_string("!\n\r");
			ret_val = release_processor();
			#ifdef DEBUG_0
				printf("proc2: ret_val=%d\n", ret_val);
			#endif /* DEBUG_0 */
		}
		i++;
	}
}

/**
 * @brief: a process that tests the allocation and deallocation of a memory block
 */
void test1(void){
	
	int failed = 0;
	int initial;
	int final;
	void * requested;
	
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
	
	if(failed == 0){
		printf("G099_test: test 1 OK\n\r");
	} else {
		printf("G099_test: test 1 FAIL\n\r");
		FAILED ++;
	}
	
	while(1) {
		release_processor();
	}
}

void test2(void){
	int failed = 0;
	int initial = 0;
	int final = 0; 
	
	initial = (int)get_process_priority(1);
	set_process_priority(1,2);
	final = get_process_priority (1);
	
	if(initial == final || final != 2) {
		failed = failed + 1;
	}
	
	if(failed == 0){
		printf("G099_test: test 2 OK\n\r");
	} else {
		printf("G099_test: test 2 FAIL\n\r");
		FAILED ++;
	}
	
	while(1) {
		release_processor();
	}
	
}

/**
 * @brief: a process that tests when trying to free a memory block twice, returns an error
 */
void test3(void){
	int failed = 0;
	int ret_code;
	void * requested;

	requested = request_memory_block();
	ret_code = release_memory_block(requested);
	if (ret_code != RTX_OK) {
		failed++;
	}
	ret_code = release_memory_block(requested);

	if (ret_code != RTX_ERR) {
		failed++;
	}

	release_memory_block(requested);
	if(failed == 0){
		printf("G099_test: test 3 OK\n\r");
	} else {
		printf("G099_test: test 3 FAIL\n\r");
		FAILED ++;
	}
		while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests the out of memory exception
 */
void test4(void){
	
	
	while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests 
 */
void test5(void){
		while(1) {
		release_processor();
	}
	
}
/**
 * @brief: a process that tests 
 */
void test6(void){
		while(1) {
		release_processor();
	}
	
}
