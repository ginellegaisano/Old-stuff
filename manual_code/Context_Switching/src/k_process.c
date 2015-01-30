/**
 * @file:   k_process.c  
 * @brief:  process management C file
 * @author: Yiqing Huang
 * @author: Thomas Reidemeister
 * @date:   2014/01/17
 * NOTE: The example code shows one way of implementing context switching.
 *       The code only has minimal sanity check. There is no stack overflow check.
 *       The implementation assumes only two simple user processes and NO HARDWARE INTERRUPTS. 
 *       The purpose is to show how context switch could be done under stated assumptions. 
 *       These assumptions are not true in the required RTX Project!!!
 *       If you decide to use this piece of code, you need to understand the assumptions and
 *       the limitations. 
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "uart_polling.h"
#include "k_process.h"
//REESY SO POPs OH MAYGAWD
#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

//RTX_ERR and RTX_OK are both defined in rtx.h (remember that, okay?) u fukkers

/* ----- Global Variables ----- */
PCB **gp_pcbs;                  /* array of pcbs */
PCB *gp_current_process = NULL; /* always point to the current RUN process */

/* process initialization table */
PROC_INIT g_proc_table[NUM_TEST_PROCS];
extern PROC_INIT g_test_procs[NUM_TEST_PROCS];

/**
 * @biref: initialize all processes in the system
 * NOTE: We assume there are only two user processes in the system in this example.
 */
void blah () {
	#ifdef DEBUG_0
						printf("Blah: \n gp_current_process state= %d!\n", gp_current_process->m_state);
	
	#endif /* DEBUG_0 */
}

void blah1 (char i) {
	#ifdef DEBUG_0
							printf("%c\n", i);			
						printf("Blah123: \n gp_current_process state= %d!\n", gp_current_process->m_state);
	
	#endif /* DEBUG_0 */
}

void process_init() 
{
	int i;
	U32 *sp;
  
        /* fill out the initialization table */
	set_test_procs();
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_proc_table[i].m_pid = g_test_procs[i].m_pid;
		g_proc_table[i].m_priority = g_test_procs[i].m_priority;
		g_proc_table[i].m_stack_size = g_test_procs[i].m_stack_size;
		g_proc_table[i].mpf_start_pc = g_test_procs[i].mpf_start_pc;
		g_proc_table[i].m_is_i = g_test_procs[i].m_is_i;
	}
  
	/* initilize exception stack frame (i.e. initial context) for each process */
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		int j;
		(gp_pcbs[i])->m_pid = (g_proc_table[i]).m_pid;
		(gp_pcbs[i])->m_state = NEW;
		
		sp = alloc_stack((g_proc_table[i]).m_stack_size);
		*(--sp)  = INITIAL_xPSR;      // user process initial xPSR  
		*(--sp)  = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
		for ( j = 0; j < 6; j++ ) { // R0-R3, R12 are cleared with 0
			*(--sp) = 0x0;
		}
		(gp_pcbs[i])->mp_sp = sp;
	}
}

/*@brief: scheduler, pick the pid of the next to run process
 *@return: PCB pointer of the next to run process
 *         NULL if error happens
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */

PCB *scheduler(void)
{
	if (gp_current_process == NULL) {
				blah1('x');
		gp_current_process = gp_pcbs[0]; 
		return gp_pcbs[0];
	}

	if ( gp_current_process == gp_pcbs[0] ) {
		blah1('y');
		return gp_pcbs[1];
	} else if ( gp_current_process == gp_pcbs[1] ) {
		blah1('z');
		return gp_pcbs[0];
	} else {
		return NULL;
	}
}

/*@brief: switch out old pcb (p_pcb_old), run the new pcb (gp_current_process)
 *@param: p_pcb_old, the old pcb that was in RUN
 *@return: RTX_OK upon success
 *         RTX_ERR upon failure
 *PRE:  p_pcb_old and gp_current_process are pointing to valid PCBs.
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */
int process_switch(PCB *p_pcb_old) 
{
	PROC_STATE_E state;
	blah1('0');

	state = gp_current_process->m_state;

	if (state == NEW) {
			blah1('1');
		if (gp_current_process != p_pcb_old && p_pcb_old->m_state != NEW) {
			p_pcb_old->m_state = RDY;
			p_pcb_old->mp_sp = (U32 *) __get_MSP();
		}
		gp_current_process->m_state = RUN;
		blah1('2');
		__set_MSP((U32) gp_current_process->mp_sp);
		blah1('7');
		__rte();  // pop exception stack frame from the stack for a new processes
		blah1('8');

	} 

	/* The following will only execute if the if block above is FALSE */
	if (gp_current_process != p_pcb_old) {
		blah1('3');
		if (state == RDY){
			blah1('4');			
			p_pcb_old->m_state = RDY; 
			p_pcb_old->mp_sp = (U32 *) __get_MSP(); // save the old process's sp
			gp_current_process->m_state = RUN;
			__set_MSP((U32) gp_current_process->mp_sp); //switch to the new proc's stack    
		} else {
			blah1('5');
			gp_current_process = p_pcb_old; // revert back to the old proc on error
			return RTX_ERR;
		} 
	}
	blah1('9');
	return RTX_OK;
}
/**
 * @brief release_processor(). 
 * @return RTX_ERR on error and zero on success
 * POST: gp_current_process gets updated to next to run process
 */
int k_release_processor(void)
{
	PCB *p_pcb_old = NULL;
	int re;
	blah1('v');
	p_pcb_old = gp_current_process;
	gp_current_process = scheduler();
	blah1('a');
	
	if ( gp_current_process == NULL  ) {
		blah1('b');
		gp_current_process = p_pcb_old; // revert back to the old process
		return RTX_ERR;
	}
  if ( p_pcb_old == NULL ) {
		blah1('c');
		p_pcb_old = gp_current_process;
	}
	re = process_switch(p_pcb_old);
			blah1('d');

			blah1((char)re);

	return RTX_OK;
}

/**
 * @brief get_priority()
 * @param process_id the pid of the requested process
 * @return priority value of process
 */
int get_process_priority(int process_id) {
	if (process_id < 0 || process_id > NUM_TEST_PROCS)
		return -1;
	return g_proc_table[process_id].m_priority;
}
int set_process_priority(int process_id, int priority){
	if (process_id < 0 || process_id > NUM_TEST_PROCS)
		return RTX_ERR;
	g_proc_table[process_id].m_priority = priority;
	return RTX_OK;
}

