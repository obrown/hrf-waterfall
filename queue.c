#include "queue.h"

#include <stdlib.h>
#include <stdio.h>


void queue_init(queue_t *q) {
  pthread_mutex_init(&q->lock, NULL);
  q->first = NULL;
  q->last = NULL;
  q->size = 0;
}

void queue_destroy(queue_t *q) {
  pthread_mutex_lock(&q->lock);
  queue_entry_t *current = q->first;
  while (current) {
    queue_entry_t *tmp = current->next;
    free(current->val);
    free(current);
    current = tmp;
  }
  pthread_mutex_destroy(&q->lock);
}

void queue_append(queue_t *q, void *val) {
  queue_entry_t *entry = malloc(sizeof(queue_entry_t));
  entry->val = val;

  pthread_mutex_lock(&q->lock);

  entry->next = NULL;
  if (q->size == 0) {
    q->first = entry;
    q->last = entry;
    entry->prev = NULL;
  } else {
    q->last->next = entry;
    entry->prev = q->last;
    q->last = entry;
  }

  q->size++;

  pthread_mutex_unlock(&q->lock);
}

void *queue_pop(queue_t *q) {
  pthread_mutex_lock(&q->lock);

  void *val = NULL;
  if (q->size == 1) {
    val = q->first->val;
    free(q->first);
    q->first = NULL;
    q->last = NULL;
    q->size--;
  } else if (q->size > 1) {
    val = q->first->val;
    queue_entry_t *snd = q->first->next;
    free(q->first);
    snd->prev = NULL;
    q->first = snd;
    q->size--;
  }

  pthread_mutex_unlock(&q->lock);

  return val;
}

size_t queue_size(queue_t *q) {
  size_t size = q->size;
  return size;
}
