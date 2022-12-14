#ifndef EMBSYS_LAB1_QUEUE_H_
#define EMBSYS_LAB1_QUEUE_H_

#include <pthread.h>

typedef struct queue_struct_t queue_struct_t;

struct queue_struct_t {
  void* data;
  queue_struct_t* next;
};

typedef struct {
  int* data;
  int size;  // size of the queue
  queue_struct_t* head;
  queue_struct_t* tail;
  pthread_mutexattr_t* mutex_attr;
  pthread_mutex_t* mutex;
  pthread_cond_t not_empty;
} queue_t;

extern int queue_create(queue_t* queue);
extern int queue_destroy(queue_t* queue);
extern int queue_enqueue(queue_t* queue, void* data);
extern int queue_dequeue(queue_t* queue, void** data);

#endif /* EMBSYS_LAB1_QUEUE_H_ */
