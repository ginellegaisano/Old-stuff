/**
 * @file:   msg.h
 * @brief:  Message Send and Receive header
 * @author: GG (yes re), Reesey, RayMak, and LJ
 * @date:   2015/03/10
 */
 
#ifndef MSG_H_
#define MSG_H
/* ----- Definitions ----- */
#define DEFAULT 0
#define KCD_REG 1

#define KCD_INTERRUPT_ID 15


typedef struct msgbuf msgbuf;
struct msgbuf {
	int mtype;
	char mtext[1];
};

typedef struct Message Message;
struct Message{
	int sender_id;
	int destination_id;
	int time;
	int delay;
	msgbuf *message;
};




void *receive_message(int *);
int k_send_message(int, void *);
int k_delayed_send(int, void *, int);


#endif
