#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "delete.h"
#include "../Common/requests.h"
#include "../Common/loggers.h"

int _delete_file_or_folder(const char *system_path);

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
            if (_delete_file_or_folder(entry_path) != 0)
            {
                closedir(dir);
                return -1;
            }
        }
    }

    closedir(dir);
    return 0;
}

int _delete_file_or_folder(const char *system_path)
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
        if (remove(system_path) != 0)
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }

    return 0;
}

int delete_file_or_folder(int ssid, const char *path)
{
    // char system_path[MAX_PATH_LENGTH];
    // snprintf(system_path, MAX_PATH_LENGTH, "%d/%s", ssid, path);

    return _delete_file_or_folder(path);
}
