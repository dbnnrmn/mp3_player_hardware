#include <semaphore.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>

typedef struct  {
    uint8_t *data;
    int size;
}audiochunk_t;

typedef struct  {
    audiochunk_t *next;
    audiochunk_t *chunk;
} audiochunk_node_t;

typedef struct  {
    audiochunk_node_t *first;
    audiochunk_node_t *last;
    pthread_mutex_t mutex;
    int quantity;
    int capacity;
    sem_t *full;
    sem_t *empty;
} audiochunk_queue_t;


audiochunk_queue_t* create_audiochunk_queue(int capacity);
int insert_audiochank(audiochunk_t *chunk, audiochunk_queue_t *queue);
int get_audiochunk(audiochunk_t *chunk, audiochunk_queue_t *queuee, int bytes);