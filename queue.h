#ifndef _QUEUE_H_
#define _QUEUE_H_

struct queue_node {
	void* val;
	struct queue_node* next;
};	


struct queue_t {
	struct queue_node* last;
	struct queue_node* current;
};

struct queue_t* create_queue();

void push(struct queue_t* queue, void* element);
void pushFirst(struct queue_t* queue, void* element);
void pushAfterFirst(struct queue_t* queue, void* element);
void* poll(struct queue_t* queue);
void* peek(struct queue_t* queue);

void free_queue(struct queue_t* queue);
void free_queue_each(struct queue_t* queue, void(*free_exec)(void*));

int is_empty(struct queue_t* queue);
#endif
