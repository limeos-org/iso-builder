/**
 * This code is responsible for validating system dependencies before
 * starting the ISO build process.
 */

#include "all.h"

// Required files that must exist on the host system.
static const char *required_files[] = {
    CONFIG_SPLASH_LOGO_PATH,
    CONFIG_BLACK_PNG_PATH,
    CONFIG_ISOLINUX_BIN_PATH,
    CONFIG_ISOLINUX_MBR_PATH,
    CONFIG_LDLINUX_PATH,
    CONFIG_VESAMENU_C32_PATH,
    CONFIG_LIBUTIL_C32_PATH,
    CONFIG_LIBCOM32_C32_PATH
};

// Required commands that must be available in PATH.
static const char *required_commands[] = {
    "debootstrap",
    "mksquashfs",
    "xorriso",
    "mkfs.fat",
    "grub-mkimage",
    "tar",
    "chroot"
};

int validate_dependencies(void)
{
    int missing = 0;

    // Check required files.
    const int file_count = sizeof(required_files) / sizeof(required_files[0]);
    for (int i = 0; i < file_count; i++)
    {
        if (!file_exists(required_files[i]))
        {
            LOG_ERROR("Missing required file: %s", required_files[i]);
            missing = 1;
        }
    }

    // Check required commands.
    const int command_count = sizeof(required_commands) / sizeof(required_commands[0]);
    for (int i = 0; i < command_count; i++)
    {
        if (!is_command_available(required_commands[i]))
        {
            LOG_ERROR("Missing required command: %s", required_commands[i]);
            missing = 1;
        }
    }

    return missing;
}

int is_command_available(const char *name)
{
    // Get PATH environment variable.
    char *path_env = getenv("PATH");
    if (path_env == NULL)
    {
        return 0;
    }

    // Duplicate PATH string for tokenization.
    char *path = strdup(path_env);
    if (path == NULL)
    {
        return 0;
    }

    // Search each directory in PATH for the executable.
    char *dir = strtok(path, ":");
    while (dir != NULL)
    {
        // Build full path and check if executable exists.
        char full_path[COMMAND_PATH_MAX_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, name);
        if (access(full_path, X_OK) == 0)
        {
            free(path);
            return 1;
        }
        dir = strtok(NULL, ":");
    }

    // Clean up and return not found.
    free(path);
    return 0;
}
