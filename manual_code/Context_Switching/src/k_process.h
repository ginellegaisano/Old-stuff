/**
 * @file:   k_process.h
 * @brief:  process management header file
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 */

#ifndef K_PROCESS_H_
#define K_PROCESS_H_

#include "k_rtx.h"
#include "k_memory.h"

/* ----- Definitions ----- */

#define INITIAL_xPSR 0x01000000        	/* user process initial xPSR value */

/* ----- Functions ----- */

void process_init(void);               	/* initialize all procs in the system */
PCB *scheduler(void);                  	/* pick the pid of the next to run process */
int k_release_process(void);           	/* kernel release_process function */
extern Queue **ready_qs;								/* ready queue*/
extern Queue **blocked_resource_qs;			/* blocked resources queue*/

extern U32 *alloc_stack(U32 size_b);  	/* allocate stack for a process */
extern void __rte(void);             	  /* pop exception stack frame */
extern void set_test_procs(void);     	/* test process initial set up */
extern Element* pop(Queue* self);						/* pop from anu queue structure*/
extern void pushToReadyQ (int priority, Element* p_pcb_old);
extern void printReadyQ (char* tag);
extern void printBlockedQ (char* tag);
extern void push (Queue* queue, Element* p_pcb_old);
extern Element* popFromReadyQ (int priority);
extern Queue* getReadyQ(int priority);
extern Queue* getBlockedResourceQ(int priority);

int k_get_process_priority(int process_id);
int k_set_process_priority(int process_id, int priority); 

#endif /* ! K_PROCESS_H_ */
