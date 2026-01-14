/**
 * This code is responsible for caching downloaded APT packages via bind
 * mounts to speed up subsequent builds.
 */

#include "all.h"

int get_package_cache_dir(char *out_path, size_t buffer_length)
{
    // Get the base cache directory.
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        return -1;
    }

    // Append the package cache subdirectory.
    snprintf(out_path, buffer_length, "%s/packages/apt", cache_dir);
    return 0;
}

int setup_package_cache_mount(const char *rootfs_path)
{
    // Get host package cache directory.
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_package_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        return -1;
    }

    // Create host cache directory if it doesn't exist.
    if (mkdir_p(cache_dir) != 0)
    {
        LOG_WARNING("Failed to create package cache directory");
        return -1;
    }

    // Construct target directory path inside rootfs.
    char target_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(
        target_dir, sizeof(target_dir),
        "%s/var/cache/apt/archives",
        rootfs_path
    );

    // Ensure target directory exists.
    if (mkdir_p(target_dir) != 0)
    {
        LOG_WARNING("Failed to create apt archives directory in rootfs");
        return -1;
    }

    // Quote paths for shell safety.
    char quoted_src[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(cache_dir, quoted_src, sizeof(quoted_src)) != 0)
    {
        return -1;
    }

    char quoted_dst[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(target_dir, quoted_dst, sizeof(quoted_dst)) != 0)
    {
        return -1;
    }

    // Bind mount the cache directory.
    char command[COMMAND_MAX_LENGTH];
    snprintf(
        command, sizeof(command),
        "mount --bind %s %s",
        quoted_src, quoted_dst
    );
    if (run_command(command) != 0)
    {
        LOG_WARNING("Failed to mount package cache");
        return -1;
    }

    return 0;
}

int teardown_package_cache_mount(const char *rootfs_path)
{
    // Construct target directory path inside rootfs.
    char target_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(
        target_dir, sizeof(target_dir),
        "%s/var/cache/apt/archives",
        rootfs_path
    );

    // Quote path for shell safety.
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(target_dir, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -1;
    }

    // Unmount the bind mount.
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "umount %s", quoted_path);
    if (run_command(command) != 0)
    {
        LOG_WARNING("Failed to unmount package cache");
        return -1;
    }

    return 0;
}
