/**
 * @file:   msg.h
 * @brief:  Message Send and Receive functions
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/03/10
 */
 

#include "msg.h"
#include "rtx.h"
#include "queue.h"
#include "timer.h"
#include "k_rtx.h"
#include "k_memory.h"
#include "printf.h"


void setMessageText(msgbuf* message, char *text, int length) {
	int i = 0;

	
	while (i < 120) {
		message->mtext[i] = NULL;
		i++;
	}
	i = 0;
	while(i < length) {
		message->mtext[i] = text[i];
		i++;
	} 
	text = NULL;
}

int checkMessageText(msgbuf* message, char *text) {
	int i = 0;
	int j = 0;
	
	while (i < sizeof(message->mtext)/sizeof(char) && j < sizeof(text)/sizeof(char)) {
		if (message->mtext[i] != text[j]) {
			return 0;
		}
		i++;
		j++;
	}
	return 1;
}

//Packages message into an envelope  to be put in a mailbox
Envelope *build_envelope(int process_id, msgbuf *message_envelope, int delay) {
	Envelope *envelope = (Envelope *)k_request_memory_block();
		
	//building envelope data
	envelope->sender_id = gp_current_process->m_pid;
	/*if(message_envelope->mtype == KCD_REG) {
			envelope->sender_id = KCD_INTERRUPT_ID;
	}*/
	envelope->destination_id = process_id;
	envelope->message = message_envelope;
	envelope->time = get_time();
	envelope->delay = delay;
	return envelope;
}

//Releases memory associated with evelope
int destroy_envelope(Envelope *envelope){

		if(gp_current_process->m_pid != envelope->destination_id){
			return RTX_ERR;
		}

		
		return k_release_memory_block((void *)envelope);
};

//Creates an message to be passed into send message
msgbuf *k_allocate_message(int type, char *text, int length){
		msgbuf *message = k_request_memory_block();
		message->mtype = type;

		setMessageText(message, text, length);	
		return message;
}

//Frees the memory associated with a message
int k_deallocate_message(msgbuf *message){
		int i = sizeof(int);
		Block *block = (Block *)message;
		block->pid = gp_current_process->m_pid;
		while (i < sizeof(message->mtext)/sizeof(char)) {
			message->mtext[i] = NULL;
			i++;
		}
		return k_release_memory_block(message);
};


//Creates an message to be passed into send message
msgbuf *allocate_message(int type, char *text, int length){
		msgbuf *message = request_memory_block();
		message->mtype = type;

		setMessageText(message, text, length);	
		return message;
}

//Frees the memory associated with a message
int deallocate_message(msgbuf *message){
		Block *block = (Block *)message;
		block->pid = gp_current_process->m_pid;
		
		return release_memory_block(message);
};



//pushes an evelope onto a mailbox
int push_mailbox(Envelope *envelope) {
	Element *element;
	Element *popped;
	PCB *process;
	Queue *mailbox;

	__disable_irq();
 	element = k_request_element();
	process = gp_pcbs[envelope->destination_id];
	mailbox = process->mailbox;
	element->data = envelope;

	push(mailbox, element);


		//check if destination process is blocked on received for message type
	if( process->m_state == BLOCKED_ON_RECEIVE) {
		process->m_state = RDY;
		popped = removeFromQ(getBlockedReceiveQ(process->m_priority), process->m_pid);  		
		pushToReadyQ(process->m_priority, popped);
		if(process->m_priority < gp_current_process->m_priority){
			__enable_irq();
			k_release_processor();
		}
	}
	__enable_irq();

	return RTX_OK;
}

//Pops the first envelope from the mailbox and frees memory for associated element
Envelope *pop_mailbox(int process_id){
	Envelope *envelope;
	Element *element;
	PCB *process = gp_pcbs[process_id];
	Queue *mailbox = process->mailbox;

	__disable_irq();
	element = pop(mailbox);
	envelope = (Envelope *)element->data;
	element->data = NULL;
	k_release_element_block(element);
	__enable_irq();
	return envelope;
}


int k_send_message(int process_id, void *message_envelope) {
	Envelope *envelope;
	
	if(process_id > NUM_PROCS || process_id < 1) {
		return RTX_ERR;
	}
	
	envelope = build_envelope(process_id, message_envelope, 0);
		
	if(envelope == NULL) {
		return RTX_ERR;
	}
	
	push_mailbox(envelope);
	
	return RTX_OK;
}

//returns a pointer to the message, 

void *k_receive_message(int *sender_id) {	
	PCB *process = gp_current_process;
	Queue *mailbox = gp_current_process->mailbox;
	Envelope *received;
	msgbuf *message;

	
	int priority;
	Element *element;
		
	//If current process has no messages block it
	if(mailbox->first == NULL) {
		gp_current_process->m_state = BLOCKED_ON_RECEIVE;
		priority = g_proc_table[gp_current_process->m_pid].m_priority;
		
		//push PCB of current process on blocked_resource_qs; 
		element = k_request_element();
		element->data = gp_current_process;
		push(getBlockedReceiveQ(priority), element);
		k_release_processor();
	}
	
	__disable_irq();

	process = gp_current_process;
	mailbox = gp_current_process->mailbox;
	received = pop_mailbox(process->m_pid);

	message = received->message;

	*sender_id =  received->sender_id;

	priority = destroy_envelope(received);

	__enable_irq();
		
	return (void *)message;
}

int k_delayed_send(int process_id, void *message_envelope, int delay){
	Envelope *envelope;
	Queue *timed;
	Element *element = k_request_element();
	
	if(process_id > NUM_PROCS || process_id < 1) {
		return RTX_ERR;
	}
	
	envelope = build_envelope(process_id, message_envelope, delay);
	element->data = envelope;
		
	if(envelope == NULL) {
		return RTX_ERR;
	}
	
	timed = getTimedQ();
	push(timed, element);
	
	return RTX_OK;
}



