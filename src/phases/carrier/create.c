/**
 * This code is responsible for creating the carrier rootfs by copying
 * from the base rootfs and installing carrier-specific packages.
 */

#include "all.h"

int create_carrier_rootfs(const char *base_path, const char *path)
{
    char command[COMMAND_MAX_LENGTH];
    char quoted_base[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];

    LOG_INFO("Creating carrier rootfs at %s", path);

    // Quote paths to prevent shell injection.
    if (shell_quote_path(base_path, quoted_base, sizeof(quoted_base)) != 0)
    {
        LOG_ERROR("Failed to quote base path");
        return -1;
    }
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        LOG_ERROR("Failed to quote path");
        return -1;
    }

    // Copy the base rootfs.
    LOG_INFO("Copying base rootfs...");
    snprintf(command, sizeof(command), "cp -a %s %s", quoted_base, quoted_path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy base rootfs");
        return -1;
    }

    // Install carrier-specific packages.
    LOG_INFO("Installing carrier environment packages...");
    if (run_chroot(path,
        "apt-get install -y --no-install-recommends " CONFIG_CARRIER_PACKAGES) != 0)
    {
        LOG_ERROR("Failed to install required packages");
        return -2;
    }

    // Copy kernel and initrd to standard paths for boot loaders.
    if (copy_kernel_and_initrd(path) != 0)
    {
        return -3;
    }

    LOG_INFO("Carrier rootfs created successfully");

    return 0;
}
