// A C program to show implementation of LRU cache
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Include this header for time functions
#include <unistd.h>
// include the sleep function
#include <sys/types.h>

// A Queue Node (Queue is implemented using Doubly Linked
// List)
typedef struct QNode
{
    struct QNode *prev, *next;
    unsigned
        pageNumber; // the page number stored in this QNode
} QNode;

// A Queue (A FIFO collection of Queue Nodes)
typedef struct Queue
{
    unsigned count;          // Number of filled frames
    unsigned numberOfFrames; // total number of frames
    QNode *front, *rear;
} Queue;

// A hash (Collection of pointers to Queue Nodes)
typedef struct Hash
{
    int capacity;  // how many pages can be there
    QNode **array; // an array of queue nodes
    char **path;   // an array of strings (paths)
    int *ssid;     // an array of integers (ssids)
    time_t *time;  // an array of time_t (time values)
} Hash;

// A utility function to create a new Queue Node. The queue
// Node will store the given 'pageNumber'
QNode *newQNode(unsigned pageNumber)
{
    // Allocate memory and assign 'pageNumber'
    QNode *temp = (QNode *)malloc(sizeof(QNode));
    temp->pageNumber = pageNumber;

    // Initialize prev and next as NULL
    temp->prev = temp->next = NULL;

    return temp;
}

// A utility function to create an empty Queue.
// The queue can have at most 'numberOfFrames' nodes
Queue *createQueue(int numberOfFrames)
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));

    // The queue is empty
    queue->count = 0;
    queue->front = queue->rear = NULL;

    // Number of frames that can be stored in memory
    queue->numberOfFrames = numberOfFrames;

    return queue;
}

// A utility function to create an empty Hash of given
// capacity
Hash *createHash(int capacity)
{
    // Allocate memory for hash
    Hash *hash = (Hash *)malloc(sizeof(Hash));
    hash->capacity = capacity;

    // Create an array of pointers for referring queue nodes
    hash->array = (QNode **)malloc(hash->capacity * sizeof(QNode *));

    // Create an array of strings for paths
    hash->path = (char **)malloc(hash->capacity * sizeof(char *));

    // Create an array of integers for ssids
    hash->ssid = (int *)malloc(hash->capacity * sizeof(int));

    // Create an array of time_t for time values
    hash->time = (time_t *)malloc(hash->capacity * sizeof(time_t));

    // Initialize all hash entries as empty
    int i;
    for (i = 0; i < hash->capacity; ++i)
    {
        hash->array[i] = NULL;
        hash->path[i] = NULL;
        hash->ssid[i] = -1; // Initialize ssid to -1 (assuming -1 is not a valid ssid)
        hash->time[i] = 0;  // Initialize time to 0
    }

    return hash;
}

// A function to check if there is slot available in memory
int AreAllFramesFull(Queue *queue)
{
    return queue->count == queue->numberOfFrames;
}

// A utility function to check if queue is empty
int isQueueEmpty(Queue *queue)
{
    return queue->rear == NULL;
}

// A utility function to delete a frame from queue
void deQueue(Queue *queue)
{
    if (isQueueEmpty(queue))
        return;

    // If this is the only node in list, then change front
    if (queue->front == queue->rear)
        queue->front = NULL;

    // Change rear and remove the previous rear
    QNode *temp = queue->rear;
    queue->rear = queue->rear->prev;

    if (queue->rear)
        queue->rear->next = NULL;

    free(temp);

    // decrement the number of full frames by 1
    queue->count--;
}

// A function to add a page with given 'pageNumber' to both
// the queue and hash
void Enqueue(Queue *queue, Hash *hash, unsigned pageNumber, const char *path, int ssid, time_t currentTime)
{
    // If all frames are full, remove the page at the rear
    if (AreAllFramesFull(queue))
    {
        // Remove page from hash
        hash->array[queue->rear->pageNumber] = NULL;
        hash->path[queue->rear->pageNumber] = NULL;
        hash->ssid[queue->rear->pageNumber] = -1;
        hash->time[queue->rear->pageNumber] = 0;
        deQueue(queue);
    }

    // Create a new node with the given page number,
    // And add the new node to the front of the queue
    QNode *temp = newQNode(pageNumber);
    temp->next = queue->front;

    // If the queue is empty, change both front and rear
    // pointers
    if (isQueueEmpty(queue))
        queue->rear = queue->front = temp;
    else // Else change the front
    {
        queue->front->prev = temp;
        queue->front = temp;
    }

    // Add page entry to hash also
    hash->array[pageNumber] = temp;
    hash->path[pageNumber] = strdup(path);
    hash->ssid[pageNumber] = ssid;
    hash->time[pageNumber] = currentTime; // Set the time to the provided time

    // Increment the number of full frames
    queue->count++;
}

// This function is called when a page with given
// 'pageNumber' is referenced from cache (or memory). There
// are two cases:
// 1. Frame is not there in memory, we bring it in memory
// and add to the front of queue
// 2. Frame is there in memory, we move the frame to front
// of queue
void deletePath(Queue *queue, Hash *hash, const char *path, int totalPages)
{
    // Iterate through each entry in the hashtable
    for (int i = 0; i < hash->capacity; ++i)
    {
        // Check if the entry is not empty and has the matching path
        if (hash->path[i] != NULL && strcmp(hash->path[i], path) == 0)
        {
            // Free the memory allocated for the path
            free(hash->path[i]);
            hash->path[i] = NULL;

            // Set ssid to -1 (assuming -1 is not a valid ssid)
            hash->ssid[i] = -1;

            // Set time to 0
            hash->time[i] = 0;

            // Remove the entry from the queue
            QNode *node = hash->array[i];

            // If the node is the front, update front
            if (node == queue->front)
                queue->front = node->next;

            // If the node is the rear, update rear
            if (node == queue->rear)
                queue->rear = node->prev;

            // Update the pointers of the adjacent nodes
            if (node->prev)
                node->prev->next = node->next;
            if (node->next)
                node->next->prev = node->prev;

            // Free the memory allocated for the node
            free(node);

            // Break the loop after finding and deleting the path
            break;
        }
    }
}

void ReferencePage(Queue *queue, Hash *hash, unsigned pageNumber, const char *path, int ssid, time_t referenceTime)
{
    QNode *reqPage = hash->array[pageNumber];

    // The page is not in cache, bring it
    if (reqPage == NULL)
        Enqueue(queue, hash, pageNumber, path, ssid, referenceTime);

    // Page is there and not at the front, change pointer
    else if (reqPage != queue->front)
    {
        // Unlink requested page from its current location
        // in the queue.
        reqPage->prev->next = reqPage->next;
        if (reqPage->next)
            reqPage->next->prev = reqPage->prev;

        // If the requested page is rear, then change rear
        // as this node will be moved to the front
        if (reqPage == queue->rear)
        {
            queue->rear = reqPage->prev;
            queue->rear->next = NULL;
        }

        // Put the requested page before the current front
        reqPage->next = queue->front;
        reqPage->prev = NULL;

        // Change prev of the current front
        reqPage->next->prev = reqPage;

        // Change front to the requested page
        queue->front = reqPage;

        // Update path, ssid, and time in the hash
        hash->path[pageNumber] = strdup(path);
        hash->ssid[pageNumber] = ssid;
        hash->time[pageNumber] = referenceTime;
    }
}

// Driver code is given below for implementation of LRU
int main()
{
    // Let cache can hold 4 pages
    Queue *q = createQueue(4);

    // Let 10 different pages can be requested (pages to be
    // referenced are numbered from 0 to 9
    Hash *hash = createHash(10);

    // Let us refer pages 1, 2, 3, 1, 4, 5
    time_t currentTime = time(NULL); // Get the current time
    ReferencePage(q, hash, 1, "path1", 123, currentTime);
    // sleep(3);
    ReferencePage(q, hash, 2, "path2", 456, currentTime);
    // sleep(1);
    ReferencePage(q, hash, 3, "path3", 789, currentTime);
    // sleep(2);
    ReferencePage(q, hash, 1, "path1", 123, currentTime); // Duplicate reference, won't change the order
    ReferencePage(q, hash, 4, "path4", 111, currentTime);
    ReferencePage(q, hash, 5, "path5", 222, currentTime);
    // Delete path "path3" from the hashtable
    // deletePath(q, hash, "path3", 10);
    // Let us print cache frames after the above-referenced
    // pages
    QNode *current = q->front;
    while (current != NULL)
    {
        printf("Page: %d, Path: %s, SSID: %d, Time: %ld\n", current->pageNumber, hash->path[current->pageNumber], hash->ssid[current->pageNumber], hash->time[current->pageNumber]);
        current = current->next;
    }

    return 0;
}
