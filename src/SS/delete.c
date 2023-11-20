#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>


#include "delete.h"
#include "../Common/requests.h"
#include "../Common/loggers.h"
// #include "client_handler.c"

int try_lock_filee(FILE *file, short type)
{
    int fd = fileno(file); // Get the file descriptor associated with the FILE pointer

    struct flock lock;
    lock.l_type = type; // lock for both read and write depending on the type
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;

    // Attempt to obtain the write lock without blocking
    if (fcntl(fd, F_SETLK, &lock) == -1)
    {
        // Handle the case when the lock cannot be acquired immediately
        log_errno_error("Error obtaining write lock: %s\n");
        // perror("Error obtaining write lock");
        return -1;
    }

    // Lock obtained successfully
    return 0;
}

void unlock_filee(FILE *file)
{
    int fd = fileno(file); // Get the file descriptor associated with the FILE pointer

    struct flock unlock;
    unlock.l_type = F_UNLCK; // Unlock
    unlock.l_start = 0;
    unlock.l_whence = SEEK_SET;
    unlock.l_len = 0;

    if (fcntl(fd, F_SETLK, &unlock) == -1)
    {
        log_errno_error("Error unlocking file: %s\n");
        // perror("Error unlocking file");
        exit(EXIT_FAILURE);
    }
}

int delete_folder_contents(const char *system_path)
{
    DIR *dir = opendir(system_path);
    if (dir == NULL)
    {
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char entry_path[MAX_PATH_LENGTH];
            snprintf(entry_path, sizeof(entry_path), "%s/%s", system_path, entry->d_name);
            if (delete_file_or_folder(entry_path) != 0)
            {
                closedir(dir);
                return -1;
            }
        }
    }

    closedir(dir);
    return 0;
}

int delete_file_or_folder(const char *system_path)
{
    struct stat path_stat;
    if (stat(system_path, &path_stat) != 0)
    {
        return -1;
    }

    if (S_ISDIR(path_stat.st_mode))
    {
        if (delete_folder_contents(system_path) != 0)
        {
            return -1;
        }

        if (rmdir(system_path) != 0)
        {
            return -1;
        }
    }
    else if (S_ISREG(path_stat.st_mode))
    {
        FILE *file = fopen(system_path, "a");
        if (file == NULL)
        {
            return -1;
        }
        
        // check if file is present in writing_file in a for loop 
        // if present then log error and return -1
        for (int i = 0; i < writing_file_count; i++)
        {
            if (strcmp(writing_file[i], system_path) == 0)
            {
                log_error("File is being written to: %s\n");
                return -1;
            }
        }
        try_lock_filee(file , F_WRLCK);

        if (remove(system_path) != 0)
        {
            return -1;
        }
        unlock_filee(file);
        fclose(file);
    }
    else
    {
        return -1;
    }

    return 0;
}
