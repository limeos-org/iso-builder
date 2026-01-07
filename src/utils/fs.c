/**
 * This code is responsible for filesystem utility operations.
 */

#include "all.h"

int mkdir_p(const char *path)
{
    char command[MAX_COMMAND_LENGTH];

    // Construct the mkdir command with parent directory creation.
    snprintf(command, sizeof(command), "mkdir -p %s", path);

    // Execute the command and check for errors.
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    return 0;
}

int copy_file(const char *src, const char *dst)
{
    char command[MAX_COMMAND_LENGTH];

    // Construct the copy command.
    snprintf(command, sizeof(command), "cp %s %s", src, dst);

    // Execute the command and check for errors.
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    return 0;
}

int file_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

int write_file(const char *path, const char *content)
{
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        LOG_ERROR("Failed to create file %s: %s", path, strerror(errno));
        return -1;
    }

    if (fputs(content, file) == EOF)
    {
        LOG_ERROR("Failed to write to file %s", path);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}
