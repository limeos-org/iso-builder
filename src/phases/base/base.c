/**
 * This code is responsible for orchestrating the base phase.
 */

#include "all.h"

int run_base_phase(const char *rootfs_dir, int use_cache)
{
    // Try to restore from cache if enabled.
    char cache_path[COMMAND_PATH_MAX_LENGTH];
    if (use_cache && has_rootfs_cache(cache_path, sizeof(cache_path)))
    {
        LOG_INFO("Found cached base rootfs");
        if (restore_rootfs_from_cache(cache_path, rootfs_dir) == 0)
        {
            LOG_INFO("Phase 2 complete: Base rootfs ready (from cache)");
            return 0;
        }
        LOG_WARNING("Cache restore failed, rebuilding from scratch");
    }

    // Create base rootfs from scratch.
    if (create_base_rootfs(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create base rootfs");
        return -1;
    }

    // Strip noncritical files from rootfs.
    if (strip_base_rootfs(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to strip base rootfs");
        return -1;
    }

    // Save to cache for future builds (if caching is enabled).
    if (use_cache)
    {
        if (save_rootfs_to_cache(rootfs_dir) != 0)
        {
            // Cache save failure is not fatal - just log warning.
            LOG_WARNING("Failed to save rootfs to cache (build will continue)");
        }
    }

    LOG_INFO("Phase 2 complete: Base rootfs ready");
    
    return 0;
}
