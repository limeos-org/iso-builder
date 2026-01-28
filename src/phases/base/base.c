/**
 * This code is responsible for orchestrating the base phase.
 */

#include "all.h"

int run_base_phase(const char *rootfs_dir)
{
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
        return -2;
    }

    LOG_INFO("Phase 2 complete: Base rootfs ready");

    return 0;
}
