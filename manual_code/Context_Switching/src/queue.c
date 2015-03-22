/**
 * @file:   queue.c
 * @brief:  Queue structure
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/03/05
 */

#include "queue.h"
#include "rtx.h"
#include "uart_polling.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

//array of queues, organized by priority
Queue* blocked_resource_qs[NUM_PRIORITIES];
Queue* blocked_receive_qs[NUM_PRIORITIES];
Queue* ready_qs[NUM_PRIORITIES];
Queue* timed_q[1];

Element* pop(Queue* self) {
	Element* element = NULL;
	
	if (self == NULL || self->first == NULL) {
		return NULL;
	} else if (self->first->next == NULL) { //queue only has 1 element
		element = self->first;
		element->next = NULL;
		self->first = NULL;
		self->last = NULL;
		return element;
	}
	element = self->first;
	self->first = self->first->next;
	element->next = NULL;
	return element;
};

Element* removeFromQ(Queue* self, int id) {
	Element* iterator = self->first;
	Element* element = NULL;
	
	if (iterator == NULL) {
		return element;
	}
	
	if (((PCB*)(iterator->data))->m_pid == id) {
		element = pop(self);
		return element;
	}
	
	while (iterator->next != NULL && ((PCB*)(iterator->next->data))->m_pid != (id)) {
		iterator = iterator->next;
	}
	if (iterator->next != NULL && ((PCB*)(iterator->next->data))->m_pid == (id)) {
		element = iterator->next;
		if (element == self->last) {
			self->last = iterator;
		}
		iterator->next = element->next;
	}
	
	return element;
};

int push(Queue* self, Element* element) {
	element->next = NULL;
	
	if (self->first == NULL) { //queue was formerly empty
			self->first = element;
			self->last = element;
	} else {
		self->last->next = element;
		self->last = element;
	}
	self->last->next = NULL;
	
	return RTX_OK;
};


void printReadyQ(char* tag) {
	Element* iterator = NULL;
	int i;
	__disable_irq();
	uart1_put_string("\n\rReady Queue: ");
	uart1_put_string(tag);
	uart1_put_string("\n\r");
	for (i = 0; i < NUM_PRIORITIES; i++) {
		iterator = getReadyQ(i)->first;
		uart1_put_string("Queue: ");
		if (i >= 10) {
			uart1_put_char('1');
		}
		uart1_put_char(i%10 + '0');
			uart1_put_string("\n\r");

		while (iterator != NULL) {
			PCB* pcb = iterator->data;
			uart1_put_string("PID: ");
			if ( pcb->m_pid >= 10) {
				uart1_put_char('1');
			}
			uart1_put_char( pcb->m_pid%10 + '0');
			uart1_put_string("\n\r");

			iterator = iterator->next;
		}
		uart1_put_string("\n\r");
	}
	uart1_put_string("\n\r\n\r");
	__enable_irq();
}

void printBlockedQ(char* tag) {
	Element* iterator = NULL;
	int i;
	__disable_irq();
	uart1_put_string("\n\rBlocked Queue: ");
	uart1_put_string(tag);
	uart1_put_string("\n\r");
	for (i = 0; i < NUM_PRIORITIES; i++) {
		iterator = getBlockedResourceQ(i)->first;
		uart1_put_string("Queue: ");
		if (i >= 10) {
			uart1_put_char('1');
		}
		uart1_put_char(i%10 + '0');
			uart1_put_string("\n\r");
		while (iterator != NULL) {
			PCB* pcb = iterator->data;
			uart1_put_string("PID: ");
			if ( pcb->m_pid >= 10) {
				uart1_put_char('1');
			}
			uart1_put_char( pcb->m_pid%10 + '0');
			uart1_put_string("\n\r");
			iterator = iterator->next;
		}
		uart1_put_string("\n\r");
	}
	uart1_put_string("\n\r\n\r");
	__enable_irq();
}

void printBlockedReceiveQ(char* tag) {
	Element* iterator = NULL;
	int i;
	__disable_irq();
	uart1_put_string("\n\rBlocked Received Queue: ");
	uart1_put_string(tag);
	uart1_put_string("\n\r");
	for (i = 0; i < NUM_PRIORITIES; i++) {
		iterator = getBlockedReceiveQ(i)->first;
		uart1_put_string("Queue: ");
		if (i >= 10) {
			uart1_put_char('1');
		}
		uart1_put_char(i%10 + '0');
			uart1_put_string("\n\r");
		while (iterator != NULL) {
			PCB* pcb = iterator->data;
			uart1_put_string("PID: ");
			if ( pcb->m_pid >= 10) {
				uart1_put_char('1');
			}
			uart1_put_char( pcb->m_pid%10 + '0');
			uart1_put_string("\n\r");
			iterator = iterator->next;
		}
		uart1_put_string("\n\r");
	}
	uart1_put_string("\n\r\n\r");
	__enable_irq();
}


/**
 * @brief pushToReadyQ(). 
 * @param priority of the pcb to push onto the ready Queue
 * @param pointer to the pcb
 * 
 * POST: add Element pcb to ready queue at index [priority]
 */
void pushToReadyQ (int priority, Element* p_pcb_old) {
	push(ready_qs[priority], p_pcb_old);
}

/**
 * @brief popFromReadyQ(). 
 * @param priority
 * @return first pcb at priority
 *
 * POST: ready_qs is updated. first pcb of index priority is removed
 */
Element* popFromReadyQ (int priority) {
	return pop(ready_qs[priority]);
}

/**
 * @brief getReadyQ().
 * @param priority
 * @return returns *Queue stored in ready_qs at index[priority] 
 */
Queue* getReadyQ(int priority) {
	return ready_qs[priority];
}

/**
 * @brief setReadyQ().
 * @param priority
 * @param q
 */
void setReadyQ(int priority, Queue* q) {
	ready_qs[priority] = q;
}

/**
 * @brief getBlockedResourceQ().
 * @param priority
 * @return returns *Queue stored in blocked_resource_qs at index[priority] 
 */
Queue* getBlockedResourceQ(int priority) {
	return blocked_resource_qs[priority];
}

/**
 * @brief setBlockedResourceQ().
 * @param priority
 * @param q
 */
void setBlockedResourceQ(int priority, Queue* q) {
	blocked_resource_qs[priority] = q;
}
/**
 * @brief getBlockedReceiveQ().
 * @param priority
 * @return returns *Queue stored in blocked_receive_qs at index[priority] 
 */
Queue* getBlockedReceiveQ(int priority) {
	return blocked_receive_qs[priority];
}
/**
 * @brief setBlockedResourceQ().
 * @param priority
 * @param q
 */
void setBlockedReceiveQ(int priority, Queue* q) {
	blocked_receive_qs[priority] = q;
}

/**
 * @brief getTimedQ().
 * @param priority
 * @return returns *Queue stored in blocked_receive_qs at index[priority] 
 */
Queue* getTimedQ(void) {
	return timed_q[0];
}
/**
 * @brief getTimedQ().
 * @param priority
 * @param q
 */
void setTimedQ(Queue* q) {
	timed_q[0] = q;
}

