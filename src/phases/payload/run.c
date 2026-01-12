/**
 * This code is responsible for orchestrating the payload phase.
 */

#include "all.h"

int run_payload_phase(
    const char *base_rootfs_dir, const char *rootfs_dir,
    const char *tarball_path, const char *version,
    const char *cache_dir
)
{
    // Check for cached payload rootfs.
    if (cache_dir && has_cached_payload_rootfs(cache_dir))
    {
        LOG_INFO("Found cached payload rootfs, restoring...");
        if (restore_payload_rootfs_from_cache(cache_dir, rootfs_dir) == 0)
        {
            // Update branding with current version (in case version changed).
            if (brand_payload_rootfs(rootfs_dir, version) != 0)
            {
                LOG_ERROR("Failed to brand payload rootfs");
                return -1;
            }

            if (package_payload_rootfs(rootfs_dir, tarball_path) != 0)
            {
                LOG_ERROR("Failed to package payload rootfs");
                return -1;
            }

            rm_rf(rootfs_dir);
            LOG_INFO("Phase 3 complete: Payload rootfs restored from cache");
            return 0;
        }
        LOG_WARNING("Failed to restore from cache, rebuilding...");
    }

    if (create_payload_rootfs(base_rootfs_dir, rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create payload rootfs");
        return -1;
    }

    // Mount shared apt cache if caching is enabled.
    if (cache_dir)
    {
        if (mount_apt_cache(cache_dir, rootfs_dir) != 0)
        {
            LOG_WARNING("Failed to mount apt cache, continuing without it");
        }
    }

    if (brand_payload_rootfs(rootfs_dir, version) != 0)
    {
        LOG_ERROR("Failed to brand payload rootfs");
        if (cache_dir) unmount_apt_cache(rootfs_dir);
        return -1;
    }

    if (create_default_user(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create default user");
        if (cache_dir) unmount_apt_cache(rootfs_dir);
        return -1;
    }

    // Unmount apt cache before cleanup.
    if (cache_dir)
    {
        unmount_apt_cache(rootfs_dir);
    }

    // Remove firmware that may have been reinstalled by linux-image package.
    cleanup_unnecessary_firmware(rootfs_dir);

    // Clean up apt cache after all packages are installed.
    if (cleanup_apt_directories(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to cleanup apt directories");
        return -1;
    }

    // Cache the payload rootfs for future builds.
    if (cache_dir)
    {
        if (save_payload_rootfs_to_cache(rootfs_dir, cache_dir) != 0)
        {
            LOG_WARNING("Failed to cache payload rootfs");
        }
    }

    if (package_payload_rootfs(rootfs_dir, tarball_path) != 0)
    {
        LOG_ERROR("Failed to package payload rootfs");
        return -1;
    }

    rm_rf(rootfs_dir);
    LOG_INFO("Phase 3 complete: Payload rootfs packaged");
    return 0;
}
