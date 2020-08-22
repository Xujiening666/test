#ifndef __NOTIFIER_H__
#define __NOTIFIER_H__

//internal header
typedef int32_t  (*Notifier_call_t)(uint32_t message, uint32_t aux);//notifer call-back

//FIXME: user number
#define NOTIFIER_MAX                    (20)

//the status of notifier
#define NOTIFIER_USED   1
#define NOTIFIER_FREE   0

//notifier structure
typedef struct notifier
{
	uint32_t        flag;  //bit0 used to indicate free or use
	Notifier_call_t  pcb;   //cb function pointer
	struct notifier *next;  //next notifier object
} notifier_t;

//external header
int32_t notifier_init(void);
int32_t notifier_exit(void);

int32_t notifier_insert(struct notifier **head, Notifier_call_t pcb);
int32_t notifier_delete(struct notifier **head, Notifier_call_t pcb);
int32_t notifier_notify(struct notifier **head, uint32_t message, uint32_t aux);
#endif
