
#include "embsys_lab1_queue.h"

#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static int queue_mutex(queue_t* queue) {
  // allocate memory for the mutex
  queue->mutex = malloc(sizeof(pthread_mutex_t));
  if (queue->mutex == NULL) {
    perror("ERROR: QUEUE -> queue_create() -> malloc() failed to allocate memory for the mutex ");
    return EXIT_FAILURE;
  }
  // allocate memory for mutex attributes
  queue->mutex_attr = malloc(sizeof(pthread_mutexattr_t));
  if (queue->mutex_attr == NULL) {
    perror("ERROR: QUEUE -> queue_create() -> malloc() failed to allocate memory for the mutex attributes ");
    free(queue->mutex);
    queue->mutex = NULL;
    return EXIT_FAILURE;
  }
  if (pthread_mutexattr_init(queue->mutex_attr) != 0) {
    perror("ERROR: QUEUE -> queue_create() -> pthread_mutexattr_init() failed to initialize the mutex attributes ");
    goto exit_failure;
  }
  // If a thread attempts to relock without first unlocking or unlock a mutex that is unlocked, it will return an error.
  if (pthread_mutexattr_settype(queue->mutex_attr, PTHREAD_MUTEX_ERRORCHECK) != 0) {
    perror("ERROR: QUEUE -> queue_create() -> pthread_mutexattr_settype() failed to set the mutex attributes ");
    goto exit_failure;
  }
  if (pthread_mutex_init(queue->mutex, queue->mutex_attr) != 0) {
    perror("ERROR: QUEUE -> queue_create() -> pthread_mutex_init() failed to initialize the mutex ");
    goto exit_failure;
  }
  return EXIT_SUCCESS;
exit_failure:
  free(queue->mutex_attr);
  free(queue->mutex);
  queue->mutex_attr = NULL;
  queue->mutex = NULL;
  return EXIT_FAILURE;
}

extern int queue_create(queue_t* queue) {
  // malloc a clean space for the queue
  queue = calloc((size_t)1, sizeof(queue_t));
  if (queue == NULL) {
    perror("ERROR: QUEUE -> queue_create() -> calloc() failed to allocate memory for the queue ");
    return EXIT_FAILURE;
  }
  // empty queue, head and tail point to the same node
  queue->head = queue->tail = NULL;
  // zero size
  queue->size = 0;
  // initialize the mutex
  if (queue_mutex(queue) == EXIT_FAILURE) {
    perror("ERROR: QUEUE -> queue_create() -> queue_mutex() failed to initialize the mutex ");
    free(queue);
    queue = NULL;
    return EXIT_FAILURE;
  }
  // initialize the condition variable
  queue->not_empty = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
  return EXIT_SUCCESS;
}

extern int queue_enqueue(queue_t* queue, void* data) {
  // protect the queue
  if (pthread_mutex_lock(queue->mutex) != 0) {
    perror("ERROR: QUEUE -> queue_enqueue() -> pthread_mutex_lock() failed to lock the mutex ");
    return EXIT_FAILURE;
  }
  // create a new queue struct to hold the data
  queue_struct_t* new_node = calloc((size_t)1, sizeof(queue_struct_t));
  if (new_node == NULL) {
    perror("ERROR: QUEUE -> queue_enqueue() -> calloc() failed to allocate memory for the new node ");
    return EXIT_FAILURE;
  }
  new_node->data = data;
  new_node->next = NULL;
  // if the queue is empty, the new node is the head and tail
  if (queue->size == 0 && queue->head == NULL && queue->tail == NULL) {
    queue->head = queue->tail = new_node;
    queue->size++;
  } else {
    // otherwise, add the new node to the tail
    queue->tail->next = new_node;
    queue->tail = new_node;
    queue->size++;
  }
  // unlock the mutex
  if (pthread_mutex_unlock(queue->mutex) != 0) {
    perror("ERROR: QUEUE -> queue_enqueue() -> pthread_mutex_unlock() failed to unlock the mutex ");
    return EXIT_FAILURE;
  }
  // signal the condition variable
  if (pthread_cond_signal(&queue->not_empty) != 0) {
    perror("ERROR: QUEUE -> queue_enqueue() -> pthread_cond_signal() failed to signal the condition variable ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

extern int queue_dequeue(queue_t* queue, void** data) {
  // protect the queue
  if (pthread_mutex_lock(queue->mutex) != 0) {
    perror("ERROR: QUEUE -> queue_dequeue() -> pthread_mutex_lock() failed to lock the mutex ");
    return EXIT_FAILURE;
  }
  // if the queue is empty, wait for the condition variable
  while (queue->size == 0 && queue->head == NULL && queue->tail == NULL) {
    if (pthread_cond_wait(&queue->not_empty, queue->mutex) != 0) {
      perror("ERROR: QUEUE -> queue_dequeue() -> pthread_cond_wait() failed to wait for the condition variable ");
      return EXIT_FAILURE;
    }
  }
  // get the data from the head
  *data = queue->head->data;
  // if the queue has only one element, set the head and tail to NULL
  if (queue->size == 1 && queue->head == queue->tail) {
    free(queue->head);
    queue->head = queue->tail = NULL;
    queue->size--;
  } else {
    // otherwise, set the head to the next element
    queue_struct_t* temp = queue->head;
    queue->head = queue->head->next;
    free(temp);
    queue->size--;
  }
  // unlock the mutex
  if (pthread_mutex_unlock(queue->mutex) != 0) {
    perror("ERROR: QUEUE -> queue_dequeue() -> pthread_mutex_unlock() failed to unlock the mutex ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

extern int queue_destroy(queue_t* queue) {
  // protect the queue
  if (pthread_mutex_lock(queue->mutex) != 0) {
    perror("ERROR: QUEUE -> queue_destroy() -> pthread_mutex_lock() failed to lock the mutex ");
    return EXIT_FAILURE;
  }
  // free the queue
  while (queue->size > 0) {
    queue_struct_t* temp = queue->head;
    // free stored data
    if (temp->data != NULL) {
      free(temp->data);
      temp->data = NULL;
    }
    queue->head = queue->head->next;
    free(temp);
    queue->size--;
  }
  // unlock the mutex
  if (pthread_mutex_unlock(queue->mutex) != 0) {
    perror("ERROR: QUEUE -> queue_destroy() -> pthread_mutex_unlock() failed to unlock the mutex ");
    return EXIT_FAILURE;
  }
  // destroy the mutex and the mutex attributes
  if (pthread_mutex_destroy(queue->mutex) != 0) {
    perror("ERROR: QUEUE -> queue_destroy() -> pthread_mutex_destroy() failed to destroy the mutex ");
    return EXIT_FAILURE;
  }
  if (pthread_mutexattr_destroy(queue->mutex_attr) != 0) {
    perror("ERROR: QUEUE -> queue_destroy() -> pthread_mutexattr_destroy() failed to destroy the mutex attributes ");
    return EXIT_FAILURE;
  }
  free(queue->mutex);
  free(queue->mutex_attr);
  queue->mutex = NULL;
  queue->mutex_attr = NULL;
  // destroy the condition variable
  if (pthread_cond_destroy(&queue->not_empty) != 0) {
    perror("ERROR: QUEUE -> queue_destroy() -> pthread_cond_destroy() failed to destroy the condition variable ");
    return EXIT_FAILURE;
  }
  // free the queue
  free(queue);
  queue = NULL;
  return EXIT_SUCCESS;
}
