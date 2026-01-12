/**
 * This code is responsible for orchestrating the base phase.
 */

#include "all.h"

int run_base_phase(const char *rootfs_dir, const char *cache_dir)
{
    // Check if we can restore from cache.
    if (cache_dir && has_cached_base_rootfs(cache_dir))
    {
        LOG_INFO("Found cached base rootfs, restoring...");
        if (restore_base_rootfs_from_cache(cache_dir, rootfs_dir) == 0)
        {
            LOG_INFO("Phase 2 complete: Base rootfs restored from cache");
            return 0;
        }
        LOG_WARNING("Failed to restore from cache, building fresh");
    }

    // Create fresh rootfs.
    if (create_base_rootfs(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create base rootfs");
        return -1;
    }

    if (strip_base_rootfs(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to strip base rootfs");
        return -1;
    }

    // Save to cache for future builds.
    if (cache_dir)
    {
        if (save_base_rootfs_to_cache(rootfs_dir, cache_dir) != 0)
        {
            LOG_WARNING("Failed to cache base rootfs (continuing anyway)");
        }
    }

    LOG_INFO("Phase 2 complete: Base rootfs ready");
    return 0;
}
