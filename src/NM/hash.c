#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

unsigned int hash(char *path)
{
    unsigned int hashval = 0;
    for (; *path != '\0'; path++)
    {
        hashval = *path + (hashval << 5) - hashval;
    }
    return hashval % HASHTABLE_SIZE;
}

void initializeHashtable(Hashtable *ht)
{
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        ht->entries[i].entry_time = 0;
    }
}

void addPath(Hashtable *ht, char *path, int ssid)
{
    unsigned int index = hash(path);

    CacheEntry newEntry;
    strcpy(newEntry.path, path);
    newEntry.ssid = ssid;
    newEntry.entry_time = time(NULL);

    ht->entries[index] = newEntry;
}

void deletePath(Hashtable *ht, char *path)
{
    unsigned int index = hash(path);
    ht->entries[index].entry_time = 0;
}

int getSSID(Hashtable *ht, char *path)
{
    unsigned int index = hash(path);

    if (strcmp(ht->entries[index].path, path) == 0 && ht->entries[index].entry_time != 0)
    {
        return ht->entries[index].ssid;
    }
    else
    {
        return -1;
    }
}

void printHashtable(Hashtable *ht)
{
    printf("Hashtable contents:\n");
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        if (ht->entries[i].entry_time != 0)
        {
            printf("Index %d: Path: %s, SSID: %d, Entry Time: %ld\n", i, ht->entries[i].path, ht->entries[i].ssid, ht->entries[i].entry_time);
        }
    }
    printf("\n");
}

// int main()
// {
//     // Create a hashtable
//     Hashtable ht;
//     // Initialize the hashtable
//     initializeHashtable(&ht);
//     // Example usage
//     addPath(&ht, "/example/path1", 123);

//     // sleep(2);
//     addPath(&ht, "/example/path2", 456);
//     // addPath(&ht, "/example/path1", 127);
//     // addPath(&ht, "/example/path2", 457);
//     // addPath(&ht, "/example/path1", 173);
//     // addPath(&ht, "/example/path2", 476);
//     // addPath(&ht, "/example/path1", 723);
//     // addPath(&ht, "/example/path2", 756);
//     // addPath(&ht, "/example/path1", 223);
//     // addPath(&ht, "/example/path2", 466);
//     // addPath(&ht, "/example/path1", 103);
//     // addPath(&ht, "/example/path2", 406);
//     printHashtable(&ht);
//     // printf("SSID for /example/path1: %d\n", getSSID(&ht, "/example/path1"));
//     // printf("SSID for /example/path2: %d\n", getSSID(&ht, "/example/path2"));

//     // Delete path "/example/path1"
//     // deletePath(&ht, "/example/path1");

//     // Try to get the SSID for the deleted path
//     // printf("SSID for /example/path1 after deletion: %d\n", getSSID(&ht, "/example/path1"));

//     return 0;
// }
