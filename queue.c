#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// implementation of queue
	struct queue_t* create_queue(){
		struct queue_t* queue = (struct queue_t*) malloc(sizeof(struct queue_t));
		queue->current = NULL;
		queue->last = NULL;

		return queue;
	}
	void push(struct queue_t* queue, void* element){
		struct queue_node *node = (struct queue_node *) malloc(sizeof(struct queue_node));
		node->val = element;
		node->next = NULL;

		if(! (queue->last)){
			queue->current = node;
			queue->last = node;
			return;	
		}

		queue->last->next = node;
		queue->last = node;
	}

	void pushFirst(struct queue_t* queue, void* element){
		struct queue_node *node = (struct queue_node *) malloc(sizeof(struct queue_node));
		node->val = element;
		node->next = NULL;

		if(! (queue->current)){
			queue->current = node;
			queue->last = node;
			return;
		}
		
		node->next = queue->current;
		queue->current = node;
	}

	void pushAfterFirst(struct queue_t* queue, void* element){
		struct queue_node *node = (struct queue_node *) malloc(sizeof(struct queue_node));
		node->val = element;
		node->next = NULL;

		if(! (queue->current)){
			queue->current = node;
			queue->last = node;
			return;
		}

		struct queue_node* second;
		second = queue->current->next;

		queue->current->next = node;
		node->next = second;
	}
	// get first element but not remove
	void* peek(struct queue_t* queue){
		if(! (queue->current)){
			return NULL;
		}

		return queue->current->val;
	}
	// get first element and remove from list
	void* poll(struct queue_t* queue){
		if(! (queue->current)){
			return NULL;
		}
			
		void* val = queue->current->val;

		struct queue_node* n = queue->current;
		
		if(queue->current->next != NULL){
			queue->current = queue->current->next;
		}else{
			queue->current = NULL;
			queue->last = NULL;
		}
		
		free(n);


	return val;
}

void free_queue(struct queue_t *queue){	
	while(! is_empty(queue)){
		void* t = poll(queue);
		free(t);
	}
}

void free_queue_exec(struct queue_t *queue, void (*free_exec)(void*)){
	while(! is_empty(queue)){
		void* t = poll(queue);
		free_exec(t);
	}
}


int is_empty(struct queue_t* queue){
	if(queue == NULL){
		return true;
	}
	return queue->current == NULL;
}

