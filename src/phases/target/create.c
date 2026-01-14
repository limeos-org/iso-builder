/**
 * This code is responsible for creating the target rootfs by copying
 * from the base rootfs and installing target-specific packages.
 */

#include "all.h"

int create_target_rootfs(const char *base_path, const char *path, int use_cache)
{
    LOG_INFO("Creating target rootfs at %s", path);

    // Quote paths to prevent shell injection.
    char quoted_base[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(base_path, quoted_base, sizeof(quoted_base)) != 0)
    {
        LOG_ERROR("Failed to quote base path");
        return -1;
    }

    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        LOG_ERROR("Failed to quote path");
        return -1;
    }

    // Copy the base rootfs.
    LOG_INFO("Copying base rootfs...");
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "cp -a %s %s", quoted_base, quoted_path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy base rootfs");
        return -1;
    }

    // Set up package cache mount if caching is enabled.
    int package_cache_mounted = 0;
    if (use_cache && setup_package_cache_mount(path) == 0)
    {
        package_cache_mounted = 1;
    }

    // Install target-specific packages.
    // DEBIAN_FRONTEND=noninteractive prevents prompts from locales,
    // console-setup, and keyboard-configuration packages.
    LOG_INFO("Installing target system packages...");
    int install_result = run_chroot(path,
        "DEBIAN_FRONTEND=noninteractive "
        "apt-get install -y --no-install-recommends " CONFIG_TARGET_PACKAGES);

    // Tear down package cache mount if it was set up.
    if (package_cache_mounted)
    {
        teardown_package_cache_mount(path);
    }

    if (install_result != 0)
    {
        LOG_ERROR("Failed to install required packages");
        return -2;
    }

    LOG_INFO("Target rootfs created successfully");

    return 0;
}
