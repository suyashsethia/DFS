#include "create.h"
#include "../Common/requests.h"
#include <sys/stat.h>

int create_folder(const char *path)
{
    return mkdir(path, 0777);
}
int create_file(const char *path)
{
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        return -1;
    }
    fclose(file);
    return 0;
}
