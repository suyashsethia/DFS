#include "create.h"
#include "../Common/requests.h"
#include <sys/stat.h>

int create_folder(int ssid, const char *path)
{
    char folder_path[MAX_PATH_LENGTH];
    snprintf(folder_path, MAX_PATH_LENGTH, "%d/%s", ssid, path);
    return mkdir(folder_path, 0777);
}
int create_file(int ssid, const char *path)
{
    char file_path[MAX_PATH_LENGTH];
    snprintf(file_path, MAX_PATH_LENGTH, "%d/%s", ssid, path);
    FILE *file = fopen(file_path, "w");
    if (file == NULL)
    {
        return -1;
    }
    fclose(file);
    return 0;
}
