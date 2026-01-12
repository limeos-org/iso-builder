/**
 * This code is responsible for creating a minimal base rootfs that both
 * payload and carrier environments will be derived from.
 */

#include "all.h"

int create_base_rootfs(const char *path)
{
    char command[COMMAND_MAX_LENGTH];
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];

    LOG_INFO("Creating base rootfs at %s", path);

    // Quote the path to prevent shell injection.
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        LOG_ERROR("Failed to quote path");
        return -1;
    }

    // Run debootstrap to create a minimal Debian rootfs.
    snprintf(
        command, sizeof(command),
        "debootstrap --variant=minbase %s %s",
        CONFIG_DEBIAN_RELEASE, quoted_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: debootstrap");
        return -1;
    }

    // Update package lists for later package installation.
    LOG_INFO("Updating package lists...");
    if (run_chroot(path, "apt-get update") != 0)
    {
        LOG_ERROR("Failed to update package lists");
        return -2;
    }

    LOG_INFO("Base rootfs created successfully");

    return 0;
}
