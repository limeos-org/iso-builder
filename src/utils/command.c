/**
 * This code is responsible for executing shell commands and filesystem operations.
 */

#include "all.h"

int run_command(const char *command)
{
    return system(command);
}

int run_chroot(const char *rootfs_path, const char *command)
{
    char quoted_path[SHELL_QUOTED_MAX_LENGTH];
    char full_command[MAX_COMMAND_LENGTH];

    // Quote the rootfs path to prevent command injection.
    if (shell_quote_path(rootfs_path, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -2;
    }

    // Execute the command inside the chroot environment.
    snprintf(full_command, sizeof(full_command), "chroot %s %s", quoted_path, command);
    if (run_command(full_command) != 0)
    {
        return -1;
    }

    return 0;
}

int mkdir_p(const char *path)
{
    char command[MAX_COMMAND_LENGTH];
    char quoted_path[SHELL_QUOTED_MAX_LENGTH];

    // Quote the path to prevent command injection.
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        LOG_ERROR("Failed to quote path: %s", path);
        return -1;
    }

    // Construct the mkdir command with parent directory creation.
    snprintf(command, sizeof(command), "mkdir -p %s", quoted_path);

    // Execute the command and check for errors.
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: mkdir -p %s", path);
        return -1;
    }

    return 0;
}

int copy_file(const char *src, const char *dst)
{
    char command[MAX_COMMAND_LENGTH];
    char quoted_src[SHELL_QUOTED_MAX_LENGTH];
    char quoted_dst[SHELL_QUOTED_MAX_LENGTH];

    // Quote paths to prevent command injection.
    if (shell_quote_path(src, quoted_src, sizeof(quoted_src)) != 0)
    {
        LOG_ERROR("Failed to quote source path: %s", src);
        return -1;
    }
    if (shell_quote_path(dst, quoted_dst, sizeof(quoted_dst)) != 0)
    {
        LOG_ERROR("Failed to quote destination path: %s", dst);
        return -1;
    }

    // Construct the copy command.
    snprintf(command, sizeof(command), "cp %s %s", quoted_src, quoted_dst);

    // Execute the command and check for errors.
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: cp %s %s", src, dst);
        return -1;
    }

    return 0;
}

int rm_rf(const char *path)
{
    char quoted_path[SHELL_QUOTED_MAX_LENGTH];
    char command[MAX_COMMAND_LENGTH];

    // Quote the path to prevent command injection.
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -2;
    }

    // Execute the recursive remove command.
    snprintf(command, sizeof(command), "rm -rf %s", quoted_path);
    if (run_command(command) != 0)
    {
        return -1;
    }

    return 0;
}

int rm_file(const char *path)
{
    char quoted_path[SHELL_QUOTED_MAX_LENGTH];
    char command[MAX_COMMAND_LENGTH];

    // Quote the path to prevent command injection.
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -2;
    }

    // Execute the remove command.
    snprintf(command, sizeof(command), "rm -f %s", quoted_path);
    if (run_command(command) != 0)
    {
        return -1;
    }

    return 0;
}

int chmod_file(const char *mode, const char *path)
{
    char quoted_path[SHELL_QUOTED_MAX_LENGTH];
    char command[MAX_COMMAND_LENGTH];

    // Quote the path to prevent command injection.
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -2;
    }

    // Execute the chmod command.
    snprintf(command, sizeof(command), "chmod %s %s", mode, quoted_path);
    if (run_command(command) != 0)
    {
        return -1;
    }

    return 0;
}

int symlink_file(const char *target, const char *link_path)
{
    char quoted_target[SHELL_QUOTED_MAX_LENGTH];
    char quoted_link[SHELL_QUOTED_MAX_LENGTH];
    char command[MAX_COMMAND_LENGTH];

    // Quote the target and link paths.
    if (shell_quote(target, quoted_target, sizeof(quoted_target)) != 0)
    {
        return -2;
    }
    if (shell_quote_path(link_path, quoted_link, sizeof(quoted_link)) != 0)
    {
        return -2;
    }

    // Execute the symlink command.
    snprintf(command, sizeof(command), "ln -sf %s %s", quoted_target, quoted_link);
    if (run_command(command) != 0)
    {
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
    // Open the file for writing.
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        LOG_ERROR("Failed to create file %s: %s", path, strerror(errno));
        return -1;
    }

    // Write the content to the file.
    if (fputs(content, file) == EOF)
    {
        LOG_ERROR("Failed to write to file %s", path);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}
