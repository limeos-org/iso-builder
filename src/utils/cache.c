/**
 * This code is responsible for caching build artifacts to speed up
 * subsequent builds. It handles base rootfs caching and shared apt cache.
 */

#include "all.h"

int init_cache(const char *cache_dir)
{
    char apt_cache_path[COMMAND_PATH_MAX_LENGTH];

    LOG_INFO("Initializing cache directory: %s", cache_dir);

    // Create the main cache directory.
    if (mkdir_p(cache_dir) != 0)
    {
        LOG_ERROR("Failed to create cache directory: %s", cache_dir);
        return -1;
    }

    // Create the apt cache subdirectory.
    if (get_apt_cache_path(cache_dir, apt_cache_path, sizeof(apt_cache_path)) != 0)
    {
        return -1;
    }
    if (mkdir_p(apt_cache_path) != 0)
    {
        LOG_ERROR("Failed to create apt cache directory");
        return -1;
    }

    return 0;
}

int has_cached_base_rootfs(const char *cache_dir)
{
    char tarball_path[COMMAND_PATH_MAX_LENGTH];

    snprintf(tarball_path, sizeof(tarball_path), "%s/%s",
        cache_dir, CONFIG_CACHE_BASE_ROOTFS_FILENAME);

    return file_exists(tarball_path);
}

int save_base_rootfs_to_cache(const char *rootfs_path, const char *cache_dir)
{
    char tarball_path[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];
    char quoted_tarball[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_rootfs[COMMAND_QUOTED_MAX_LENGTH];

    snprintf(tarball_path, sizeof(tarball_path), "%s/%s",
        cache_dir, CONFIG_CACHE_BASE_ROOTFS_FILENAME);

    LOG_INFO("Caching base rootfs to %s", tarball_path);

    // Quote paths for shell safety.
    if (shell_quote_path(tarball_path, quoted_tarball, sizeof(quoted_tarball)) != 0)
    {
        LOG_ERROR("Failed to quote tarball path");
        return -1;
    }
    if (shell_quote_path(rootfs_path, quoted_rootfs, sizeof(quoted_rootfs)) != 0)
    {
        LOG_ERROR("Failed to quote rootfs path");
        return -1;
    }

    // Create tarball of the rootfs.
    // Use -C to change to parent dir and archive just the directory name.
    snprintf(command, sizeof(command),
        "tar -czf %s -C %s .",
        quoted_tarball, quoted_rootfs);

    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create rootfs cache tarball");
        return -1;
    }

    LOG_INFO("Base rootfs cached successfully");
    return 0;
}

int restore_base_rootfs_from_cache(const char *cache_dir, const char *rootfs_path)
{
    char tarball_path[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];
    char quoted_tarball[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_rootfs[COMMAND_QUOTED_MAX_LENGTH];

    snprintf(tarball_path, sizeof(tarball_path), "%s/%s",
        cache_dir, CONFIG_CACHE_BASE_ROOTFS_FILENAME);

    LOG_INFO("Restoring base rootfs from cache");

    // Quote paths for shell safety.
    if (shell_quote_path(tarball_path, quoted_tarball, sizeof(quoted_tarball)) != 0)
    {
        LOG_ERROR("Failed to quote tarball path");
        return -1;
    }
    if (shell_quote_path(rootfs_path, quoted_rootfs, sizeof(quoted_rootfs)) != 0)
    {
        LOG_ERROR("Failed to quote rootfs path");
        return -1;
    }

    // Create the target directory.
    if (mkdir_p(rootfs_path) != 0)
    {
        LOG_ERROR("Failed to create rootfs directory");
        return -1;
    }

    // Extract the cached tarball.
    snprintf(command, sizeof(command),
        "tar -xzf %s -C %s",
        quoted_tarball, quoted_rootfs);

    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to extract rootfs cache");
        return -1;
    }

    LOG_INFO("Base rootfs restored from cache");
    return 0;
}

int get_apt_cache_path(const char *cache_dir, char *out_path, size_t buffer_length)
{
    int written = snprintf(out_path, buffer_length, "%s/%s",
        cache_dir, CONFIG_CACHE_APT_DIR);

    if (written < 0 || (size_t)written >= buffer_length)
    {
        return -1;
    }

    return 0;
}

int mount_apt_cache(const char *cache_dir, const char *rootfs_path)
{
    char apt_cache_path[COMMAND_PATH_MAX_LENGTH];
    char rootfs_apt_cache[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];
    char quoted_src[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_dst[COMMAND_QUOTED_MAX_LENGTH];

    // Get the shared apt cache path.
    if (get_apt_cache_path(cache_dir, apt_cache_path, sizeof(apt_cache_path)) != 0)
    {
        return -1;
    }

    // Construct the target path inside the rootfs.
    snprintf(rootfs_apt_cache, sizeof(rootfs_apt_cache),
        "%s/var/cache/apt/archives", rootfs_path);

    // Create the target directory if it doesn't exist.
    if (mkdir_p(rootfs_apt_cache) != 0)
    {
        LOG_ERROR("Failed to create apt cache mount point");
        return -1;
    }

    // Quote paths for shell safety.
    if (shell_quote_path(apt_cache_path, quoted_src, sizeof(quoted_src)) != 0)
    {
        return -1;
    }
    if (shell_quote_path(rootfs_apt_cache, quoted_dst, sizeof(quoted_dst)) != 0)
    {
        return -1;
    }

    // Bind mount the shared apt cache.
    snprintf(command, sizeof(command),
        "mount --bind %s %s", quoted_src, quoted_dst);

    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to mount apt cache");
        return -1;
    }

    LOG_INFO("Mounted shared apt cache");
    return 0;
}

int unmount_apt_cache(const char *rootfs_path)
{
    char rootfs_apt_cache[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];

    // Construct the mount point path.
    snprintf(rootfs_apt_cache, sizeof(rootfs_apt_cache),
        "%s/var/cache/apt/archives", rootfs_path);

    // Quote path for shell safety.
    if (shell_quote_path(rootfs_apt_cache, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -1;
    }

    // Unmount the apt cache.
    snprintf(command, sizeof(command), "umount %s 2>/dev/null", quoted_path);

    // Don't fail if not mounted - just log and continue.
    if (run_command(command) != 0)
    {
        LOG_WARNING("Apt cache was not mounted or already unmounted");
    }

    return 0;
}

int has_cached_payload_rootfs(const char *cache_dir)
{
    char tarball_path[COMMAND_PATH_MAX_LENGTH];

    snprintf(tarball_path, sizeof(tarball_path), "%s/%s",
        cache_dir, CONFIG_CACHE_PAYLOAD_ROOTFS_FILENAME);

    return file_exists(tarball_path);
}

int save_payload_rootfs_to_cache(const char *rootfs_path, const char *cache_dir)
{
    char tarball_path[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];
    char quoted_tarball[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_rootfs[COMMAND_QUOTED_MAX_LENGTH];

    snprintf(tarball_path, sizeof(tarball_path), "%s/%s",
        cache_dir, CONFIG_CACHE_PAYLOAD_ROOTFS_FILENAME);

    LOG_INFO("Caching payload rootfs to %s", tarball_path);

    // Quote paths for shell safety.
    if (shell_quote_path(tarball_path, quoted_tarball, sizeof(quoted_tarball)) != 0)
    {
        LOG_ERROR("Failed to quote tarball path");
        return -1;
    }
    if (shell_quote_path(rootfs_path, quoted_rootfs, sizeof(quoted_rootfs)) != 0)
    {
        LOG_ERROR("Failed to quote rootfs path");
        return -1;
    }

    // Create tarball of the rootfs.
    snprintf(command, sizeof(command),
        "tar -czf %s -C %s .",
        quoted_tarball, quoted_rootfs);

    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create payload rootfs cache tarball");
        return -1;
    }

    LOG_INFO("Payload rootfs cached successfully");
    return 0;
}

int restore_payload_rootfs_from_cache(const char *cache_dir, const char *rootfs_path)
{
    char tarball_path[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];
    char quoted_tarball[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_rootfs[COMMAND_QUOTED_MAX_LENGTH];

    snprintf(tarball_path, sizeof(tarball_path), "%s/%s",
        cache_dir, CONFIG_CACHE_PAYLOAD_ROOTFS_FILENAME);

    LOG_INFO("Restoring payload rootfs from cache");

    // Quote paths for shell safety.
    if (shell_quote_path(tarball_path, quoted_tarball, sizeof(quoted_tarball)) != 0)
    {
        LOG_ERROR("Failed to quote tarball path");
        return -1;
    }
    if (shell_quote_path(rootfs_path, quoted_rootfs, sizeof(quoted_rootfs)) != 0)
    {
        LOG_ERROR("Failed to quote rootfs path");
        return -1;
    }

    // Create the target directory.
    if (mkdir_p(rootfs_path) != 0)
    {
        LOG_ERROR("Failed to create rootfs directory");
        return -1;
    }

    // Extract the cached tarball.
    snprintf(command, sizeof(command),
        "tar -xzf %s -C %s",
        quoted_tarball, quoted_rootfs);

    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to extract payload rootfs cache");
        return -1;
    }

    LOG_INFO("Payload rootfs restored from cache");
    return 0;
}
