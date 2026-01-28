/**
 * This code is responsible for validating system dependencies before
 * starting the ISO build process.
 */

#include "all.h"

/** Required files that must exist on the host system. */
const char *const REQUIRED_FILES[] = {
    CONFIG_SPLASH_LOGO_PATH
};
const int REQUIRED_FILES_COUNT =
    sizeof(REQUIRED_FILES) / sizeof(REQUIRED_FILES[0]);

/** Required commands that must be available in PATH. */
const char *const REQUIRED_COMMANDS[] = {
    "debootstrap",
    "mksquashfs",
    "grub-mkrescue",
    "tar",
    "chroot"
};
const int REQUIRED_COMMANDS_COUNT =
    sizeof(REQUIRED_COMMANDS) / sizeof(REQUIRED_COMMANDS[0]);

int validate_dependencies(void)
{
    int missing_files = 0;
    int missing_commands = 0;

    // Check required files.
    for (int i = 0; i < REQUIRED_FILES_COUNT; i++)
    {
        if (!file_exists(REQUIRED_FILES[i]))
        {
            missing_files = 1;
        }
    }

    // Check required commands.
    for (int i = 0; i < REQUIRED_COMMANDS_COUNT; i++)
    {
        if (!is_command_available(REQUIRED_COMMANDS[i]))
        {
            missing_commands = 1;
        }
    }

    if (missing_files && missing_commands)
    {
        return -3;
    }
    if (missing_files)
    {
        return -1;
    }
    if (missing_commands)
    {
        return -2;
    }
    return 0;
}
