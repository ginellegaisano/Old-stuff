/**
 * @file:   msg.h
 * @brief:  Message Send and Receive functions
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/03/10
 */
 

#include "msg.h"
#include "rtx.h"
#include "queue.h"
#include "k_rtx.h"
#include "k_memory.h"

void *build_message(int process_id, void *message_envelope, int delay) {
	Message *message = (Message *)k_request_memory_block();
	msgbuf *msg; 
	Element *envelope;
	
	msg = (msgbuf *)message_envelope;
	
	message->sender_id = gp_current_process->m_pid;
	if(msg->mtype == KCD_REG) {
			message->sender_id = KCD_INTERRUPT_ID;
	}
	message->destination_id = process_id;
	message->message = msg;
	//message->time = current time;
	message->delay = delay;
	envelope = k_request_element();
	envelope->data = (Message*)(message);
	
	return envelope;
}

int push_mailbox(int process_id, void *message) {
	PCB *process;
	Element *pcb;

	Queue *mailbox;
	
	
		//cast void pointer to msg
	process = gp_pcbs[process_id];
	mailbox = process->mailbox;
	
	__disable_irq();
	
	push(mailbox, message);
	
		//check if destination process is blocked on received for message type
	if( process->m_state == BLOCKED_ON_RECEIVE) {
		process->m_state = RDY;
		pcb = k_request_element();
		pcb->data = (PCB*)(process);
		pushToReadyQ(process->m_priority, pcb);
		removeFromQ(getBlockedReceiveQ(process->m_priority), process->m_pid);  
		//removeFromBlockedResource(process); To be implememented
		if(process->m_priority < gp_current_process->m_priority){
			k_release_processor();
		}
	}
	__enable_irq();
	
	return RTX_OK;
}

int k_send_message(int process_id, void *message_envelope) {
	Element *envelope;
	
	if(process_id > NUM_PROCS || process_id < 1) {
		return RTX_ERR;
	}
	
	envelope = build_message(process_id, message_envelope, 0);
		
	if(envelope == NULL) {
		return RTX_ERR;
	}
	
	push_mailbox(process_id, envelope);
	
	return RTX_OK;
}

//returns a pointer to the message, 
void *receive_message(int *sender_id) {
	Message *ret_val;
	Queue *mailbox = gp_current_process->mailbox;
	if(mailbox->first == NULL) {
		gp_current_process->m_state = BLOCKED_ON_RECEIVE;
		k_release_processor();
	}
	__disable_irq();

	ret_val = (Message *)((Element *)pop(mailbox)->data);
	*sender_id =  ret_val->sender_id;
	
	__enable_irq();
		
	return (void *)ret_val->message;
}

int k_delayed_send(int process_id, void *message_envelope, int delay){
	Element *envelope;
	Queue *timed;
	
	if(process_id > NUM_PROCS || process_id < 1) {
		return RTX_ERR;
	}
	
	envelope = build_message(process_id, message_envelope, delay);
		
	if(envelope == NULL) {
		return RTX_ERR;
	}
	
	timed = getTimedQ();
	push(timed, envelope);
	
	return RTX_OK;
}



