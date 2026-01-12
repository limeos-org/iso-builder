/**
 * This code is responsible for orchestrating the carrier phase.
 */

#include "all.h"

int run_carrier_phase(
    const char *base_rootfs_dir,
    const char *rootfs_dir,
    const char *tarball_path,
    const char *components_dir,
    const char *cache_dir
)
{
    int from_cache = 0;

    // Check for cached carrier rootfs (base + packages, before payload/components).
    if (cache_dir && has_cached_carrier_rootfs(cache_dir))
    {
        LOG_INFO("Found cached carrier rootfs, restoring...");
        if (restore_carrier_rootfs_from_cache(cache_dir, rootfs_dir) == 0)
        {
            from_cache = 1;
        }
        else
        {
            LOG_WARNING("Failed to restore from cache, rebuilding...");
        }
    }

    if (!from_cache)
    {
        if (create_carrier_rootfs(base_rootfs_dir, rootfs_dir) != 0)
        {
            LOG_ERROR("Failed to create carrier rootfs");
            return -1;
        }

        // Cache the carrier rootfs after packages are installed.
        if (cache_dir)
        {
            if (save_carrier_rootfs_to_cache(rootfs_dir, cache_dir) != 0)
            {
                LOG_WARNING("Failed to cache carrier rootfs");
            }
        }
    }

    // Mount shared apt cache for bundle_packages.
    if (cache_dir)
    {
        if (mount_apt_cache(cache_dir, rootfs_dir) != 0)
        {
            LOG_WARNING("Failed to mount apt cache, continuing without it");
        }
    }

    if (embed_payload_rootfs(rootfs_dir, tarball_path) != 0)
    {
        LOG_ERROR("Failed to embed payload rootfs");
        if (cache_dir) unmount_apt_cache(rootfs_dir);
        return -1;
    }

    if (install_carrier_components(rootfs_dir, components_dir) != 0)
    {
        LOG_ERROR("Failed to install components");
        if (cache_dir) unmount_apt_cache(rootfs_dir);
        return -1;
    }

    if (configure_carrier_init(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure init");
        if (cache_dir) unmount_apt_cache(rootfs_dir);
        return -1;
    }

    if (bundle_packages_with_cache(rootfs_dir, cache_dir) != 0)
    {
        LOG_ERROR("Failed to bundle packages");
        if (cache_dir) unmount_apt_cache(rootfs_dir);
        return -1;
    }

    // Unmount apt cache before cleanup.
    if (cache_dir)
    {
        unmount_apt_cache(rootfs_dir);
    }

    // Clean up apt cache after all packages are installed.
    if (cleanup_apt_directories(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to cleanup apt directories");
        return -1;
    }

    LOG_INFO("Phase 4 complete: Carrier rootfs created");
    return 0;
}
