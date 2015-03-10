/** 
 * @file:   k_rtx.h
 * @brief:  kernel deinitiation and data structure header file
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 */
 
#ifndef K_RTX_H_
#define K_RTX_H_

#define NUM_PRIORITIES 5
#define NUM_TEST_PROCS 6
#define NUM_TESTS 5
#define NUM_PROCS 14
#include "rtx.h"

#ifdef DEBUG_0
#define USR_SZ_STACK 0x180         /* user proc stack size 512B   */
#else
#define USR_SZ_STACK 0x100         /* user proc stack size 218B  */
#endif /* DEBUG_0 */

/*----- Types -----*/
typedef unsigned char U8;
typedef unsigned int U32;
typedef int bool;
#define true 1
#define false 0

/* process states, note we only assume four states in this example */
typedef enum {NEW = 0, RDY, RUN, BLOCKED_ON_RESOURCE, BLOCKED_ON_RECEIVE} PROC_STATE_E;  

typedef struct Element Element;
struct Element {
	Element* next; //therefore we can take out PCB -> next. 
	void* data;
	void* block; //a pointer to the start of the memory block that this element is in.
} ;
typedef struct Queue Queue;

struct Queue {
		Element* first;
		Element* last;
};


/*
  PCB data structure definition.
  You may want to add your own member variables
  in order to finish P1 and the entire project 
*/
typedef struct PCB PCB;
struct PCB 
{ 
	
	//struct pcb *mp_next;  /* next pcb, not used in this example */  
	U32 *mp_sp;		/* stack pointer of the process */
	U32 m_pid;		/* process id */
	PROC_STATE_E m_state;   /* state of the process */
	int m_priority;	
	Queue *mailbox;
	
};

//typedef struct BlockedElement BlockedElement;






#endif // ! K_RTX_H_
