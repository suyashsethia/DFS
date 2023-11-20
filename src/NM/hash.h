#ifndef HASH_H
#define HASH_H

#include <time.h>

typedef struct QNode
{
    struct QNode *prev, *next;
    unsigned pageNumber;
} QNode;

typedef struct Queue
{
    unsigned count;
    unsigned numberOfFrames;
    QNode *front, *rear;
} Queue;

typedef struct Hash
{
    int capacity;
    QNode **array;
    char **path;
    int *ssid;
    time_t *time;
} Hash;

QNode *newQNode(unsigned pageNumber);
Queue *createQueue(int numberOfFrames);
Hash *createHash(int capacity);
int AreAllFramesFull(Queue *queue);
int isQueueEmpty(Queue *queue);
void deQueue(Queue *queue);
void Enqueue(Queue *queue, Hash *hash, unsigned pageNumber, const char *path, int ssid, time_t currentTime);
void ReferencePage(Queue *queue, Hash *hash, unsigned pageNumber, const char *path, int ssid, time_t referenceTime);

#endif // HASH_H
