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
void * testMem;

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=USR_SZ_STACK;
		g_test_procs[i].m_is_i=false;
	}
  
	g_test_procs[0].mpf_start_pc = &null_process;
	g_test_procs[1].mpf_start_pc = &proc1;
	g_test_procs[2].mpf_start_pc = &proc2;
}

void null_process(void) {
	while (1) {
		#	ifdef DEBUG_0
						printf("null!!\n\r");			
			#endif 
		testMem = request_memory_block();
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
	release_memory_block(testMem);
	//void * meow = k_request_memory_block();
	/*void * woof = k_request_memory_block();
	void * chirp = k_request_memory_block();*/

	while (1) {
		if ( i != 0 && i%5 == 0 ) {
			uart0_put_string("\n\r");
		/*k_release_memory_block(meow);
			k_release_memory_block(woof);
			k_release_memory_block(chirp);*/

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
