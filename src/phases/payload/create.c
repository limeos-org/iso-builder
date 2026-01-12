/**
 * This code is responsible for creating the payload rootfs by copying
 * from the base rootfs and installing payload-specific packages.
 */

#include "all.h"

int create_payload_rootfs(const char *base_path, const char *path)
{
    char command[COMMAND_MAX_LENGTH];
    char quoted_base[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];

    LOG_INFO("Creating payload rootfs at %s", path);

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

    // Install payload-specific packages.
    // DEBIAN_FRONTEND=noninteractive prevents prompts from locales,
    // console-setup, and keyboard-configuration packages.
    LOG_INFO("Installing payload system packages...");
    if (run_chroot(path,
        "DEBIAN_FRONTEND=noninteractive "
        "apt-get install -y --no-install-recommends " CONFIG_PAYLOAD_PACKAGES) != 0)
    {
        LOG_ERROR("Failed to install required packages");
        return -2;
    }

    LOG_INFO("Payload rootfs created successfully");

    return 0;
}
