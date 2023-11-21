#ifndef HASH_H
#define HASH_H

#include <time.h>

#define HASHTABLE_SIZE 10

typedef struct
{
    char path[256];
    time_t entry_time;
    int ssid;
} CacheEntry;

typedef struct
{
    CacheEntry entries[HASHTABLE_SIZE];
} Hashtable;

unsigned int hash(char *path);
void initializeHashtable(Hashtable *ht);
void addPath(Hashtable *ht, char *path, int ssid);
void deletePath(Hashtable *ht, char *path);
int getSSID(Hashtable *ht, char *path);
void printHashtable(Hashtable *ht);

#endif // HASH_H
