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

void setMessageText(msgbuf* message, char text[], int textLength) {
	int i = 0;
	int j = 0;
	
	while (i < sizeof(message->mtext) || j < textLength) {
		if (j < textLength) {
			message->mtext[i] = text[j];
		} else {
			message->mtext[i] = NULL;
		}
		i++;
		j++;
	}
}

int checkMessageText(msgbuf* message, char text[]) {
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
	if(message_envelope->mtype == KCD_REG) {
			envelope->sender_id = KCD_INTERRUPT_ID;
	}
	envelope->destination_id = process_id;
	envelope->message = message_envelope;
	message->time = get_time();
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
msgbuf *allocate_message(int type, char text[]){
		msgbuf *message = request_memory_block();
	
		message->mtype = type;
		setMessageText(message, text, sizeof(text));	
		return message;
}

//Frees the memory associated with a message
int deallocate_message(msgbuf *message){
		return release_memory_block(message);
};



//pushes an evelope onto a mailbox
int push_mailbox(Envelope *envelope) {
	Element *element = k_request_element();
	
	PCB *process = gp_pcbs[envelope->destination_id];
	Queue *mailbox = process->mailbox;
	
	Element *pcb;
	
	element->data = envelope;
	push(mailbox, element);
	
	__disable_irq();
	
		//check if destination process is blocked on received for message type
	if( process->m_state == BLOCKED_ON_RECEIVE) {
		process->m_state = RDY;
		pcb = k_request_element();
		pcb->data = process;
		pushToReadyQ(process->m_priority, pcb);
		removeFromQ(getBlockedReceiveQ(process->m_priority), process->m_pid);  
		
		if(process->m_priority < gp_current_process->m_priority){
			k_release_processor();
		}
	}
	__enable_irq();
	
	return RTX_OK;
}

//Pops the first envelope from the mailbox and frees memory for associated element
Envelope *pop_mailbox(int process_id){
	PCB *process = gp_pcbs[process_id];
	Queue *mailbox = process->mailbox;
	Element *element = pop(mailbox);
	Envelope *envelope = (Envelope *)element->data;
	//TODO: k_release_element(element);
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
void *receive_message(int *sender_id) {	
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
		
		//push PCB of current process on blocked_resource_qs; << here we are pushing a PCB. <<
		element = k_request_element();
		element->data = gp_current_process;
		push(getBlockedReceiveQ(priority), element);
		release_processor();
	}
	
	__disable_irq();

	received = pop_mailbox(process->m_pid);
	message = received->message;
	*sender_id =  received->sender_id;
	
	destroy_envelope(received);
	__enable_irq();
		
	return (void *)message;
}

//adds a message the the timed queue for the timer to send after a set number of seconds denoted by delay parameter
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



