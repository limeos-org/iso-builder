/**
 * This code is responsible for orchestrating the base phase.
 */

#include "all.h"

int run_base_phase(const char *rootfs_dir)
{
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

    LOG_INFO("Phase 2 complete: Base rootfs ready");
    return 0;
}
