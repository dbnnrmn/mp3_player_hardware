#include "audiochank.h"

audiochunk_queue_t* insert_audiochunk(int capacity)
{
    audiochunk_queue_t *queue = malloc(sizeof(audiochunk_queue_t));
    queue->first = NULL;
    queue->last = NULL;
    queue->quantity = 0;
    queue->capacity = capacity;
    queue->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    
    sem_unlink("/full");
    sem_unlink("/empty");
    
    queue->full = sem_open("/full", O_CREAT, 0644, 0);
    queue->empty = sem_open("/empty", O_CREAT, 0644, capacity);
    
    return queue;
}

int insert_audiochank(audiochunk_t *chunk, audiochunk_queue_t *queue)
{
    if (queue == NULL) {
        return 0;
    }
    
    audiochunk_node_t *new_chunk_list = malloc(sizeof(audiochunk_node_t));
    new_chunk_list->chunk = chunk;
    new_chunk_list->next = NULL;
    
    sem_wait(queue->empty);
    
    pthread_mutex_lock(&queue->mutex);
    
    if (queue->quantity == 0) {
        queue->first = new_chunk_list;
    } else {
        queue->last->next = new_chunk_list;
    }
    queue->last = new_chunk_list;
    
    queue->quantity++;
    
    pthread_mutex_unlock(&queue->mutex);
    
    sem_post(queue->full);
    
    return 1;
}


int  get_audiochunk(audiochunk_t *chunk, audiochunk_queue_t *queue, int bytes)
{
    if (queue == NULL) {
        return 0;
    }
    
    sem_wait(queue->full);
    pthread_mutex_lock(&queue->mutex);
    
    audiochunk_node_t *first = queue->first;
    if (first->chunk->size <= bytes) {
        chunk = first->chunk;
        queue->first = queue->first->next;
        
        queue->quantity--;
        if (queue->quantity == 0) {
            queue->first = NULL;
            queue->last = NULL;
        }
        
        free(first);
        sem_post(queue->empty);
    } else {
        sem_post(queue->full);
        
        chunk = malloc(sizeof(audiochunk_t));
        (*chunk)->size = bytes;
        (*chunk)->data = malloc(bytes);
        memcpy((*chunk)->data, first->chunk->data, bytes);
        
        first->chunk->size -= bytes;
        uint8_t *remaining_data = malloc(first->chunk->size);
        memcpy(remaining_data, first->chunk->data + bytes, first->chunk->size);
        free(first->chunk->data);
        first->chunk->data = remainingData;
    }
    
    pthread_mutex_unlock(&queue->mutex);
    
    return 1;
}