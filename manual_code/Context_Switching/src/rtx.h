/* @brief: rtx.h User API
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/02/01
 */
#ifndef RTX_H_
#define RTX_H_

/* ----- Definitations ----- */
#define RTX_ERR -1
#define RTX_OK 0
#define NULL 0


/* Process Priority. The bigger the number is, the lower the priority is*/
#define HIGH    0
#define MEDIUM  1
#define LOW     2
#define LOWEST  3

/* -------- Consts -------- */
#ifdef DEBUG_0
#define USR_SZ_STACK 0x180         /* user proc stack size 512B   */
#else
#define USR_SZ_STACK 0x100         /* user proc stack size 218B  */
#endif /* DEBUG_0 */

/* ----- Types ----- */
typedef unsigned int U32;
typedef int bool;
#define true 1
#define false 0

/* initialization table item */
typedef struct proc_init
{	
	int m_pid;	        /* process id */ 
	int m_priority;         /* initial priority, not used in this example. */ 
	int m_stack_size;       /* size of stack in words */
	void (*mpf_start_pc) ();/* entry point of the process */    
} PROC_INIT;

/* ----- RTX User API ----- */
#define __SVC_0  __svc_indirect(0)

extern void k_rtx_init(void);
#define rtx_init() _rtx_init((U32)k_rtx_init)
extern void __SVC_0 _rtx_init(U32 p_func);

extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

extern void *k_request_element(void);
#define request_element() _request_element((U32)k_request_element)
extern void *_request_element(U32 p_func) __SVC_0;

extern void *k_request_memory_block(void);
#define request_memory_block() _request_memory_block((U32)k_request_memory_block)
extern void *_request_memory_block(U32 p_func) __SVC_0;

extern int k_get_total_num_blocks(void);
#define get_total_num_blocks(void) _get_total_num_blocks((U32)k_get_total_num_blocks)
extern int _get_total_num_blocks(U32 p_func) __SVC_0;
/* __SVC_0 can also be put at the end of the function declaration */

extern int k_release_memory_block(void *);
#define release_memory_block(p_mem_blk) _release_memory_block((U32)k_release_memory_block, p_mem_blk)
extern int _release_memory_block(U32 p_func, void *p_mem_blk) __SVC_0;

extern int k_get_process_priority(int);
#define get_process_priority(process_id) _get_process_priority((U32)k_get_process_priority, process_id)
extern int _get_process_priority(U32 p_func, int process_id) __SVC_0;

extern int k_set_process_priority(int,int);
#define set_process_priority(process_id, priority) _set_process_priority((U32)k_set_process_priority, process_id, priority)
extern int _set_process_priority(U32 p_func, int process_id, int priority) __SVC_0;

extern int k_delayed_send(int, void *, int);
#define delayed_send(process_id, message_envelope, delay) _delayed_send((U32)k_delayed_send, process_id, message_envelope, delay)
extern int _delayed_send(U32 p_func, int process_id, void *message_envelope, int delay) __SVC_0;

extern int k_send_message(int, void *);
#define send_message(process_id, message_envelope) _send_message((U32)k_send_message, process_id, message_envelope)
extern int _send_message(U32 p_func, int process_id, void *message_envelope) __SVC_0;

extern void *k_receive_message(int *sender_id);
#define receive_message(sender_id) _receive_message((U32)k_receive_message, sender_id)
extern void * _receive_message(U32 p_func, int *sender_id) __SVC_0;

#endif /* !RTX_H_ */
