/**
 * @file:   k_process.c  
 * @brief:  process management C file
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
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
#include "sys_proc.h"
#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* ----- Global Variables ----- */
PCB **gp_pcbs;                  /* array of pcbs */
PCB *gp_current_process = NULL; /* always point to the current RUN process */

/* process initialization table */
PROC_INIT g_proc_table[NUM_PROCS];
extern PROC_INIT g_test_procs[NUM_PROCS];

/**
 * @brief: initialize all processes in the system
 */
void process_init() 
{
	int i;
	U32 *sp;
	Element* element;
  
  /* fill out the initialization table */
	set_test_procs();

	for ( i = 1; i < NUM_PROCS - 2; i++ ) {
		g_proc_table[i].m_pid = g_test_procs[i-1].m_pid;
		g_proc_table[i].m_priority = g_test_procs[i-1].m_priority;
		g_proc_table[i].m_stack_size = g_test_procs[i-1].m_stack_size;
		g_proc_table[i].mpf_start_pc = g_test_procs[i-1].mpf_start_pc;
	}
	
		g_proc_table[0].m_pid = 0;
		g_proc_table[0].m_priority = 4;
		g_proc_table[0].m_stack_size = USR_SZ_STACK;
		g_proc_table[0].mpf_start_pc = &null_process;
	

		g_proc_table[NUM_PROCS - 2].m_pid = NUM_PROCS - 2;
		g_proc_table[NUM_PROCS - 2].m_priority = 4;
		g_proc_table[NUM_PROCS - 2].m_stack_size = USR_SZ_STACK;
		g_proc_table[NUM_PROCS - 2].mpf_start_pc = &wall_clock;
	
		g_proc_table[NUM_PROCS - 1].m_pid = NUM_PROCS - 1;
		g_proc_table[NUM_PROCS - 1].m_priority = 4;
		g_proc_table[NUM_PROCS - 1].m_stack_size = USR_SZ_STACK;
		g_proc_table[NUM_PROCS - 1].mpf_start_pc = &CRT_print;
	
	/* initilize exception stack frame (i.e. initial context) for each process */
	for ( i = 0; i < NUM_PROCS; i++ ) {
		int j;
		(gp_pcbs[i])->m_pid = (int)(g_proc_table[i]).m_pid;
		(gp_pcbs[i])->m_state = NEW;
		(gp_pcbs[i])->m_priority = (g_proc_table[i]).m_priority;
		
		sp = alloc_stack((g_proc_table[i]).m_stack_size);
		*(--sp)  = INITIAL_xPSR;      // user process initial xPSR  
		*(--sp)  = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
		for ( j = 0; j < 6; j++ ) { // R0-R3, R12 are cleared with 0
			*(--sp) = 0x0;
		}
		(gp_pcbs[i])->mp_sp = sp;
		element = k_request_element();
		element->data = (PCB*)(gp_pcbs[i]);
		pushToReadyQ((g_proc_table[i]).m_priority, element);
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
	Element* element;
	PCB* pcb;
	int i;
	for (i = 0; i < NUM_PRIORITIES; i++) {
		element = popFromReadyQ(i);
		if (element != NULL) {
			pcb = (PCB*) element -> data;
			element->data = NULL;
			if (gp_current_process == NULL) {
				gp_current_process = pcb;
			}
			return pcb;
		}
	}
	return NULL;
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
	Element* element;
	state = gp_current_process->m_state;

	if (state == NEW) {
		if (gp_current_process != p_pcb_old && p_pcb_old->m_state != NEW) {

			p_pcb_old->mp_sp = (U32 *) __get_MSP();
			if (p_pcb_old->m_state != BLOCKED_ON_RESOURCE && p_pcb_old->m_state != BLOCKED_ON_RECEIVE) {
				p_pcb_old->m_state = RDY;
				element = k_request_element();
				element->data = p_pcb_old;
				pushToReadyQ(p_pcb_old->m_priority, element);
			}
			//need to push to respective priority ready queue
			//need to go to the gp_proc_table to get priority
			
		}
			gp_current_process->m_state = RUN;
		__set_MSP((U32) gp_current_process->mp_sp);
		__rte();  // pop exception stack frame from the stack for a new processes
	}

	/* The following will only execute if the if block above is FALSE */
	if (state == RDY){
		p_pcb_old->mp_sp = (U32 *) __get_MSP(); // save the old process's sp
		if (p_pcb_old->m_state != BLOCKED_ON_RESOURCE && p_pcb_old->m_state != BLOCKED_ON_RECEIVE) {
			p_pcb_old->m_state = RDY;
			element = k_request_element();
			element->data = p_pcb_old;
			pushToReadyQ(p_pcb_old->m_priority, element);
		}
		gp_current_process->m_state = RUN;
		__set_MSP((U32) gp_current_process->mp_sp); //switch to the new proc's stack    
	} else {
		gp_current_process = p_pcb_old; // revert back to the old proc on error
		return RTX_ERR;
	}
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
	p_pcb_old = gp_current_process;
	gp_current_process = scheduler();
	if ( gp_current_process == NULL  ) {
		gp_current_process = p_pcb_old; // revert back to the old process
		return RTX_ERR;
	}
  if ( p_pcb_old == NULL ) {
		p_pcb_old = gp_current_process;
		gp_current_process->m_state = RUN;
		__set_MSP((U32) gp_current_process->mp_sp);
		__rte();
	}
	
	process_switch(p_pcb_old);
	return RTX_OK;
}

/**
 * @brief get_process_priority()
 * @param process_id the pid of the requested process
 * @return priority value of process
 */
int k_get_process_priority(int process_id) {
	if (process_id < 0 || process_id > NUM_PROCS){
		return -1;
	}
	return (int)g_proc_table[process_id].m_priority;
}
/**
 * @brief set_process_priority(), sets new priority for the requested process
 * @param process_id the pid of the requested process, NEW priority of the requested process
 * @return: RTX_OK upon success
 *         RTX_ERR upon failure
 */
int k_set_process_priority(int process_id, int priority){
	Element* element;
	bool flag = false;
	if (process_id < 0 || process_id > NUM_PROCS || priority < 0 || priority > NUM_PRIORITIES)
		return RTX_ERR;
	if (gp_pcbs[process_id]->m_state == RDY || gp_pcbs[process_id]->m_state == NEW) {		
		element = removeFromQ(getReadyQ(gp_pcbs[process_id]->m_priority), process_id);
		pushToReadyQ(priority, element);

	} else if (gp_pcbs[process_id]->m_state == BLOCKED_ON_RESOURCE) {
		element = removeFromQ(getBlockedResourceQ(gp_pcbs[process_id]->m_priority), process_id);
		push(getBlockedResourceQ(priority), element);
	}

	if ((gp_current_process->m_pid == process_id && priority > gp_current_process->m_priority) 
		|| (gp_current_process->m_pid != process_id && priority < gp_current_process->m_priority)){
		flag = true;	
	}
	if (gp_current_process->m_pid == process_id) {
		gp_current_process->m_priority = priority;
	}
	g_proc_table[process_id].m_priority = priority;
	gp_pcbs[process_id]->m_priority = priority;
	

	if (flag)
		k_release_processor();
	
	return RTX_OK;
}

