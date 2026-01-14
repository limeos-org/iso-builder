/**
 * This code is responsible for caching the base rootfs as a tarball to
 * speed up subsequent builds.
 */

#include "all.h"

#define CACHE_FILENAME "base-rootfs.tar.gz"

int cache_exists(char *out_cache_path, size_t buffer_length)
{
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        return 0;
    }

    snprintf(out_cache_path, buffer_length, "%s/%s", cache_dir, CACHE_FILENAME);

    return file_exists(out_cache_path);
}

int restore_from_cache(const char *cache_path, const char *rootfs_dir)
{
    LOG_INFO("Restoring base rootfs from cache...");

    char quoted_cache[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(cache_path, quoted_cache, sizeof(quoted_cache)) != 0)
    {
        return -1;
    }

    char quoted_dir[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(rootfs_dir, quoted_dir, sizeof(quoted_dir)) != 0)
    {
        return -1;
    }

    if (mkdir_p(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create rootfs directory");
        return -1;
    }

    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "tar -xzf %s -C %s", quoted_cache, quoted_dir);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to extract cached rootfs");
        return -1;
    }

    LOG_INFO("Base rootfs restored from cache");
    return 0;
}

int save_to_cache(const char *rootfs_dir)
{
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        LOG_WARNING("Cannot save to cache: unable to determine cache directory");
        return -1;
    }

    if (mkdir_p(cache_dir) != 0)
    {
        LOG_WARNING("Cannot save to cache: failed to create cache directory");
        return -1;
    }

    char cache_path[COMMAND_PATH_MAX_LENGTH];
    snprintf(cache_path, sizeof(cache_path), "%s/%s", cache_dir, CACHE_FILENAME);

    char quoted_cache[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(cache_path, quoted_cache, sizeof(quoted_cache)) != 0)
    {
        return -1;
    }

    char quoted_dir[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(rootfs_dir, quoted_dir, sizeof(quoted_dir)) != 0)
    {
        return -1;
    }

    LOG_INFO("Saving base rootfs to cache...");

    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "tar -czf %s -C %s .", quoted_cache, quoted_dir);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create cache tarball");
        rm_file(cache_path);
        return -1;
    }

    LOG_INFO("Base rootfs cached at %s", cache_path);

    return 0;
}
