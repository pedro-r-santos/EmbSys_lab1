#ifndef EMBSYS_LAB1_QUEUE_H_
#define EMBSYS_LAB1_QUEUE_H_

#include <pthread.h>

#include "../lab1_print/embsys_lab1_print.h"

// typedef struct queue_struct_t queue_struct_t;

typedef struct queue_element_t {
  void* data;
  struct queue_element_t* next;
} queue_element_t;

typedef struct queue_t {
  int size;  // size of the queue
  queue_element_t* head;
  queue_element_t* tail;
  pthread_mutexattr_t* mutex_attr;
  pthread_mutex_t* mutex;
  pthread_cond_t not_empty;
} queue_t;

extern queue_t* queue_create(void);
extern int queue_destroy(queue_t* queue);
extern int queue_enqueue(queue_t* queue, void* data);
extern int queue_dequeue(queue_t* queue, void** data);

#endif /* EMBSYS_LAB1_QUEUE_H_ */
